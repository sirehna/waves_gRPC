.PHONY: all

CONTAINER_NAME=grpc-waves-python-server

all:
	cp ../wave_grpc.proto .
	cp ../wave_types.proto .
	docker build . -t ${CONTAINER_NAME}
	rm -f wave_grpc.proto wave_types.proto

clean:
	rm -f __pycache__