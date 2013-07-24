<?php

/**
 * Migrate product page nodes.
 */
class ImanimoProductPageNodes extends Migration {

  public $dependencies = array(
    'ImanimoBranchNodes',
    'ImanimoLayoutTerms',
  );

  public $csvColumns = array(
    array('sku', 'SKU'),
    array('title', 'Product'),
    array('body', 'Description'),
    array('_commerce_price', 'Price'),
    array('_field_product_color', 'Color'),
    array('_field_inventory', 'Inventory'),
    array('_field_images', 'Images'),
  );

  public function __construct() {
    parent::__construct();

    // Create a map object for tracking the relationships between source rows.
    // We set the SKU, just to allow us to go over duplicate rows.
    // see ImanimoProductPageNodes::prepareRow().
    $key = array(
      'sku' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );

    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationNode::getKeySchema());

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/products.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationNode('product_page', array('text_format' => 'filtered_html'));

    $this->addFieldMapping('title', 'title');

    // Hardcode to the only branch we have.
    $this
      ->addFieldMapping('field_repo_branch')
      ->defaultValue(2);

    $this
      ->addFieldMapping('body', 'body')
      ->arguments(array('format' => 'full_html'));

    $this
      ->addFieldMapping('field_file_path', 'title')
      ->callbacks(array($this, 'prepareFieldFilePath'));


    // Add "product_page" layout.
    $this->addFieldMapping(OG_VOCAB_FIELD)
      ->defaultValue('product_page')
      ->sourceMigration('ImanimoLayoutTerms');


    $this
      ->addFieldMapping('uid')
      ->defaultValue(1);
  }

  /**
   * Ignore existing nodes.
   */
  public function prepareRow($row) {
    $query = new EntityFieldQuery();
    $result = $query
      ->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'product_page')
      ->propertyCondition('title', $row->title, '=')
      ->range(0, 1)
      ->count()
      ->execute();

    // If node exists, return FALSE to prevent re-processing the same node.
    return !$result;
  }

  /**
   * Auto create the file path from the title.
   */
  public function prepareFieldFilePath($value) {
    ctools_include('cleanstring');
    return 'products/' . ctools_cleanstring($value, array('lower case' => TRUE));
  }
}
