<?php

/**
 * Migrate Commerce products.
 */
class ImanimoProductCommerceProducts extends Migration {
  public $csvColumns = array(
    array('id', 'SKU'),
    array('sku', 'SKU'),
    array('commerce_price', 'Price'),
    array('field_images', 'Images'),
  );

  public $entityType = 'commerce_product';
  public $bundle = 'product';

  public function __construct() {
    parent::__construct();

    $key = array(
      'sku' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationEntityAPI::getKeySchema('commerce_product'));

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/commerce_product/product_page.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'product');

    $this->addFieldMapping('title');

    $this->addFieldMapping('sku', 'sku');

    $this->addFieldMapping('commerce_price', 'commerce_price');

    $this
      ->addFieldMapping('field_image', 'field_image')
      ->separator('|');

    $this
      ->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);

    $this
      ->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'imanimo_migrate') . '/images/commerce_product');
  }

  public function prepare($row) {
    $row->title = $row->product . ' - ' . $row->color;
  }
}
