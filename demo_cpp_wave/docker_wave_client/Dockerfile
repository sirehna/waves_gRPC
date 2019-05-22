FROM debian:9-slim
ADD wait-for-it.sh /usr/.
RUN chmod 740 /usr/wait-for-it.sh
RUN apt-get update && \
    apt-get install --yes --no-install-recommends \
        time
ADD wave_client /usr
CMD /usr/wave_client
