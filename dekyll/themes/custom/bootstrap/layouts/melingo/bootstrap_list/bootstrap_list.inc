<?php

/**
 * Implements hook_panels_layouts()
 */
// Plugin definition
$plugin = array(
  'title' => t('12'),
  'category' => t('Bootstrap dekyll word list'),
  'icon' => 'bootstrap_list.png',
  'theme' => 'bootstrap-list',
  'admin css' => '../../bootstrap_panels_admin.css',
  'regions' => array(
    'practice' => t('Practice column'),
    'quiz' => t('Quiz column'),
    'wordlist' => t('Wordlist column'),
  ),
);
