.PHONY: docker-builder vic-hack

docker-builder:
	docker build -t armbuilder docker-builder/.

all: vic-hack

vic-hack:
	docker container run  \
	-v "$(PWD)":/cpp/src/vector-hack \
	-w /cpp/src/vector-hack \
	--user $(UID):$(GID) \
	armbuilder \
	arm-linux-gnueabi-gcc src/main.cpp -o build/vic-hack -static