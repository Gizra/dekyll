<?php

use GitWrapper\GitWorkingCopy;

/**
 * OG-vocab config sync.
 */
class ConfigSyncOgVocab extends ConfigSyncBase {

  /**
   * Import OG-vocabs.
   */
  public function import() {
    if (empty($this->config['content_types']['taxonomy'])) {
      return;
    }

    foreach ($this->config['content_types']['taxonomy'] as $bundle => $configs) {
      foreach ($configs as $vocab_name => $config) {
        $branch_id = $this->branchId;

        $vocab_name = trim($vocab_name);

        // Check if vocabulary already exists.
        // Suffix the machine name with the group ID.
        $machine_name = trim(drupal_strtolower(str_replace(' ', '_', $vocab_name))) . '_' . $branch_id;
        if (!$vocabulary = taxonomy_vocabulary_machine_name_load($machine_name)) {
          // Create a vocabulary.
          $vocabulary = new stdClass();
          $vocabulary->name = trim($vocab_name);
          $vocabulary->machine_name = $machine_name;
          taxonomy_vocabulary_save($vocabulary);

          // Associate with the group.
          og_vocab_relation_save($vocabulary->vid, 'node', $branch_id);
        }

        // Create an OG-vocab, or load an existing one.
        $og_vocab = og_vocab_load_og_vocab($vocabulary->vid, 'node', $bundle, NULL, TRUE);

        // @todo: Add validation for user's data.
        switch ($config['widget']) {
          case 'autocomplete':
            $widget_type = 'entityreference_autocomplete';
            break;

          case 'tags':
            $widget_type = 'entityreference_autocomplete_tags';
            break;

          case 'select':
          default:
            $widget_type = 'options_select';
        }

        $og_vocab->settings = array(
          'required' => $config['required'],
          'cardinality' => $config['cardinality'],
          'widget_type' => $widget_type,
        );

        $og_vocab->save();
      }
    }
  }

  /**
   * Export OG-vocabs.
   */
  public function export(&$config) {
    $branch_id = $this->branchId;

    // Get all Vocabularies and OG-vocabs of the group.
    if (!$relations = og_vocab_relation_get_by_group('node', $branch_id)) {
      return;
    }

    $vids = array();
    foreach ($relations as $relation) {
      $vids[] = $relation->vid;
    }

    $vocabularies = taxonomy_vocabulary_load_multiple($vids);

    $query = new EntityFieldQuery();
    $result = $query->entityCondition('entity_type', 'og_vocab')
      ->propertyCondition('vid', $vids, 'IN')
      ->execute();

    $og_vocabs = entity_load('og_vocab', array_keys($result['og_vocab']));

    foreach ($og_vocabs as $og_vocab) {
      $vocabulary = $vocabularies[$og_vocab->vid];
      $settings = $og_vocab->settings;

      switch ($settings['widget_type']) {
        case 'entityreference_autocomplete':
          $widget_type = 'autocomplete';
          break;

        case 'entityreference_autocomplete_tags':
          $widget_type = 'tags';
          break;

        case 'options_select':
        default:
          $widget_type = 'select';
      }

      $config['content_types']['taxonomy'][$og_vocab->bundle][$vocabulary->name] = array(
        'required' => $settings['required'],
        'cardinality' => $settings['cardinality'],
        'widget' => $widget_type,
      );
    }
  }
}
