<?php

/**
 * Migrate Page nodes.
 */
class ImanimoPageNodes extends ImanimoMigration {
  public $csvColumns = array(
    array('language', 'Language'),
    array('path', 'Path'),
    array('link_weight', 'Link weight'),
    array('body', 'Body'),
  );

  public $entityType = 'node';
  public $bundle = 'page';

  public $dependencies = array(
    'ImanimoOfferCategoriesTerms',
  );

  public function __construct() {
    parent::__construct();

    $this->addFieldMapping('body', 'body')
      ->arguments(array('format' => 'full_html'));;

    $this
      ->addFieldMapping('field_category', 'field_category')
      ->sourceMigration('ImanimoOfferCategoriesTerms');

    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('path', 'path');
  }
}

/**
 * Migrate Menu.
 */
class ImanimoMenu extends Migration {

  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('menu_name', 'Menu name'),
    array('description', 'Description'),
  );


  public function __construct() {
    parent::__construct();

    // Creating dummy destination, otherwise the migration won't work.
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );

    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationMenu::getKeySchema('menu'));
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/menu/menu.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationMenu();

    $this->addFieldMapping('menu_name', 'menu_name');

    $this->addFieldMapping('title', 'menu_name');

    $this->addFieldMapping('description', 'description');

    // Add i18n Support.
    $this->addFieldMapping('i18n_mode')
      ->defaultValue(I18N_MODE_MULTIPLE);

  }
}

/**
 * Migrate Menu links.
 */
class ImanimoMenuLinks extends Migration {

  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('menu_name', 'Menu name'),
    array('title', 'Title'),
    array('language', 'Language'),
    array('link_path', 'Path'),
    array('link_weight', 'Link weight'),
    array('parent', 'Parent'),
    // We will "push" the class to the correct place in the prepare function.
    array('options', 'Class'),
  );

  public $dependencies = array(
    'ImanimoPageNodes',
    'ImanimoMenu',
  );

  public function __construct() {
    parent::__construct();

    // Creating dummy destination, otherwise the migration won't work.
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );

    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationMenuLinks::getKeySchema('menu-links'));
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/menu/menu-links.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationMenuLinks();

    $this->addFieldMapping('menu_name', 'menu_name');

    $this->addFieldMapping('link_title', 'title');

    $this->addFieldMapping('language', 'language');

    $this->addFieldMapping('link_path', 'link_path');

    // The router path might change in the prepare function.
    $this->addFieldMapping('router_path', 'link_path');

    $this->addFieldMapping('weight', 'link_weight');

    $this
      ->addFieldMapping('options', 'options')
      ->separator('|');

    $this
      ->addFieldMapping('plid', 'parent')
      ->defaultValue(0);

    $this
      ->addFieldMapping('expanded', NULL)
      ->defaultValue(1);

    $this
      ->addFieldMapping('customized', NULL)
      ->defaultValue(1);
  }


  public function prepare($entity, $row) {
    if ($entity->options) {
      // Push the CSS class definition to the correct place.
      $entity->options = array(
        'attributes' => array('class' => $entity->options)
      );
    }

    // Get the non-aliases URL.
    if ($source = drupal_lookup_path('source', $row->link_path, $row->language)) {
      // Check if the path is a node
      $item = menu_get_item($source);

      // The router-path for a node should be 'node/%'.
      $entity->link_path = $source;
      $entity->router_path = $item['path'];
    }

    if (!$row->parent) {
      return;
    }

    // Get parent link ID from migrates' table.
    $query = db_select('migrate_map_imanimomenulinks', 'ml');
    $result = $query->fields('ml')
      ->condition('ml.sourceid1', $row->parent)
      ->execute()
      ->fetchAssoc();

    $entity->plid = $result['destid1'];
  }
}

/**
 * Migrate product nodes.
 */
class ImanimoProductNodes extends ImanimoMigration {
  public $csvColumns = array(
    array('sku', 'SKU'),
    array('language', 'Language'),
    array('body', 'Body'),
    array('field_category', 'Category'),
  );

  public $entityType = 'node';
  public $bundle = 'product';

  public $dependencies = array('ImanimoProducts');

  public function __construct() {
    parent::__construct();

    $this->addFieldMapping('body', 'body')
      ->arguments(array('format' => 'full_html'));;
    $this->addFieldMapping('field_product', 'sku');
    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('field_category', 'field_category');
  }

