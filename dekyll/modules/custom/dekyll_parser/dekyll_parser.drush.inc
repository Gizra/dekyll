<?php

/**
 * @file
 * Drush integration for the Dekyll Parser module.
 */

require DRUPAL_ROOT . '/profiles/dekyll/libraries/vendor/autoload.php';

use GitWrapper\GitWrapper;
use Symfony\Component\Finder\Finder;
use Symfony\Component\Yaml\Parser;
use Symfony\Component\Yaml\Dumper;

/**
 * Implements hook_drush_command().
 */
function dekyll_parser_drush_command() {
  $items['dekyll-write-file'] = array(
    'description' => dt('Write an item form the queue into the Jekyll file.'),
  );

  $items['dekyll-sync'] = array(
    'description' => dt('Sync Drupal with Jekyll.'),
    'options' => array(
      'force' => dt('Optional. Force re-sync even if the file did not change.'),
    ),
  );

  $items['dekyll-clone'] = array(
    'description' => dt('Clone a Git repository.'),
  );

  return $items;
}

/**
 * Write an item form the queue into the Jekyll file.
 */
function drush_dekyll_parser_dekyll_write_file() {
  $queue = DrupalQueue::get('dekyll_write');
  if (!$queue_item = $queue->claimItem()) {
    return;
  }

  // "git pull" before writing the file.
  $path = variable_get('file_public_path', conf_path() . '/files') . '/git';
  $git_wrapper = new GitWrapper();
  $git = $git_wrapper ->workingCopy($path);

  // Checkout to the needed branch.
  $git->checkout($queue_item['branch']);

  // Get the HEAD version.
  $hash = $git->run(array('rev-parse --verify HEAD'));
  drush_log(dt('Latest commit before pull is @hash', array('@hash' => $hash)));

  $git->pull();
  drush_log(dt('git pull'));

  if ($hash != $git->run(array('rev-parse --verify HEAD'))) {
    // The latest commit hash changed, so we need to re-sync.
    if (!$sync = drush_dekyll_parser_dekyll_sync()) {
      // We can't write the file, as we have sync problems.
      return;
    }
  }

  $nid = $queue_item['nid'];

  // Write the file.
  $wrapper = entity_metadata_wrapper('node', $nid);
  $overrides = array();

  foreach (field_info_instances('node', 'post') as $field_name => $instance) {
    if (empty($instance['settings']['jekyll_map'])) {
      continue;
    }

    // The Jekyll variable name.
    $jekyll = $instance['settings']['jekyll_map'];
    $field = field_info_field($field_name);

    switch ($field['type']) {
      case 'entityreference':
      case 'taxonomy_term':
        if ($field['cardinality'] == 1) {
          $overrides[$jekyll] = $wrapper->{$field_name}->label();
        }
        else {
          foreach ($wrapper->{$field_name} as $sub_wrapper) {
            $overrides[$jekyll][] = $sub_wrapper->label();
          }
        }
        break;
      default:
        if ($field['cardinality'] == 1) {
          $overrides[$jekyll] = $wrapper->{$field_name}->value();
        }
        else {
          foreach ($wrapper->{$field_name} as $sub_wrapper) {
            $overrides[$jekyll][] = $sub_wrapper->value();
          }
        }
        break;
    }
  }

  $overrides['title'] = $wrapper->label();

  $full_path = $path . '/' . $wrapper->field_full_path->value();
  $contents = file_get_contents($full_path);

  // Get the values from the YAML front header.
  $split_contents = explode(YAML_SEPARATOR, $contents, 3);
  $yaml_contents = $split_contents[1];
  $text = $split_contents[2];

  $yaml = new Parser();
  $yaml_parsed = $yaml->parse($yaml_contents);

  // Add the overrides.
  $overrides = array_merge($yaml_parsed, $overrides);

  $dumper = new Dumper();

  $dump = array(
    YAML_SEPARATOR,
    // Dump the YAML expanded, and not inline.
    $dumper->dump($overrides, 2) . "\n",
    YAML_SEPARATOR,
    $text,
  );

  file_put_contents($full_path, implode('', $dump));

  // Commit and push.
  $git
    ->commit('Changes of file.')
    ->push();
}


/**
 * Sync Drupal with Jekyll.
 *
 * Iterate over all the files and create or update a node to link to them.
 * We use _dekyll.yml file to hold each file, its hash and the related node ID.
 */
