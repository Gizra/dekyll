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
    if (!$relations = og_vocab_relation_get_by_group('node', $this->gid)) {
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

      $values = is_array($yaml[$name]) ? $yaml[$name] : array($yaml[$name]);

      foreach ($values as $value) {
        if (!$term = taxonomy_get_term_by_name($value, $vocabulary->machine_name)) {
          $values = array(
            'name' => $value,
            'vid' => $vocabulary->vid,
          );
          $term = entity_create('taxonomy_term', $values);
          taxonomy_term_save($term);
        }
      }

      $term = is_array($term) ? reset($term) : $term;
      $terms[] = $term;
    }

    // Add term to the OG-Vocab field.
    $wrapper->{OG_VOCAB_FIELD}->set($terms);
  }
}
