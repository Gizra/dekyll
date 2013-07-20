<?php

/**
 * @file
 * Install, uninstall and update functions for Dekyll Sync.
 */

function dekyll_sync_unstinall() {
  variable_del('dekyll_file_public_path');
}
