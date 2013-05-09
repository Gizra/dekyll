<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('3-6-3 stacked'),
  'category' => t('Bootstrap Three Columns'),
  'icon' => 'bootstrap_threecol_3_6_3_stacked.png',
  'theme' => 'bootstrap_threecol_3_6_3_stacked',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'top' => t('Top'),
    'left' => t('Left side'),
    'middle' => t('Middle column'),
    'right' => t('Right side'),
    'bottom' => t('Bottom'),
  ),
);
