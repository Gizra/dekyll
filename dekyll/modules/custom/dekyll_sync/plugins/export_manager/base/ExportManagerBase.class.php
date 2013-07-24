<?php

/**
 * @file
 * Base export manager.
 */

use GitWrapper\GitWrapper;
use GitWrapper\GitWorkingCopy;
use GitWrapper\GitException;
use Symfony\Component\Yaml\Parser;
use Symfony\Component\Yaml\Dumper;

class ExportManagerBase implements ExportManagerInterface {

  /**
   * The branch ID.
   */
  private $branchId;

  /**
   * Determine if the repository is canonical.
   */
  private $isCanonical;

  /**
   * The export manager plugin array.
   */
  private $plugin;

  /**
   * The parent node ID.
   */
  private $nid;

  /**
   * The entities to export routes.
   */
  private $routes;

  /**
   * Constructor for the export manager.
   */
  public function __construct($plugin, $nid) {
    $this->plugin = $plugin;
    $this->nid = $nid;

    // Make sure we get an updated node, even when running under "Waiting queue"
    // in the command line.
    $node = node_load($nid, NULL, TRUE);

    $wrapper = entity_metadata_wrapper('node', $node);
    $this->branchId = $wrapper->field_repo_branch->value(array('identifier' => TRUE));
    $this->isCanonical = $wrapper->{OG_AUDIENCE_FIELD}->field_repo_canonical->value();

    $this->setRoutes($this->getExportRoutes());
  }

  /**
   * @param mixed $routes
   */
  public function setRoutes($routes) {
    $this->routes = $routes;
  }

  /**
   * @return mixed
   */
  public function getRoutes() {
    return $this->routes;
  }

  /**
   * Get a routes array of the entities to export and their parents.
   */
  public function getExportRoutes() {
    $wrapper = entity_metadata_wrapper('node', $this->nid);
    $routes = array();

    $this->prepareExportRoutes($wrapper, $routes);
    return $routes;
  }

  /**
   * Recursive function to prepare the array of files to export.
   *
   * @param EntityDrupalWrapper $wrapper
   *   The entity wrapped using entity_metadata_wrapper().
   * @param array $routes
   *   Flat array of entities that needs to be processed. Each route represents
   *   a page that needs to be built.
   * @param array $parent_info
   *   Optional; The route info of the existin page parent.
   * @param bool $is_page
   *   Optional; Determine if the current entity is representing a page.
   *   Defaults to TRUE.
   * @param string $jeykll_name
   *   Optional; The name of the property that should be used in the YAML file.
   *   Defaults to empty string, which means the field name will be used.
   */
  private function prepareExportRoutes(EntityDrupalWrapper $wrapper, &$routes = array(), $parent_info = array(), $is_page = TRUE, $jekyll_name = '') {
    // Get all the field instances of the bundle.
    $identifier = $wrapper->type() . ':' . $wrapper->getIdentifier();

    $entity_type = $wrapper->type();
    $bundle = $wrapper->getBundle();

    $parents = array();

    if ($parent_info) {
      $parents = $parent_info['parents'];
      $parents[] = $parent_info['identifier'];

      foreach ($parents as $parent_identifier) {
        $routes[$parent_identifier]['children'][] = $identifier;
      }
    }

    $routes[$identifier] = array(
      'wrapper' => $wrapper,
      'sync_map' => dekyll_sync_get_content_sync_map($entity_type, $bundle),
      'identifier' => $identifier,
      'page' => $is_page,
      'parents' => $parents,
      // For now we get only the first level children.
      'children' => array(),
      // We pass the file path even if the entity isn't a page, so it's children
      // can use it, if needed.
      'file_path' => $this->getFilePath($wrapper, $parent_info, $is_page),
      'jekyll_name' => $jekyll_name,
    );

    foreach (field_info_instances($entity_type, $bundle) as $field_name => $instance) {
      $field = field_info_field($field_name);

      if (!in_array($field['type'], array('entityreference', 'commerce_product_reference'))) {
        // Not an entity/ product reference field.
        continue;
      }

      if (empty($instance['settings']['content_sync']['plugin_name'])) {
        continue;
      }

      if (!in_array($instance['settings']['content_sync']['plugin_name'], array('entity_reference', 'commerce_product'))) {
        // Field doesn't use the entity-reference or product reference content
        // sync plugins.
        continue;
      }

      // The field is referencing an entity that should be used as a page.
      $child_is_page = !empty($instance['settings']['content_sync']['settings']['create_page']);

      if ($child_is_page && $is_page) {
        // Set the page to FALSE, as the children are pages.
        $routes[$identifier]['page'] = FALSE;
      }



      $jekyll_name = !empty($instance['settings']['content_sync']['settings']['jekyll_name']) ? $instance['settings']['content_sync']['settings']['jekyll_name'] : $field_name;

      // Iterate over the children, and let them add their info.
      // @todo: Handle with nicer code.
      if ($field['cardinality'] == 1) {
        $sub_wrapper = $wrapper->{$field_name};
        $this->prepareExportRoutes($sub_wrapper, $routes, $routes[$identifier], $child_is_page, $jekyll_name);
      }
      else {
        foreach ($wrapper->{$field_name} as $sub_wrapper) {
          $this->prepareExportRoutes($sub_wrapper, $routes, $routes[$identifier], $child_is_page, $jekyll_name);
        }
      }
    }
  }

