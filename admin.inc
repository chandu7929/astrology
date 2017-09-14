<?php

/**
 * @file
 * Contains custom functions for astrology.
 */

 /**
  * Function returns day of the year using date function.
  */
function dayofyear2date($day, $format = 'd/m/Y') {
  $date = mktime(0, 0, 0, 1, $day);
  return date($format, $date);
}

/**
 * Returns formatted date value.
 */
function get_format_date_value($formatter, $newdate) {
  $timestamps = strtotime($newdate);
  $month = date('m', $timestamps);
  $day = date('d', $timestamps);
  $formated_value = date($formatter, mktime(0, 0, 0, $month, $day));
  return $formated_value;
}

/**
 * Returns first and last day of week.
 */
function get_first_last_dow($newdate = '') {
  // +1 for first day of week as Monday.
  $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 1);
  // +7 for last day of week as Sunday.
  $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 7);
  return $week_days;
}

/**
 * Array of months.
 */
function months_array() {
  $months_array = array(
    1 => t('January'),
    2 => t('February'),
    3 => t('March'),
    4 => t('April'),
    5 => t('May'),
    6 => t('June'),
    7 => t('July'),
    8 => t('August'),
    9 => t('September'),
    10 => t('October'),
    11 => t('November'),
    12 => t('December'),
  );
  return $months_array;
}

/**
 * Array of days.
 */
function days_array() {
  return array_combine(range(1, 31), range(1, 31));
}

/**
 * Check for default set astrology.
 */
function check_default_astrology($astrology_id, $status, $op = FALSE) {

  $astrology_default = variable_get('astrology_default');

  if ($op && $status == '1') {
    db_update('astrology')
      ->fields(array(
        'enabled' => '0',
      ))->condition('id', $astrology_default, '=')
      ->execute();

    variable_set('astrology_default', $astrology_id);
  }
  elseif ($astrology_id != $astrology_default && $status) {
    db_update('astrology')
      ->fields(array(
        'enabled' => '0',
      ))->condition('id', $astrology_default, '=')
      ->execute();

    variable_set('astrology_default', $astrology_id);
  }
  elseif ($astrology_id == $astrology_default && !$status) {
    db_update('astrology')
      ->fields(array(
        'enabled' => '1',
      ))->condition('id', 1, '=')
      ->execute();
    variable_set('astrology_default', 1);
  }
}

/**
 * Array of years, next 10 years.
 */
function years_array() {
  $years_array = array_combine(range(date("Y") - 5, date("Y") + 5), range(date("Y") - 5, date("Y") + 5));
  return $years_array;
}

/**
 * Get associative array of astrology.
 */
function astrology_get_array($astrology_id) {
  $astrology = db_select('astrology', 'a')
    ->fields('a')
    ->condition('id', $astrology_id, '=')
    ->execute()
    ->fetchAssoc();
  return $astrology;
}

/**
 * Get astrology List, returns array('id' => int, 'name' => string, ..)
 */
function astrology_get_list_array() {
  $data = array();
  $rows = db_select('astrology', 'a')
    ->fields('a', array('id', 'name'))
    ->execute();
  while ($row = $rows->fetchObject()) {
    $data[$row->id] = $row->name;
  }
  return $data;
}

/**
 * Get associative array of astrological sign.
 */
function astrology_sign_array($sign_id, $astrology_id) {

  $query = db_select('astrology_signs', 'as_')
    ->fields('as_')
    ->condition('id', $sign_id, '=')
    ->condition('astrology_id ', $astrology_id, '=')
    ->execute();
  if (!$query->rowCount()) {
    drupal_not_found();
    drupal_exit();
  }
  return $query->fetchAssoc();
}

/**
 * Return TRUE if text_id belongs to a valid sign.
 */
function is_valid_text($sign_id, $text_id) {

  $query = db_select('astrology_text', 'at_')
    ->fields('at_', ['id', 'text', 'text_format'])
    ->condition('id', $text_id, '=')
    ->condition('astrology_sign_id ', $sign_id, '=')
    ->execute();
  if (!$query->rowCount()) {
    drupal_not_found();
    drupal_exit();
  }
  $result = $query->fetchObject();
  return $result;
}

/**
 * Astrology sign list, returns array('id' => int, 'name' => string, ..)
 */
function astrology_sign_list_array($astrology_id) {
  $data = array();
  $data[0] = 'ALL';

  $rows = db_select('astrology_signs', 'as_')
    ->fields('as_', array('id', 'name'))
    ->condition('astrology_id', $astrology_id, '=')
    ->execute();
  while ($row = $rows->fetchObject()) {
    $data[$row->id] = $row->name;
  }

  return $data;
}

