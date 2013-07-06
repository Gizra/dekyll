<?php

/**
 * @file
 * Export manager interface.
 */


interface ExportManagerInterface {

  /**
   * Constructor for the export manager.
   */
  public function __construct($plugin, $nid, $tree = array());

  /**
   * Get a tree array of the entities to export and their children.
   */
  public function getExportRoutes();

  /**
   * Call export method of the content sync plugins, per route.
   *
   * @todo: Find a better name for routes.
   *
   * @param $routes
   * @return mixed
   */
  public function export($options = array());
}
