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


## Build (Ubuntu Xenial)

[Dockerfile](Dockerfile) is supplied for Ubuntu Xenial image. If you want to compile this project in Xenial using libraries from repositiores only, check Dockerfile to see what packages you need.

## Build (Docker)

To build using Docker, you can use this command to build guest system:
```
docker build -t dor/dor:qviewer .
```
And then this to compile executable:
```
docker run -it -v `pwd`:/opt/jenkins-qviewer -u $(id -u):$(id -g) dor/dor:qviewer
```
It will create a `build` folder insidte project's root folder with all cmake and make output.