/**
 * Generate operations link example edit delete etc.
 */
function astrology_get_operations_link($row, $context) {

  // Build a list of all the accessible operations for the current Astrology.
  $operations = array();
  $destination = drupal_get_destination();
  switch ($context) {
    case 'astrology-list-one':

      $operations['list_sign'] = array(
        'title' => t('signs'),
        'href' => "admin/config/astrology/list/$row->id/signs",
      );
      $operations['list_text'] = array(
        'title' => t('text'),
        'href' => "admin/config/astrology/list/$row->id/list_text",
      );
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => "admin/config/astrology/list/$row->id/edit",
        'query' => $destination,
      );
      break;

    case 'astrology-list-other':
      $operations['list_sign'] = array(
        'title' => t('signs'),
        'href' => "admin/config/astrology/list/$row->id/signs",
      );
      $operations['list_text'] = array(
        'title' => t('text'),
        'href' => "admin/config/astrology/list/$row->id/list_text",
      );
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => "admin/config/astrology/list/$row->id/edit",
        'query' => $destination,
      );
      $operations['add_sign'] = array(
        'title' => t('add sign'),
        'href' => "admin/config/astrology/list/$row->id/add_sign",
      );
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => "admin/config/astrology/list/$row->id/delete",
        'query' => $destination,
      );
      break;

    case 'astrology-list-sign-one':
      $operations['Add text'] = array(
        'title' => t('add text'),
        'href' => "admin/config/astrology/list/$row->astrology_id/signs/$row->id/add_text",
      );
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => "admin/config/astrology/list/$row->astrology_id/signs/$row->id/edit",
        'query' => $destination,
      );
      break;

    case 'astrology-list-sign-other':
      $operations['Add text'] = array(
        'title' => t('add text'),
        'href' => "admin/config/astrology/list/$row->astrology_id/signs/$row->id/add_text",
      );
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => "admin/config/astrology/list/$row->astrology_id/signs/$row->id/edit",
        'query' => $destination,
      );
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => "admin/config/astrology/list/$row->astrology_id/signs/$row->id/delete",
        'query' => $destination,
      );
      break;
  }
  return $operations;
}

/**
 * Display table of astrology with names, enabled and description.
 */
