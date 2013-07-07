<?php

/**
 * Migrate "Repository" nodes.
 */
class ImanimoRepositoryNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'repository';

  public function __construct() {
    parent::__construct();
    $this->addFieldMapping(OG_GROUP_FIELD)->defaultValue(TRUE);
  }

  public function complete($entity, $row) {
    // Set the first repository as the default repository.
    if (!variable_get('imanimo_og_default_repository')) {
      variable_set('imanimo_og_default_repository', $entity->nid);
    }
  }
}

