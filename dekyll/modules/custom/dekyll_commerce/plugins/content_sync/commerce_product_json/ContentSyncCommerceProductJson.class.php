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

    $value = $wrapper->commerce_price->value();
    unset($value['data']);

    $dump = array(
      'price' => $value,
    );

    file_put_contents($json_full_path, drupal_json_encode($dump));

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
