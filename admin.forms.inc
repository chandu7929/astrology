<?php

/**
 * @file
 * Contains administrative forms and their handlers.
 */

/**
 * Get star sign based on DOB form.
 */
function astrology_get_birth_sign_form() {

  /*Only day and week format uses datepicker*/
  /*Jquery UI Theme not included by default*/
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'astrology') . '/js/add_text.js');

  $form['item'] = array(
    '#markup' => t('Please enter your date of birth to find your astrological sign (start sign or sun sign).'),
  );
  $form['value'] = array(
    '#type' => 'textfield',
    '#size' => 10,
    '#date_date_format' => 'm/d/Y',
    '#title' => t('Select Date of Birth'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Find'),
  );
  return $form;
}

/**
 * Get star sign based on DOB form_submit.
 */
function astrology_get_birth_sign_form_submit($form_id, $form_values) {

  $astrology_id = variable_get('astrology_default', 1);

  $dob = $form_values['values']['value'];
  $month = date('n', strtotime($dob));
  $day = date('j', strtotime($dob));
  $found = FALSE;

  $query = db_select('astrology_signs', 'h')
    ->fields('h', ['id', 'name', 'date_from_range', 'date_to_range'])
    ->condition('astrology_id', $astrology_id, '=')
    ->execute();
  while ($row = $query->fetchObject()) {
    $fdate = explode('/', $row->date_from_range);
    $tdate = explode('/', $row->date_to_range);
    if ($month == $fdate[0] && $day >= $fdate[1] || $month == $tdate[0] && $day <= $tdate[1]) {
      $found = TRUE;
      drupal_goto('astrology/' . drupal_strtolower($row->name) . '/sunsign');
      break;
    }
  }
  if (!$found) {
    drupal_set_message(t('Sorry, not able to find astrological sign for you.'), 'error');
  }
}

/**
 * Astrology edit form.
 */
