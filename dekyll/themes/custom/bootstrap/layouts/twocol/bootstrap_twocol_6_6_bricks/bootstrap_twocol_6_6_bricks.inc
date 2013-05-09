<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('6-6 bricks'),
  'category' => t('Bootstrap Two Columns'),
  'icon' => 'bootstrap_twocol_6_6_bricks.png',
  'theme' => 'bootstrap_twocol_6_6_bricks',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'top' => t('Top'),
    'left_above' => t('Left above'),
    'right_above' => t('Right above'),
    'middle' => t('Middle'),
    'left_below' => t('Left below'),
    'right_below' => t('Right below'),
    'bottom' => t('Bottom'),
  ),
);
