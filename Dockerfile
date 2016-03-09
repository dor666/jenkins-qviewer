FROM ubuntu:xenial
MAINTAINER Dorian Apanel <dorian.apanel@gmail.com>

RUN apt-get update && apt-get install -y cmake gcc g++ git subversion

RUN apt-get install -y qtbase5-dev qtdeclarative5-dev

CMD mkdir -p /opt/jenkins-qviewer/build && cd /opt/jenkins-qviewer/build && cmake .. && make
