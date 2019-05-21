#  docker build . -t docker_wave_ghz
FROM debian:9-slim as builder
ADD https://github.com/bojand/ghz/releases/download/v0.35.1/ghz_0.35.1_Linux_x86_64.tar.gz /usr
RUN cd /usr && tar -xzf /usr/ghz_0.35.1_Linux_x86_64.tar.gz
ADD https://raw.githubusercontent.com/vishnubob/wait-for-it/9995b721327eac7a88f0dce314ea074d5169634f/wait-for-it.sh /usr/.
RUN chmod 740 /usr/wait-for-it.sh

FROM debian:9-slim
COPY --from=builder /usr/wait-for-it.sh /usr/.
COPY --from=builder /usr/ghz /usr/bin/.
COPY --from=builder /usr/ghz-web /usr/bin/.
