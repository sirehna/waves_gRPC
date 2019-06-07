FROM python:3.6
RUN apt-get update && \
    apt-get install --yes --no-install-recommends \
        pylama
ENTRYPOINT ["pylama"]
