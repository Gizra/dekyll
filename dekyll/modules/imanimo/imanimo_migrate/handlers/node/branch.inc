<?php

/**
 * Migrate "Branch" nodes.
 */
class ImanimoRepositoryNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'branch';

  public $dependencies = array('ImanimoRepositoryNodes');

  public function __construct() {
    parent::__construct();
    $this->addFieldMapping(OG_GROUP_FIELD)->defaultValue(TRUE);
  }

  public function complete($entity, $row) {
    // Set the first branch as the default repository.
    if (!variable_get('imanimo_og_default_branch')) {
      variable_set('imanimo_og_default_branch', $entity->nid);
    }
  }
}

