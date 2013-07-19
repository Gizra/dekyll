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
}
