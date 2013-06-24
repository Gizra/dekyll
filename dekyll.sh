#!/bin/bash

cd www

drush process-waiting-queue dekyll_clone -v &
drush process-waiting-queue dekyll_export -v &