<?php

/**
 * Field handler to present an edit link to a cart line item.
 */
class commerce_cart_handler_field_cart_line_item_link_edit extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['line_item_id'] = 'line_item_id';
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => '', 'translatable' => TRUE);
    $options['show_quantity'] = array('default' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );

    $form['show_quantity'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display a textfield quantity widget on the edit form.'),
      '#default_value' => $this->options['show_quantity'],
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $line_item_id = $this->get_value($values, 'line_item_id');

    $path = 'cart/line-items/' . $line_item_id . '/edit';
    $menu_item = menu_get_item($path);

    // Exit if no access to the menu path
    if (empty($menu_item['access'])) {
      return;
    }

    // Build query parameters
    $query_params = drupal_get_destination();
    $query_params['show_quantity'] = $this->options['show_quantity'];
    $query_params['token'] = drupal_get_token('commerce_line_item_edit:' . $line_item_id);

    // render link
    return l($text, $path, array('query' => $query_params));
  }
}
