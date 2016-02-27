# jenkinsQViewer
Display jenkins jobs (status and progress).
Inspired by https://github.com/jan-molak/jenkins-build-monitor-plugin, but this is a standalone app, and does not require to login after reboot. Username and token can be specified by command line params.

Working:
* Display list of jobs, their progress(buggy colors) and status(green/red).

Not working:
* SSL broken, will ignore errors and connect anyway.
* Progress color wrong.
* Prints bunch of errors at startup (list delegate initialization).
* Displays all jobs, views not supported yet.
