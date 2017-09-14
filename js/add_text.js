/**
 * @file
 */

(function ($) {
  Drupal.behaviors.astrology = {
    attach: function (context, settings) {
      $('#edit-value').datepicker();
    }
  };
}(jQuery));
