"""gRPC sample client for wave models."""

from concurrent import futures
import json
import time
import logging

import grpc

import waves_pb2
import waves_pb2_grpc

_ONE_DAY_IN_SECONDS = 60 * 60 * 24

SERVICE_NAME = "waves-client"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d ['
    + SERVICE_NAME
    + '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(logging.INFO)


def serve():
    """Connect to the gRPC wave server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    waves_pb2_grpc.add_WavesServicer_to_server(
        waves_pb2_grpc.WavesServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    try:
        while True:
            time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server.stop(0)


class Waves:
    """This class wraps the gRPC calls for use with "normal" python types."""

    def __init__(self, channel, parameters):
        """Constructor."""
        self.stub = waves_pb2_grpc.WavesStub(channel)
        response = self.set_parameters(parameters)
        if response:
            LOGGER.error('Error connecting to wave server: %s', response)

    def set_parameters(self, parameters):
        """Set the wave model's YAML parameters."""
        request = waves_pb2.SetParameterRequest(parameters=parameters)
        return self.stub.set_parameters(request).error_message

    def elevations(self, xyts):
        """Compute wave elevations on a grid."""
        xytgrid = waves_pb2.XYTGrid()
        for xyt in xyts:
            xytgrid.x.append(xyt['x'])
            xytgrid.y.append(xyt['y'])
            xytgrid.t = xyt['t']
        xyztgrid = self.stub.elevations(xytgrid)
        return list(map(lambda x, y, z:
                        {'x': x,
                         'y': y,
                         'z': z,
                         't': xyztgrid.t},
                        xyztgrid.x,
                        xyztgrid.y,
                        xyztgrid.z))

    def dynamic_pressures(self, xyzts):
        """Compute the wave dynamic pressure on a grid."""
        xyztgrid = waves_pb2.XYZTGrid()
        xyztgrid.t = xyzts['t']
        for xyt in xyzts:
            xyztgrid.x.append(xyt['x'])
            xyztgrid.y.append(xyt['y'])
            xyztgrid.z.append(xyt['z'])
            xyztgrid.t = xyt['t']
        response = self.stub.dynamic_pressures(xyztgrid)
        return list(map(lambda x, y, z, pdyn:
                        {'x': x,
                         'y': y,
                         'z': z,
                         't': response.t,
                         'pdyn': pdyn
                         },
                        response.x,
                        response.y,
                        response.z,
                        response.pdyn))

    def orbital_velocities(self, xyzts):
        """Compute the wave particles orbital velocity on a grid."""
        xyztgrid = waves_pb2.XYZTGrid()
        xyztgrid.t = xyzts['t']
        for xyt in xyzts:
            xyztgrid.x.append(xyt['x'])
            xyztgrid.y.append(xyt['y'])
            xyztgrid.z.append(xyt['z'])
        response = self.stub.orbital_velocities(xyztgrid)
        return list(map(lambda x, y, z, vx, vy, vz:
                        {'x': x,
                         'y': y,
                         'z': z,
                         't': response.t,
                         'vx': vx,
                         'vy': vy,
                         'vz': vz},
                        response.x,
                        response.y,
                        response.z,
                        response.vx,
                        response.vy,
                        response.vz))

    def spectrum(self, t):
        """Get a linear wave spectrum at a given point in space-time."""
        request = waves_pb2.SpectrumRequest()
        request.t = t
        response = self.stub.spectrum(request)
        return [{'a': e.a,
                 'omega': e.omega,
                 'psi': e.psi,
                 'k': e.k,
                 'phase': e.phase} for e in response.spectrum_element]


def run():
    """Launch the server & run some gRPC calls."""
    with grpc.insecure_channel('server:50051') as channel:
        parameters = """
        {Hs: 5, Tp: 15, gamma: 1.2, waves propagating to: 0, omega: [1,2,3]}
        """
        LOGGER.info('Creating Waves instance')
        waves = Waves(channel, parameters)
        elevations_request = [{'x': 1, 'y': 2, 't': 3}, {'x': 6, 'y': 5, 't': 4}]
        LOGGER.info('Elevations for %s', json.dumps(elevations_request))
        elevations = waves.elevations(elevations_request)
        LOGGER.info('Got following elevations from server: %s', json.dumps(elevations))


if __name__ == '__main__':
    LOGGER.info('Starting client')
    run()
