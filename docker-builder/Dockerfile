FROM golang:buster

RUN dpkg --add-architecture armel

RUN apt-get update && apt-get install -y \
	g++-arm-linux-gnueabi \
	gcc-arm-linux-gnueabi \
	libc6-dev-armhf-cross \
	libopus-dev:armel \
	libogg-dev:armel \
	android-liblog-dev:armel android-liblog:armel \
	upx

ENV CC="arm-linux-gnueabi-gcc"
ENV CXX="arm-linux-gnueabi-g++-8"