<?php

/**
 * Raw values content sync.
 */
class ContentSyncRaw extends ContentSyncBase {

  /**
   * Import node title.
   */
  public function import(EntityDrupalWrapper $wrapper, $yaml = array(), $text = '') {
    $plugin_name = $this->plugin['name'];
    foreach ($this->syncMap[$plugin_name] as $field_name) {
      $instance = field_info_instance($wrapper->type(), $field_name, $wrapper->getBundle());

      $jekyll_name = $this->getJekyllName($instance, $field_name);

      $value = isset($yaml[$jekyll_name]) ? $yaml[$jekyll_name] : NULL;

      $wrapper->{$field_name}->set($value);
    }
  }

  /**
   * Export raw values.
   */
  public function export(EntityDrupalWrapper $wrapper, &$yaml = array(), &$text = '', $files_info) {
    $plugin_name = $this->plugin['name'];
    foreach ($this->syncMap[$plugin_name] as $field_name) {
      $instance = field_info_instance($wrapper->type(), $field_name, $wrapper->getBundle());
      $jekyll_name = $this->getJekyllName($instance, $field_name);

      if (!($value = $wrapper->{$field_name}->raw()) || $value == 'nil') {
        unset($yaml[$jekyll_name]);
        continue;
      }

      $yaml[$jekyll_name] = $value;
    }
  }


  /**
   * Access callback.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      return TRUE;
    }
    elseif (in_array($op, array('import', 'export'))) {
      $plugin_name = $this->plugin['name'];
      return !empty($this->syncMap[$plugin_name]);
    }
  }

}
