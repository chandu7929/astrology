<?php

namespace Drupal\astrology\Form;

use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
use Drupal\Core\Url;
use Drupal\astrology\Controller\UtilityController;

/**
 * Class AstrologyDeleteSignForm.
 */
class AstrologyDeleteForm extends ConfirmFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'astrology_delete';
  }

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete this astrology?');
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    return new Url('astrology.list_astrology');
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $astrology_id = NULL) {
    if ($astrology_id == 1) {
      drupal_set_message($this->t("This astrology can not be deleted"), 'error');
      throw new AccessDeniedHttpException();
    }
    $form['#title'] = $this->getQuestion();
    $this->astrology_id = $astrology_id;
    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $utility = new UtilityController();
    $result = db_query("SELECT enabled,name FROM {astrology} WHERE id = :id ",
    [':id' => $this->astrology_id]);
    $row = $result->fetchObject();

    // Get all sign ids.
    $sign_ids = db_query("SELECT id FROM {astrology_signs} WHERE astrology_id = :id ",
    [':id' => $this->astrology_id]);

    // Delete all text for particular sign id.
    while ($ids = $sign_ids->fetchObject()) {
      db_query("DELETE FROM {astrology_text} WHERE astrology_sign_id = :id",
      [':id' => $ids->id]);
    }

    // Now deleted all sign ids.
    db_query("DELETE FROM {astrology_signs} WHERE astrology_id = :id",
    [':id' => $this->astrology_id]);

    // Finally delete astrology.
    $result = db_query("DELETE FROM {astrology} WHERE id = :id",
    [':id' => $this->astrology_id]);
    if ($row->enabled) {
      $utility->updateDefaultAstrology($this->astrology_id, $row->enabled, 'delete');
    }
    drupal_set_message($this->t("Astrology %name deleted.", ['%name' => $row->name]));
    $form_state->setRedirect('astrology.list_astrology');
  }

}