function astrology_edit_form($form_state, $form_id, $astrology_id) {

  $astrology = astrology_get_array($astrology_id);
  $disabled = FALSE;
  if ($astrology_id == 1) {
    $disabled = TRUE;
  }
  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology['id'],
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#default_value' => $astrology['name'],
    '#title' => t('Name'),
    '#required' => TRUE,
    '#disabled' => $disabled,
  );
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#default_value' => $astrology['enabled'],
    '#title' => t('Enabled'),
    '#disabled' => $disabled,
  );
  $form['about'] = array(
    '#type' => 'text_format',
    '#format' => $astrology['about_format'],
    '#title' => t('Description'),
    '#default_value' => $astrology['about'],
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Astrology edit form_validate.
 */
function astrology_edit_form_validate(&$form_id, &$form_values) {

  // Check if astrology name exists.
  $query = db_select('astrology', 'a')
    ->fields('a', ['name', 'id'])
    ->condition('name', $form_values['values']['name'])
    ->condition('id', $form_values['values']['id'], '<>')
    ->execute();
  if ($query->rowCount() > 0) {
    form_set_error('name', t('Astrology name "!name" is already taken', array(
      '!name' => $form_values['values']['name'],
    ))
    );
    $form_values['rebuild'] = TRUE;
  }
}

/**
 * Astrology edit form_submit.
 */
function astrology_edit_form_submit($form_id, $form_values) {

  $exists = db_query("SELECT *  FROM {astrology} WHERE id = :id", array(':id' => $form_values['values']['id']));
  if ($exists->fetchAssoc()) {
    db_update('astrology')
      ->fields(array(
        'name' => $form_values['values']['name'],
        'enabled' => $form_values['values']['enabled'],
        'about' => $form_values['values']['about']['value'],
        'about_format' => $form_values['values']['about']['format'],
      ))
      ->condition('id', $form_values['values']['id'], '=')
      ->execute();
    check_default_astrology($form_values['values']['id'], $form_values['values']['enabled']);
    drupal_set_message(t('Updated astrology %name', array('%name' => $form_values['values']['name'])));
  }
}

/**
 * Astrology add form.
 */
function astrology_add_form() {

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
  );
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
  );
  $form['about'] = array(
    '#type' => 'text_format',
    '#format' => 'full_html',
    '#title' => t('Description'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Astrology add form_validate.
 */
function astrology_add_form_validate(&$form_id, &$form_values) {

  // Check if astrology name exists.
  $result = db_query("SELECT name FROM {astrology} WHERE name = :name ", array(':name' => $form_values['values']['name']));
  if ($result->rowCount() > 0) {
    form_set_error('name', t('Astrology name !name is already taken', array(
      '!name' => $form_values['values']['name'],
    )));
    $form_values['rebuild'] = TRUE;
  }
}

/**
 * Astrology add form_submit.
 */
function astrology_add_form_submit($form_id, $form_values) {

  // Insert astrology to DB.
  $astrology_id = db_insert('astrology')
    ->fields(array(
      'name' => $form_values['values']['name'],
      'enabled' => $form_values['values']['enabled'],
      'about' => $form_values['values']['about']['value'],
      'about_format' => $form_values['values']['about']['format'],
    ))->execute();
  check_default_astrology($astrology_id, $form_values['values']['enabled'], TRUE);
  drupal_goto('admin/config/astrology/list');
  drupal_set_message(t('Astrology %name created', array('%name' => $form_values['values']['name'])));
}

/**
 * Delete astrology sign form.
 */
function astrology_delete_sign_form($form_state, $form_id, $astrology_id, $sign_id) {

  $astrology_sign = astrology_sign_array($sign_id, $astrology_id);
  $destination = drupal_get_destination();

  if ($astrology_sign['astrology_id'] == 1) {
    drupal_set_message(t("You can't delete this sign"), 'error');
    drupal_access_denied();
    drupal_exit();
  }

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_sign['id'],
  );
  $form['name'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_sign['name'],
  );
  $form['astrology_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_sign['astrology_id'],
  );
  return confirm_form(
   $form,
   t('Are you sure you want to delete !name?', array('!name' => $astrology_sign['name'])),
   $destination,
   t('This action cannot be undone.'),
   t('Delete'),
   t('Cancel')
   );
}

/**
 * Delete astrology sign form_submit.
 */
function astrology_delete_sign_form_submit($form_id, $form_values) {

  $astrology_id = $form_values['values']['astrology_id'];
  $sign_id = $form_values['values']['id'];

  // Delete all text for particular sign id, before deleting sign.
  db_delete('astrology_text')
    ->condition('astrology_sign_id', $sign_id, '=')
    ->execute();

  // Finally deleting sign.
  db_delete('astrology_signs')
    ->condition('id', $sign_id, '=')
    ->condition('astrology_id', $astrology_id, '=')
    ->execute();
  drupal_set_message(t("Sing %name deleted.", array('%name' => $form_values['values']['name'])));
}

/**
 * Edit astrology sign form.
 */
function astrology_edit_sign_form($form_state, $form_id, $astrology_id, $sign_id) {

  $astrology_sign = astrology_sign_array($sign_id, $astrology_id);
  $date_range_from = explode('/', $astrology_sign['date_from_range']);
  $date_range_to = explode('/', $astrology_sign['date_to_range']);
  $is_disabled = FALSE;
  if ($astrology_sign['astrology_id'] == 1) {
    $is_disabled = TRUE;
  }

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_sign['id'],
  );
  $form['astrology_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_sign['astrology_id'],
  );
  $form['name'] = [
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $astrology_sign['name'],
    '#disabled' => $is_disabled,
    '#required' => TRUE,
  ];
  $form['icon'] = [
    '#type' => 'file',
    '#title' => t('icon'),
  ];
  $form['date_range'] = [
    '#type' => 'fieldset',
    '#title' => t('Date range value'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  ];
  $form['date_range']['date_range_from'] = [
    '#type' => 'fieldset',
    '#title' => t('From date'),
  ];
  $form['date_range']['date_range_to'] = [
    '#type' => 'fieldset',
    '#title' => t('To date'),
  ];
  $form['date_range']['date_range_from']['from_date_month'] = [
    '#type' => 'select',
    '#title' => t('Month'),
    '#options' => months_array(),
    '#default_value' => $date_range_from[0],
    '#disabled' => $is_disabled,
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_from']['from_date_day'] = [
    '#type' => 'select',
    '#title' => t('Day'),
    '#options' => days_array(),
    '#default_value' => $date_range_from[1],
    '#disabled' => $is_disabled,
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_to']['to_date_month'] = [
    '#type' => 'select',
    '#title' => t('Month'),
    '#options' => months_array(),
    '#default_value' => $date_range_to[0],
    '#disabled' => $is_disabled,
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_to']['to_date_day'] = [
    '#type' => 'select',
    '#title' => t('Day'),
    '#options' => days_array(),
    '#default_value' => $date_range_to[1],
    '#disabled' => $is_disabled,
    '#required' => TRUE,
  ];
  $form['about'] = [
    '#type' => 'text_format',
    '#format' => $astrology_sign['about_sign_format'],
    '#title' => t('Description'),
    '#default_value' => $astrology_sign['about_sign'],
    '#required' => TRUE,
  ];
  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
  ];
  if ($is_disabled) {
    $form['note'] = [
      '#type' => 'fieldset',
      '#title' => t('Note'),
      '#description' => t('Sign <strong>:sign</strong> belongs to the default astrology zodiac, hence you are only allowed to edit few information.', [':sign' => $astrology_sign['name']]),
    ];
  }
  return $form;
}

