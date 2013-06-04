<?php

use GitWrapper\GitWorkingCopy;

/**
 * OG-vocab config sync.
 *
 * @todo: Allow plugins to form alter, so we can deny OG-vocab delete?
 * or move this to OG-vocab access of CRUD?
 */
class ConfigSyncBranch extends ConfigSyncBase {

  /**
   * Import OG-vocabs.
   */
  public function import() {
    // Add the current branch to the "branch" vocabulary.
    $machine_name = 'branch_' . $this->gid;

    if (!$vocabulary = taxonomy_vocabulary_machine_name_load($machine_name)) {
      // Create a vocabulary and OG-vocab for the branch.
      $vocabulary = new stdClass();
      $vocabulary->name = 'branch';
      $vocabulary->machine_name = $machine_name;
      taxonomy_vocabulary_save($vocabulary);

      // Associate with the group.
      og_vocab_relation_save($vocabulary->vid, 'node', $this->gid);
    }

    // Create an OG-vocab for each group-content.
    $group_content = og_get_all_group_content_bundle();
    foreach (array_keys($group_content['node']) as $bundle) {
      $og_vocab = og_vocab_load_og_vocab($vocabulary->vid, 'node', $bundle, NULL, TRUE);
      if (empty($og_vocab->is_new)) {
        // OG-vocab already exists.
        continue;
      }

      $og_vocab->settings = array(
        'required' => TRUE,
        'cardinality' => 1,
        'widget_type' => 'options_select',
      );

      $og_vocab->save();
    }
  }

  /**
   * We do not export this branch.
   */
  public function export(&$config) {}

}
