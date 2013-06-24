<?php

/**
 * OG-Vocab content sync.
 */
class ContentSyncOgVocab extends ContentSyncBase {

  /**
   * Import YAML into OG-vocabs.
   *
   * @todo: We assume that sync has updated all OG-vocabs by the data in the
   * Jekyll file, and deleted any OG-vocab that doesn't match (i.e. complex
   * data).
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {
    // Get all Vocabularies of the group.
    if (!$relations = og_vocab_relation_get_by_group('node', $this->branchId)) {
      return;
    }

    $vids = array();
    foreach ($relations as $relation) {
      $vids[] = $relation->vid;
    }

    $vocabularies = taxonomy_vocabulary_load_multiple($vids);

    // The terms to save into the OG-vocab field.
    $terms = array();

    foreach ($vocabularies as $vocabulary) {
      $name = $vocabulary->name;
      if (empty($yaml[$name])) {
        // Property doesn't exist.
        continue;
      }

      $term_names = is_array($yaml[$name]) ? $yaml[$name] : array($yaml[$name]);
      foreach ($term_names as $term_name) {
        $terms[] = $this->getTerm($term_name, $vocabulary);
      }
    }

    $wrapper->{OG_VOCAB_FIELD}->set($terms);
  }

  /**
   * Helper function to create or get an existing term.
   *
   * @param $term_name
   *   The term name.
   * @param $vocabulary
   *   The vocabulary object.
   *
   * @return
   *   A taxonomy term object.
   */
  private function getTerm($term_name, $vocabulary) {
    if (!$term = taxonomy_get_term_by_name($term_name, $vocabulary->machine_name)) {
      $values = array(
        'name' => $term_name,
        'vid' => $vocabulary->vid,
      );
      $term = entity_create('taxonomy_term', $values);
      taxonomy_term_save($term);
    }

    // taxonomy_get_term_by_name() returns an array, so normalize it.
    return is_array($term) ? reset($term) : $term;
  }


  /**
   * Export OG-vocab
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    if (!$terms = $wrapper->{OG_VOCAB_FIELD}->value()) {
      return;
    }

    // Re-group terms by their vocabulary.
    $values = array();
    foreach ($terms as $term) {
      $values[$term->vid][] = $term->name;
    }

    $vids = array_keys($values);

    $vocabularies = taxonomy_vocabulary_load_multiple($vids);

    // Get the related OG-vocabs, to know if the values are array or not.
    $query = new EntityFieldQuery();
    $result = $query->entityCondition('entity_type', 'og_vocab')
      ->propertyCondition('vid', $vids, 'IN')
      ->execute();

    $og_vocabs = entity_load('og_vocab', array_keys($result['og_vocab']));

    // Normalize the data that we want, the vocabulary ID as the key, and TRUE
    // if the value should be an array.
    $value_types = array();
    foreach ($og_vocabs as $og_vocab) {
      $value_types[$og_vocab->vid] = $og_vocab->settings['cardinality'] != 1;
    }

    foreach ($values as $vid => $vocab_terms) {
      // Populate the YAML.
      $vocab_name = $vocabularies[$vid]->name;

      if (!$value_types[$vid]) {
        $vocab_terms = $vocab_terms[0];
      }

      $yaml[$vocab_name] = $vocab_terms;
    }
  }

  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      return og_vocab_is_og_vocab_field($instance['entity_type'], $field['field_name'], $instance['bundle']);
    }
    elseif (in_array($op, array('import', 'export'))) {
      $plugin_name = $this->plugin['name'];
      $sync_map = $this->syncMap[$plugin_name];
      return in_array(OG_VOCAB_FIELD, $sync_map);
    }
  }
}
