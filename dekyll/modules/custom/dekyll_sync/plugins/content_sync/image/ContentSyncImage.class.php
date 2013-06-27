<?php

/**
 * Image field content sync.
 */
class ContentSyncImage extends ContentSyncBase {

  /**
   * Import YAML into OG-vocabs.
   *
   * @todo: We assume that sync has updated all OG-vocabs by the data in the
   * Jekyll file, and deleted any OG-vocab that doesn't match (i.e. complex
   * data).
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {}

  /**
   * Export images.
   *
   * We copy the images to the image folder under the node's path.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    $plugin_name = $this->plugin['name'];

    $dirname = '';
    if ($file_path = $this->filePath) {
      $path_parts = pathinfo($file_path);
      $dirname = $path_parts['dirname'];
    }

    $image_path = $dirname . './images';

    $full_path = dekyll_repository_get_repo_path($this->branchId). '/' . $file_path;

    $path_parts = pathinfo($full_path);
    $image_full_path = $path_parts['dirname'] . '/images';

    $file_names = array();

    if (!file_exists($image_full_path)) {
      drupal_mkdir($image_full_path, NULL, TRUE);
    }

    foreach ($this->syncMap[$plugin_name] as $field_name) {
      if (!$files = $wrapper->{$field_name}->value()) {
        // No files.
        continue;
      }

      // We can't use is_array() as even the single result is an array.
      $files = empty($files['fid']) ? $files : array($files);

      foreach ($files as $file) {
        file_unmanaged_copy($file['uri'], $image_full_path . '/' . $file['filename'], FILE_EXISTS_REPLACE);
        // Add the new file names.
        $file_name = $image_path . '/' . $file['filename'];
        $file_names[] = $file_name;

        // We expect that {{ BASE_PATH }} will prefix the file name
        // the Jekyll file.
        $yaml[$field_name][] = '/' . $file_name;
      }
    }
    return $file_names;
  }

  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      return $field['type'] == 'image';
    }

    return parent::access($op, $field, $instance);
  }
}
