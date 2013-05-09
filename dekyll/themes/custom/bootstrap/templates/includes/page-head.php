<header class="row" id="header" role="banner">
  <div class="span7 center">

    <?php if (!empty($ads['TopBanner'])): ?>
      <?php print $ads['TopBanner']; ?>
    <?php endif; ?>

    <div class="span3 header-logo">
      <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
        <img src="<?php print $logo; ?>" />
      </a>
    </div>
  </div>
</header>
