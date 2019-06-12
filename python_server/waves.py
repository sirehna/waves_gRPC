"""Integrates a scalar wave model in a gRPC server."""

import logging
import waves_pb2
import waves_pb2_grpc
import grpc

SERVICE_NAME = "waves-server"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d ['
    + SERVICE_NAME
    + '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(logging.INFO)


class WavesServicer(waves_pb2_grpc.WavesServicer):
    """Implements the gRPC methods defined in waves.proto."""

    def __init__(self, model):
        """Constructor.

        Parameters
        ----------
        model : AbstractWaveModel
            Implements the scalar wave model to use.

        """
        self.model = model

    def set_parameters(self, request, context):
        """Set the parameters of self.model.

        Parameters
        ----------
        request : SetParameterRequest
            Defined in waves.proto.

        Returns
        -------
        dict
            Should contain the following fields:
            - error_message (string): empty if everything went OK.

        """
        LOGGER.info('Received parameters: %s', request.parameters)
        try:
            self.model.set_parameters(request.parameters)
        except KeyError as exception:
            context.set_details("Unable to find key "
                                + str(exception)
                                + " in the YAML. ")
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return waves_pb2.SetParameterResponse(error_message=repr(exception))
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return waves_pb2.SetParameterResponse(error_message=repr(exception))
        return waves_pb2.SetParameterResponse(error_message='')

    def elevations(self, request, context):
        """Get wave elevations from self.model.

        Parameters
        ----------
        request : XYTGrid
            Defined in waves.proto.

        Returns
        -------
        XYZTGrid
            Defined in waves.proto.

        """
        LOGGER.info('Got elevation request')
        xys = map(lambda x, y: {'x': x, 'y': y}, request.x, request.y)
        try:
            z_s = [self.model.elevation(xy['x'], xy['y'], request.t) for xy in xys]
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = waves_pb2.XYZTGrid()
        response.x[:] = request.x
        response.y[:] = request.y
        response.z[:] = z_s
        response.t = request.t
        return response

    def dynamic_pressures(self, request, context):
        """Get dynamic pressure from self.model.

        Parameters
        ----------
        request : XYTZGrid
            Defined in waves.proto.

        Returns
        -------
        DynamicPressuresResponse
            Defined in waves.proto.

        """
        LOGGER.info('Got dynamic pressure request')
        xyzs = map(lambda x, y, z: {'x': x, 'y': y, 'z': request.z},
                   request.x, request.y, request.z)
        try:
            pdyn = [self.model.dynamic_pressure(xyz['x'], xyz['y'], request.t)
                    for xyz in xyzs]
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = waves_pb2.DynamicPressuresResponse()
        response.x[:] = request.x
        response.y[:] = request.y
        response.z[:] = request.z
        response.t = request.t
        response.pdyn[:] = pdyn
        return response

    def orbital_velocities(self, request, context):
        """Get orbital velocities from self.model.

        Parameters
        ----------
        request : XYTZGrid
            Defined in waves.proto.

        Returns
        -------
        OrbitalVelocitiesResponse
            Defined in waves.proto.

        """
        LOGGER.info('Got orbital velocities request')
        xyzs = map(lambda x, y, z: {x: x, y: y, z: z}, request.x, request.y,
                   request.z)
        try:
            vorbs = [self.model.orbital_velocity(xyz.x, xyz.y, xyz.z, request.t)
                     for xyz in xyzs]
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
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
        """Get spectrum from self.model.

        Parameters
        ----------
        request : SpectrumRequest
            Defined in waves.proto.

        Returns
        -------
        SpectrumResponse
            Defined in waves.proto.

        """
        LOGGER.info('Got spectrum request')
        try:
            spectrum = self.model.spectrum(request.x, request.y, request.t)
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = waves_pb2.SpectrumResponse()
        response.si[:] = spectrum.si
        response.dj[:] = spectrum.dj
        response.omega[:] = spectrum.omega
        response.psi[:] = spectrum.psi
        response.phase[:] = spectrum.phase
        return response

    def angular_frequencies_for_rao(self, _, context):
        """Get angular frequencies from self.model.

        Returns
        -------
        AngularFrequencies
            Defined in waves.proto.

        """
        LOGGER.info('Got angular_frequencies_for_rao request')
        try:
            omegas = self.model.angular_frequencies_for_rao()
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = waves_pb2.AngularFrequencies()
        response.omegas[:] = omegas
        return response

    def directions_for_rao(self, _, context):
        """Get the incidences from self.model.

        Returns
        -------
        Directions
            Defined in waves.proto.

        """
        LOGGER.info('Got directions_for_rao request')
        try:
            psis = self.model.directions_for_rao()
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = waves_pb2.Directions()
        response.psis[:] = psis
        return response
