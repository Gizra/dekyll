<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('4-4-4'),
  'category' => t('Bootstrap Three Columns'),
  'icon' => 'bootstrap_threecol_4_4_4.png',
  'theme' => 'bootstrap_threecol_4_4_4',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'left' => t('Left side'),
    'middle' => t('Middle column'),
    'right' => t('Right side')
  ),
);
