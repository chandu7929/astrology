<?php

namespace Drupal\astrology\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;

/**
 * Class UtilityController.
 */
class UtilityController extends ControllerBase {

  /**
   * Returns timestamps for the given date.
   */
  public function getTimestamps($date = '') {
    return strtotime($date);
  }

  /**
   * Returns first and last day of week.
   */
  public function getFirstLastDow($newdate = '') {
    // +1 for first day of week as Monday.
    $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 1);
    // +7 for last day of week as Sunday.
    $week_days[] = mktime(0, 0, 0, date("n", $newdate), date("j", $newdate) - date("N", $newdate) + 7);

    return $week_days;
  }

  /**
   * Returns formatted date value.
   */
  public function getFormatDateValue($formatter, $newdate) {
    $timestamps = strtotime($newdate);
    $month = date('m', $timestamps);
    $day = date('d', $timestamps);
    $formated_value = date($formatter, mktime(0, 0, 0, $month, $day));
    return $formated_value;
  }

  /**
   * Returns day of the year.
   */
  public function getDoy($day, $format = 'Y-m-d') {
    $date = mktime(0, 0, 0, 1, $day);
    return date($format, $date);
  }

  /**
   * Returns associative array of days.
   */
  public function getDaysArray() {
    $days_array = [];
    $i = 1;
    while ($i <= 31) {
      $days_array[$i] = $i;
      $i += 1;
    }
    return $days_array;
  }

  /**
   * Returns array of months.
   */
  public function getMonthsArray() {
    $months_array = [
      1 => $this->t('January'),
      2 => $this->t('February'),
      3 => $this->t('March'),
      4 => $this->t('April'),
      5 => $this->t('May'),
      6 => $this->t('June'),
      7 => $this->t('July'),
      8 => $this->t('August'),
      9 => $this->t('September'),
      10 => $this->t('October'),
      11 => $this->t('November'),
      12 => $this->t('December'),
    ];
    return $months_array;
  }

  /**
   * Returns array of years, next 10 years.
   */
  public function getYearsArray() {
    $years_array = array_combine(range(date("Y"), date("Y") + 10), range(date("Y"), date("Y") + 10));
    return $years_array;
  }

  /**
   * Returns associative array of astrologies.
   */
  public function getAstrologyArray() {
    $query = db_query("SELECT id, name FROM astrology");
    $data = [];
    while ($row = $query->fetchObject()) {
      $data[$row->id] = $row->name;
    }
    return $data;
  }

  /**
   * Return associative array of astrology.
   */
  public function getAstrologyListSignArray($astrology_id) {
    $query = db_query("SELECT id, name FROM {astrology_signs} WHERE astrology_id = :hid", [
      ':hid' => $astrology_id,
    ]);
    $data = [];
    $data[0] = 'ALL';
    while ($row = $query->fetchObject()) {
      $data[$row->id] = $row->name;
    }
    return $data;
  }

  /**
   * Return associative array of astrology.
   */
  public function getAstrologySignArray($sign_id, $astrology_id) {

    $query = db_select('astrology_signs', 'as_')
      ->fields('as_')
      ->condition('id', $sign_id, '=')
      ->condition('astrology_id ', $astrology_id, '=')
      ->execute();
    $query->allowRowCount = TRUE;
    if (!$query->rowCount()) {
      throw new AccessDeniedHttpException();
    }
    return $query->fetchAssoc();
  }

  /**
   * Return TRUE if text_id belongs to a valid sign.
   */
  public function isValidText($sign_id, $text_id) {

    $query = db_select('astrology_text', 'at_')
      ->fields('at_', ['id', 'text', 'text_format'])
      ->condition('id', $text_id, '=')
      ->condition('astrology_sign_id ', $sign_id, '=')
      ->execute();
    $query->allowRowCount = TRUE;
    if (!$query->rowCount()) {
      throw new AccessDeniedHttpException();
    }
    $result = $query->fetchObject();
    return $result;
  }

  /**
   * Update configurations, if changes made on astrology settings page.
   */
  private function updateAstrologyConfigSettings($enabled, $default_astrology, $astrology_id) {

    $astrology_config = \Drupal::configFactory()->getEditable('astrology.settings');
    db_update('astrology')
      ->fields([
        'enabled' => $enabled,
      ])->condition('id', $default_astrology, '=')
      ->execute();

    $astrology_config->set('astrology', $astrology_id)->save();

    // Invalidate astrology block cache on add/update astrology.
    self::invalidateAstrologyBlockCache();
  }

  /**
   * Update default astrology if there is a change.
   */
  public function updateDefaultAstrology($astrology_id, $status, $op) {

    $astrology_config = \Drupal::config('astrology.settings');
    $default_astrology = $astrology_config->get('astrology');

    if ($op == 'new' && $status) {
      $this->updateAstrologyConfigSettings('0', $default_astrology, $astrology_id);
    }
    elseif ($op == 'update' && $astrology_id != $default_astrology && $status) {
      $this->updateAstrologyConfigSettings('0', $default_astrology, $astrology_id);
    }
    elseif ($op == 'update' && $astrology_id == $default_astrology && !$status) {
      $this->updateAstrologyConfigSettings('1', '1', '1');
    }
    elseif ($op == 'delete' && $astrology_id == $default_astrology) {
      $this->updateAstrologyConfigSettings('1', '1', '1');
    }
  }

  /**
   * Invalidate astrology block cache.
   */
  public static function invalidateAstrologyBlockCache() {
    \Drupal::service('cache_tags.invalidator')
      ->invalidateTags(['astrology_block']);
  }

}
