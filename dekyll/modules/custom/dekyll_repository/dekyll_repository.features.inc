<?php
/**
 * @file
 * dekyll_repository.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dekyll_repository_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "page_manager" && $api == "pages_default") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function dekyll_repository_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function dekyll_repository_node_info() {
  $items = array(
    'branch' => array(
      'name' => t('Branch'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'repository' => array(
      'name' => t('Repository'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
