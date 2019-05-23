from concurrent import futures
import time
import math
import logging

import grpc

import waves_pb2
import waves_pb2_grpc

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

class WavesServicer(waves_pb2_grpc.WavesServicer):

  def set_parameters(self, request, context):
    pass

  def get_elevation(self, request, context):
    pass

  def get_dynamic_pressure(self, request, context):
    pass

  def get_orbital_velocity(self, request, context):
    pass

  def get_spectrum(self, request, context):
    pass

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    waves_pb2_grpc.add_WavesServicer_to_server(
        WavesServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    try:
        while True:
            time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server.stop(0)


if __name__ == '__main__':
    logging.basicConfig()
serve()
