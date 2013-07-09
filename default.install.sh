#!/bin/bash

chmod 777 www/sites/default/
bash scripts/build

# Get composer
curl -sS https://getcomposer.org/installer | php
php composer.phar install --working-dir="./dekyll/libraries"

cd www

mkdir sites/default/files
chmod -R 777 sites/default/files

# Install profile, as "Github pages"
drush si -y dekyll --account-pass=admin --db-url=mysql://root:root@localhost/dekyll dekyl_installation_type_form.dekyll_installation_type=github_pages

# Set the SSH key
drush vset dekyll_ssh_key "$(cat ~/.ssh/id_rsa.pub)"

# Set Github Application
# drush vset github_connect_client_id YOUR-CLIENT-ID
# drush vset github_connect_client_secret YOUR-CLIENT-SECRET

# Execute Dekyll queue workers.
. ../dekyll.sh