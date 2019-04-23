FROM debian:9-slim
ADD https://raw.githubusercontent.com/vishnubob/wait-for-it/9995b721327eac7a88f0dce314ea074d5169634f/wait-for-it.sh /usr/.
RUN chmod 740 /usr/wait-for-it.sh
ADD greeter_sync_client /usr
CMD /usr/greeter_sync_client
