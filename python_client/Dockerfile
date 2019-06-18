FROM python:3.6

RUN pip3 install --upgrade pip  \
 && pip3 install \
       grpcio-tools
RUN mkdir -p /usr/python
RUN chmod a+rwx /usr/python
ENV PYTHONPATH /usr/python
RUN echo "#!/bin/bash" > entrypoint.sh
RUN echo "python3 -m grpc_tools.protoc -I /proto --python_out=/usr/python --grpc_python_out=/usr/python/ wave_types.proto" >> entrypoint.sh
RUN echo "python3 -m grpc_tools.protoc -I /proto --python_out=/usr/python --grpc_python_out=/usr/python/ wave_grpc.proto" >> entrypoint.sh
RUN echo "python3 \"\$@\"" >> entrypoint.sh
RUN chmod a+x entrypoint.sh
ADD waves.py .
ENTRYPOINT ["./entrypoint.sh", "waves.py"]