function drush_dekyll_parser_dekyll_sync() {
  $path = variable_get('file_public_path', conf_path() . '/files') . '/git';


  $git_wrapper = new GitWrapper();
  $git = $git_wrapper->workingCopy($path);

  // Map file path to the node ID.
  $map = array();
  if (file_exists($path . '/_dekyll.yml')) {
    // Open the _dekyll.yml and get the map.
    $contents = file_get_contents($path . '/_dekyll.yml');

    // Get the map.
    $yaml = new Parser();
    $map = $yaml->parse($contents);
  }

  // Iterate over all the valid files, and compare them to the map value.
  $finder = new Finder();
  $finder
    ->files()
    ->in($path)
    // @todo: Exclude directories with underscore except of _posts and _drafts
    // ->notPath('/^_.*/')
    ->path('_posts')
    ->notPath('assets');

  $force_sync = drush_get_option('force');

  foreach ($finder as $file) {
    // Check if we have this file in the map.
    $file_path = $file->getRelativePathname();
    $hash = trim($git->run(array('hash-object ' . $file_path)));
    if (!empty($map[$file_path]) && !$force_sync) {
      // Check if the hash changed.
      if ($hash == $map[$file_path]['hash']) {
        // File didn't change.
        continue;
      }
    }

    $nid = !empty($map[$file_path]['nid']) ? $map[$file_path]['nid'] : NULL;

    // Create node.
    $node = drush_jekyll_file_to_node($file_path, $nid);
    $map[$file_path] = array(
      'hash' => $hash,
      'nid' => $node->nid,
    );
  }

  // Write the map to file.
  $dumper = new Dumper();
  file_put_contents($path . '/_dekyll.yml', $dumper->dump($map, 2));

  if ($git->hasChanges()) {
    $git->add('./_dekyll.yml')
      ->commit('Update _dekyll.yml file')
      ->push();
  }

  return TRUE;
}

/**
 * Create a node or update existing from file.
 *
 * @todo: Do actually field mapping.
 *
 * @param $file_path
 *   The file full path.
 */
function drush_jekyll_file_to_node($file_path, $nid = NULL) {
  $path = variable_get('file_public_path', conf_path() . '/files') . '/git';
  $contents = file_get_contents($path . '/' . $file_path);

  // Get the values from the YAML front header.
  // @todo: Isn't there a line break?
  $split_contents = explode('---', $contents, 3);

  $text = $split_contents[2];

  // @todo: Be smarter about the type.
  $bundle = 'post';

  if ($nid) {
    $wrapper = entity_metadata_wrapper('node', $nid);
  }
  else {
    $node = (object) array(
      'type' => $bundle,
      // @todo: Set to another user.
      'uid' => 1,
    );

    node_object_prepare($node);
    $wrapper = entity_metadata_wrapper('node', $node);
  }

  //
  $yaml = new Parser();
  $yaml_parsed = $yaml->parse($split_contents[1]);

  if (!empty($yaml_parsed['title'])) {
    $title = $yaml_parsed['title'];
  }
  elseif (!empty($yaml_parsed['tagline'])) {
    // No title found, try the tagline.
    $title = $yaml_parsed['tagline'];
  }
  else {
    // Fallback to the file path.
    $title = $file_path;
  }

  $wrapper->title->set($title);

  // Add the text to the body field.
  $wrapper->body->set(array('value' => $text, 'format' => 'full_html'));
  $wrapper->field_full_path->set($file_path);


  // Set the layout.
  $layout = !empty($yaml_parsed['layout']) ? $yaml_parsed['layout'] : 'default';

  // Get the term of the layout, or add a new one if doesn't exist.
  // @todo: Make more generic.
  // Check if the vocabulary exists, or create a new one.
  $vocab_name = 'layout';

  $field_name = 'field_' . $vocab_name;

  if (!$vocabulary = taxonomy_vocabulary_machine_name_load($vocab_name)) {
    // Create a vocabulary.
    $vocabulary = new stdClass();
    $vocabulary->name = $vocab_name;
    $vocabulary->machine_name = trim(drupal_strtolower(str_replace(' ', '_', $vocab_name)));
    taxonomy_vocabulary_save($vocabulary);

    // Attach a taxonomy term field to the bundle.
    if (!field_info_field($field_name)) {
      $field = array(
        'field_name' => $field_name,
        'type' => 'taxonomy_term_reference',
        // @todo: Be smarter about cardinality.
        'cardinality' => 1,
        'settings' => array(
          'allowed_values' => array(
            array(
              'vocabulary' => $vocab_name,
              'parent' => 0,
            ),
          ),
        ),
      );
      field_create_field($field);
    }

    if (!field_info_instance('node', $field_name, $bundle)) {
      $instance = array(
        'field_name' => $field_name,
        'entity_type' => 'node',
        'label' => $layout,
        'bundle' => $bundle,
        'widget' => array(
          'type' => 'select',
          'weight' => -4,
        ),
        'display' => array(
          'default' => array(
            'type' => 'taxonomy_term_reference_link',
            'weight' => 10,
          ),
          'teaser' => array(
            'type' => 'taxonomy_term_reference_link',
            'weight' => 10,
          ),
        ),
      );
      field_create_instance($instance);
    }
  }

  if (!$term = taxonomy_get_term_by_name($layout, $vocab_name)) {
    $values = array(
      'name' => $layout,
      'vid' => $vocabulary->vid,
    );
    $term = entity_create('taxonomy_term', $values);
    taxonomy_term_save($term);
  }

  $wrapper->{$field_name}->set($term);
  $wrapper->save();

  return $wrapper->value();
}

/**
 * Clone a Git repository.
 */
function drush_dekyll_parser_dekyll_clone() {
  if (!$repository = variable_get('dekyll_git_repository')) {
    throw new Exception('Git repository does not exist.');
  }

  $wrapper = new GitWrapper();
  $path = variable_get('file_public_path', conf_path() . '/files') . '/git';
  $git = $wrapper->workingCopy($path);
  if (!$git->isCloned() && !$git->clone($repository)) {
    throw new Exception(format_string('Git repository @repo could not be cloned.', array('@repo' => $repository)));
  }
}
