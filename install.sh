#!/bin/bash

chmod 777 www/sites/default/
bash scripts/build

cd www

mkdir sites/default/files
chmod -R 777 sites/default/files

drush si -y dekyll --account-pass=admin --db-url=mysql://root:root@localhost/dekyll
drush mi --all --user=1
