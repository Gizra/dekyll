<div class="panel-display bootstrap-twocol-6-6-bricks" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
  <div class="row-fluid">
    <div class="panel-panel span12">
      <?php print $content['top']; ?>
    </div>
  </div>
  <div class="row-fluid">
    <div class="panel-panel span6">
      <?php print $content['left_above']; ?>
    </div>
    <div class="panel-panel span6">
      <?php print $content['right_above']; ?>
    </div>
  </div>
  <div class="row-fluid">
    <div class="panel-panel span12">
      <?php print $content['middle']; ?>
    </div>
  </div>
  <div class="row-fluid">
    <div class="panel-panel span6">
      <?php print $content['left_below']; ?>
    </div>
    <div class="panel-panel span6">
      <?php print $content['right_below']; ?>
    </div>
  </div>
  <div class="row-fluid">
    <div class="panel-panel span12">
      <?php print $content['bottom']; ?>
    </div>
  </div>
</div>
