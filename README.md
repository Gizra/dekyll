**Deprectaed, see [blog post](http://gizra.com/content/zariz-means-agile/) about Zariz.**

Drush commands
==============
Either execute the commands manually:
 drush process-waiting-queue dekyll_clone -v &
 drush process-waiting-queue dekyll_export -v &

Or, execute the bash command
. ./dekyll.sh

That's a dot followed by a space. This causes the currently running shell to interpret the script, instead of launching a subshell. 
The jobs will then be accessible from the standard job control, using the "jobs" command.

