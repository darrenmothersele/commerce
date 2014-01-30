<?php

/**
 * @file
 * The page and form callbacks for use by the shopping cart.
 */


/**
 * Redirects invalid checkout attempts or displays the checkout form if valid.
 */
function commerce_cart_checkout_router() {
  global $user;

  // Load the shopping cart order.
  if ($order = commerce_cart_order_load($user->uid)) {
    $wrapper = entity_metadata_wrapper('commerce_order', $order);
  }

  // If no shopping cart order could be found, redirect away from checkout.
  // TODO: Redirect to the cart page instead which would then appear as an
  // empty shopping cart page.
  if (empty($order) || commerce_line_items_quantity($wrapper->commerce_line_items, commerce_product_line_item_types()) == 0) {
    drupal_set_message(t('Add some items to your cart and then try checking out.'));
    drupal_goto(variable_get('commerce_checkout_empty_redirect', ''));
  }

  drupal_goto('checkout/' . $order->order_id);
}

/**
 * Displays the shopping cart form and associated information.
 */
function commerce_cart_view() {
  global $user;

  // Default to displaying an empty message.
  $content = theme('commerce_cart_empty_page');

  // First check to make sure we have a valid order.
  if ($order = commerce_cart_order_load($user->uid)) {
    $wrapper = entity_metadata_wrapper('commerce_order', $order);

    // Only show the cart form if we found product line items.
    if (commerce_line_items_quantity($wrapper->commerce_line_items, commerce_product_line_item_types()) > 0) {

      // Add the form for editing the cart contents.
      $content = commerce_embed_view('commerce_cart_form', 'default', array($order->order_id), 'cart');
    }
  }

  return $content;
}

/**
 * Form callback wrapper: edit a cart line item.
 *
 * @param $line_item
 *   The line item object to edit through the form.
 * @param $show_quantity
 *   Boolean indicating whether or not to show the quantity widget; defaults to
 *   FALSE resulting in a hidden field holding the quantity.
 * @param $context
 *   Information on the context of the form's placement, allowing it to update
 *   product fields on the page based on the currently selected default product.
 *
 * @see commerce_cart_add_to_cart_form()
 */
function commerce_cart_line_item_form_wrapper($line_item, $show_quantity = NULL, $context = array()) {
  if (!isset($_GET['token']) || !drupal_valid_token($_GET['token'], 'commerce_line_item_edit:' . $line_item->line_item_id) || !commerce_cart_line_item_form_menu_item_access($line_item)) {
    return MENU_ACCESS_DENIED;
  }

  // Resolve $show_quantity.
  $show_quantity_default = FALSE;
  if (isset($show_quantity)) {
    // Set to function argument
    $show_quantity = (bool) $show_quantity;
  }
  elseif (isset($_GET['show_quantity'])) {
    // Safely resolve from query parameter
    $show_quantity = !empty($_GET['show_quantity']);
  }
  else {
    // Set to default
    $show_quantity = $show_quantity_default;
  }

  if (empty($context)) {
    $context = array(
      'view_mode' => 'display',
      'class_prefix' => 'cart-line-item-edit-form-' . $line_item->line_item_id
    );
  }

  // Render edit form.
  $form = drupal_get_form('commerce_cart_add_to_cart_form', $line_item, $show_quantity, $context, TRUE);

  return $form;
}
