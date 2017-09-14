<?php

namespace Drupal\astrology\Form;

use Drupal\astrology\Controller\UtilityController;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class AstrologyAddForm.
 */
class AstrologyAddForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'astrology_add_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $form['name'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Name'),
      '#required' => TRUE,
    ];
    $form['enabled'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Enabled'),
    ];
    $form['about'] = [
      '#type' => 'text_format',
      '#format' => 'full_html',
      '#title' => $this->t('Description'),
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#button_type' => 'primary',
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);

    // Check if astrology name exists.
    $name = $form_state->getValue('name');
    $query = db_select('astrology', 'a')
      ->fields('a', ['name'])
      ->condition('name', $name)
      ->execute();
    $query->allowRowCount = TRUE;
    if ($query->rowCount() > 0) {
      $form_state->setErrorByName('name', $this->t('Astrology name ":name" is already taken', [':name' => $name]));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    // Insert astrology.
    $utility = new UtilityController();
    $name = $form_state->getValue('name');
    $enabled = $form_state->getValue('enabled');
    $about = $form_state->getValue('about');
    $astrology_id = db_insert('astrology')
      ->fields([
        'name' => $name,
        'enabled' => $enabled,
        'about' => $about['value'],
        'about_format' => $about['format'],
      ])->execute();
    if ($enabled) {
      $utility->updateDefaultAstrology($astrology_id, $enabled, 'new');
    }
    $form_state->setRedirect('astrology.list_astrology');
    drupal_set_message($this->t('Astrology :name created', [':name' => $name]));
  }

}
