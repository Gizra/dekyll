<?php
/**
 * @file
 * dekyll_message.features.field.inc
 */

/**
 * Implements hook_field_default_fields().
 */
function dekyll_message_field_default_fields() {
  $fields = array();

  // Exported field: 'message-dekyll_export-field_jekyll_build_status'.
  $fields['message-dekyll_export-field_jekyll_build_status'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_jekyll_build_status',
      'foreign keys' => array(),
      'indexes' => array(
        'value' => array(
          0 => 'value',
        ),
      ),
      'locked' => '0',
      'module' => 'list',
      'settings' => array(
        'allowed_values' => array(
          0 => 'Needs build',
          1 => 'Built',
          2 => 'Build error',
        ),
        'allowed_values_function' => '',
      ),
      'translatable' => '0',
      'type' => 'list_integer',
    ),
    'field_instance' => array(
      'bundle' => 'dekyll_export',
      'default_value' => array(
        0 => array(
          'value' => '0',
        ),
      ),
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'list',
          'settings' => array(),
          'type' => 'list_default',
          'weight' => 2,
        ),
        'message_notify_email_body' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'message_notify_email_subject' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'message',
      'field_name' => 'field_jekyll_build_status',
      'label' => 'Build status',
      'required' => 1,
      'settings' => array(
        'content_sync' => array(
          'plugin_name' => '',
        ),
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'options',
        'settings' => array(),
        'type' => 'options_select',
        'weight' => '3',
      ),
    ),
  );

  // Exported field: 'message-dekyll_export-field_message_page'.
  $fields['message-dekyll_export-field_message_page'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_message_page',
      'foreign keys' => array(
        'node' => array(
          'columns' => array(
            'target_id' => 'nid',
          ),
          'table' => 'node',
        ),
      ),
      'indexes' => array(
        'target_id' => array(
          0 => 'target_id',
        ),
      ),
      'locked' => '0',
      'module' => 'entityreference',
      'settings' => array(
        'handler' => 'base',
        'handler_settings' => array(
          'behaviors' => array(
            'views-select-list' => array(
              'status' => 0,
            ),
          ),
          'sort' => array(
            'type' => 'none',
          ),
          'target_bundles' => array(),
        ),
        'target_type' => 'node',
      ),
      'translatable' => '0',
      'type' => 'entityreference',
    ),
    'field_instance' => array(
      'bundle' => 'dekyll_export',
      'default_value' => NULL,
      'default_value_function' => '',
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'entityreference',
          'settings' => array(
            'link' => FALSE,
          ),
          'type' => 'entityreference_label',
          'weight' => 1,
        ),
        'message_notify_email_body' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'message_notify_email_subject' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'message',
      'field_name' => 'field_message_page',
      'label' => 'Page',
      'required' => 1,
      'settings' => array(
        'behaviors' => array(
          'prepopulate' => array(
            'status' => 0,
          ),
        ),
        'content_sync' => array(
          'plugin_name' => '',
        ),
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'entityreference',
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'path' => '',
          'size' => '60',
        ),
        'type' => 'entityreference_autocomplete',
        'weight' => '2',
      ),
    ),
  );

  // Exported field: 'message-dekyll_export-field_message_repo_branch'.
  $fields['message-dekyll_export-field_message_repo_branch'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_message_repo_branch',
      'foreign keys' => array(
        'node' => array(
          'columns' => array(
            'target_id' => 'nid',
          ),
          'table' => 'node',
        ),
      ),
      'indexes' => array(
        'target_id' => array(
          0 => 'target_id',
        ),
      ),
      'locked' => '0',
      'module' => 'entityreference',
      'settings' => array(
        'handler' => 'base',
        'handler_settings' => array(
          'behaviors' => array(
            'views-select-list' => array(
              'status' => 0,
            ),
          ),
          'sort' => array(
            'type' => 'none',
          ),
          'target_bundles' => array(
            'branch' => 'branch',
          ),
        ),
        'target_type' => 'node',
      ),
      'translatable' => '0',
      'type' => 'entityreference',
    ),
    'field_instance' => array(
      'bundle' => 'dekyll_export',
      'default_value' => NULL,
      'default_value_function' => '',
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'entityreference',
          'settings' => array(
            'link' => FALSE,
          ),
          'type' => 'entityreference_label',
          'weight' => 0,
        ),
        'message_notify_email_body' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'message_notify_email_subject' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'message',
      'field_name' => 'field_message_repo_branch',
      'label' => 'Branch',
      'required' => 1,
      'settings' => array(
        'behaviors' => array(
          'prepopulate' => array(
            'status' => 0,
          ),
        ),
        'content_sync' => array(
          'plugin_name' => '',
        ),
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'entityreference',
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'path' => '',
          'size' => '60',
        ),
        'type' => 'entityreference_autocomplete',
        'weight' => '1',
      ),
    ),
  );

  // Exported field: 'message-dekyll_export-field_rsync_status'.
  $fields['message-dekyll_export-field_rsync_status'] = array(
    'field_config' => array(
      'active' => '1',
      'cardinality' => '1',
      'deleted' => '0',
      'entity_types' => array(),
      'field_name' => 'field_rsync_status',
      'foreign keys' => array(),
      'indexes' => array(
        'value' => array(
          0 => 'value',
        ),
      ),
      'locked' => '0',
      'module' => 'list',
      'settings' => array(
        'allowed_values' => array(
          0 => 'Not synced',
          1 => 'Synced',
          2 => 'Sync error',
        ),
        'allowed_values_function' => '',
      ),
      'translatable' => '0',
      'type' => 'list_integer',
    ),
    'field_instance' => array(
      'bundle' => 'dekyll_export',
      'default_value' => array(
        0 => array(
          'value' => '0',
        ),
      ),
      'deleted' => '0',
      'description' => '',
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'list',
          'settings' => array(),
          'type' => 'list_default',
          'weight' => 3,
        ),
        'message_notify_email_body' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
        'message_notify_email_subject' => array(
          'label' => 'above',
          'settings' => array(),
          'type' => 'hidden',
          'weight' => 0,
        ),
      ),
      'entity_type' => 'message',
      'field_name' => 'field_rsync_status',
      'label' => 'Rsync status',
      'required' => 1,
      'settings' => array(
        'content_sync' => array(
          'plugin_name' => '',
        ),
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'options',
        'settings' => array(),
        'type' => 'options_select',
        'weight' => '4',
      ),
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Branch');
  t('Build status');
  t('Page');
  t('Rsync status');

  return $fields;
}