/**
 * Edit astrology sign form_validate.
 */
function astrology_edit_sign_form_validate(&$form_id, &$form_values) {
  // Check if astrology name exists.
  $name = $form_values['values']['name'];
  $sign_id = $form_values['values']['id'];
  $query = db_select('astrology_signs', 'a')
    ->fields('a', ['name'])
    ->condition('name', $name)
    ->condition('id', $sign_id, '<>')
    ->execute();
  if ($query->rowCount()) {
    form_set_error('name', t('Astrology name !name is already taken', array(
      '!name' => $form_values['values']['name'],
    )));
  }
}

/**
 * Edit astrology sign form_submit.
 */
function astrology_edit_sign_form_submit($form_id, $form_values) {
  $file_path = 'public://astrology/image';
  file_prepare_directory($file_path, FILE_CREATE_DIRECTORY);
  $from_date = $form_values['values']['from_date_month'];
  $about_sign = $form_values['values']['about'];
  $from_date .= '/' . $form_values['values']['from_date_day'];
  $to_date = $form_values['values']['to_date_month'];
  $to_date .= '/' . $form_values['values']['to_date_day'];

  $validators = array();
  if ($file = file_save_upload('icon', $validators, $file_path, FILE_EXISTS_REPLACE)) {
    $file->status = FILE_STATUS_PERMANENT;
    db_update('astrology_signs')
      ->fields(array(
        'name' => $form_values['values']['name'],
        'icon' => $file->uri,
        'date_from_range' => $from_date,
        'date_to_range' => $to_date,
        'about_sign' => $about_sign['value'],
        'about_sign_format' => $about_sign['format'],
      ))
      ->condition('id', $form_values['values']['id'], '=')
      ->condition('astrology_id', $form_values['values']['astrology_id'], '=')
      ->execute();
    file_save($file);
    drupal_set_message(t('Updated %name', array('%name' => $form_values['values']['name'])));
  }
  else {
    db_update('astrology_signs')
      ->fields(array(
        'name' => $form_values['values']['name'],
        'date_from_range' => $from_date,
        'date_to_range' => $to_date,
        'about_sign' => $about_sign['value'],
        'about_sign_format' => $about_sign['format'],
      ))
      ->condition('id', $form_values['values']['id'], '=')
      ->condition('astrology_id', $form_values['values']['astrology_id'], '=')
      ->execute();
    drupal_set_message(t('Updated %name', array('%name' => $form_values['values']['name'])));
  }
}

/**
 * Add astrology sign form.
 */
