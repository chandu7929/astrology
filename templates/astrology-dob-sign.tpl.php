<?php

/**
 * @file
 * Rendered templates for astrology sign text.
 *
 * - $sign object contains information for sign you are requesting text.
 * - $about_sign_summary a summary text about sign.
 * - $formatter i.e day, week, month and year.
 * - $date_range_sign.
 */

  $sign_name = drupal_strtolower($sign->name);
  $img_url = file_create_url($sign->icon);
  $url = url('astrology/' . $sign_name . '/' . $formatter);
  $sign_url = 'astrology/' . $sign_name . '/details';

  $img = t('<img src="!img-url" alt="!alt" height="75" width="75" />', array(
    '!img-url' => $img_url,
    '!alt' => $sign->name,
    '!name' => $sign->name,
  ));
?>
<div class="astrology-name-icon-date">
  <h1><?php print $sign->name; ?></h1>
  <?php print l($img, $sign_url, array('html' => TRUE)); ?>
  <span><?php print $date_range_sign; ?></span>
</div>

<div class="astrology-about-body">
  <p><?php print t('!text', array('!text' => $about_sign_summary)); ?></p>
  <span><?php print l(t('Read more'), $sign_url); ?></span>
</div>

<div class="astrology-formatter-link">
  <nav>
    <ul>
      <?php
        $nav = ['year', 'month', 'week', 'day'];
        foreach ($nav as $item) {
          $link = 'astrology/' . $sign_name . '/' . $item;
          print '<li>' . l(t('Astrology for !format', array('!format' => $item)), $link) . '</li>';
        }
      ?>
    </ul>
  </nav>
</div>
