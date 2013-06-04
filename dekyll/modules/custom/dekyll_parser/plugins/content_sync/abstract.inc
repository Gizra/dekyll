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
  public function __construct($plugin, $gid, $file_path, GitWorkingCopy $git);

  /**
   * Import content from Jekyll file.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '');

  /**
   * Export content to Jekyll file.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '');
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

  public function __construct($plugin, $gid, $file_path, GitWorkingCopy $git) {
    $this->plugin = $plugin;
    $this->gid = $gid;
    $this->filePath = $file_path;
    $this->git = $git;
  }

  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {}

  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '') {}
}
