"""Launches an gRPC wave server."""

import grpc
from concurrent import futures
import time
import logging


import waves_pb2_grpc
import waves
from wave_model_stub import WaveModelStub 

service_name = "waves-server"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d ['
           + service_name
           + '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


def get_model():
    return WaveModelStub()


_ONE_DAY_IN_SECONDS = 60 * 60 * 24


def serve():
    """Launch the gRPC server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    waves_pb2_grpc.add_WavesServicer_to_server(
        waves.WavesServicer(get_model()), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    try:
        while True:
            time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server.stop(0)


if __name__ == '__main__':
    logger.info('Starting waves server...')
    serve()
    logger.info('Stopped waves server')
