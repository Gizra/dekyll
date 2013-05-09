<?php

/**
 * Implements hook_panels_layouts().
 */
// Plugin definition
$plugin = array(
  'title' => t('11-1'),
  'category' => t('Bootstrap Two Columns'),
  'icon' => 'bootstrap_twocol_11_1.png',
  'theme' => 'bootstrap_twocol_11_1',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'left' => t('Left side'),
    'right' => t('Right side')
  ),
);
