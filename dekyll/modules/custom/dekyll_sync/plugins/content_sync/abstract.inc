<?php

require DRUPAL_ROOT . '/profiles/dekyll/libraries/vendor/autoload.php';

use GitWrapper\GitWrapper;
use GitWrapper\GitWorkingCopy;
use Symfony\Component\Yaml\Parser;
use Symfony\Component\Yaml\Dumper;

/**
 * Dekyll Content Sync interface
 */
interface ContentSyncInterface {

  /**
   * Constructor for the Content sync class.
   * @param $plugin
   * @param array $sync_map
   * @param null $branch_id
   * @param null $file_path
   */
  public function __construct($plugin, $sync_map = array(), $branch_id = NULL, $file_path = NULL);

  /**
   * Import content from Jekyll file.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '');

  /**
   * Export content to Jekyll file.
   *
   * @return
   *   Array with the files to create.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info);

  /**
   * Determine access.
   *
   * @param $op
   *   The operation to perform. Values:
   *   - "settings": Show on the field UI's instance settings form.
   * @param $field
   *   The field array, if $op is 'settings'.
   * @param $instance
   *   The field array, if $op is 'settings'.
   *
   * @return
   *   TRUE if access is granted.
   */
  public function access($op, $field = NULL, $instance = NULL);

  /**
   * Generate a settings form for this handler.
   */
  public function settingsForm($field, $instance);
}

/**
 * An abstract implementation of ContentSyncInterface.
 */
abstract class ContentSyncBase implements ContentSyncInterface {

  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * Array with content sync mapping.
   *
   * @see dekyll_sync_get_content_sync_map()
   */
  protected $syncMap = array();

  /**
   * The Branch node ID.
   */
  protected $branchId;

  /**
   * The Jekyll file's path.
   */
  protected $filePath;

  public function __construct($plugin, $sync_map = array(), $branch_id = NULL, $file_path = NULL) {
    $this->plugin = $plugin;

    $this->syncMap = $sync_map;
    $this->syncMap += array($plugin['name'] => array());

    $this->branchId = $branch_id;
    $this->filePath = $file_path;
  }

  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {
    return array();
  }

  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    return array();
  }

  public function access($op, $field = NULL, $instance = NULL) {
    if (in_array($op, array('import', 'export'))) {
      $plugin_name = $this->plugin['name'];
      return !empty($this->syncMap[$plugin_name]);
    }
  }

  public function settingsForm($field, $instance) {
    if (!$this->plugin['jekyll name']) {
      return;
    }

    $settings = !empty($instance['settings']['content_sync']['settings']) ? $instance['settings']['content_sync']['settings'] : array();
    $settings += array(
      // Empty, means defaults to the field name.
      'jekyll_name' => '',
    );
    $form['jekyll_name'] = array(
      '#title' => t('Jekyll name'),
      '#description' => t('The name of the property in Jekyll YAML to map the field to.'),
      '#type' => 'textfield',
      '#default_value' => $settings['jekyll_name'],
    );

    return $form;
  }

  /**
   * Helper function to get the Jekyll name from the settings.
   *
   * @param $instance
   * @param $field_name
   */
  public function getJekyllName($instance, $field_name) {
    return !empty($instance['settings']['content_sync']['settings']['jekyll_name']) ? $instance['settings']['content_sync']['settings']['jekyll_name'] : $field_name;
  }
}

/**
 * A broken implementation of ContentSyncBase.
 */
class ContentSyncBaseBroken extends ContentSyncBase {

  public function settingsForm($field, $instance) {
    $form['behavior_handler'] = array(
      '#markup' => t('The selected Content Sync handler is broken.'),
    );
    return $form;
  }
}

