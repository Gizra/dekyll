<?php

/**
 * Entity reference content sync.
 */
class ContentSyncEntityReference extends ContentSyncBase {

  /**
   * Import entity reference field.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {
    $plugin_name = $this->plugin['name'];
    $bundle = $wrapper->getBundle();

    $target_items = array();

    foreach ($this->syncMap[$plugin_name] as $field_name) {
      if (empty($yaml[$field_name])) {
        // Entity reference field is empty.
        continue;
      }

      $field = field_info_field($field_name);
      $instance = field_info_instance($wrapper->type(), $field_name, $bundle);

      $target_type = $field['settings']['target_type'];
      $unique_field = $instance['settings']['content_sync']['settings']['unique_field'];

      $entity_info = entity_get_info($target_type);

      foreach ($yaml[$field_name] as $unique_field_value => $values) {

        // Check if the entity already exists by the unique field, or create
        // a new one.
        $query = new EntityFieldQuery();
        $result = $query
          ->entityCondition('entity_type', $target_type)
          // @todo: Remove the "column" hardcoding?
          ->fieldCondition($unique_field, 'value', $unique_field_value, '=')
          ->range(0, 1)
          ->execute();

        if (!empty($result[$target_type])) {
          $entity = entity_load_single($target_type, key($result[$target_type]));
        }
        else {
          // @todo: Remove hardcoding.
          $target_bundle = 'repository';
          $entity = entity_create($target_type, array($entity_info['bundle keys']['bundle'] => $target_bundle));
        }

        $target_wrapper = entity_metadata_wrapper($target_type, $entity);

        // Add the unique field to the values, so it will be set.
        $values[$unique_field] = $unique_field_value;

        foreach ($values as $target_field_name => $target_value) {
          if (!isset($target_wrapper->{$target_field_name})) {
            // Field doesn't exist.
            continue;
          }
          $target_wrapper->{$target_field_name}->set($target_value);
        }

        $target_wrapper->save();
        $target_items[] = $entity;
      }
    }

    $wrapper->{$field_name}->set($target_items);
  }

  /**
   * Export entity reference field.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '') {
    $plugin_name = $this->plugin['name'];
    $bundle = $wrapper->getBundle();
    foreach ($this->syncMap[$plugin_name] as $field_name) {
      if (!$entities = $wrapper->{$field_name}->value()) {
        // Entity reference field is empty.
        continue;
      }

      $field = field_info_field($field_name);
      $instance = field_info_instance($wrapper->type(), $field_name, $bundle);

      $view_mode = $instance['settings']['content_sync']['settings']['view_mode'];
      $unique_field = $instance['settings']['content_sync']['settings']['unique_field'];

      $target_type = $field['settings']['target_type'];

      // Get all the fields that are visible in the view mode, and output them.
      $entities = is_array($entities) ? $entities : array($entities);

      foreach ($entities as $entity) {
        $target_wrapper = entity_metadata_wrapper($target_type, $entity);
        $target_bundle = $target_wrapper->getBundle();

        $delta = $target_wrapper->{$unique_field}->raw();

        foreach (field_info_instances($target_type, $target_bundle) as $target_field_name => $target_instance) {
          if ($target_field_name == $unique_field) {
            // We already use the unique property as the key of the array.
            continue;
          }

          if (empty($target_instance['display'][$view_mode])) {
            // View mode doesn't exist.
            continue;
          }

          if ($target_instance['display'][$view_mode]['type'] == 'hidden') {
            // Field is hidden, so it should not be exported.
            continue;
          }

          if (!$value = $target_wrapper->$target_field_name->raw()) {
            // Target entity doesn't have value.
            // @todo: Should we still export it?
            continue;
          }

          $yaml[$field_name][$delta][$target_field_name] = $value;
        }
      }
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
}
