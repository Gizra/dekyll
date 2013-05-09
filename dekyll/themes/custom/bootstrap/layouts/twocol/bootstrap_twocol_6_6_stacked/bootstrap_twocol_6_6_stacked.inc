<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('6-6 stacked'),
  'category' => t('Bootstrap Two Columns'),
  'icon' => 'bootstrap_twocol_6_6_stacked.png',
  'theme' => 'bootstrap_twocol_6_6_stacked',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'top' => t('Top'),
    'left' => t('Left'),
    'right' => t('Right'),
    'bottom' => t('Bottom'),
  ),
);
