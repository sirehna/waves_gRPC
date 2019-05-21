# Helloworld CPP Demo with GRPC and Docker

## How to run it
With docker : `$ make test`
with docker-compose :
```
$ make dc_test
$ make dc_stop
```
The client sends a request to the server with parameter "world", the server prefixes it with "Hello ", and sends it back.

## .proto file
`helloworld.proto`

Here are defined data and services.

## Synchronous implementation
- Files concerned: `greeter_sync_client` and `greeter_sync_server`

Most basic example adapted from https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld demo.

The generated functions used to decode protobuf data after it is sent can be found here : https://developers.google.com/protocol-buffers/docs/reference/cpp-generated#singular-numeric-fields-proto3.

## Asynchronous implementation
- Files concerned: `greeter_async_client` or `greeter_async_client_loop`, and `greeter_async_server`

Asynchronous example adapted from https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld#writing-asynchronous-client-and-server demo.

What's new : the use of `CompletionQueue` API to manage asynchronicity.

`greeter_async_client_loop` loops to repeat the message sent only once by `greeter_async_client`.