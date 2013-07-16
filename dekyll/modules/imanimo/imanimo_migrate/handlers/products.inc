<?php

/**
 * Migrate Commerce products.
 */
class ImanimoProductCommerceProducts extends Migration {
  public $csvColumns = array(
    array('sku', 'SKU'),
    array('_product_name', 'Product'),
    array('_product_description', 'Description'),
    array('commerce_price', 'Price'),
    array('field_product_color', 'Color'),
    array('_stock', 'Stock'),
    array('field_images', 'Images'),
  );

  public function __construct() {
    parent::__construct();

    $this->dependencies = array('ImanimoProductColorTerms');

    $key = array(
      'sku' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationEntityAPI::getKeySchema('commerce_product'));

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/products.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationCommerceProduct('commerce_product', 'product');

    $this->addFieldMapping('title');

    $this->addFieldMapping('sku', 'sku');

    $this->addFieldMapping('commerce_price', 'commerce_price');

    $this
      ->addFieldMapping('field_product_color', 'field_product_color')
      ->sourceMigration('ImanimoProductColorTerms');


    $this
      ->addFieldMapping('field_images', 'field_images')
      ->separator('|');

    $this
      ->addFieldMapping('field_images:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);

    $this
      ->addFieldMapping('field_images:source_dir')
      ->defaultValue(drupal_get_path('module', 'imanimo_migrate') . '/images/commerce_product');

    $this->addFieldMapping('uid')->defaultValue(1);
  }

  public function prepare($entity, $row) {
    dpm($entity);
    $row->title = $row->product . ' - ' . $row->color;
  }
}
