<?php

/**
 * Jekyll build (build://) stream wrapper class.
 *
 * Provides support for storing publicly accessible site built with Jekyll.
 */
class DekyllPublicStreamWrapper extends DrupalPublicStreamWrapper {
  /**
   * Implements abstract public function getDirectoryPath()
   */
  public function getDirectoryPath() {
    return variable_get('dekyll_file_public_path', conf_path() . '/builds');
  }

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URI of a public file.
   */
  function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    return $GLOBALS['base_url'] . '/' . self::getDirectoryPath() . '/' . drupal_encode_path($path);
  }
}
