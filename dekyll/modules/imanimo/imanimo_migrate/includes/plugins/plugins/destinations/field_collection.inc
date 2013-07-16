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
    $entity = entity_create('field_collection_item', array('field_name' => $this->bundle));
    $updating = FALSE;
    // The host entity cannot be reset - we only set it on initial insert
    $host_entity = entity_load_single($this->hostEntityType, $collection->host_entity_id);
    $entity->setHostEntity($this->hostEntityType, $host_entity);

    unset($collection->host_entity_id);

    foreach ((array) $collection as $field => $value) {
      $entity->{$field} = $value;
    }
    $this->prepare($entity, $row);

    migrate_instrument_start('field_collection_save');
    $status = $entity->save();
    migrate_instrument_stop('field_collection_save');

    // TODO: status doesn't return correct value, so just check the
    // entity was created.
    if ($entity->item_id) {
      $this->complete($entity, $row);
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }
      return array($entity->item_id);
    }
    else {
      return FALSE;
    }
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
