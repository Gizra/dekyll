#!/bin/bash

chmod 777 www/sites/default
rm -rf www/
mkdir www

bash scripts/build
cd www

drush si -y dekyll --account-pass=admin --db-url=mysql://root@localhost/dekyll
drush mi --all --user=1
