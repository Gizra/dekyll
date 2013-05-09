<?php

/**
 * Implimenting hook_preprocess_table()
 * Bootstrapping Tables
 */
function bootstrap_preprocess_table(&$vars) {
  $vars['attributes']['class'] = array();
  $vars['attributes']['class'][] = 'table';
  $vars['attributes']['class'][] = 'table-striped';
  $vars['attributes']['class'][] = 'table-bordered';
  return;
}