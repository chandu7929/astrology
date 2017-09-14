<?php

/**
 * @file
 * Rendered templates for astrology sign text.
 *
 * - $sign object contains information for sign.
 */

 $sign_name = drupal_strtolower($sign->name);
 $img_url = file_create_url($sign->icon);

  $img = t('<img src="!img-url" alt="!alt" height="75" width="75" />', array(
    '!img-url' => $img_url,
    '!alt' => $sign->name,
    '!name' => $sign->name,
  ));
?>
<div class="astrology-name-icon-date">
  <?php print $img; ?>
  <span><?php print $sign->date_from_range . ' - ' . $sign->date_to_range; ?></span>
</div>
<div class="astrology-about-body">
  <p><?php print t('!text', array('!text' => $sign->about_sign)); ?></p>
</div>
