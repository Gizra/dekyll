<?php

/**
 * Commerce product JSON content sync.
 */
class ContentSyncCommerceProductJson extends ContentSyncBase {

  /**
   * Export to JSON.
   *
   * @todo Add thumbnail using Imagine.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    if ($wrapper->type() != 'commerce_product') {
      return;
    }

    $file_path = $this->filePath;

    $path_parts = pathinfo($file_path);
    $json_path = $path_parts['dirname'] . '/product.json';
    $file_names[] = $json_path;

    $full_path = dekyll_repository_get_repo_path($this->gid). '/' . $path_parts['dirname'];

    if (!file_exists($full_path)) {
      drupal_mkdir($full_path, NULL, TRUE);
    }

    $json_full_path = $full_path . '/product.json';

    // Price.
    $value = $wrapper->commerce_price->value();
    unset($value['data']);

    $info = array();
    $info['price'] = $value;

    // Size info.
    $info['entity_id'] = $wrapper->getIdentifier();

    // Array keyed by the size name, and the field collection item ID as the
    // value.
    $info['sizes'] = array();

    foreach ($wrapper->field_inventory as $sub_wrapper) {
      $size_name = $sub_wrapper->field_product_size->label();
      $info['sizes'][$size_name] = $sub_wrapper->getIdentifier();
    }

    file_put_contents($json_full_path, drupal_json_encode($info));

    return $file_names;
  }

  /**
   * @todo: Add $wrapper to the access.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      return;
    }

    // @todo: Should check entity is commerce product.
    return TRUE;
  }
}
