<div class="panel-display bootstrap-threecol-1-10-1" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
  <div class="row-fluid top-wrapper">
    <div class="panel-panel span12">
      <?php print $content['top']; ?>
    </div>
  </div>
  <div class="row-fluid center-wrapper">
    <div class="panel-panel span1">
      <?php print $content['left']; ?>
    </div>
    <div class="panel-panel span10">
      <?php print $content['middle']; ?>
    </div>
    <div class="panel-panel span1">
      <?php print $content['right']; ?>
    </div>
  </div>
  <div class="row-fluid bottom-wrapper">
    <div class="panel-panel span12">
      <?php print $content['bottom']; ?>
    </div>
  </div>
</div>
