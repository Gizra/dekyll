<?php
/**
 * @file
 */

/**
 * Destination class implementing migration into field_collection.
 */
class MigrateDestinationFieldCollection extends MigrateDestinationEntity {
  /**
   * The type of entity hosting this collection field (e.g., node).
   *
   * @var string
   */
  protected $hostEntityType;

  static public function getKeySchema() {
    return array(
      'cid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'ID of field collection item',
      ),
    );
  }

  /**
   * Basic initialization.
   *
   * @param array $options
   *  Options applied to collections.
   */
  public function __construct($bundle, array $options = array()) {
    parent::__construct('field_collection_item', $bundle, $options);
    $this->hostEntityType = $options['host_entity_type'];
  }

  /**
   * Returns a list of fields available to be mapped for this collection (bundle).
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping).
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields = migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle);
    $fields['host_entity_id'] = t('Field collection host ID');
    return $fields;
  }

  /**
   * Import a single term.
   *
   * @param $collection
   *  Collection object to build. Pre-filled with any fields mapped in the migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   *
   * @return array
   *  Array of key fields (item_id only in this case) of the collection that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $collection, stdClass $row) {
    if (empty($collection->field_product_size)) {
      return;
    }

    $return = array();

    $host_entity = entity_load_single($this->hostEntityType, $collection->host_entity_id['destid1']);
    unset($collection->host_entity_id);

    foreach (array_keys($collection->field_product_size) as $delta) {
      if (!is_numeric($delta)) {
        continue;
      }
      $entity = entity_create('field_collection_item', array('field_name' => $this->bundle));
      $updating = FALSE;
      // The host entity cannot be reset - we only set it on initial insert
      $entity->setHostEntity($this->hostEntityType, $host_entity);

      $field_names = array(
        'field_product_size',
        'field_product_stock',
      );

      foreach ($field_names as $field_name) {
        $entity->{$field_name}[] = $collection->{$field_name}[$delta];

        if (!empty($collection->{$field_name}['arguments'])) {
          $entity->{$field_name}['arguments'] = $collection->{$field_name}['arguments'];
        }
      }

      $this->prepare($entity, $row);

      migrate_instrument_start('field_collection_save');
      $entity->save();
      migrate_instrument_stop('field_collection_save');

      // TODO: status doesn't return correct value, so just check the
      // entity was created.
      if ($entity->item_id) {
        $this->complete($entity, $row);
        $return[] = $entity->item_id;
      }
    }

    if ($updating) {
      $this->numUpdated++;
    }
    else {
      $this->numCreated++;
    }

    // @todo: We are returning only the first field collection.

    return array($return[0]);
  }

  /**
   * Delete a migrated collection.
   *
   * @param $key
   *  Array of fields representing the key.
   */
  public function rollback(array $key) {
    $item_id = reset($key);

    $this->prepareRollback($item_id);
    $field_collection_item = field_collection_item_load($item_id);
    // If the collection wasn't imported we can't roll it back so check if the
    // loaded object matches the fieldcollection item class.
    if ($field_collection_item instanceof FieldCollectionItemEntity) {
      $field_collection_item->delete();
    }

    $this->completeRollback($item_id);
    return TRUE;
  }
}
