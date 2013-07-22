<?php

/**
 * Migrate Commerce products.
 */
class ImanimoProductCommerceProducts extends Migration {
  public $csvColumns = array(
    array('sku', 'SKU'),
    array('title', 'Product'),
    array('_product_description', 'Description'),
    array('commerce_price', 'Price'),
    array('field_product_color', 'Color'),
    array('_stock', 'Stock'),
    array('field_images', 'Images'),
  );

  public function __construct() {
    parent::__construct();

    $this->dependencies = array(
      'ImanimoProductColorTerms',
      'ImanimoProductPageNodes',
    );

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

    $this->addFieldMapping('title', 'title');

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

  public function prepareRow($row) {
    $row->original_title = $row->title;
    $row->title = ucwords($row->title . ' - ' . $row->field_product_color);
  }

  /**
   * Associate the commerce product with the product page node.
   *
   * We do this here, as the CSV file tries to be user friendly and allows
   * entering the commerce product on each line along with the node it belongs
   * to.
   *
   * To prevent hitting the database for getting the same product page node over
   * and over again, we maintain a static variable that holds only the last
   * queried node.
   */
  public function complete($entity, stdClass $row) {
    $node = $this->getProductPageNode($row->original_title);
    $wrapper = entity_metadata_wrapper('node', $node);
    $wrapper->field_product[] = $entity;
    $wrapper->save();
  }

  /**
   * Get an existing or create a new product page node by title.
   *
   * @param $title
   *   The node title to get or create.
   *
   * @return
   *   An existing or new (saved) node object.
   */
  private function getProductPageNode($title) {
    $cache = &drupal_static(__METHOD__, array());

    if (isset($cache[$title])) {
      return $cache[$title];
    }

    // Reset cache.
    $cache = array();

    $query = new EntityFieldQuery();
    $result = $query
      ->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'product_page')
      ->propertyCondition('title', $title, '=')
      ->range(0, 1)
      ->execute();

    if (!empty($result['node'])) {
      $cache[$title] = key($result['node']);
      return $cache[$title];
    }

    throw new MigrateException(format_string('Product page @title does not exist.', array('@title' => $title)));
  }
}
