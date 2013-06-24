<?php

/**
 * @file
 * Drush integration for the Dekyll Parser module.
 */

/**
 * Implements hook_drush_command().
 */
function dekyll_sync_drush_command() {
  $items['dekyll-export'] = array(
    'description' => dt('Export a node into Jekyll file(s).'),
    // @todo: ID should be a required argument.
    'options' => array(
      'id' => dt('The node ID to write to Jekyll file.'),
      'branch' => dt('The git branch to write the file to.'),
    ),
  );

  $items['dekyll-import'] = array(
    'description' => dt('Sync Drupal with Jekyll.'),
    // @todo: ID should be a required argument.
    'options' => array(
      'force' => dt('Optional. Force re-sync even if the file did not change.'),
      // @todo: Allow passing multiple IDs?
      'id' => dt('The node ID of the Branch, to sync'),
      'type' => dt('Should sync import "config" or "content". Both by defaults.'),
    ),
  );

  $items['dekyll-config-update'] = array(
    'description' => dt('Sync Jekyll configuration file (_dekyll.yml) with the Drupal configuration.'),
    'options' => array(
      // @todo: Allow passing multiple IDs?
      'id' => dt('The node ID of the repository, to sync'),
    ),
  );

  $items['dekyll-clone'] = array(
    'description' => dt('Clone a specific repository branch.'),
    // @todo: ID should be a required argument.
    'options' => array(
      'id' => dt('The node ID of the Branch, to clone.'),
    ),
  );

  return $items;
}

/**
 * Write an item form the queue into the Jekyll file.
 */
function drush_dekyll_sync_dekyll_export() {
  if (!$nid = drush_get_option('id')) {
    return;
  }

  dekyll_sync_export($nid);
}


/**
 * Sync Drupal with Jekyll, of a specific group.
 */
function drush_dekyll_sync_dekyll_import() {
  if (!$branch_id = drush_get_option('id')) {
    return;
  }
  $sync_type = drush_get_option('type', 'config,content');
  $sync_type = explode(',', $sync_type);

  $force_sync = drush_get_option('force');


  dekyll_sync_import($branch_id, $sync_type, $force_sync);
}

/**
 * Sync Jekyll configuration file (_dekyll.yml) with the Drupal configuration.
 */
function drush_dekyll_sync_dekyll_config_update() {
  if (!$branch_id = drush_get_option('id')) {
    return;
  }

  // Get the YAML file.
  $config = $original_config = drush_dekyll_sync_get_yml($gid, '_dekyll.yml');

  $path = dekyll_repository_get_repo_path($gid);

  $git_wrapper = new GitWrapper();
  $git = $git_wrapper ->workingCopy($path);

  // Config sync.
  foreach (dekyll_sync_get_config_syncs() as $plugin) {
    if (!$class = ctools_plugin_load_class('dekyll_sync', 'config_sync', $plugin['name'], 'class')) {
      continue;
    }
    $config_sync = new $class($plugin, $gid, $git);
    $config_sync->export($config);
  }

  if ($config == $original_config) {
    // Config didn't change.
    return;
  }

  // Write the YAML file.
  dekyll_sync_save_yml($gid, '_dekyll.yml', $config);
}

/**
 * Write an item form the queue into the Jekyll file.
 */
function drush_dekyll_sync_dekyll_clone() {
  if (!$nid = drush_get_option('id')) {
    return;
  }

  dekyll_repository_clone($nid);
}
