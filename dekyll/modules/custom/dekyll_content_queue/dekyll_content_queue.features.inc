<?php
/**
 * @file
 * dekyll_content_queue.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dekyll_content_queue_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function dekyll_content_queue_node_info() {
  $items = array(
    'queue' => array(
      'name' => t('Queue'),
      'base' => 'node_content',
      'description' => t('Reference other content.'),
      'has_title' => '1',
      'title_label' => t('Queue name'),
      'help' => '',
    ),
  );
  return $items;
}
