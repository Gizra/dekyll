<?php

/**
 * Commerce product content sync.
 */
class ContentSyncCommerceProduct extends ContentSyncEntityReference {

  /**
   * Overrides ContentSyncEntityReference::getRefernceTargets()
   */
  public function getRefernceTargets($field, $instance) {
    $target_bundles = array_filter($instance['settings']['referenceable_types']);

    return array(
      'target_type' => 'commerce_product',
      'target_bundles' => $target_bundles ? array_keys($target_bundles) : array(),
    );
  }

  /**
   * Check field is of type product reference.
   */
  public function access($op, $field = NULL, $instance = NULL) {
    if ($op == 'settings') {
      return $field['type'] == 'commerce_product_reference';
    }
    return parent::access($op, $field, $instance);
  }
}
