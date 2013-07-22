<?php

/**
 * Migrate product page nodes.
 */
class ImanimoProductPageNodes extends Migration {

  public $dependencies = array(
    'ImanimoBranchNodes',
  );

  public $csvColumns = array(
    array('sku', 'SKU'),
    array('_title', 'Product'),
    array('body', 'Description'),
    array('_commerce_price', 'Price'),
    array('_field_product_color', 'Color'),
    array('_field_inventory', 'Inventory'),
    array('_field_images', 'Images'),
  );

  public function __construct() {
    parent::__construct();

    // Create a map object for tracking the relationships between source rows
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );

    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationNode::getKeySchema());

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/products.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationNode('product_page', array('text_format' => 'filtered_html'));

    $this
      ->addFieldMapping('body', 'body')
      ->arguments(array('format' => 'full_html'));

    $this
      ->addFieldMapping('field_repo_branch')
      ->defaultValue(2);

    $this
      ->addFieldMapping('field_product', 'sku')
      ->sourceMigration('ImanimoProductCommerceProducts');

    $this
      ->addFieldMapping('uid')
      ->defaultValue(1);

  }

}
