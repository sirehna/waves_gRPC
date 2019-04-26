FROM debian:9-slim
ADD https://raw.githubusercontent.com/vishnubob/wait-for-it/9995b721327eac7a88f0dce314ea074d5169634f/wait-for-it.sh /usr/.
RUN chmod 740 /usr/wait-for-it.sh
RUN apt-get update && \
    apt-get install --yes --no-install-recommends \
        time
ADD wave_client /usr
CMD /usr/wave_client
