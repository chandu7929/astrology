<?php

/**
 * @file
 * Astrology block content.
 *
 * - $signs an array of object containing list of sign and its information.
 * - $blank_msg contains message for administrator if astrology block do not
 *   have any sign.
 * - $formatter a string variable contains day, wee, month, and year.
 */
drupal_add_css(drupal_get_path('module', 'astrology') . '/css/astrology.css');
$p = '';
if(empty($signs)){
  if($blank_msg){
    $p = $blank_msg;
  }else{
    $p = t('No sign to display here, please contact your site administrator.');
  }
}?>
<ul class='clearfix menu'>

<?php foreach ($signs as $key => $row) {
  $url = url('astrology/' . drupal_strtolower($row->name) . '/' . $formatter);
  $img_url = file_create_url($row->icon);
  $p .= t('<li class="menu-item sign-name"> <a href="@img-link-url">
    <img src="!img-url" alt="!alt" height="20" width="20" />!name</a></li>',
    array(
      '@img-link-url' => $url,
      '!img-url' => $img_url,
      '!alt' => $row->name,
      '!name' => $row->name,
    ));
}
print $p;
?>
</ul>
<!-- <div id="astrology" class="astrology-block">
  <?php print $p; ?>
</div> -->