function astrology_list() {
  $data = array();
  $header = array(
    t('Name'),
    t('Description'),
    t('Enabled'),
    t('Operations'),
  );

  $rows = db_select('astrology', 'a')
    ->fields('a')
    ->execute();

  while ($row = $rows->fetchObject()) {
    if ($row->id == 1) {

      $operations = astrology_get_operations_link($row, 'astrology-list-one');
      $data[] = array(
        'name' => l($row->name, "admin/config/astrology/list/$row->id/signs"),
        'description' => text_summary($row->about, $row->about_format),
        'enabled' => ($row->enabled == 1) ? t('<strong>Yes</strong>') : t('No'),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__astrology_operations',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }
    else {
      $operations = astrology_get_operations_link($row, 'astrology-list-other');
      $data[] = array(
        'name' => l($row->name, "admin/config/astrology/list/$row->id/signs"),
        'description' => text_summary($row->about, $row->about_format),
        'enabled' => ($row->enabled == 1) ? t('<strong>Yes</strong>') : t('No'),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__astrology_operations',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }
  }

  $output = theme('table', array(
    'header'  => $header,
    'rows'    => $data,
    'id'      => 'astrology_table',
    'class'   => 'astrology',
  ));

  $astrology_default_form = drupal_get_form('astrology_set_default_form');
  $output .= drupal_render($astrology_default_form);
  return $output;
}

/**
 * Display table of astrological signs.
 */
function astrology_list_signs($arg) {
  $data = array();
  $header = array(t('Icon'), t('Name'), t('Description'), t('Operations'));
  $query = query_all_signs($arg->id);
  while ($row = $query->fetchObject()) {

    $img_url = file_create_url($row->icon);
    $img = t('<img src="!img-url" alt="!alt" height="30" width="30" />', array(
      '!img-url' => $img_url,
      '!alt' => $row->name,
    ));
    if ($arg->id == 1) {
      $operations = astrology_get_operations_link($row, 'astrology-list-sign-one');
      $data[] = array(
        'icon' => $img,
        'name' => t('!name', array('!name' => $row->name)),
        'description' => text_summary($row->about_sign, $row->about_sign_format),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__astrology_operations',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }
    else {
      $operations = astrology_get_operations_link($row, 'astrology-list-sign-other');
      $data[] = array(
        'icon' => $img,
        'name' => t('!name', array('!name' => $row->name)),
        'description' => text_summary($row->about_sign, $row->about_sign_format),
        'operations' => array(
          'data' => array(
            '#theme' => 'links__astrology_operations',
            '#links' => $operations,
            '#attributes' => array('class' => array('links', 'inline')),
          ),
        ),
      );
    }
  }
  $message = t('There is no any sign added yet,
   click <a href="@add-sign">Add sign</a> to add sign.', array(
     '@add-sign' => url("admin/config/astrology/list/$arg->id/add_sign"),
   ));
  $table = theme('table', array(
    "header" => $header,
    "rows" => $data,
    "caption" => '',
    "empty" => $message,
  ));

  return $table;
}

/**
 * Astrology list sign text.
 */
function astrology_list_text($astrology_id) {

  $formater = variable_get('astrology_admin_format', 'day');
  $astrology_sign = variable_get('astrology_sign_id');

  switch ($formater) {
    case 'day':
      $format = 'z';
      $newdate = variable_get('astrology_admin_cdate', date('m/d/Y'));
      $timestmp = get_format_date_value('z', $newdate);
      if ($astrology_sign) {
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));
        $query->fields('hs', array('icon', 'name', 'astrology_id'));
        $query->fields('h', array('id'));
        $query->condition('h.id', $astrology_id, '=')
          ->condition('hs.id', $astrology_sign, '=')
          ->condition('ht.format_character', $format, '=')
          ->condition('ht.value', $timestmp, '=')
          ->execute();
      }
      else {
        $subquery = db_select('astrology', 'h');
        $subquery->join('astrology_signs', 'hs', 'hs.astrology_id = h.id');
        $subquery->fields('hs', array('id'))
          ->condition('hs.astrology_id', $astrology_id, '=');
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));
        $query->fields('hs', array('icon', 'name'));
        $query->fields('h', array('id'));
        $query->extend('TableSort')->extend('PagerDefault')->limit(5)
          ->condition('h.id', $astrology_id, '=')
          ->condition('astrology_sign_id', $subquery, 'IN')
          ->condition('format_character', $format, '=')
          ->condition('value', $timestmp, '=')
          ->execute();
      }
      break;

    case 'week':
      $format = 'W';
      $newdate = variable_get('astrology_admin_cdate', date('m/d/Y'));
      $timestmp = get_format_date_value('W', $newdate);
      if ($astrology_sign) {
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name', 'astrology_id'));
        $query->fields('h', array('id'));
        $query->condition('h.id', $astrology_id, '=')
          ->condition('hs.id', $astrology_sign, '=')
          ->condition('ht.format_character', $format, '=')
          ->condition('ht.value', $timestmp, '=')
          ->execute();
      }
      else {
        $subquery = db_select('astrology', 'h');
        $subquery->join('astrology_signs', 'hs', 'hs.astrology_id = h.id');
        $subquery->fields('hs', array('id'))
          ->condition('hs.astrology_id', $astrology_id, '=');
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name'));
        $query->fields('h', array('id'));
        $query->extend('TableSort')->extend('PagerDefault')->limit(5)
          ->condition('h.id', $astrology_id, '=')
          ->condition('astrology_sign_id', $subquery, 'IN')
          ->condition('format_character', $format, '=')
          ->condition('value', $timestmp, '=')
          ->execute();
      }
      break;

    case 'month':
      $format = 'n';
      $timestmp = variable_get('astrology_admin_cdate', date('n'));
      if ($astrology_sign) {
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name', 'astrology_id'));
        $query->fields('h', array('id'));
        $query->condition('h.id', $astrology_id, '=')
          ->condition('hs.id', $astrology_sign, '=')
          ->condition('ht.format_character', $format, '=')
          ->condition('ht.value', $timestmp, '=')
          ->execute();
      }
      else {
        $subquery = db_select('astrology', 'h');
        $subquery->join('astrology_signs', 'hs', 'hs.astrology_id = h.id');
        $subquery->fields('hs', array('id'))
          ->condition('hs.astrology_id', $astrology_id, '=');
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name'));
        $query->fields('h', array('id'));
        $query->extend('TableSort')->extend('PagerDefault')->limit(5)
          ->condition('h.id', $astrology_id, '=')
          ->condition('astrology_sign_id', $subquery, 'IN')
          ->condition('format_character', $format, '=')
          ->condition('value', $timestmp, '=')
          ->execute();
      }
      $month = date('F, Y', mktime(0, 0, 0, (int) $timestmp, date('d'), date('y')));
      break;

    case 'year':
      $format = 'o';
      $timestmp = variable_get('astrology_admin_cdate', date('o'));
      if ($astrology_sign) {
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name', 'astrology_id'));
        $query->fields('h', array('id'));
        $query->condition('h.id', $astrology_id, '=')
          ->condition('hs.id', $astrology_sign, '=')
          ->condition('ht.format_character', $format, '=')
          ->condition('ht.value', $timestmp, '=')
          ->execute();
      }
      else {
        $subquery = db_select('astrology', 'h');
        $subquery->join('astrology_signs', 'hs', 'hs.astrology_id = h.id');
        $subquery->fields('hs', array('id'))
          ->condition('hs.astrology_id', $astrology_id, '=');
        $query = db_select('astrology_text', 'ht');
        $query->join('astrology_signs', 'hs', 'hs.id = ht.astrology_sign_id');
        $query->join('astrology', 'h', 'hs.astrology_id = h.id');
        $query->fields('ht', array(
          'id', 'text', 'text_format', 'value',
          'astrology_sign_id', 'post_date',
        ));        $query->fields('hs', array('icon', 'name'));
        $query->fields('h', array('id'));
        $query->extend('TableSort')->extend('PagerDefault')->limit(5)
          ->condition('h.id', $astrology_id, '=')
          ->condition('astrology_sign_id', $subquery, 'IN')
          ->condition('format_character', $format, '=')
          ->condition('value', $timestmp, '=')
          ->execute();
      }
      break;
  }

  $headers = array(
    array('data' => t('Icon')),
    array('data' => t('Name')),
    array('data' => t('Text')),
    array('data' => t('!dated', ['!dated' => $formater])),
    array(
      'data' => t('action'),
    ),
  );

  $result = $query->execute();
  $rows = array();
  $destination = array('query' => array(drupal_get_destination()));

  // Loop through each item and add to the $rows array.
  foreach ($result as $row) {
    $icon = t('<img src="@src" alt="!alt" height="!height" width="!width" />',
      array(
        '@src' => file_create_url($row->icon),
        '!alt' => $row->name,
        '!height' => '30',
        '!width' => '30',
      ));
    $url = "admin/config/astrology/$astrology_id/sign/$row->astrology_sign_id/text/$row->id/edit";
    $edit = l(t('edit'), $url, $destination);
    $weeks = get_first_last_dow($row->post_date);
    $week = date('j, M', $weeks[0]) . ' to ' . date('j, M', $weeks[1]);

    $rows[] = array(
      $icon,
      t(':name', array(':name' => $row->name)),
      t(':text', array(
        ':text' => text_summary($row->text, $row->text_format),
      )),
      (($formater == 'day') ? date('j M,Y', $row->post_date) :
       (($formater == 'week') ? $week : (($formater == 'month') ?
       $month : $timestmp))),
      $edit,
    );
  }

  $table = theme('table',
    array(
      "header" => $headers,
      "rows" => $rows,
      "attributes" => array(),
      'caption' => '',
      "sticky" => TRUE,
      "colgroups" => array(),
      "empty" => t('No data found'),
    )) . theme("pager");
  $astrology_search_text_form = drupal_get_form('astrology_search_text_form', $astrology_id);
  $output = drupal_render($astrology_search_text_form) . $table;
  return $output;
}

/**
 * Rendered content for block astrology.
 */
function astrology_block_content() {

  $astrology_id = variable_get('astrology_default', 1);
  $formatter = variable_get('astrology_format', 'day');
  $query = db_select('astrology_signs', 's')
    ->fields('s')
    ->condition('s.astrology_id', $astrology_id, '=')
    ->execute();
  $signs = [];
  $blank_msg = NULL;

  while ($row = $query->fetchObject()) {
    $signs[] = $row;
  }
  if (user_access('administer') && empty($signs)) {
    $blank_msg = 'The default selected astrology does not contains any sign, please add one or more sign to display here.';
  }
  $build[] = array(
    '#theme' => 'astrology-block',
    '#formatter' => $formatter,
    '#signs' => $signs,
    '#blank_msg' => $blank_msg,
  );
  return $build;
}

/**
 * Returns astrological sign for given DOB.
 */
function astrology_get_birth_sign($sign_name) {

  $astrology_id = variable_get('astrology_default', 1);
  $formatter = variable_get('astrology_format', 'day');
  $query = db_select('astrology_signs', 'as_')
    ->fields('as_')
    ->condition('name', $sign_name, '=')
    ->condition('astrology_id ', $astrology_id, '=')
    ->execute();
  $sign = $query->fetchObject();
  if (!$query->rowCount()) {
    drupal_not_found();
    drupal_exit();
  }
  $about_sign_summary = text_summary($sign->about_sign, $sign->about_sign_format);
  $from_date = explode('/', $sign->date_from_range);
  $to_date = explode('/', $sign->date_to_range);
  $from_month = $from_date[0];
  $from_day = $from_date[1];
  $to_month = $to_date[0];
  $today = $to_date[1];
  $date_range_sign = date('M, j', mktime(0, 0, 0, $from_month, $from_day));
  $date_range_sign .= ' - ' . date('M, j', mktime(0, 0, 0, $to_month, $today));

  $content[] = array(
    '#theme' => 'get-birth-sign',
    '#sign' => $sign,
    '#formatter' => $formatter,
    '#date_range_sign' => $date_range_sign,
    '#about_sign_summary' => $about_sign_summary,
  );
  drupal_set_title(t('Your astrological sign is "!sign"', array('!sign' => $sign_name)));
  return $content;
}

/**
 * Sign details page.
 */
function astrology_get_sign_details($sign_name) {

  $astrology_id = variable_get('astrology_default', 1);
  $query = db_select('astrology_signs', 'as_')
    ->fields('as_')
    ->condition('name', $sign_name, '=')
    ->condition('astrology_id ', $astrology_id, '=')
    ->execute();
  $sign = $query->fetchObject();
  if (!$query->rowCount()) {
    drupal_not_found();
    drupal_exit();
  }
  $content[] = array(
    '#theme' => 'get-sign-detail',
    '#sign' => $sign,
  );
  drupal_set_title(t('About "!sign"', array('!sign' => $sign_name)));
  return $content;
}

/**
 * Get astrology text for specific sign and format.
 */
function astrology_get_astrology($sign_name, $formatter, $next_prev = NULL) {

  $astrology = variable_get('astrology_default', 1);
  $astrology_sign_info = variable_get('astrology_sign_info', 1);
  $format_char = $formatter;
  $allowed_format = ['day', 'week', 'month', 'year'];
  $query = db_select('astrology_signs', 'h')
    ->fields('h')
    ->condition('name', $sign_name, '=')
    ->condition('astrology_id ', $astrology, '=')
    ->execute();
  $sign = $query->fetchObject();
  if (!$query->rowCount() || !in_array($formatter, $allowed_format)) {
    drupal_not_found();
    drupal_exit();
  }
  if ($next_prev && !astrology_check_valid_date($formatter, $next_prev)) {
    drupal_not_found();
    drupal_exit();
  }
  $about_sign_summary = text_summary($sign->about_sign, $sign->about_sign_format);
  $from_date = explode('/', $sign->date_from_range);
  $to_date = explode('/', $sign->date_to_range);
  $from_month = $from_date[0];
  $from_day = $from_date[1];
  $to_month = $to_date[0];
  $today = $to_date[1];

  switch ($format_char) {
    case 'day':
      $format = 'z';
      $date = $next_prev ? $next_prev : date('z');
      $post_date = mktime(0, 0, 0, 1, ($date + 1), date('o'));
      $date_format = 'l, j F';
      break;

    case 'week':
      $format = 'W';
      $date = $next_prev ? $next_prev : date('W');
      $post_date = mktime(0, 0, 0, 1, (4 + 7 * ($date - 1)), date('o'));
      $date_format = 'j, M';
      break;

    case 'month':
      $format = 'n';
      $date = $next_prev ? $next_prev : date('n');
      $post_date = mktime(0, 0, 0, $date);
      $date_format = 'F';
      break;

    case 'year':
      $format = 'o';
      $date = $next_prev ? $next_prev : date('o');
      $date_format = 'Y';
      $post_date = mktime(0, 0, 0, 1, 1, $date);
      break;
  }

  $query1 = db_select('astrology_text', 'h')
    ->fields('h', ['text', 'id', 'post_date'])
    ->condition('value', $date)
    ->condition('astrology_sign_id', $sign->id)
    ->condition('format_character', $format)
    ->execute();
  $astrology_text = NULL;
  $text = $query1->fetchObject();
  if ($query1->rowCount()) {
    $astrology_text = $text->text;
  }

  if ($next_prev) {
    $next_prev_val = astrology_check_next_prev($formatter, $next_prev);
  }
  else {
    $next_prev = $date;
    $next_prev_val = astrology_check_next_prev($formatter, $next_prev);
  }

  $weeks = astrology_get_first_last_dow($post_date);

  $date_range_sign = date('M, j', mktime(0, 0, 0, $from_month, $from_day));
  $date_range_sign .= ' - ' . date('M, j', mktime(0, 0, 0, $to_month, $today));
  $content[] = array(
    '#theme' => 'get-astrology',
    '#sign' => $sign,
    '#sign_info' => $astrology_sign_info,
    '#formatter' => $formatter,
    '#date_format' => $date_format,
    '#astrology_text' => $astrology_text,
    '#about_sign_summary' => $about_sign_summary,
    '#post_date' => $post_date,
    '#weeks' => $weeks,
    '#date_range_sign' => $date_range_sign,
    '#for_date' => $next_prev_val,
  );
  return $content;
}

/**
 * Generate next previous link.
 */
function astrology_check_next_prev($formatter, $next_prev) {

  $next_prev_val = [];

  // If leap year, set number of days in year.
  $last_day_of_year = date('L') ? 365 : 364;

  // If leap year, set max week number in year.
  $last_week_of_year = date('L') ? 53 : 52;

  $last_month_of_year = 12;
  $max_year_range = date('Y') + 5;
  $min_year_range = date('Y') - 5;

  switch ($formatter) {
    case 'day':
      if ($next_prev < $last_day_of_year && $next_prev > 0) {
        $next_prev_val['next'] = $next_prev + 1;
        $next_prev_val['prev'] = ($next_prev - 1) ? $next_prev - 1 : '+0';
      }
      elseif ($next_prev == $last_day_of_year) {
        $next_prev_val['next'] = FALSE;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      elseif ($next_prev == '0') {
        $next_prev_val['next'] = 1;
        $next_prev_val['prev'] = FALSE;
      }
      break;

    case 'week':
      if ($next_prev >= 1 && $next_prev < $last_week_of_year) {
        $next_prev_val['next'] = $next_prev + 1;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      elseif ($next_prev == $last_week_of_year) {
        $next_prev_val['next'] = FALSE;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      break;

    case 'month':
      if ($next_prev >= 1 && $next_prev < $last_month_of_year) {
        $next_prev_val['next'] = $next_prev + 1;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      elseif ($next_prev == $last_month_of_year) {
        $next_prev_val['next'] = FALSE;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      break;

    case 'year':
      if ($next_prev > $min_year_range && $next_prev < $max_year_range) {
        $next_prev_val['next'] = $next_prev + 1;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      elseif ($next_prev == $max_year_range) {
        $next_prev_val['next'] = FALSE;
        $next_prev_val['prev'] = $next_prev - 1;
      }
      elseif ($next_prev == $min_year_range) {
        $next_prev_val['next'] = $next_prev + 1;
        $next_prev_val['prev'] = FALSE;
      }
      break;
  }
  return $next_prev_val;
}

/**
 * Returns first and last day of week.
 */
function astrology_get_first_last_dow($newdate = '') {

  // +1 for first day of week as Monday.
  $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 1);
  // +7 for last day of week as Sunday.
  $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 7);
  return $week_days;
}

/**
 * Check valid next prev date.
 */
function astrology_check_valid_date($formatter, $next_prev) {

  if (!is_numeric($next_prev)) {
    return FALSE;
  }

  // If leap year, set number of days in year.
  $last_day_of_year = date('L') ? 365 : 364;

  // If leap year, set max week number in year.
  $last_week_of_year = date('L') ? 53 : 52;

  $last_month_of_year = 12;
  $max_year_range = date('Y') + 5;
  $min_year_range = date('Y') - 5;

  if ($formatter == 'day' && ($next_prev < 0 || $next_prev > $last_day_of_year)) {
    return FALSE;
  }
  if ($formatter == 'week' && ($next_prev < 1 || $next_prev > $last_week_of_year)) {
    return FALSE;
  }
  if ($formatter == 'month' && ($next_prev < 1 || $next_prev > $last_month_of_year)) {
    return FALSE;
  }
  if ($formatter == 'year' && ($next_prev < $min_year_range || $next_prev > $max_year_range)) {
    return FALSE;
  }
  return TRUE;
}
