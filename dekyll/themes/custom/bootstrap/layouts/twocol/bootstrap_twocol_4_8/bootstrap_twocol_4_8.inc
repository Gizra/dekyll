<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('4-8'),
  'category' => t('Bootstrap Two Columns'),
  'icon' => 'bootstrap_twocol_4_8.png',
  'theme' => 'bootstrap_twocol_4_8',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'left' => t('Left side'),
    'right' => t('Right side')
  ),
);
