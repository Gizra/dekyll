<?php
/**
 * @file
 * dekyll_repository.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function dekyll_repository_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'branch_content';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Branch content';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Type */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'node';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  /* Sort criterion: Content: Title */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'node';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Contextual filter: Content: Branch (field_repo_branch) */
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['id'] = 'field_repo_branch_target_id';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['table'] = 'og_membership';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['field'] = 'field_repo_branch_target_id';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_repo_branch_target_id']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

  /* Display: Content pane */
  $handler = $view->new_display('panel_pane', 'Content pane', 'panel_pane_1');
  $handler->display->display_options['argument_input'] = array(
    'field_repo_branch_target_id' => array(
      'type' => 'context',
      'context' => 'entity:node.nid',
      'context_optional' => 0,
      'panel' => '0',
      'fixed' => '',
      'label' => 'Content: Branch (field_repo_branch)',
    ),
  );
  $translatables['branch_content'] = array(
    t('Master'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('Type'),
    t('All'),
    t('Content pane'),
    t('View panes'),
  );
  $export['branch_content'] = $view;

  return $export;
}
