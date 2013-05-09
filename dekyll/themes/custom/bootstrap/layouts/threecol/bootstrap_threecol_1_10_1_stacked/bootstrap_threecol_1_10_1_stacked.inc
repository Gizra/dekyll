<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('1-10-1 stacked'),
  'category' => t('Bootstrap Three Columns'),
  'icon' => 'bootstrap_threecol_1_10_1_stacked.png',
  'theme' => 'bootstrap_threecol_1_10_1_stacked',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'top' => t('Top'),
    'left' => t('Left side'),
    'middle' => t('Middle column'),
    'right' => t('Right side'),
    'bottom' => t('Bottom'),
  ),
);
