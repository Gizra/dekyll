#!/bin/bash

chmod 777 www/sites/default/
bash scripts/build

cd www

mkdir sites/default/files
chmod -R 777 sites/default/files

# todo: Move this to a better place
drush dl composer-8.x-1.0-alpha10 -n
drush vset composer_manager_vendor_dir profiles/dekyll/libraries/composer
drush composer-manager install

drush si -y dekyll --account-pass=admin --db-url=mysql://root:root@localhost/dekyll
drush mi --all --user=1