function astrology_add_sign_form($form_state, $form_id, $astrology_id) {

  $destination = drupal_get_destination();
  if ($astrology_id == 1) {
    drupal_set_message(t('No more sign can be added'), 'error');
    drupal_goto($destination);
  }
  $form['astrology_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_id,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
  );
  $form['icon'] = array(
    '#type' => 'file',
    '#title' => t('icon'),
  );
  $form['date_range'] = [
    '#type' => 'fieldset',
    '#title' => t('Date range value'),
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  ];
  $form['date_range']['date_range_from'] = [
    '#type' => 'fieldset',
    '#title' => t('From date'),
  ];
  $form['date_range']['date_range_to'] = [
    '#type' => 'fieldset',
    '#title' => t('To date'),
  ];
  $form['date_range']['date_range_from']['from_date_month'] = [
    '#type' => 'select',
    '#title' => t('Month'),
    '#options' => months_array(),
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_from']['from_date_day'] = [
    '#type' => 'select',
    '#title' => t('Day'),
    '#options' => days_array(),
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_to']['to_date_month'] = [
    '#type' => 'select',
    '#title' => t('Month'),
    '#options' => months_array(),
    '#required' => TRUE,
  ];
  $form['date_range']['date_range_to']['to_date_day'] = [
    '#type' => 'select',
    '#title' => t('Day'),
    '#options' => days_array(),
    '#required' => TRUE,
  ];
  $form['about'] = [
    '#type' => 'text_format',
    '#format' => 'full_html',
    '#title' => t('Description'),
    '#required' => TRUE,
  ];
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Add astrology sign form_validate.
 */
function astrology_add_sign_form_validate(&$form_id, &$form_values) {

  // Check if astrology name exists.
  $name = $form_values['values']['name'];
  $query = db_select('astrology_signs', 'a')
    ->fields('a', ['name'])
    ->condition('name', $name)
    ->execute();
  if ($query->rowCount() > 0) {
    form_set_error('name', t('Sign name "!name" is already taken', ['!name' => $name]));
    $form_values['rebuild'] = TRUE;
  }
}

/**
 * Add astrology sign form_submit.
 */
function astrology_add_sign_form_submit($form_id, $form_values) {

  $file_path = 'public://astrology/image';
  $validators = array();

  $from_date = $form_values['values']['from_date_month'];
  $to_date = $form_values['values']['to_date_month'];
  $about = $form_values['values']['about'];

  $from_date .= '/' . $form_values['values']['from_date_day'];
  $to_date .= '/' . $form_values['values']['to_date_day'];

  file_prepare_directory($file_path, FILE_CREATE_DIRECTORY);
  if ($file = file_save_upload('icon', $validators, $file_path, FILE_EXISTS_REPLACE)) {
    $file->status = FILE_STATUS_PERMANENT;
    db_insert('astrology_signs')
      ->fields(array(
        'astrology_id' => $form_values['values']['astrology_id'],
        'name' => $form_values['values']['name'],
        'icon' => $file->uri,
        'date_from_range' => $from_date,
        'date_to_range' => $to_date,
        'about_sign' => $about['value'],
        'about_sign_format' => $about['format'],
      ))
      ->execute();
    file_save($file);
  }
  else {
    db_insert('astrology_signs')
      ->fields(array(
        'astrology_id' => $form_values['values']['astrology_id'],
        'name' => $form_values['values']['name'],
        'date_from_range' => $from_date,
        'date_to_range' => $to_date,
        'about_sign' => $about['value'],
        'about_sign_format' => $about['format'],
      ))
      ->execute();
  }
  drupal_goto('admin/config/astrology/list/' . $form_values['values']['astrology_id'] . '/signs');
  drupal_set_message(t('Sign %name added', array('%name' => $form_values['values']['name'])));
}

/**
 * Astrology delete form.
 */
function astrology_delete_form($form_state, $form_id, $astrology_id) {

  $destination = drupal_get_destination();

  if ($astrology_id == 1) {
    drupal_set_message(t("You can't delete this astrology"), 'error');
    drupal_access_denied();
    drupal_exit();
    drupal_goto($destination);
  }
  if (variable_get('astrology_default') == $astrology_id) {
    drupal_set_message(t("You can't delete this astrology, as currently its enabled, please disable to delete."), 'error');
    drupal_goto($destination);
  }

  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_id,
  );
  return confirm_form(
   $form,
   t('Are you sure you want to delete this item?'),
   $destination,
   t('This action cannot be undone.'),
   t('Delete'),
   t('Cancel')
   );
}

