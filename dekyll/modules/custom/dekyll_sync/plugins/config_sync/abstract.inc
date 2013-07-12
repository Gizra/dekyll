<?php

require DRUPAL_ROOT . '/profiles/dekyll/libraries/vendor/autoload.php';

use GitWrapper\GitWrapper;
use GitWrapper\GitWorkingCopy;
use Symfony\Component\Yaml\Parser;
use Symfony\Component\Yaml\Dumper;

/**
 * Dekyll Config Sync interface
 */
interface ConfigSyncInterface {

  /**
   * Constructor for the Config sync.
   */
  public function __construct($plugin, $branch_id, GitWorkingCopy $git);

  /**
   * Import config from _dekyll.yml
   */
  public function import();

  /**
   * Prepare export config to _dekyll.yml
   */
  public function export(&$config);
}

/**
 * An abstract implementation of ConfigSyncInterface.
 */
abstract class ConfigSyncBase implements ConfigSyncInterface {

  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * The branch node ID.
   */
  protected $branchId;

  /**
   * The GitWrapper\GitWorkingCopy object.
   */
  protected $git;

  /**
   * Array with the parsed YAML from _dekyll.yml, or default array() values if
   * file doesn't exist.
   */
  protected $config = array();

  public function __construct($plugin, $branch_id, GitWorkingCopy $git) {
    $this->plugin = $plugin;
    $this->branchId = $branch_id;
    $this->git = $git;

    $path = dekyll_repository_get_repo_path($branch_id) . '/_dekyll.yml';

    if (file_exists($path)) {
      // Open the node_map.yml and get the map.
      $contents = file_get_contents($path);

      // Get the map.
      $yaml = new Parser();
      $this->config = $yaml->parse($contents);
    }

    $this->config = $this->config ? $this->config : $this->getConfigDefault();
  }

  /**
   * Add default config.
   *
   * @return
   *   Array with the default config.
   */
  protected function getConfigDefault() {
    $return = array(
      'content_types' => array(
        'taxonomy' => array(
          'post' => array(
            'tags' => array(
              'required' => FALSE,
              'cardinality' => FIELD_CARDINALITY_UNLIMITED,
              'widget' => 'tags',
            ),
            'category' => array(
              'required' => FALSE,
              'cardinality' => 1,
              'widget' => 'autocomplete',
            ),
          ),
        ),
      ),
    );

    // Add the "layout" vocabulary to each group-content.
    $bundles = og_get_all_group_content_bundle();
    foreach (array_keys($bundles['node']) as $bundle) {
      if ($bundle == 'branch') {
        continue;
      }
      $return['content_types']['taxonomy'][$bundle]['layout'] = array(
        'required' => TRUE,
        'cardinality' => 1,
        'widget' => 'select',
      );
    }

    return $return;
  }

  public function import() {}

  public function export(&$config) {}

  /**
   * Save the config to _dekyll.yml
   */
  public function postExport() {
    $path = dekyll_repository_get_repo_path($this->branchId);

    $git_wrapper = new GitWrapper();
    $git = $git_wrapper ->workingCopy($path);

    $file_path = $path . '/_dekyll.yml';

    $dumper = new Dumper();
    file_put_contents($file_path, $dumper->dump($this->config, 2));

    // Commit and push.
    $git
      ->add($file_path)
      ->commit('Changes of file.')
      ->push();
  }
}
