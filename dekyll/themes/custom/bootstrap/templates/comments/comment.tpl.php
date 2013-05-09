<div class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="content"<?php print $content_attributes; ?>>
    <div class="first-col">
        <?php print $picture ?>
        <?php print render($content['field_score']); ?>
    </div>

    <div class="last-col">
      <div class="submitted">
        <?php print $submitted; ?>
      </div>

      <?php if ($new): ?>
        <span class="new"><?php print $new ?></span>
      <?php endif; ?>

      <?php print render($content['comment_body']);  ?>
    </div>
  </div>


  <?php print render($content['links']) ?>
</div>
