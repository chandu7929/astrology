<?php

namespace Drupal\astrology\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Cache\Cache;

/**
 * Provides a 'AstrologyBlock' block.
 *
 * @Block(
 *  id = "astrology",
 *  admin_label = @Translation("Astrology"),
 * )
 */
class AstrologyBlock extends BlockBase {

  /**
   * {@inheritdoc}
   */
  public function build() {

    $is_admin = \Drupal::currentUser()->hasPermission('Administrator');
    $astrology_config = \Drupal::config('astrology.settings');
    $astrology_id = $astrology_config->get('astrology');
    $formatter = $astrology_config->get('format_character');
    $query = db_select('astrology_signs', 's')
      ->fields('s')
      ->condition('s.astrology_id', $astrology_id, '=')
      ->execute();
    $query->allowRowCount = TRUE;
    $signs = NULL;
    $blank_msg = NULL;

    if ($query->rowCount()) {
      $signs = $query;
    }
    if ($is_admin) {
      $blank_msg = 'The default selected astrology does not contains any sign, please add one or more sign to display here.';
    }
    $build = [];
    $build[] = [
      '#theme' => 'astrology',
      '#formatter' => $formatter,
      '#signs' => $signs,
      '#blank_msg' => $blank_msg,
      '#attached' => [
        'library' => [
          'astrology/astrology.module',
        ],
      ],
    ];
    return $build;
  }

  /**
   * {@inheritdoc}
   */
  public function getCacheTags() {
    // Set cache tag for astrology block.
    return Cache::mergeTags(parent::getCacheTags(), ['astrology_block']);
  }

}
