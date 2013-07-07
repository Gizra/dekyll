<?php

/**
 * Migrate "Product Color" terms.
 */
class ImanimoProductColorTerms extends ImanimoMigration {

  public $entityType = 'taxonomy_term';
  public $bundle = 'product_color';

  public function __construct() {
    parent::__construct();

    $this->addFieldMapping('field_product_color_image')
      ->defaultValue('black.jpg');

    $this
      ->addFieldMapping('field_product_color_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);

    $this
      ->addFieldMapping('field_product_color_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'imanimo_migrate') . '/images/' . $this->entityType . '/' . $this->bundle);
  }

  /**
   * Set the image field to be the term name.
   */
  public function prepareRow($row) {
    $row->field_product_color_image = $row->id . '.jpg';
  }

}

