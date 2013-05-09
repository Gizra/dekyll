<?php

/**
 * Implements hook_panels_layouts()
 */
// Plugin definition
$plugin = array(
  'title' => t('12'),
  'category' => t('Bootstrap One Column'),
  'icon' => 'bootstrap_onecol.png',
  'theme' => 'bootstrap_onecol',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array('middle' => t('Middle column')),
);
