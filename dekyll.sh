#!/bin/bash

drush process-waiting-queue dekyll_clone -v --uri=http://localhost/dekyll/www &
drush process-waiting-queue dekyll_export -v --uri=http://localhost/dekyll/www &