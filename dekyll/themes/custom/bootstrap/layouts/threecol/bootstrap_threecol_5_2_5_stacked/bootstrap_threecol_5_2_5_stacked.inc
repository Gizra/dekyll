<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('5-2-5 stacked'),
  'category' => t('Bootstrap Three Columns'),
  'icon' => 'bootstrap_threecol_5_2_5_stacked.png',
  'theme' => 'bootstrap_threecol_5_2_5_stacked',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'top' => t('Top'),
    'left' => t('Left side'),
    'middle' => t('Middle column'),
    'right' => t('Right side'),
    'bottom' => t('Bottom'),
  ),
);