  /**
   * Get the file path.
   *
   * @param EntityDrupalWrapper $wrapper
   *   The entity wrapped using entity_metadata_wrapper().
   * @param array $parent_info
   *   Optional; The route info of the existin page parent.
   * @param bool $is_page
   *   Optional; Determine if the current entity is representing a page.
   *   Defaults to TRUE.
   */
  private function getFilePath($wrapper, $parent_info, $is_page) {
    if (!$is_page) {
      return;
    }

    $parent_dirname = '';
    if (!empty($parent_info['file_path'])) {
      // Get the parent file path.
      $parent_file_path = $parent_info['file_path'];
      $path_parts = pathinfo($parent_file_path);

      $parent_dirname = $path_parts['dirname'] . '/';
    }

    // Prepare the file path.
    if (isset($wrapper->field_file_path) && $file_path = $wrapper->field_file_path->value()) {
      return $parent_dirname . $file_path;
    }

    // No field path, so use the label.
    ctools_include('cleanstring');
    $label = ctools_cleanstring($wrapper->label(), array('lower case' => TRUE));
    return $parent_dirname . $label . '/index.html';
  }


  /**
   * Implements ExportManagerInterface::export().
   */
  public function export() {
    $message = dekyll_message_create_message_export($this->nid);
    $parser = new Parser();
    $dumper = new Dumper();

    $path = dekyll_repository_get_repo_path($this->branchId);

    $routes = $this->getRoutes();
    foreach ($routes as &$route) {
      if (!$route['page']) {
        // Not a page.
        continue;
      }

      $yaml_contents = array();
      $file_path = $route['file_path'];
      $route['file_names'] = array($file_path);

      $full_path = $path . '/' . $file_path;

      $text = '';

      if (!$this->isCanonical && file_exists($full_path)) {
        $contents = file_get_contents($full_path);

        // Get the values from the YAML front header.
        $split_contents = explode(YAML_SEPARATOR, $contents, 3);
        $yaml_contents = $split_contents[1];
        $text = $split_contents[2];
      }

      $yaml = $yaml_contents ? $parser->parse($yaml_contents) : array();

      // Iterate over the parents, self and children.
      foreach (array('parents', 'self', 'children') as $type) {

        switch ($type) {
          case 'parents':
            $item_identifiers = $route['parents'];
            break;

          case 'self':
            $item_identifiers = array($route['identifier']);
            break;

          case 'children':
            $item_identifiers = $route['children'];
        }

        foreach ($item_identifiers as $item_identifier) {
          $route_item = $routes[$item_identifier];
          $jekyll_name = $route_item['jekyll_name'];

          // If no Jekyll name, or parents or self.
          if (!$jekyll_name || in_array($type, array('parents', 'self'))) {
            $yaml_item = &$yaml;
          }
          else {
            $yaml_item = &$yaml[$jekyll_name][];
          }

          // Add the file path info.
          // @todo: move to plugin.
          $yaml_item['file_path'] = $route_item['file_path'];

          // Get the text.
          // @todo: Make configurable.
          $wrapper = $route_item['wrapper'];
          $text = isset($wrapper->body) && $wrapper->body->value() ? $wrapper->body->value->raw() : $text;

          $sync_map = $route_item['sync_map'];

          foreach (dekyll_sync_get_content_syncs() as $plugin) {
            if (!$class_name = ctools_plugin_load_class('dekyll_sync', 'content_sync', $plugin['name'], 'class')) {
              // Class no longer exists.
              continue;
            }

            if (drupal_is_cli()) {
              $params = array(
                '@plugin' => $plugin['title'],
                '@identifier' => $route_item['identifier'],
                '@file_path' => $file_path
              );
              drush_log(dt('Executing @plugin on @identifier for file @file_path.', $params));
            }

            $content_sync = new $class_name($plugin, $sync_map, $this->branchId, $file_path);

            $new_file_names = array();
            if ($content_sync->access('export', NULL, NULL, $sync_map)) {
              // YAML and text are passed by reference.
              // Add file names that were created by the content sync handlers.
              // @todo: Use file(s)-info?
              $new_file_names = $content_sync->export($wrapper, $yaml_item, $text, $route);
            }

            if ($new_file_names) {
              $route['file_names'] = array_merge($route['file_names'], $new_file_names);
            }
          }
        }
      }

      // Dump file.
      $dump = array(
        YAML_SEPARATOR,
        // Dump the YAML expanded, and not inline.
        $dumper->dump($yaml, 5) . "\n",
        YAML_SEPARATOR,
        $text,
      );

      $full_path = $path . '/' . $file_path;

      if (!file_exists($full_path)) {
        $path_parts = pathinfo($full_path);
        drupal_mkdir($path_parts['dirname'], NULL, TRUE);
      }

      file_put_contents($full_path, implode('', $dump));
    }

    $this->setRoutes($routes);

    dekyll_build_build_jekyll_site($this->branchId, $message);


    // Add to Git.
    $this->AddToGit();
    return $this;
  }

