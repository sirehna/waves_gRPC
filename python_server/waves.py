from concurrent import futures
import time
import math
import logging

import grpc

import waves_pb2
import waves_pb2_grpc

from wave_model_stub import WaveModelStub 

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

service_name = "waves-server"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d [' + service_name +
    '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


class WavesServicer(waves_pb2_grpc.WavesServicer):

  def __init__(self, model):
    self.model = model

  def set_parameters(self, request, context):
    logger.info('Received parameters: ' + request.parameters)
    self.model.set_parameters(request.parameters)
    return waves_pb2.SetParameterResponse(error_message = '')

  def elevations(self, request, context):
    logger.info('Got elevation request')
    xys = map(lambda x,y: {'x':x,'y':y}, request.x, request.y)
    zs = [self.model.elevation(xy['x'], xy['y'], request.t) for xy in xys]
    response = waves_pb2.XYZTGrid()
    response.x[:] = request.x
    response.y[:] = request.y
    response.z[:] = zs
    response.t = request.t
    return response

  def dynamic_pressures(self, request, context):
    logger.info('Got dynamic pressure request')
    xyzs = map(lambda x,y,z: {'x':x,'y':y,'z':request.z}, request.x, request.y, request.z)
    pdyn = [self.model.dynamic_pressure(xyz['x'], xyz['y'], request.t) for xyz in xyzs]
    response = waves_pb2.DynamicPressuresResponse()
    response.x[:] = request.x
    response.y[:] = request.y
    response.z[:] = request.z
    response.t = request.t
    response.pdyn[:] = pdyn
    return response

  def orbital_velocities(self, request, context):
    logger.info('Got orbital velocities request')
    xys = map(lambda x,y: {x:x,y:y}, request.x, request.y)
    vorbs = [self.model.orbital_velocity(xyz.x, xyz.y, xyz.z, xyzt.t) for xyz in xyzs]
    response = waves_pb2.OrbitalVelocitiesResponse()
    response.x[:] = request.x
    response.y[:] = request.y
    response.z[:] = request.z
    response.t = request.t
    response.vx[:] = [vorb['vx'] for vorb in vorbs]
    response.vy[:] = [vorb['vy'] for vorb in vorbs]
    response.vz[:] = [vorb['vz'] for vorb in vorbs]
    return response

  def spectrum(self, request, context):
    logger.info('Got spectrum request')
    response = waves_pb2.SpectrumResponse()
    spectrum_element = aves_pb2.WaveSpectrumElement()
    spectrum_element.a     = 1
    spectrum_element.omega = 2
    spectrum_element.psi   = 3
    spectrum_element.k     = 4
    spectrum_element.phase = 5
    response.spectrum_element.append(spectrum_element)
    return response


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    waves_pb2_grpc.add_WavesServicer_to_server(
        WavesServicer(WaveModelStub()), server)
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