  public function prepareRow($row) {
    $row->uid = 1;

    $product = commerce_product_load_by_sku($row->sku);
    $row->sku = $product->product_id;
  }
}

/**
 * Migrate "Community" nodes.
 */
class ImanimoCommunityNodes extends ImanimoMigration {
  public $entityType = 'node';
  public $bundle = 'community';

  public function __construct() {
    parent::__construct();
    $this->addFieldMapping(OG_GROUP_FIELD)->defaultValue(TRUE);
  }

  public function complete($entity, $row) {
    // Set the first community as the default community.
    if (!variable_get('imanimo_og_default_community')) {
      variable_set('imanimo_og_default_community', $entity->nid);
    }
  }
}

/**
 * Migrate users.
 */
class ImanimoUsers extends Migration {
  /**
   * Map the field and properties to the CSV header.
   */
  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('name', 'Username'),
    array('pass', 'Password'),
    array('mail', 'Email'),
  );

  public $entityType = 'user';

  public function __construct() {
    parent::__construct();
    $this->description = t('Import users from a CSV file.');

    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('mail', 'mail');
    $this->addFieldMapping('roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);
    $this->addFieldMapping('status')
         ->defaultValue(TRUE);

    // Create a map object for tracking the relationships between source rows
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $destination_handler = new MigrateDestinationUser();
    $this->map = new MigrateSQLMap($this->machineName, $key, $destination_handler->getKeySchema());

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/' . $this->entityType . '/user.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationUser();
  }
}

/**
 * Migrate transaction entities.
 */
class ImanimoTransaction extends ImanimoTransactionMigration {

  public $entityType = 'transaction';
  public $bundle = 'transaction';

  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('payer', 'Payer'),
    array('payee', 'Payee'),
    array('type', 'Type'),
    array('creator', 'Creator'),
    array('created', 'Created'),
    array('transaction_purpose', 'Purpose'),
    array('currency', 'Currency'),
    array('amount', 'Amount'),
  );

  public $dependencies = array(
    'ImanimoUsers',
  );

  public function __construct() {
    parent::__construct();

    $this->addFieldMapping('type', 'type');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('transaction_purpose', 'transaction_purpose');
    $this
      ->addFieldMapping('payer', 'payer')
      ->sourceMigration('ImanimoUsers');
    $this
      ->addFieldMapping('payee', 'payee')
      ->sourceMigration('ImanimoUsers');
    $this
      ->addFieldMapping('creator', 'creator')
      ->sourceMigration('ImanimoUsers');
  }

  public function prepare($entity, $row) {
    // Set the worth value.
    // TODO: See if it's possible to set it with ('worth:currcode', 'currency').
    $worth = array(
      'quantity' => $row->amount,
      'currcode' => $row->currency,
    );
    $entity->worth = array(LANGUAGE_NONE => array($worth));
  }


  public function complete($entity, $row) {
    // Re-set the created value, because it gets overridden, also when trying to
    // set it with entity_save.
    db_update('mcapi_transactions')
      ->fields(array('created' => $row->created,))
      ->condition('xid', $entity->xid)
      ->execute();
  }
}

/**
 * Migrate products.
 */
class ImanimoProducts extends Migration {
  public $csvColumns = array(
    array('id', 'Unique ID'),
    array('sku', 'SKU'),
    array('price', 'Price'),
    array('field_image', 'Image'),
    array('title', 'Title'),
    array('title_he', 'Title he'),
  );

  public $entityType = 'commerce_product';
  public $bundle = 'product';

  public function __construct() {
    parent::__construct();

    $key = array(
      'sku' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationEntityAPI::getKeySchema('commerce_product'));

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'imanimo_migrate') . '/csv/commerce_product/product_page.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'product');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');
    $this->addFieldMapping('field_image', 'field_image');
    $this
      ->addFieldMapping('field_image:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this
      ->addFieldMapping('field_image:source_dir')
      ->defaultValue(drupal_get_path('module', 'imanimo_migrate') . '/images');
    $this->addFieldMapping('language')
      ->defaultValue('en');
  }

  public function complete($entity, $row) {
    $entity->{'title_field'}['he'] = array(array('value' => $row->title_he));
    entity_save('commerce_product', $entity);
  }
}
