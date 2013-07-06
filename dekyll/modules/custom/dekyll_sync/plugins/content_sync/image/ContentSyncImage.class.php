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

    foreach ($this->syncMap[$plugin_name] as $field_name) {
      $field = field_info_field($field_name);
      $instance = field_info_instance($wrapper->type(), $field_name, $wrapper->getBundle());

      $absolute_path = $instance['settings']['content_sync']['settings']['absolute_path'];
      $target_directory = $instance['settings']['content_sync']['settings']['target_directory'];

      if ($absolute_path) {
        // Absolute path.
        $image_directory = $target_directory;
      }
      else {
        // Relative path.
        $file_path = $this->filePath;
        $path_parts = pathinfo($file_path);
        $image_directory = $path_parts['dirname'] . '/' . $target_directory;
      }

      $full_image_directory = dekyll_repository_get_repo_path($this->branchId). '/' . $image_directory;

      $file_names = array();

      if (!file_exists($full_image_directory)) {
        drupal_mkdir($full_image_directory, 0644, TRUE);
      }

      if (!$files = $wrapper->{$field_name}->value()) {
        // No files.
        continue;
      }

      // We can't use is_array() as even the single result is an array.
      $files = empty($files['fid']) ? $files : array($files);

      foreach ($files as $file) {
        file_unmanaged_copy($file['uri'], $full_image_directory . '/' . $file['filename'], FILE_EXISTS_REPLACE);
        // Add the new file names.
        $file_name = $image_directory . '/' . $file['filename'];
        $file_names[] = $file_name;

        // We expect that {{ BASE_PATH }} will prefix the file name
        // the Jekyll file.
        $jekyll_name = $this->getJekyllName($instance, $field_name);
        if ($field['cardinality'] == 1) {
          $yaml[$jekyll_name] = '/' . $file_name;
        }
        else {
          $yaml[$jekyll_name][] = '/' . $file_name;
        }

      }
    }
    return $file_names;
  }

  /**
   * Settings form.
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    $settings = !empty($instance['settings']['content_sync']['settings']) ? $instance['settings']['content_sync']['settings'] : array();
    $settings += array(
      'absolute_path' => FALSE,
      'target_directory' => 'images',
    );

    $form['absolute_path'] = array(
      '#type' => 'checkbox',
      '#title' => t('Absolute path'),
      '#description' => t('Determine if the "Target directory" is relative to the page\'s file path, or absolute.'),
      '#default_value' => $settings['absolute_path'],
    );

    $form['target_directory'] = array(
      '#type' => 'textfield',
      '#title' => t('Target directory'),
      // @todo: Auto trim leading slash.
      '#description' => t('Determine the directory where the images should be sent. Should not include leading slash.'),
      '#default_value' => $settings['target_directory'],
    );
    return $form;
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
