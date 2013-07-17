<?php
/**
 * @file
 * dekyll_commerce.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function dekyll_commerce_taxonomy_default_vocabularies() {
  return array(
    'product_size' => array(
      'name' => 'Size',
      'machine_name' => 'product_size',
      'description' => 'Sizes of the products (e.g. small, medium, large).',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}
