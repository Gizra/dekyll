<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="first-col">
    <?php print $user_picture ?>
    <?php print render($content['field_score']); ?>
  </div>

  <div class="last-col">

      <a href="<?php print $node_url; ?>"><h3><?php print $title; ?></h3></a>
      <div class="submitted">
        <?php print $submitted; ?>
      </div>
  </div>

  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  </div>
  <?php print render($content['links']); ?>
  <?php print render($content['comments']); ?>
</div>
