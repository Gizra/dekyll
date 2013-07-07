<?php

/**
 * Migrate product page nodes.
 */
class ImanimoProductPageNodes extends ImanimoMigration {

  public $csvColumns = array(
    array('body', 'Body'),
  );

  public $entityType = 'node';
  public $bundle = 'product_page';

  public $dependencies = array('ImanimoProductCommerceProducts');

  public function __construct() {
    parent::__construct();

    $this
      ->addFieldMapping('body', 'body')
      ->arguments(array('format' => 'full_html'));;
  }

}
