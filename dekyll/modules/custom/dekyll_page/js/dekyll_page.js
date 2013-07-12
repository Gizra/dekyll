(function ($) {

  /**
   * Attach the machine-readable name form element behavior.
   */
  Drupal.behaviors.dekyllPost = {

    attach: function (context, settings) {
      var $form = $('form.node-form');

      if ($form.hasClass('field-path-processed')) {
        return;
      }

      $form.addClass('field-path-processed');


      var self = this;
      var source_id = '#edit-title';
      var target = '#edit-field-file-path-und-0-value, #edit-field-permalink-und-0-value';
      var $source = $(source_id, context).addClass('machine-name-source');
      var $target = $(target, context).addClass('machine-name-target');
      var $suffix = $('.form-item-field-file-path-und-0-value, .form-item-field-permalink-und-0-value', context).append('<div class="field-suffix"></div>').find('.field-suffix');
      var $wrapper = $target.closest('.form-item input');

      // All elements have to exist.
      if (!$source.length || !$target.length || !$suffix.length || !$wrapper.length) {
        return;
      }

      // Skip processing upon a form validation error on the machine name.
      if ($target.hasClass('error')) {
        return;
      }

      $form.submit(function() {
        // Allow the form to be submitted.
        $wrapper.removeAttr('disabled');
      });

      // Figure out the maximum length for the machine name.
      var options = {
        'field_prefix': '',
        'replace_pattern': '[^a-z0-9_]+',
        'replace': '-'
      }
      options.maxlength = $target.attr('maxlength');
      // Hide the form item container of the machine name form element.
      $wrapper.attr('disabled','disabled');
      // Determine the initial machine name value. Unless the machine name form
      // element is disabled or not empty, the initial default value is based on
      // the human-readable form element value.
      if ($target.is(':disabled') || $target.val() != '') {
        var machine = $target.val();
      }
      else {
        var machine = self.transliterate($source.val(), options);
      }

      // Append the machine name preview to the source field.
      var $preview = $('<span class="machine-name-value">' + options.field_prefix + Drupal.checkPlain(machine) + options.field_suffix + '</span>');
      $suffix.empty();

      // If it is editable, append an edit link.
      var $link = $('<span class="admin-link"><a href="#">' + Drupal.t('Edit') + '</a></span>')
        .click(function () {
          $wrapper.removeAttr('disabled');
          $target.focus();
          $suffix.hide();
          $source.unbind('.machineName');
          return false;
        });
      $suffix.append(' ').append($link);

      // Preview the machine name in realtime when the human-readable name
      // changes, but only if there is no machine name yet; i.e., only upon
      // initial creation, not when editing.
      if ($target.val() == '') {
        $source.bind('keyup.machineName change.machineName input.machineName', function () {
          machine = self.transliterate($(this).val(), options);
          // Set the machine name to the transliterated value.
          if (machine != '') {
            if (machine != options.replace) {
              $target.val(machine);
              $preview.html(options.field_prefix + Drupal.checkPlain(machine) + options.field_suffix);
            }
            $suffix.show();
          }
          else {
            $target.val(machine);
            $preview.empty();
          }
        });
        // Initialize machine name preview.
        $source.keyup();
      }
    },

    /**
     * Transliterate a human-readable name to a machine name.
     *
     * @param source
     *   A string to transliterate.
     * @param settings
     *   The machine name settings for the corresponding field, containing:
     *   - replace_pattern: A regular expression (without modifiers) matching
     *     disallowed characters in the machine name; e.g., '[^a-z0-9]+'.
     *   - replace: A character to replace disallowed characters with; e.g., '_'
     *     or '-'.
     *   - maxlength: The maximum length of the machine name.
     *
     * @return
     *   The transliterated source string.
     */
    transliterate: function (source, settings) {
      var rx = new RegExp(settings.replace_pattern, 'g');
      return source.toLowerCase().replace(rx, settings.replace).substr(0, settings.maxlength);
    }
  };

})(jQuery);

