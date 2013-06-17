<?php

/**
 * Title content sync.
 */
class ContentSyncTitle extends ContentSyncBase {

  /**
   * Import node title.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {
    if (!empty($yaml['title'])) {
      $title = $yaml['title'];
    }
    elseif (!empty($yaml['tagline'])) {
      // No title found, try the tagline.
      $title = $yaml['tagline'];
    }
    else {
      // Fallback to the file path.
      $title = $this->filePath;
    }

    $wrapper->title->set($title);
  }

  /**
   * Export node title.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    $yaml['title'] = $wrapper->label();
  }


  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    return $op != 'settings';
  }


}
