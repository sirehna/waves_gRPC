# Helloworld CPP Demo with GRPC and Docker

## How to run it
```sh
$ cd demo_cpp_hello_world
$ make test
$ cd ..
```

The client send a request to the server with parameter "world", the server prefixes it with "Hello ", and send it back.

## .proto file
`helloworld.proto`

Here are defined data and services.

## Synchronous implementation
- Files concerned: `greeter_sync_client` and `greeter_sync_server`

Most basic example adapted from _https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld_ demo.

## Asynchronous implementation
- Files concerned: `greeter_async_client` or `greeter_async_client_loop`, and `greeter_async_server`

Asynchronous example adapted from _https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld#writing-asynchronous-client-and-server_ demo.

What's new : the use of `CompletionQueue` API to manage asynchronicity.

`greeter_async_client_loop` loops to repeat the message sent only once by `greeter_async_client`.

## Google tests

```sh
$ cd demo_cpp_hello_world
$ make gtest
$ cd ..
```

- Files concerned: `tests.cpp` (`make gtest`) and `test_server.cpp` (`make test_server`)

`greeter_sync_server` has been divided into `greeter_sync_server` and `greeter_sync_server_main` to use gtest main during tests.
