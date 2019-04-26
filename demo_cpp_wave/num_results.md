_Results obtained by running `make dc_test`, changing, in `wave_client_main.cc`, which `request` type and `elevation_service` service are used._


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

## (repeated x, repeated y) + (repeated x, repeated y, repeated z) ; 1000 requests

Vector size             | Average time per request
------------------------|-------------------------
1                       | 0.08 ms
100                     | 0.13 ms
1 000                   | 0.38 ms
2 000                   | 0.64 ms
5 000                   | 1.6 ms
10 000                  | 3.0 ms
50 000                  | 13 ms
100 000                 | 26 ms
