<?php
/**
 * @file
 * Dekyll profile.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Allows the profile to alter the site configuration form.
 */
function dekyll_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = 'dekyll';
}

/**
 * Implements hook_install_tasks().
 */
function dekyll_install_tasks() {
  $tasks = array();

  $tasks['dekyl_installation_type_form'] = array(
    'display_name' => t('Choose installation type'),
    'type' => 'form'
  );

  $tasks['dekyll_create_roles'] = array(
    'display_name' => st('Create user roles'),
    'display' => FALSE,
  );

  $tasks['dekyll_set_permissions'] = array(
    'display_name' => st('Set Permissions'),
    'display' => FALSE,
  );

  $tasks['dekyll_set_variables'] = array(
    'display_name' => st('Set Variables'),
    'display' => FALSE,
  );

  $tasks['dekyll_setup_blocks'] = array(
    'display_name' => st('Setup Blocks'),
    'display' => FALSE,
  );

  $tasks['dekyll_set_text_formats'] = array(
    'display_name' => st('Set text formats'),
    'display' => FALSE,
  );

  $tasks['dekyll_menus_setup'] = array(
    'display_name' => st('Setup menus'),
    'display' => FALSE,
  );

  return $tasks;
}

/**
 * Task callback; Select installation form
 */
function dekyl_installation_type_form($form, &$form_state) {
  $options = array(
    'github_pages' => t('Single Jekyll site per user hosted on Github pages'),
    'normal' => t('Normal installation, with no dependncy on Github'),
  );

  $form['dekyll_installation_type'] = array(
    '#title' => t('Installation type'),
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => 'github_pages',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;

}


/**
 * Submit handler; Set installation type variable.
 */
function dekyl_installation_type_form_submit($form, &$form_state) {
  $type = $form_state['input']['dekyll_installation_type'];
  variable_set('dekyll_installation_type', $type);

  if ($type == 'github_pages') {
    module_enable(array('dekyll_github'));
  }
}

/**
 * Task callback; Setup blocks.
 */
function dekyll_setup_blocks() {
}

/**
 * Task callback; Create user roles.
 */
function dekyll_create_roles() {
  // Create a default role for site administrators, with all available
  // permissions assigned.
  $role = new stdClass();
  $role->name = 'administrator';
  $role->weight = 2;
  user_role_save($role);
  user_role_grant_permissions($role->rid, array_keys(module_invoke_all('permission')));
  // Set this as the administrator role.
  variable_set('user_admin_role', $role->rid);
  // Assign user 1 the "administrator" role.
  db_insert('users_roles')
    ->fields(array('uid' => 1, 'rid' => $role->rid))
    ->execute();
}

/**
 * Task callback; Set permissions.
 */
function dekyll_set_permissions() {
  $permissions = array(
    'create messages',
    'access resource message',
  );

  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, $permissions);
}

/**
 * Task callback; Set variables.
 */
function dekyll_set_variables() {
  $variables = array(
    // Set the default theme.
    'theme_default' => 'bootstrap_subtheme',
    'admin_theme' => 'seven',
    // Date/Time settings.
    'date_default_timezone' => 'Asia/Jerusalem',
    'date_first_day' => 1,
    'date_format_medium' => 'D, Y-m-d H:i',
    'date_format_medium_no_time' => 'D, Y-m-d',
    'date_format_short' => 'Y-m-d',
    // Enable user picture support and set the default to a square thumbnail option.
    'user_email_verification' => FALSE,
    'user_pictures' => '1',
    'user_picture_dimensions' => '1024x1024',
    'user_picture_file_size' => '800',
    'user_picture_style' => 'thumbnail',
    'user_register' => USER_REGISTER_ADMINISTRATORS_ONLY,
    // Update the menu router information.
    'menu_rebuild_needed' => TRUE,
    'jquery_update_jquery_version' => '1.8',
    'site_name' => 'Drupal-Jekyll',
    'site_frontpage' => 'homepage',

    // Page manager.
    'page_manager_node_view_disabled' => FALSE,

    // Github connect scope.
    'github_connect_scope' => 'user,public,repo',

    // We need to make sure the tmp folder works for the Github API, so we set
    // it to be under files/tmp.
    // see file_directory_temp().
    'file_temporary_path' => variable_get('file_public_path', conf_path() . '/files') . '/tmp',
  );

  foreach ($variables as $key => $value) {
    variable_set($key, $value);
  }
}

/**
 * Task callback; Set text formats.
 */
function dekyll_set_text_formats() {
  // Add text formats.
  $filtered_html_format = (object)array(
    'format' => 'filtered_html',
    'name' => 'Filtered HTML',
    'weight' => 0,
    'filters' => array(
      // URL filter.
      'filter_url' => array(
        'weight' => 0,
        'status' => 1,
      ),
      // HTML filter.
      'filter_html' => array(
        'weight' => 1,
        'status' => 1,
      ),
      // Line break filter.
      'filter_autop' => array(
        'weight' => 2,
        'status' => 1,
      ),
      // HTML corrector filter.
      'filter_htmlcorrector' => array(
        'weight' => 10,
        'status' => 1,
      ),
    ),
  );
  filter_format_save($filtered_html_format);

  $full_html_format = (object)array(
    'format' => 'full_html',
    'name' => 'Full HTML',
    'weight' => 1,
    'filters' => array(
      // URL filter.
      'filter_url' => array(
        'weight' => 0,
        'status' => 1,
      ),
      // Line break filter.
      'filter_autop' => array(
        'weight' => 1,
        'status' => 1,
      ),
      // HTML corrector filter.
      'filter_htmlcorrector' => array(
        'weight' => 10,
        'status' => 1,
      ),
    ),
  );
  filter_format_save($full_html_format);
}

/**
 * Profile task; create menu links.
 */
function dekyll_menus_setup() {
  // Add links to user menu.
  $item = array(
    'link_title' => 'Login',
    'link_path' => 'user/login',
    'menu_name' => 'user-menu',
  );
  menu_link_save($item);

  $item = array(
    'link_title' => 'Register',
    'link_path' => 'user/register',
    'menu_name' => 'user-menu',
  );
  menu_link_save($item);
}
