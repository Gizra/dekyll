<?php

/**
 * Migrate "Branch" nodes.
 */
class ImanimoBranchNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'branch';

  public $csvColumns = array(
    array('field_jekyll_base_path', 'Base path'),
    array('field_jekyll_base_path_url', 'Base path URL'),
  );

  public $dependencies = array('ImanimoRepositoryNodes');

  public function __construct() {
    parent::__construct();

    // Branch is a group.
    $this->addFieldMapping(OG_GROUP_FIELD)->defaultValue(TRUE);

    $this
      ->addFieldMapping('field_jekyll_base_path', 'field_jekyll_base_path')
      ->defaultValue('local');


    $this
      ->addFieldMapping('field_jekyll_base_path_url', 'field_jekyll_base_path_url')
      ->defaultValue('');
  }

  public function complete($entity, $row) {
    // Set the first branch as the default repository.
    if (!variable_get('imanimo_og_default_branch')) {
      variable_set('imanimo_og_default_branch', $entity->nid);
    }
  }
}

