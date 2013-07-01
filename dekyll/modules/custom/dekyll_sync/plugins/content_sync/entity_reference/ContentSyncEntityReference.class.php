<?php

/**
 * Entity reference content sync.
 */
class ContentSyncEntityReference extends ContentSyncBase {

  /**
   * Return the target type and target bundles.
   */
  public function getRefernceTargets($field, $instance) {
    return array(
      'target_type' => $field['settings']['target_type'],
      'target_bundles' => $field['settings']['handler_settings']['target_bundles'],
    );
  }

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

      $targets = $this->getRefernceTargets($field, $instance);
      $target_type = $targets['target_type'];
      $target_bundles = $targets['target_bundles'];

      $entity_info = entity_get_info($target_type);

      foreach ($yaml[$field_name] as $entity_id => $values) {

        // Check if the entity already exists by the unique field, or create
        // a new one.

        list(,, $id) = explode(':', $entity_id);

        $query = new EntityFieldQuery();
        $result = $query
          ->entityCondition('entity_type', $target_type)
          ->entityCondition('entity_id', $id)
          ->range(0, 1)
          ->execute();

        if (!empty($result[$target_type])) {
          $entity = entity_load_single($target_type, key($result[$target_type]));
        }
        else {
          if (!$target_bundles) {
            $target_bundle = reset($target_bundles);
          }
          else {
            // @todo: Check if entity info has bundles.
            $target_bundle = reset(array_keys($entity_info['bundles']));
          }

          $entity = entity_create($target_type, array($entity_info['bundle keys']['bundle'] => $target_bundle));
        }

        $target_wrapper = entity_metadata_wrapper($target_type, $entity);

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
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    $plugin_name = $this->plugin['name'];
    $bundle = $wrapper->getBundle();
    foreach ($this->syncMap[$plugin_name] as $field_name) {
      if (!$entity_ids = $wrapper->{$field_name}->value(array('identifier' => TRUE))) {
        // Entity reference field is empty.
        continue;
      }

      $entity_ids = is_array($entity_ids) ? $entity_ids : array($entity_ids);

      $field = field_info_field($field_name);
      $instance = field_info_instance($wrapper->type(), $field_name, $bundle);

      $targets = $this->getRefernceTargets($field, $instance);
      $target_type = $targets['target_type'];

      if (!empty($instance['settings']['content_sync']['settings']['create_page'])) {
        // The referenced entity is a page of its own, so write all the IDs of
        // the referenced entities.
        foreach ($entity_ids as $entity_id) {
          $yaml[$field_name][] = $target_type . ':' . $entity_id;
        }
        continue;
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
      'unique' => array(),
    );

    $settings['unique'] += array(
      'type' => 'property',
      'name' => '',
    );

    $targets = $this->getRefernceTargets($field, $instance);
    $target_type = $targets['target_type'];
    $target_bundles = $targets['target_bundles'];

    $entity_info = entity_get_info($target_type);

    $form['unique']['type'] = array(
      '#type' => 'select',
      '#title' => t('Unique key'),
      '#options' => array(
        'property' => t('A property of the base table of the entity'),
        'field' => t('A field attached to this entity'),
      ),
      '#ajax' => TRUE,
      '#limit_validation_errors' => array(),
      '#default_value' => $settings['unique']['type'],
    );

    $form['unique']['settings'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('entityreference-settings')),
      '#process' => array('_entityreference_form_process_merge_parent'),
    );

    if ($settings['unique']['type'] == 'property') {
      $form['unique']['settings']['name'] = array(
        '#type' => 'select',
        '#title' => t('Property'),
        '#description' => t('Select the property that holds the unique key to recognize the entity by.'),
        '#required' => TRUE,
        '#options' => drupal_map_assoc($entity_info['schema_fields_sql']['base table']),
        '#default_value' => $settings['unique']['name'],
      );
    }
    elseif ($settings['unique']['type'] == 'field') {
      $options = array();

      // The field types that can be used for unique ID.
      $vaild_types = array(
        'text',
        'number_integer',
      );

      // Get all the fields attached to the referenced instance(s).
      if (!$bundles = $target_bundles ? $target_bundles : array_keys($entity_info['bundles'])) {
        // The bundle is the entity name (e.g. like the user entity).
        $bundles = array($target_type);
      }

      foreach ($bundles as $bundle) {
        foreach (field_info_instances($target_type, $bundle) as $field_name => $instance) {
          $field = field_info_field($field_name);
          if (in_array($field['type'], $vaild_types)) {
            $options[$field_name] = $instance['label'];
          }
        }
      }

      if ($options) {
        $form['unique']['settings']['name'] = array(
          '#type' => 'select',
          '#title' => t('Field name'),
          '#description' => t('Select the field that holds the unique key to recognize the entity by.'),
          '#options' => $options,
          '#required' => TRUE,
          '#default_value' => $settings['unique']['settings']['name'],
        );
      }
      else {
        $form['unique']['settings']['name'] = array('#markup' => t('No valid field (text or number) attach to the entity type.'));
      }
    }

    $form['create_page'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create page'),
      '#description' => t('When enabled, a page will be created from the referenced entity.'),
      '#default_value' => $settings['create_page'],
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
