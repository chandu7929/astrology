<?php

namespace Drupal\astrology\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\astrology\Controller\UtilityController;

/**
 * Class AstrologySignAddTextForm.
 */
class AstrologySignAddTextForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'astrology_sign_add_text';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $astrology_id = NULL, $sign_id = NULL) {

    $utility = new UtilityController();
    $astrology_config = \Drupal::config('astrology.settings');
    $this->format_character = $astrology_config->get('admin_format_character');
    $this->astrology_id = $astrology_id;

    switch ($this->format_character) {
      default:
      case 'day':
        $format = 'z';
        $value = [
          '#type' => 'date',
          '#date_date_format' => 'm/d/Y',
          '#title' => $this->t('Select Day'),
          '#required' => TRUE,
        ];
        break;
      case 'week':
        $format = 'W';
        $value = [
          '#type' => 'date',
          '#date_date_format' => 'm/d/Y',
          '#title' => $this->t('Select Week'),
          '#required' => TRUE,
        ];
        break;

      case 'month':
        $format = 'n';
        $value = [
          '#type' => 'select',
          '#title' => $this->t('Select Month'),
          '#options' => $utility->getMonthsArray(),
          '#default_value' => date('n'),
        ];
        break;

      case 'year':
        $format = 'o';
        $value = [
          '#type' => 'select',
          '#title' => $this->t('Select Year'),
          '#options' => $utility->getYearsArray(),
          '#default_value' => date('o'),
        ];
        break;
    }

    $options = $utility->getAstrologySignArray($sign_id, $astrology_id);
    $form['label'] = [
      '#type' => 'label',
      '#title' => $this->t('<strong>:name</strong>', [
        ':name' => $options['name'],
      ]),
    ];
    $form['astrology_sign_id'] = [
      '#type' => 'hidden',
      '#default_value' => $sign_id,
    ];
    $form['format_character'] = [
      '#type' => 'hidden',
      '#default_value' => $format,
    ];
    $form['date_value'] = $value;
    $form['text'] = [
      '#type' => 'text_format',
      '#format' => 'full_html',
      '#title' => $this->t('Enter Text'),
      '#required' => TRUE,
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#button_type' => 'primary',
    ];
    $form['note'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Note'),
      '#description' => $this->t('You can change format ":format" setting, under administer setting tabs on the astrology configuration page  to search for other formats.', [':format' => $this->format_character]),
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $utility = new UtilityController();
    $format_character = $form_state->getValue('format_character');
    $astrology_sign_id = $form_state->getValue('astrology_sign_id');
    $date_value = $form_state->getValue('date_value');
    $text = $form_state->getValue('text');

    switch ($format_character) {
      case 'z':
        $post_date = $utility->getTimestamps($date_value);
        // Get day number of the year.
        $date = $utility->getFormatDateValue('z', $date_value);
        $date_message = $utility->getDoy($date + 1, 'l j F');
        break;

      case 'W':
        $post_date = $utility->getTimestamps($date_value);
        // Get week number of the year.
        $date = $utility->getFormatDateValue('W', $date_value);
        $timestamps = $utility->getTimestamps($date_value);
        // Get first and last day of week.
        $weeks = $utility->getFirstLastDow($timestamps);
        $date_message = date('j, M', $weeks[0]) . ' to ' . date('j, M', $weeks[1]);
        break;

      case 'n':
        $date = $date_value;
        // Get month timestamps.
        $post_date = mktime(0, 0, 0, $date_value);
        $months = $utility->getMonthsArray();
        $date_message = $months[$date_value];
        break;

      case 'o':
        $date = $date_value;
        // Get year timestamps.
        $post_date = mktime(0, 0, 0, 1, 1, $date_value);
        $date_message = $date_value;
        break;
    }

    // Check if text for sign already exists for this format.
    $result = db_select('astrology_text', 'at')
      ->fields('at', ['id', 'astrology_sign_id'])
      ->condition('astrology_sign_id', $astrology_sign_id, '=')
      ->condition('format_character', $format_character, '=')
      ->condition('value', $date, '=')
      ->execute();
    $result->allowRowCount = TRUE;
    $row = $result->fetchObject();

    if ($result->rowCount()) {
      db_update('astrology_text')
        ->fields([
          'text' => $text['value'],
          'text_format' => $text['format'],
        ])
        ->condition('id', $row->id, '=')
        ->execute();
      drupal_set_message($this->t('Text updated for the :format <strong>:date</strong>.', [
        ':format' => $this->format_character,
        ':date' => $date_message,
      ]), 'warning');
    }
    else {
      db_insert('astrology_text')
        ->fields([
          'astrology_sign_id' => $astrology_sign_id,
          'format_character' => $format_character,
          'value' => $date,
          'text' => $text['value'],
          'text_format' => $text['format'],
          'post_date' => $post_date,
        ])
        ->execute();
      drupal_set_message($this->t('Text added for the :format <strong>:date</strong>.', [
        ':format' => $this->format_character,
        ':date' => $date_message,
      ]));
    }
  }

}
