_Results obtained by running `make dc_test`, changing `request` type and which `elevation_service` service is called._

## Vector of size 1; 10000 requests

Input                    | Output               | Average time per request
-------------------------|--------------------------------------|--------
(repeated x, repeated y) | repeated z                           | 0.08 ms
(repeated x, repeated y) | (repeated x, repeated y, repeated z) | 0.08 ms
(repeated x, repeated y) | repeated (x, y, z)                   | 0.08 ms
repeated (x, y)          | repeated z                           | 0.08 ms
repeated (x, y)          | (repeated x, repeated y, repeated z) | 0.08 ms
repeated (x, y)          | repeated (x, y, z)                   | 0.08 ms

## Vector of size 1000; 1000 requests

Input                    | Output              | Average time per request
-------------------------|--------------------------------------|--------
(repeated x, repeated y) | repeated z                           | 0.25 ms
(repeated x, repeated y) | (repeated x, repeated y, repeated z) | 0.38 ms
(repeated x, repeated y) | repeated (x, y, z)                   | 0.89 ms
repeated (x, y)          | repeated z                           | 0.63 ms
repeated (x, y)          | (repeated x, repeated y, repeated z) | 0.74 ms
repeated (x, y)          | repeated (x, y, z)                   | 1.25 ms
