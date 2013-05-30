<?php

/**
 * OG-vocab config sync.
 */
class ConfigSyncOgVocab extends ConfigSyncBase {

  public function import() {
    if (empty($this->config['content_types'])) {
      return;
    }

    foreach ($this->config['content_types'] as $bundle => $configs) {
      foreach ($configs as $config) {
        if ($config['type'] != 'taxonomy') {
          continue;
        }

        $gid = $this->gid;

        // Check if vocabulary already exists.
        // Suffix the machine name with the group ID.
        $machine_name = trim(drupal_strtolower(str_replace(' ', '_', $config['name']))) . '_' . $gid;
        if (!$vocabulary = taxonomy_vocabulary_machine_name_load($machine_name)) {
          // Create a vocabulary.
          $vocabulary = new stdClass();
          $vocabulary->name = trim($config['name']);
          $vocabulary->machine_name = $machine_name;
          taxonomy_vocabulary_save($vocabulary);

          // Associate with the group.
          og_vocab_relation_save($vocabulary->vid, 'node', $gid);
        }

        // Create an OG-vocab, or load an existing one.
        $og_vocab = og_vocab_load_og_vocab($vocabulary->vid, 'node', $bundle, OG_VOCAB_FIELD, TRUE);

        // @todo: Add validation for user's data.
        switch ($config['config']['widget']) {
          case 'autocomplete':
            $widget_type = 'autocomplete';
            break;

          case 'tags':
            $widget_type = 'autocomplete_tags';
            break;

          case 'select':
          default:
            $widget_type = 'options_select';
        }

        $og_vocab->settings = array(
          'required' => $config['config']['required'],
          'cardinality' => $config['config']['cardinality'],
          'widget_type' => $widget_type,
        );

        $og_vocab->save();
      }

    }
  }

}
