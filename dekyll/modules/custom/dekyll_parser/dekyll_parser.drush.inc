<?php

/**
 * @file
 * Drush integration for the Dekyll Parser module.
 */

require DRUPAL_ROOT . '/profiles/dekyll/libraries/vendor/autoload.php';

use GitWrapper\GitWrapper;
use GitWrapper\GitException;
use Symfony\Component\Finder\Finder;
use Symfony\Component\Yaml\Parser;
use Symfony\Component\Yaml\Dumper;

/**
 * Implements hook_drush_command().
 */
function dekyll_parser_drush_command() {
  $items['dekyll-write-file'] = array(
    'description' => dt('Write an item form the queue into the Jekyll file.'),
    'options' => array(
      // @todo: Allow passing multiple IDs?
      'id' => dt('The node ID to write to Jekyll file.'),
      'branch' => dt('The git branch to write the file to.'),
    ),
  );

  $items['dekyll-sync'] = array(
    'description' => dt('Sync Drupal with Jekyll.'),
    'options' => array(
      'force' => dt('Optional. Force re-sync even if the file did not change.'),
      // @todo: Allow passing multiple IDs?
      'id' => dt('The node ID of the repository, to sync'),
    ),
  );

  $items['dekyll-config-update'] = array(
    'description' => dt('Sync Jekyll configuration file (_dekyll.yml) with the Drupal configuration.'),
    'options' => array(
      // @todo: Allow passing multiple IDs?
      'id' => dt('The node ID of the repository, to sync'),
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
  if ($nid = drush_get_option('id')) {
    $branch = drush_get_option('branch', 'master');
  }
  else {
    // Get ID from queue.
    $queue = DrupalQueue::get('dekyll_write');
    if (!$queue_item = $queue->claimItem()) {
      return;
    }

    $nid = $queue_item->data['nid'];
    $branch = $queue_item->data['branch'];
  }

  $wrapper = entity_metadata_wrapper('node', $nid);
  $gid = $wrapper->{OG_AUDIENCE_FIELD}->value(array('identifier' => TRUE));

  // "git pull" before writing the file.
  $path = dekyll_og_get_repo_path($gid);

  $git_wrapper = new GitWrapper();
  $git = $git_wrapper ->workingCopy($path);

  // Checkout to the needed branch.
  $git->checkout($branch);

  // Get the HEAD version.
  $hash = $git->run(array('rev-parse --verify HEAD'));
  drush_log(dt('Latest commit before pull is @hash', array('@hash' => $hash)));

  $git->pull();
  drush_log(dt('git pull'));

  if ($hash != $git->run(array('rev-parse --verify HEAD'))) {
    // The latest commit hash changed, so we need to re-sync.
    // @todo: Don't overwrite conflicting nodes, just mark them as conflicts.
    // maybe drush_dekyll_parser_dekyll_sync()?
    // We can't write the file, as we have sync problems.
    return;
  }

  // Write the file.
  $file_path = $wrapper->field_full_path->value();
  $full_path = $path . '/' . $file_path;
  $contents = file_get_contents($full_path);

  // Get the values from the YAML front header.
  $split_contents = explode(YAML_SEPARATOR, $contents, 3);
  $yaml_contents = $split_contents[1];

  // Get the text from the node.
  $text = $wrapper->body->value() ? $wrapper->body->value->value() : '';

  $yaml = new Parser();
  $yaml_parsed = $yaml->parse($yaml_contents);

  // Content sync, allow each plugin to change the node.
  foreach (dekyll_parser_get_content_syncs() as $plugin) {
    $class = ctools_plugin_load_class('dekyll_parser', 'content_sync', $plugin['name'], 'class');
    $config_sync = new $class($plugin, $gid, $file_path);

    // YAML and text are passed by reference.
    $config_sync->export($wrapper, $yaml_parsed, $text);
  }

  $dumper = new Dumper();

  $dump = array(
    YAML_SEPARATOR,
    // Dump the YAML expanded, and not inline.
    $dumper->dump($yaml_parsed, 10) . "\n",
    YAML_SEPARATOR,
    $text,
  );

  file_put_contents($full_path, implode('', $dump));

  // Commit and push.
  // @todo: Move to a single function.
  try {
    $git
      ->commit('Changes of file.')
      ->push();
  }
  catch (GitException $e){

  }

}


/**
 * Sync Drupal with Jekyll.
 *
 * Iterate over all the files and create or update a node to link to them.
 * We use .dekyll/node_map.yml file to hold each file, its hash and the related
 * node ID.
 */
function drush_dekyll_parser_dekyll_sync() {
  if (!$gid = drush_get_option('id')) {
    return;
  }

  // Config sync.
  foreach (dekyll_parser_get_config_syncs() as $plugin) {
    $class = ctools_plugin_load_class('dekyll_parser', 'config_sync', $plugin['name'], 'class');
    $config_sync = new $class($plugin, $gid);
    $config_sync->import();
  }

  // Content sync.
  $path = dekyll_og_get_repo_path($gid);

  $git_wrapper = new GitWrapper();
  $git = $git_wrapper->workingCopy($path);

  // Map file path to the node ID.
  $map = drush_dekyll_parser_get_yml($gid, '.dekyll/node_map.yml');

  // Iterate over all the valid files, and compare them to the map value.
  $finder = new Finder();
  $finder
    ->files()
    ->in($path)
    ->notPath('/^_(?!posts|drafts)/')
    ->notPath('assets');

  $force_sync = drush_get_option('force');

  // Get the current installation global unique identifier.
  $dekyll_guid = variable_get('dekyll_guid');

  foreach ($finder as $file) {
    // Check if we have this file in the map.
    $file_path = $file->getRelativePathname();
    $hash = trim($git->run(array('hash-object ' . $file_path)));
    if (!empty($map[$file_path][$dekyll_guid]) && !$force_sync) {
      // Check if the hash changed.
      if ($hash == $map[$file_path][$dekyll_guid]['hash']) {
        // File didn't change.
        continue;
      }
    }

    $nid = !empty($map[$file_path][$dekyll_guid]['nid']) ? $map[$file_path][$dekyll_guid]['nid'] : NULL;

    // Create node.
    if ($node = drush_jekyll_file_to_node($file_path, $gid, $nid)) {
      $map[$file_path][$dekyll_guid] = array(
        'hash' => $hash,
        'nid' => $node->nid,
      );
    }
  }

  drush_dekyll_parser_save_yml($gid, '.dekyll/node_map.yml', $map);
}

/**
 * Sync Jekyll configuration file (_dekyll.yml) with the Drupal configuration.
 */
function drush_dekyll_parser_dekyll_config_update() {
  if (!$gid = drush_get_option('id')) {
    return;
  }

  // Get the YAML file.
  $config = $original_config = drush_dekyll_parser_get_yml($gid, '_dekyll.yml');

  // Config sync.
  foreach (dekyll_parser_get_config_syncs() as $plugin) {
    $class = ctools_plugin_load_class('dekyll_parser', 'config_sync', $plugin['name'], 'class');
    $config_sync = new $class($plugin, $gid);
    $config_sync->export($config);
  }

  if ($config == $original_config) {
    // Config didn't change.
    return;
  }

  // Write the YAML file.
  drush_dekyll_parser_save_yml($gid, '_dekyll.yml', $config);
}

/**
 * @param $gid
 *   The group ID.
 * @param $file_name
 *   The file name.
 *
 * @return array
 *   The parsed YAML, or empty YAML if file doesn't exist.
 */
function drush_dekyll_parser_get_yml($gid, $file_name) {
  $path = dekyll_og_get_repo_path($gid);

  $file_path = $path . '/' . $file_name;
  if (!file_exists($file_path)) {
    return array();
  }

  // Open the YAML file .
  $contents = file_get_contents($file_path);

  // Parse the contents..
  $yaml = new Parser();
  return $yaml->parse($contents);
}

/**
 * @param $gid
 *   The group ID.
 * @param $file_name
 *   The file name.
 * @param $data
 *   The array to dump into the file.
 */
function drush_dekyll_parser_save_yml($gid, $file_name, $data = array(), $commit_message = '') {
  // Content sync.
  $path = dekyll_og_get_repo_path($gid);

  $git_wrapper = new GitWrapper();
  $git = $git_wrapper->workingCopy($path);

  $file_path = $path . '/' . $file_name;

  if (!file_exists($file_path)) {
    $path_parts = pathinfo($file_path);
    drupal_mkdir($path_parts['dirname']);
  }

  $dumper = new Dumper();
  file_put_contents($file_path, $dumper->dump($data, 10));

  $commit_message = $commit_message ? $commit_message : 'Commit ' . $file_name;

  // Push to git.
  try {
    $git
      ->commit($file_name, array('m' => $commit_message));
  }
  catch(GitException $e) {
    // File didn't change, we can return;
    return;
  }

  try {
    $git->push();
  }
  catch(GitException $e) {
    // @todo: We couldn't push. Handle this (need to pull, not internet
    // connection, etc'.)
  }
}

/**
 * Create a node or update existing from file.
 *
 * @todo: Do actually field mapping.
 *
 * @param $file_path
 *   The file full path.
 */
function drush_jekyll_file_to_node($file_path, $gid, $nid = NULL) {
  $path = dekyll_og_get_repo_path($gid);
  $contents = file_get_contents($path . '/' . $file_path);

  if (strpos($contents, '---') !== 0) {
    // File doesn't have YAML front matter.
    return;
  }

  // Get the values from the YAML front matter.
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

  $yaml = new Parser();
  $yaml_parsed = $yaml->parse($split_contents[1]);

  // Content sync, allow each plugin to change the node.
  foreach (dekyll_parser_get_content_syncs() as $plugin) {
    $class = ctools_plugin_load_class('dekyll_parser', 'content_sync', $plugin['name'], 'class');
    $config_sync = new $class($plugin, $gid, $file_path);
    $config_sync->import($wrapper, $yaml_parsed, $text);
  }


  // Add the text to the body field.
  // @todo: Is it worth a plugin?
  $wrapper->body->set(array('value' => $text, 'format' => 'dekyll_plain'));
  $wrapper->field_full_path->set($file_path);

  // Set the group.
  $wrapper->{OG_AUDIENCE_FIELD}->set($gid);
  $wrapper->save();

  return $wrapper->value();
}

/**
 * Clone a Git repository.
 */
function drush_dekyll_parser_dekyll_clone() {
  $queue = DrupalQueue::get('dekyll_clone');
  if (!$queue_item = $queue->claimItem()) {
    return;
  }

  $gid = $queue_item->data['gid'];
  $wrapper = entity_metadata_wrapper('node', $gid);
  $credentials = $wrapper->field_repo_credentials->value();
  $path = dekyll_og_get_repo_path($gid);

  $git_wrapper = new GitWrapper();
  $git = $git_wrapper->workingCopy($path);

  if (!$git->isCloned() && !$git->clone($credentials)) {
    throw new Exception(format_string('Git repository could not be cloned with @credentials.', array('@credentials' => $credentials)));
  }

  // Save clone status.
  $wrapper->field_repo_cloned->set(TRUE);

  // @todo: Can we check write access?
  $wrapper->field_repo_write_access->set(TRUE);
  $wrapper->save();
}
