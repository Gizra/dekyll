<?php

/**
 * Migrate "Repository" nodes.
 */
class ImanimoRepositoryNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'repository';

  public $csvColumns = array(
    array('field_repo_url', 'URL'),
    array('field_repo_credentials', 'Credentials'),
    array('field_repo_canonical', 'Canonical'),
  );

  public function __construct() {
    parent::__construct();

    // Repository is a group.
    $this
      ->addFieldMapping(OG_GROUP_FIELD)
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_repo_url', 'field_repo_url');

    $this->addFieldMapping('field_repo_credentials', 'field_repo_credentials');

    $this
      ->addFieldMapping('field_repo_canonical', 'field_repo_canonical')
      ->defaultValue(FALSE);
  }
}

