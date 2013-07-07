<?php

/**
 * Migrate products.
 */
class ImanimoProductCommerceProducts extends Migration {
  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('sku', 'SKU'),
    array('price', 'Price'),
    array('field_images', 'Images'),
    array('title', 'Title'),
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

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');
    $this->addFieldMapping('field_image', 'field_image');
    $this
      ->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this
      ->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'imanimo_migrate') . '/images');
    $this->addFieldMapping('language')
      ->defaultValue('en');
  }

  public function complete($entity, $row) {
    $entity->{'title_field'}['he'] = array(array('value' => $row->title_he));
    entity_save('commerce_product', $entity);
  }
}
