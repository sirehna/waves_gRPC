# Numerical CPP Demo with GRPC and Docker

## How to run it
```
$ make dc_test
$ make dc_stop
```

The client sends a request to the server with parameters x, y and t, the server computes the elevation z (using hard-coded discrete wave spectrum values), and sends it back.

Example adapted from https://grpc.io/docs/tutorials/basic/c.html#example-code-and-setup demo.

## .proto file
`wave.proto`

Here are defined data and services.

## Unary implementation
- Files concerned: `wave_client` and `wave_server`
- Service concerned: `GetElevation`

Here the client can send a set of Points (x, y) in one request. The server computes the elevation for each Point and sends back the array of results.

In protubuf, the key-word `repeated` is used to represent an array. The generated functions used to decode data after it is sent are slightly different : see https://developers.google.com/protocol-buffers/docs/reference/cpp-generated#repeatednumeric.

In order for the server to compute the discrete wave spectrum value once and not at each request, it is computed in the main of the server, and passed as an argument of the service.
To do so :
- a spectrum value attribute is defined in the server service class
- the server service class constructor getting this value as a parameter is defined (with the `explicit` keyword), and sets it.

## Server streaming implementation
- Files concerned: `wave_client` and `wave_server`
- Service concerned: `GetElevations`

Here the client can now send `t_start`, `t_end` and `dt` values with the set of Points (x, y), in its request. The server sends a stream of elevation arrays: one for each t within the time interval asked by the client.
To do so :
- keyword `stream` in service definition.
- use `grpc::ServerWriter` to write stream and `grpc::ClientReader` to read it.

