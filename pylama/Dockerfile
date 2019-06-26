FROM python:3.7-slim-stretch

RUN python -m pip install --upgrade pip \
 && python -m pip install pylama
ADD pylama.ini /etc/pylama.ini
ENTRYPOINT ["pylama", "--option", "/etc/pylama.ini"]
