<?php

/**
 * @file
 * Rendered templates for astrology sign text.
 *
 * - $formatter contains currently selected format to display astrology text
 * i.e 'day', 'week', 'month', 'year'.
 * - $sign object contains information for the astrology sign.
 * - $date_format to display.
 * - $astrology_text object contains information.
 * - $about_sign_summary contains summary text for the sign.
 * - $weeks array containing first and last day of week.
 * - $post_date posted date for the text.
 * - $date_range_sign a string from_date to_date value for sign.
 * - $sign_info a boolean value.
 * - $for_date a value that represent the text.
 */
 ?>

<?php
  $sign_name = drupal_strtolower($sign->name);
  $img_url = file_create_url($sign->icon);
  $url = url('astrology/' . $sign_name . '/' . $formatter);
  $sign_url = 'astrology/' . $sign_name . '/details';
  $prev = 'astrology/' . $sign_name . '/' . $formatter . '/' . $for_date['prev'];
  $next = 'astrology/' . $sign_name . '/' . $formatter . '/' . $for_date['next'];

  $img = t('<img src="!img-url" alt="!alt" height="75" width="75" />', array(
    '!img-url' => $img_url,
    '!alt' => $sign->name,
    '!name' => $sign->name,
  ));
?>
<div id="astrology" class="astrology-date">
  <span class="prev pull-left"><?php if($for_date['prev']){print l(t('Previous'), $prev);
 } ?></span>
  <span><?php
      if($formatter == 'week') {
        print date($date_format, $weeks[0]) . ' to ' . date($date_format, $weeks[1]);
      }else{
        print date($date_format, $post_date);
      }
    ?>
  </span>
  <span class="next pull-right"><?php if($for_date['next']){print l(t('Next'), $next);
 } ?></span>
</div>

<div class="astrology-name-icon-date">
  <h1><?php print l(t('!name', array('!name' => $sign->name)), $sign_url); ?></h1>
  <?php print l($img, $sign_url, array('html' => TRUE)); ?>
  <span><?php print $date_range_sign; ?></span>
</div>

<div class="astrology-text">
  <?php if(is_null($astrology_text)):?>
    <p><?php print t('Currently, data not available for this !format.', array('!format' => $formatter)); ?></p>
  <?php else: ?>
    <p><?php print t('!text', array('!text' => $astrology_text)); ?></p>
  <?php endif; ?>
</div>

<div class="astrology-formatter-link">
  <nav>
    <ul>
    <?php
      $nav = ['day', 'week', 'month', 'year'];
      foreach ($nav as $item) {
        if($formatter == $item){
          continue;
        }
        $link = 'astrology/' . $sign_name . '/' . $item;
        print '<li>' . l(t('Astrology for !format', array('!format' => $item)), $link) . '</li>';
      }
      print '<li>' . l(t('Find your star sign'), 'astrology/birth_sign') . '</li>';
    ?>
    </ul>
  </nav>
</div>

<?php if($sign_info): ?> 
  <div class="astrology-name-icon-date">
    <h1><?php print t('About "!name"', array('!name' => $sign->name)); ?></h1>
    <?php print l($img, $sign_url, array('html' => TRUE)); ?>
    <span><?php print $sign->date_from_range . ' - ' . $sign->date_to_range; ?></span>
  </div>

  <div class="astrology-about-text">
    <p><?php print t('!text', array('!text' => $about_sign_summary)); ?></p>
    <span><?php print l(t('Read more'), 'astrology/' . $sign_name . '/details'); ?></span>
  </div>
<?php endif; ?>