/**
 * Astrology delete form_submit.
 */
function astrology_delete_form_submit($form_id, $form_values) {

  $astrology_id = $form_values['values']['id'];
  $result = db_query("SELECT enabled,name FROM {astrology} WHERE id = :id ",
  array(':id' => $astrology_id));
  $row = $result->fetchObject();

  // Get all sign ids.
  $sign_ids = db_query("SELECT id FROM {astrology_signs} WHERE astrology_id = :id ",
  array(':id' => $astrology_id));

  // Delete all text for particular sign id.
  while ($ids = $sign_ids->fetchObject()) {
    $num_deleted = db_delete('astrology_text')
      ->condition('astrology_sign_id', $ids->id)
      ->execute();
  }

  // Now deleted all sign ids.
  $num_deleted = db_delete('astrology_signs')
    ->condition('astrology_id', $astrology_id)
    ->execute();

  // Finally delete astrology.
  $num_deleted = db_delete('astrology')
    ->condition('id', $astrology_id)
    ->execute();

  if ($num_deleted) {
    drupal_set_message(t("Astrology %name deleted.", array('%name' => $row->name)));
  }
}

/**
 * Astrology add sign text form.
 */
function astrology_add_text_form($form_id, $form_values, $astrology_id, $sign_id) {

  /*Only day and week format uses datepicker*/
  /*Jquery UI Theme not included by default*/
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'astrology') . '/js/add_text.js');
  $astrology_admin_format = variable_get('astrology_admin_format', 'day');

  switch ($astrology_admin_format) {
    default:
    case 'day':
      $format = 'z';
      $value = array(
        '#type' => 'textfield',
        '#size' => 10,
        '#title' => t('Select Day'),
        '#required' => TRUE,
      );
      break;
    case 'week':
      $format = 'W';
      $value = array(
        '#type' => 'textfield',
        '#size' => 10,
        '#title' => t('Select Week'),
        '#required' => TRUE,
      );
      break;

    case 'month':
      $format = 'n';
      $value = array(
        '#type' => 'select',
        '#title' => t('Select Month'),
        '#options' => months_array(),
        '#default_value' => date('n'),
      );
      break;

    case 'year':
      $format = 'o';
      $value = array(
        '#type' => 'select',
        '#title' => t('Select Year'),
        '#options' => years_array(),
        '#default_value' => date('o'),
      );
      break;
  }

  $options = astrology_sign_array($sign_id, $astrology_id);
  $form['label'] = array(
    '#markup' => t('<strong>!name</strong>', array('!name' => $options['name'])),
  );
  $form['astrology_sign_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $sign_id,
  );
  $form['format_character'] = array(
    '#type' => 'hidden',
    '#default_value' => $format,
  );
  $form['astrology_admin_format'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_admin_format,
  );
  $form['value'] = $value;
  $form['text'] = array(
    '#type' => 'text_format',
    '#format' => 'full_html',
    '#title' => t('Enter Text'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['note'] = array(
    '#type' => 'fieldset',
    '#title' => t('Note'),
    '#description' => t('You can change format ":format" setting, under administer setting tabs on the astrology configuration page  to search for other formats.', [':format' => $astrology_admin_format]),
  );
  return $form;
}

/**
 * Astrology add sign text form_submit.
 */
function astrology_add_text_form_submit($form_id, $form_values) {

  $values = $form_values['values'];
  switch ($values['format_character']) {
    case 'z':
      $post_date = strtotime($values['value']);
      // Get day number of the year.
      $date = get_format_date_value('z', $values['value']);
      $date_message = dayofyear2date($date + 1, 'l j F');
      break;

    case 'W':
      $post_date = strtotime($values['value']);
      // Get week number of the year.
      $date = get_format_date_value('W', $values['value']);
      // Get first and last day of week.
      $weeks = get_first_last_dow($post_date);
      $date_message = date('j, M', $weeks[0]) . ' to ' . date('j, M', $weeks[1]);
      break;

    case 'n':
      $date = $values['value'];
      // Get month timestamps.
      $post_date = mktime(0, 0, 0, $values['value']);
      $months = months_array();
      $date_message = $months[$date];
      break;

    case 'o':
      $date = $values['value'];
      // Get year timestamps.
      $post_date = mktime(0, 0, 0, 1, 1, $values['value']);
      $date_message = $values['value'];
      break;
  }
  // Check if data for sign already exists for this format.
  $result = db_select('astrology_text', 'at')
    ->fields('at', array('id', 'astrology_sign_id'))
    ->condition('astrology_sign_id', $values['astrology_sign_id'], '=')
    ->condition('format_character', $values['format_character'], '=')
    ->condition('value', $date, '=')
    ->execute();

  if ($result->rowCount()) {
    $row = $result->fetchObject();
    db_update('astrology_text')
      ->fields([
        'text' => $values['text']['value'],
        'text_format' => $values['text']['format'],
      ])
      ->condition('id', $row->id, '=')
      ->execute();
    drupal_set_message(t('Text updated for the :format <strong>:date</strong>.', [
      ':format' => $values['astrology_admin_format'],
      ':date' => $date_message,
    ]), 'warning');
  }
  else {
    db_insert('astrology_text')
      ->fields([
        'astrology_sign_id' => $values['astrology_sign_id'],
        'format_character' => $values['format_character'],
        'value' => $date,
        'text' => $values['text']['value'],
        'text_format' => $values['text']['format'],
        'post_date' => $post_date,
      ])
      ->execute();
    drupal_set_message(t('Text added for the :format <strong>:date</strong>.', [
      ':format' => $values['astrology_admin_format'],
      ':date' => $date_message,
    ]));
  }
}

/**
 * Astrology sign replace text for existing date and format form.
 */
function astrology_sign_replace_text_form($form_state, $form_id, $astrology_id, $sign_id) {

  $destination = 'admin/config/astrology/list/' . $astrology_id . '/signs/' . $sign_id . '/add_text';

  $form['astrology_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $astrology_id,
  );
  $form['astrology_sign_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $sign_id,
  );
  return confirm_form(
   $form,
   t('It seems there is already data for this, would you like to update?'),
   $destination,
   t('This action cannot be undone.'),
   t('Update'),
   t('Cancel')
   );
}

/**
 * Astrology sign replace text for existing date and format form.
 */
function astrology_sign_replace_text_form_submit($form_id, $form_values) {

  $astrology_id = $form_values['values']['astrology_id'];
  $sign_id = $form_values['values']['astrology_sign_id'];
  $text = variable_get('text', NULL);
  db_update('astrology_text')
    ->fields(array(
      'text' => $text,
    ))
    ->condition('id', $sign_id, '=')
    ->execute();
  drupal_set_message(t("Data updated !"));
  drupal_goto('admin/config/astrology/list/' . $astrology_id . '/signs/' . $sign_id . '/add_text');
}

/**
 * Astrology search text form.
 */
function astrology_search_text_form($form_id, $form_value, $astrology_id) {

  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'astrology') . '/js/add_text.js');

  $format = variable_get('astrology_admin_format', 'day');
  $default_sign_id = variable_get('astrology_sign_id', 0);
  $options = astrology_sign_list_array($astrology_id);

  switch ($format) {
    default:
    case 'day':
      $date_message = variable_get('astrology_admin_cdate', date('m/d/Y'));
      $value = array(
        '#type' => 'textfield',
        '#size' => 10,
        '#date_date_format' => 'm/d/Y',
        '#title' => t('Day'),
        '#required' => TRUE,
        '#default_value' => variable_get('astrology_admin_cdate', date('m/d/Y')),
      );
      $sign_id = array(
        '#type' => 'select',
        '#title' => t('Sign'),
        '#options' => $options,
        '#default_value' => $default_sign_id,
      );
      break;
    case 'week':
      $newdate = variable_get('astrology_admin_cdate', date('m/d/Y'));
      $post_date = strtotime($newdate);
      $weeks = get_first_last_dow($post_date);
      // Get week number of the year.
      $date_message = date('j, M', $weeks[0]) . ' to ' . date('j, M', $weeks[1]);
      $value = array(
        '#type' => 'textfield',
        '#size' => 10,
        '#date_date_format' => 'm/d/Y',
        '#title' => t('Week'),
        '#required' => TRUE,
        '#default_value' => $newdate,
      );
      $sign_id = array(
        '#type' => 'select',
        '#title' => t('Sign'),
        '#options' => $options,
        '#default_value' => $default_sign_id,
      );
      break;

    case 'month':
      $newdate = variable_get('astrology_admin_cdate', date('n'));
      $months = months_array();
      $date_message = $months[$newdate];
      $value = array(
        '#type' => 'select',
        '#title' => t('Month'),
        '#options' => $months,
        '#default_value' => $newdate,
      );
      $sign_id = array(
        '#type' => 'select',
        '#title' => t('Sign'),
        '#options' => $options,
        '#default_value' => $default_sign_id,
      );
      break;

    case 'year':
      $date_message = variable_get('astrology_admin_cdate', date('o'));
      $value = array(
        '#type' => 'select',
        '#title' => t('Year'),
        '#options' => years_array(),
        '#default_value' => variable_get('astrology_admin_cdate', date('o')),
      );
      $sign_id = array(
        '#type' => 'select',
        '#title' => t('Sign'),
        '#options' => $options,
        '#default_value' => $default_sign_id,
      );
      break;
  }

  $form['fordate'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only items where'),
  );
  if (variable_get('astrology_admin_cdate')) {
    $form['fordate']['item'] = [
      '#markup' => t('<li> where sign <strong>:sign-name</strong> </li> <li>where :format <strong>:date</strong>.</li>', [
        ':sign-name' => array_key_exists($default_sign_id, $options) ? $options[$default_sign_id] : $options[0],
        ':format' => $format,
        ':date' => $date_message,
      ]),
    ];
  }
  $form['fordate']['astrology_sign_id'] = $sign_id;
  $form['fordate']['value'] = $value;
  $form['fordate']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Find'),
  );
  $form['note'] = [
    '#type' => 'fieldset',
    '#title' => t('Note'),
    '#description' => t('You can change format ":format" setting, under administer setting tabs on the astrology configuration page  to search for other formats.', [':format' => $format]),
  ];
  return $form;
}

