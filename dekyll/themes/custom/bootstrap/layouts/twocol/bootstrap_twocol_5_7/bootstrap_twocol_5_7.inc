<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('5-7'),
  'category' => t('Bootstrap Two Columns'),
  'icon' => 'bootstrap_twocol_5_7.png',
  'theme' => 'bootstrap_twocol_5_7',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'left' => t('Left side'),
    'right' => t('Right side')
  ),
);
