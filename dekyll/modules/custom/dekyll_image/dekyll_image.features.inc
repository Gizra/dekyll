<?php
/**
 * @file
 * dekyll_image.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dekyll_image_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function dekyll_image_node_info() {
  $items = array(
    'image' => array(
      'name' => t('Image'),
      'base' => 'node_content',
      'description' => t('A single image, that can be referenced from the Queue content type, to allow creating for example slideshows.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