  /**
   * Add files to Git, commit and push.
   */
  public function AddToGit() {
    $git_wrapper = new GitWrapper();
    $git = $git_wrapper->workingCopy(dekyll_repository_get_repo_path($this->branchId, TRUE));

    foreach ($this->getRoutes() as $route) {
      if (!$route['page']) {
        continue;
      }
      foreach ($route['file_names'] as $file_name) {
        // Add files to git.
        $git->add($file_name);
        if (drupal_is_cli()) {
          drush_log(dt('Added @file to git.', array('@file' => $file_name)));
        }
      }
    }

    if (!$git->hasChanges()) {
      if (drupal_is_cli()) {
        drush_log(dt('No changes to commit.'));
      }
      return;
    }


    // Commit and push.
    // @todo: Move to a single function.
    try {
      if (drupal_is_cli()) {
        drush_log(dt('Pushing to git.'));
      }
      $git
        ->pull()
        ->commit('Changes of file.')
        ->push();
    }
    catch (GitException $e){
      // If we couldn't push, we might need to push. So re-sync.
      // @todo: Check if we have connection first, and throw error if not?
      if (drupal_is_cli()) {
        drush_log(dt('Git push error: @message.', array('@message' => $e->getMessage())), 'error');
      }
    }

    if (drupal_is_cli()) {
      drush_log(dt('Export @id done.', array('@id' => $this->nid)));
    }
  }
}
