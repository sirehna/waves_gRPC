FROM python:3.6

RUN pip3 install --upgrade pip \
 && pip3 install \
      grpcio-tools \
 && mkdir -p /usr/python \
 && chmod a+rwx /usr/python
ENV PYTHONPATH /usr/python
ADD wave_types.proto .
ADD wave_grpc.proto .
RUN python3 -m grpc_tools.protoc -I . --python_out=/usr/python --grpc_python_out=/usr/python wave_types.proto
RUN python3 -m grpc_tools.protoc -I . --python_out=/usr/python --grpc_python_out=/usr/python wave_grpc.proto
ADD waves.py .
ENTRYPOINT ["python3"]