/**
 * Astrology search text form_submit.
 */
function astrology_search_text_form_submit($form_id, $form_value) {
  $cdate = $form_value['values']['value'];
  $sign_id = $form_value['values']['astrology_sign_id'];
  variable_set('astrology_admin_cdate', $cdate);
  variable_set('astrology_sign_id', $sign_id);
}

/**
 * Astrology edit sign text form.
 */
function astrology_edit_text_form($form_id, $form_value, $astrology_id, $sign_id, $text_id) {

  $result = is_valid_text($sign_id, $text_id);
  $options = astrology_sign_array($sign_id, $astrology_id);
  $form['label'] = array(
    '#markup' => t('Name <strong>:name</strong>', array(
      ':name' => $options['name'],
    )),
  );
  $form['id'] = array(
    '#type' => 'hidden',
    '#default_value' => $result->id,
  );
  $form['text'] = array(
    '#type' => 'text_format',
    '#format' => $result->text_format,
    '#title' => t('Enter Text'),
    '#required' => TRUE,
    '#default_value' => $result->text,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Astrology edit sign text form_submit.
 */
function astrology_edit_text_form_submit($form_id, $form_value) {
  db_update('astrology_text')
    ->fields(array(
      'text' => $form_value['values']['text']['value'],
      'text_format' => $form_value['values']['text']['format'],
    ))
    ->condition('id', $form_value['values']['id'], '=')
    ->execute();
  drupal_set_message(t("Data updated !"));
}

/**
 * Set Default astrology.
 *
 * Set Default Format (day, week, month or year).
 *
 * Default astrology: Zodiac (id=1).
 *
 * Default format: day.
 *
 * Astrology settings form.
 */
function astrology_set_default_form() {

  $formats = array(
    'day' => t('Day'),
    'week' => t('Week'),
    'month' => t('Month'),
    'year' => t('Year'),
  );
  $options = astrology_get_list_array();
  $form['settings'] = array(
    '#type' => 'vertical_tabs',
  );
  // Site wide setting tab.
  $form['site'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site wide settings'),
    '#group' => 'settings',
    '#description' => t('This setting will be available for non administrative activities like visiting sign page etc.'),
  );
  // Administer setting tab.
  $form['administer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Administer settings'),
    '#group' => 'settings',
    '#description' => t('Select format to performed administrative task, like to add and search text.'),
  );
  $formats = array(
    'day' => t('Day'),
    'week' => t('Week'),
    'month' => t('Month'),
    'year' => t('Year'),
  );
  $form['site']['astrology_format'] = array(
    '#type' => 'select',
    '#title' => t('Format'),
    '#options' => $formats,
    '#default_value' => variable_get('astrology_format', 'day'),
  );
  $form['administer']['astrology_admin_format'] = array(
    '#type' => 'select',
    '#title' => t('Format'),
    '#options' => $formats,
    '#default_value' => variable_get('astrology_admin_format', 'day'),
  );
  $form['site']['astrology_default'] = array(
    '#type' => 'select',
    '#title' => t('Astrology'),
    '#options' => $options,
    '#default_value' => variable_get('astrology_default', 1),
  );
  $form['site']['item-check'] = array(
    '#type' => 'fieldset',
    '#group' => 'site',
    '#title' => t('Display sign information'),
    '#description' => t('Enable to display sign information section along with text, on the sign text page.'),
  );
  $form['site']['item-check']['astrology_sign_info'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sign information block'),
    '#default_value' => variable_get('astrology_sign_info', 1),
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/**
 * Astrology settings form_submit.
 */
function astrology_set_default_form_submit($form_id, $form_values) {

  $astrology_sign_info = $form_values['values']['astrology_sign_info'];
  $astrology_format = $form_values['values']['astrology_format'];
  $astrology_default = $form_values['values']['astrology_default'];
  $astrology_admin_format = $form_values['values']['astrology_admin_format'];

  switch ($astrology_admin_format) {
    case 'day':
      $astrology_admin_cdate = date('m/d/Y');
      break;

    case 'week':
      $astrology_admin_cdate = date('m/d/Y');
      break;

    case 'month':
      $astrology_admin_cdate = date('n');
      break;

    case 'year':
      $astrology_admin_cdate = date('o');
      break;
  }
  $result = db_query("SELECT id,name FROM {astrology} WHERE id = :id ", array(
    ':id' => $astrology_default,
  ));
  $row = $result->fetchObject();
  // Disable all astrology.
  db_update('astrology')
    ->fields(array(
      'enabled' => 0,
    ))->execute();

  // Enable all astrology.
  db_update('astrology')
    ->fields(array(
      'enabled' => 1,
    ))
    ->condition('id', $row->id, '=')
    ->execute();

  variable_set('astrology_sign_id', 0);
  variable_set('astrology_admin_cdate', $astrology_admin_cdate);
  variable_set('astrology_sign_info', $astrology_sign_info);
  variable_set('astrology_default', $astrology_default);
  variable_set('astrology_format', $astrology_format);
  variable_set('astrology_admin_format', $astrology_admin_format);
  drupal_set_message(t('The <strong>:astrology</strong> has been set as default, and data will be shown per :format', array(
    ':astrology' => $row->name,
    ':format' => $astrology_format,
  ))
  );
}
