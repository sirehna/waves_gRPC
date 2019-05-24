from concurrent import futures
import json
import time
import math
import logging

import grpc

import waves_pb2
import waves_pb2_grpc

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

service_name = "wave-client"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d [' + service_name +
    '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


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


class Waves:
    def __init__(self, channel, parameters):
        self.stub = waves_pb2_grpc.WavesStub(channel)
        response = self.set_parameters(parameters)
        if response:
            logger.error('Error when initializing gRPC connection to wave server: ' + response)

    def set_parameters(self, parameters):
        request = waves_pb2.SetParameterRequest(parameters = parameters)
        return self.stub.set_parameters(request).error_message

    def elevations(self, xyts):
        xytgrid = waves_pb2.XYTGrid()
        for xyt in xyts:
            xytgrid.x.append(xyt['x'])
            xytgrid.y.append(xyt['y'])
            xytgrid.t = xyt['t']
        xyztgrid = self.stub.elevations(xytgrid)
        return list(map(lambda x,y,z: {'x':x,'y':y,'z':z,'t':xyztgrid.t}, xyztgrid.x, xyztgrid.y, xyztgrid.z))


    def dynamic_pressures(self, xyzts):
        xyztgrid = waves_pb2.XYZTGrid()
        xyztgrid.t = xyt['t']
        for xyt in xyts:
            xyztgrid.x.append(xyt['x'])
            xyztgrid.y.append(xyt['y'])
            xyztgrid.z.append(xyt['z'])
            xyztgrid.t = xyt['t']
        response = self.stub.dynamic_pressures(xyztgrid)
        return list(map(lambda x,y,z,pdyn: {'x':x,'y':y,'z':z,'t':reponse.t,'pdyn':pdyn}, response.x, response.y, response.z, response.pdyn))

    def orbital_velocities(sef, xyzts):
        xyztgrid = waves_pb2.XYZTGrid()
        xyztgrid.t = xyt['t']
        for xyt in xyts:
            xyztgrid.x.append(xyt['x'])
            xyztgrid.y.append(xyt['y'])
            xyztgrid.z.append(xyt['z'])
        response = self.stub.orbital_velocities(xyztgrid)
        return list(map(lambda x,y,z,vx,vy,vz: {'x':x,'y':y,'z':z,'t':reponse.t,'vx':vx,'vy':vy,'vz':vz}, response.x, response.y, response.z, response.vx, response.vy, response.vz))

    def spectrum(self, t):
        request = waves_pb2.SpectrumRequest()
        request.t = t
        response = self.stub.spectrum(request)
        return [{'a':e.a,omega:'e'.omega,'psi':e.psi,'k':e.k,'phase':e.phase} for e in response.spectrum_element]



def run():
    with grpc.insecure_channel('server:50051') as channel:
        parameters = 'some yaml'
        logger.info('Creating Waves instance')
        waves = Waves(channel, parameters)
        elevations_request = [{'x': 1, 'y': 2, 't': 3}, {'x': 6, 'y': 5, 't': 4}]
        logger.info('Elevations for ' + json.dumps(elevations_request))
        elevations = waves.elevations(elevations_request)
        logger.info('Got following elevations from server: ' + json.dumps(elevations))

if __name__ == '__main__':
    logger.info('Starting client')
    run()
