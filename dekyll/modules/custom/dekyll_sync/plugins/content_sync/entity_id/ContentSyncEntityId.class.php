<?php

/**
 * Entity ID content sync.
 */
class ContentSyncEntityId extends ContentSyncBase {

  /**
   * Export entity ID.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    if (isset($wrapper->{OG_AUDIENCE_FIELD}) && !$wrapper->{OG_AUDIENCE_FIELD}->field_repo_canonical->value()) {
      // Repository isn't canonical.
      return;
    }
    $yaml['entity_id'] = $wrapper->type() . ':' . $wrapper->getIdentifier();
  }


  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    return $op != 'settings';
  }
}
