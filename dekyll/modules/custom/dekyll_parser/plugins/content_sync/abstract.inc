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
   */
  public function __construct($plugin, $sync_map = array(), $gid = NULL, $file_path = NULL, GitWorkingCopy $git = NULL);

  /**
   * Import content from Jekyll file.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '');

  /**
   * Export content to Jekyll file.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '');

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
   * @see dekyll_parser_get_content_sync_map()
   */
  protected $syncMap = array();

  /**
   * The group ID.
   */
  protected $gid;

  /**
   * The Jekyll file's path.
   */
  protected $filePath;

  /**
   * The GitWrapper\GitWorkingCopy object.
   */
  protected $git;

  public function __construct($plugin, $sync_map = array(), $gid = NULL, $file_path = NULL, GitWorkingCopy $git = NULL) {
    $this->plugin = $plugin;

    $this->syncMap = $sync_map;
    $this->syncMap += array($plugin['name'] => array());

    $this->gid = $gid;
    $this->filePath = $file_path;
    $this->git = $git;
  }

  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {}

  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '') {}

  public function access($op, $field = NULL, $instance = NULL) {
  }

  public function settingsForm($field, $instance) {}
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

