<?php

$plugin = array(
  'title' => t('Commerce products JSON'),
  'description' => t('Export the price and a link to thumbnail as JSON.'),
  'class' => 'ContentSyncCommerceProductJson',
  // Make sure the plugin happens after we already have the image styles, so
  // we can add the thumbnail to the JSON.
  'weight' => 99,
);
