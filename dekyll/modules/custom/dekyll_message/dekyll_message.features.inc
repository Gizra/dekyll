<?php
/**
 * @file
 * dekyll_message.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dekyll_message_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_default_message_type().
 */
function dekyll_message_default_message_type() {
  $items = array();
  $items['dekyll_export'] = entity_import('message_type', '{
    "name" : "dekyll_export",
    "description" : "Dekyll export",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : {
      "token options" : { "clear" : 0 },
      "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" }
    },
    "language" : "en",
    "arguments" : null,
    "message_text" : { "en" : [
        {
          "value" : "Export log",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003EExport log\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  return $items;
}
