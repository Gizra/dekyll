<?php

/**
 * @file
 * Export manager interface.
 */


interface ExportManagerInterface {

  /**
   * Constructor for the export manager.
   */
  public function __construct($plugin, $nid);

  /**
   * Get a tree array of the entities to export and their children.
   */
  public function getExportRoutes();

  /**
   * Call export method of the content sync plugins, per route.
   *
   * @return \ExportManagerInterface
   *   The current object, to allow chaining.
   */
  public function export();
}
