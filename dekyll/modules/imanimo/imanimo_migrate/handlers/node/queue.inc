<?php

/**
 * Migrate "Queue" nodes.
 */
class ImanimoQueueNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'queue';

  public $csvColumns = array(
    array('Reference', 'field_queue_node'),
    array('Limit', 'field_queue_limit'),
  );

  public $dependencies = array(
    'ImanimoBranchNodes',
    'ImanimoProductPageNodes',
  );

  public function __construct() {
    parent::__construct();

    // Branch is a group.
    $this->addFieldMapping(OG_GROUP_FIELD)->defaultValue(TRUE);

    $this
      ->addFieldMapping('field_queue_node', 'field_queue_node')
      ->sourceMigration(array('ImanimoProductPageNodes'));


    $this
      ->addFieldMapping('field_queue_limit', 'field_queue_limit')
      ->defaultValue(0);

    $this
      ->addFieldMapping('uid')
      ->defaultValue(1);
  }
}

