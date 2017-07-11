<?php

/**
 * @file
 * content_type_vocab_hello_world.features.inc
 */

/**
 * Implements hook_node_info().
 */
function content_type_vocab_hello_world_node_info() {
  $items = array(
    'hello_world_article' => array(
      'name' => t('Hello World Article'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
