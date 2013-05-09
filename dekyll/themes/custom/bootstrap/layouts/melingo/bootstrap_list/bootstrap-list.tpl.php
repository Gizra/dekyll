<div class="panel-display row-fluid bootstrap-onecol-12" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>
  <div class="panel-panel span12">
    <div id="collapseOne" class="accordion-body in collapse" style="height: auto;">
      <div class="accordion-inner">
        <ul id="myTab" class="nav nav-tabs">
          <li class=""><a href="#practice" data-toggle="tab"><?php print t('Practice');?></a></li>
          <li class=""><a href="#quiz" data-toggle="tab"><?php print t('Quiz');?></a></li>
          <li class="active"><a href="#wordlist" data-toggle="tab"><?php print t('Word List');?></a></li>
        </ul>
        <div id="myTabContent" class="tab-content">
          <div class="tab-pane fade" id="practice">
            <?php print $content['practice'];?>
          </div>
          <div class="tab-pane fade " id="quiz">
            <?php print $content['quiz'];?>
          </div>
          <div class="tab-pane fade active in" id="wordlist">
            <?php print $content['wordlist'];?>
          </div>
        </div>
      </div>
  </div>
</div>