<?php

/**
 * Entity reference content sync.
 */
class ContentSyncEntityReference extends ContentSyncBase {

  /**
   * Import entity reference field.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {}

  /**
   * Export entity reference field.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '') {}

  /**
   * Check field is of type entity reference.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      if ($field['type'] != 'entityreference') {
        // Field is not entity reference type.
        return;
      }

      $field_name = $field['field_name'];

      // Check this is not an OG-vocab or OG-audience field.
      if (og_is_group_audience_field($field_name)) {
        return;
      }

      if (og_vocab_is_og_vocab_field($instance['entity_type'], $field_name, $instance['bundle'])) {
        return;
      }

      return TRUE;
    }
    elseif (in_array($op, array('import', 'export'))) {
      $plugin_name = $this->plugin['name'];
      return !empty($this->syncMap[$plugin_name]);
    }
  }

  /**
   * Settings form.
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    $settings = !empty($instance['settings']['content_sync']['settings']) ? $instance['settings']['content_sync']['settings'] : array();
    $settings += array(
      'view_mode' => 'default',
      'unique_field' => FALSE,
    );

    $target_type = $field['settings']['target_type'];
    $target_bundles = $field['settings']['handler_settings']['target_bundles'];

    $entity_info = entity_get_info($target_type);

    $options = array();
    foreach ($entity_info['view modes'] as $key => $value) {
      $options[$key] = $value['label'];
    }

    $form['view_mode'] = array(
      '#type' => 'select',
      '#title' => t('View mode'),
      '#options' => $options,
      '#required' => TRUE,
      '#default_value' => $settings['view_mode'],
    );

    // Get all the fields attached to the referenced instance(s).
    if (!$bundles = $target_bundles ? $target_bundles : array_keys($entity_info['bundles'])) {
      // The bundle is the entity name (e.g. like the user entity).
      $bundles = array($target_type);
    }

    $options = array();

    // The field types that can be used for unique ID.
    $vaild_types = array(
      'text',
      'number_integer',
    );

    foreach ($bundles as $bundle) {
      foreach (field_info_instances($target_type, $bundle) as $field_name => $instance) {
        $field = field_info_field($field_name);
        if (in_array($field['type'], $vaild_types)) {
          $options[$field_name] = $instance['label'];
        }
      }
    }

    $form['unique_field'] = array(
      '#type' => 'select',
      '#title' => t('Unique ID field'),
      '#description' => t('Select the field that holds the unique key to recognize the entity by.'),
      '#options' => $options,
      '#required' => TRUE,
      '#default_value' => $settings['unique_field'],
    );

    return $form;
  }
}
