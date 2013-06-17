<?php
/**
 * @file
 * dekyll_commerce.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dekyll_commerce_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function dekyll_commerce_node_info() {
  $items = array(
    'product_page' => array(
      'name' => t('Product Page'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
