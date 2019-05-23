FROM python:3.6

RUN pip3 install --upgrade pip  \
 && pip3 install \
       grpcio-tools
ADD waves.proto .
RUN python3 -m grpc_tools.protoc -I . --python_out=. --grpc_python_out=. waves.proto
ADD waves.py .
ENTRYPOINT python3 waves.py
