<?php

/**
 * Entity ID content sync.
 */
class ContentSyncEntityId extends ContentSyncBase {

  /**
   * Export entity ID.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    $yaml['entity_id'] = $wrapper->type() . ':' . $wrapper->getIdentifier();
  }


  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    return $op != 'settings';
  }
}
