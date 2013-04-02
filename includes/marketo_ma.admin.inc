<?php

/**
 * @file
 * Administrative page callbacks for the marketo module.
 */

/**
 * Builds and returns the Marketo settings form.
 */
function marketo_admin_settings_form($form, &$form_state) {
  module_load_include('inc', 'marketo', 'includes/marketo');

  $form['marketo_tracking_options'] = array(
    '#title' => t('Tracking Options'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['marketo_tracking_options']['marketo_tracking_method'] = array(
    '#title' => t('Tracking Method'),
    '#type' => 'radios',
    '#options' => array(
      'munchkin' => t('Munchking Javascript API'),
      'soap' => t('SOAP API (Synchronous)'),
      'soap_async' => t('SOAP API (Asynchronous)'),
    ),
    '#default_value' => variable_get('marketo_tracking_method', MARKETO_TRACKING_METHOD_DEFAULT),
    '#required' => TRUE,
    '#descritpion' => t('Asynchronous will queue requests and send data when cron runs.'),
  );
  $form['marketo_tracking_options']['marketo_munchkin_lead_source'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Lead Source'),
    '#default_value' => variable_get('marketo_munchkin_lead_source'),
    '#description' => t('If set, LeadSource will be set to this value unless specifically overridden during data collection.'),
  );
  $form['marketo_tracking_options']['marketo_visibility_pages'] = array(
    '#title' => t('Add tracking to specific pages'),
    '#type' => 'radios',
    '#options' => array(
      t('Every page except the listed pages'),
      t('The listed pages only'),
    ),
    '#default_value' => variable_get('marketo_visibility_pages', 0),
    '#required' => TRUE
  );
  $form['marketo_tracking_options']['marketo_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Pages'),
    '#title_display' => 'invisible',
    '#default_value' => variable_get('marketo_pages', MARKETO_PAGES),
    '#description' => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
    '#rows' => 10,
  );
  $form['marketo_tracking_options']['marketo_webform_fields'] = array(
    '#type' => 'textarea',
    '#title' => t('Marketo fields'),
    '#description' => t('Pipe "|" delimited strings of [API Name]|[Friendly Label]. Enter one field per line. This information can be found in the Marketo admin page at Admin > Field Management > Export Field Names'),
    '#rows' => 10,
    '#prefix' => '<div id="marketo-webform-fields-wrapper">',
    '#suffix' => '</div>',
  );

  if (isset($form_state['triggering_element']['#array_parents']) && in_array('marketo_webform_fields_soap', $form_state['triggering_element']['#array_parents'])) {
    $data = _marketo_get_fields();
    $mwf_default = "";
    foreach ($data as $key => $value) {
      $mwf_default .= $key . "|" . $value . "\n";
    }
    $form['marketo_tracking_options']['marketo_webform_fields']['#value'] = $mwf_default;
  }
  else {
    $form['marketo_tracking_options']['marketo_webform_fields']['#default_value'] = variable_get('marketo_webform_fields', MARKETO_WEBFORM_FIELD_DEFAULTS);
  }

  // @todo This button should not be available until the SOAP fields have been saved
  $form['marketo_tracking_options']['marketo_webform_fields_soap'] = array(
    '#type' => 'button',
    '#value' => t('Retrieve from Marketo'),
    '#ajax' => array(
      'callback' => '_marketo_webform_fields_soap_callback',
      'wrapper' => 'marketo-webform-fields-wrapper',
    )
  );
  $form['marketo_munchkin'] = array(
    '#title' => t('Munchkin Javascript API'),
    '#type' => 'fieldset',
    '#description' => t('Values can be found on the Munchkin Admin page at Admin > Integration > Munchkin'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['marketo_munchkin']['marketo_munchkin_account_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Munchkin Account ID'),
    '#default_value' => variable_get('marketo_munchkin_account_id', ''),
    '#required' => TRUE,
  );
  $form['marketo_munchkin']['marketo_munchkin_api_private_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Private Key'),
    '#default_value' => variable_get('marketo_munchkin_api_private_key', ''),
    '#required' => TRUE,
  );
  $form['marketo_munchkin']['marketo_munchkin_advanced'] = array(
    '#title' => t('Advanced Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  $form['marketo_munchkin']['marketo_munchkin_advanced']['marketo_munchkin_javascript_library'] = array(
    '#type' => 'textfield',
    '#title' => t('Munchkin Javascript Library'),
    '#default_value' => variable_get('marketo_munchkin_javascript_library', '//munchkin.marketo.net/munchkin.js'),
    '#required' => TRUE,
  );
  $form['marketo_munchkin']['marketo_munchkin_advanced']['marketo_munchkin_partition'] = array(
    '#type' => 'textfield',
    '#title' => t('Workspace (Partition)'),
    '#default_value' => variable_get('marketo_munchkin_partition', ''),
    '#required' => FALSE
  );
  $form['marketo_soap'] = array(
    '#title' => t('SOAP API'),
    '#type' => 'fieldset',
    '#description' => t('Values can be found on the SOAP API Admin page at Admin > Integration > SOAP API'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $form['marketo_soap']['marketo_soap_endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('SOAP endpoint'),
    '#default_value' => variable_get('marketo_soap_endpoint', ''),
    '#required' => TRUE,
  );
  $form['marketo_soap']['marketo_soap_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID'),
    '#default_value' => variable_get('marketo_soap_user_id', ''),
    '#required' => TRUE,
  );
  $form['marketo_soap']['marketo_soap_encryption_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Encryption Key'),
    '#default_value' => variable_get('marketo_soap_encryption_key', ''),
    '#required' => TRUE,
  );
  return system_settings_form($form);
}

/**
 * Implements hook_form_validate().
 */
function marketo_admin_settings_form_validate($form, &$form_state) {
  $form_state['values']['marketo_soap_endpoint'] = trim($form_state['values']['marketo_soap_endpoint']);
  $form_state['values']['marketo_soap_user_id'] = trim($form_state['values']['marketo_soap_user_id']);
  $form_state['values']['marketo_soap_encryption_key'] = trim($form_state['values']['marketo_soap_encryption_key']);

  $soap_is_valid = _marketo_validate_soap_configuration(
      $form_state['values']['marketo_soap_user_id'], $form_state['values']['marketo_soap_encryption_key'], $form_state['values']['marketo_soap_endpoint']
  );
  if (!$soap_is_valid) {
    form_set_error('marketo_soap_endpoint', 'Unable to validate SOAP API settings.');
    form_set_error('marketo_soap_user_id');
    form_set_error('marketo_soap_encryption_key');
  }
}

function _marketo_validate_soap_configuration($accessKey, $secretKey, $soapEndPoint) {
  module_load_include('inc', 'marketo', 'includes/marketo.soap');
  try {
    $client = new marketoClient($accessKey, $secretKey, $soapEndPoint);
    $result = $client->getFields();
    if (!$result) {
      $result = FALSE;
    }
  } catch (Exception $e) {
    $result = FALSE;
  }
  return $result;
}

function _marketo_webform_fields_soap_callback($form, &$form_state) {
  return $form['marketo_tracking_options']['marketo_webform_fields'];
}