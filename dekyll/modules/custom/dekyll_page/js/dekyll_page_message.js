(function ($) {

$(function() {

  var url = Drupal.settings.dekyll.basePath + 'message/' + Drupal.settings.dekyll.page.mid + '.json';

  var getBuildStatus = function() {
    $.getJSON(url, function(data) {
      if (data.field_jekyll_build_status == 1) {
        $('#iframe-preview').attr('src', Drupal.settings.dekyll.page.url);

        // We are done here.
        return;
      }

      // Repeat every second.
      var t = setTimeout(function() {
        getBuildStatus(t);
      }, 1000);

    }).error(function() {console.log('error')});
  };

  getBuildStatus();

});

})(jQuery);

