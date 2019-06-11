FROM python:3.6

ADD requirements.txt .
RUN pip3 install -r requirements.txt

ADD waves.proto .
RUN python3 -m grpc_tools.protoc -I . --python_out=. --grpc_python_out=. waves.proto
ADD airy.py server.py wave_model_interface.py waves.py ./
ENTRYPOINT python3 server.py
