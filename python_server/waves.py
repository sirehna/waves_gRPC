"""Integrates a scalar wave model in a gRPC server."""

from difflib import SequenceMatcher
import logging
import wave_types_pb2
import wave_grpc_pb2_grpc
import grpc
import yaml
from concurrent import futures
import time


SERVICE_NAME = "waves-server"

logging.basicConfig(
    format='%(asctime)s,%(msecs)d ['
    + SERVICE_NAME
    + '] - %(levelname)-4s [%(filename)s:%(lineno)d] %(message)s',
    datefmt='%d-%m-%Y:%H:%M:%S')
LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(logging.INFO)


def similar(first_string, second_string):
    """Return a score between 0 (strings are different) and 1 (identical)."""
    return SequenceMatcher(None, first_string, second_string).ratio()


def closest_match(expected_keys, unknown_key):
    """Give a suggestion for the parameter name."""
    if expected_keys:
        return "\nMaybe you meant: " + \
         max(expected_keys, key=lambda k: similar(unknown_key, k)) + \
         " <-> " + unknown_key + "?"
    return ""


NOT_IMPLEMENTED = "is not implemented in this model."


class AbstractWaveModel:
    """Defines a (scalar) wave model.

    Vectorization is done by WavesServicer in module 'waves'.
    """

    def set_parameters(self, parameters):
        """Initialize the wave model with YAML parameters.

        Parameters
        ----------
        parameters : string
            YAML string containing the parameters of this model. Can be empty.

        Returns
        -------
        Nothing

        """
        raise NotImplementedError('set_parameters ' + NOT_IMPLEMENTED)

    def elevation(self, x, y, t):
        """Calculate the elevations of the free surface at any point in time.

        Parameters
        ----------
        x : float
            Position (in meters) at which we want the elevation. Projected on
            the X-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        y : float
            Position (in meters) at which we want the elevation. Projected on
            the Y-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        t : float
            Simulation time (in seconds). All values are calculated at that
            instant. The documentation of each waves model should describe how
            the phases are defined.

        Returns
        -------
        float
            Free surface height along the Z-axis (oriented downwards) in
            meters.

        """
        raise NotImplementedError('elevation ' + NOT_IMPLEMENTED)

    def dynamic_pressure(self, x, y, z, t):
        """Calculate the dynamic pressure due to waves.

        Parameters
        ----------
        x : float
            Position (in meters) at which we want the elevation. Projected on
            the X-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        y : float
            Position (in meters) at which we want the elevation. Projected on
            the Y-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        z : float
            Position (in meters) at which we want the elevation. Projected on
            the Z-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        t : float
            Simulation time (in seconds). All values are calculated at that
            instant. The documentation of each waves model should describe how
            the phases are defined.

        Returns
        -------
        float
            Dynamic pressure (in Pascal) at (x,y,z,t).

        """
        raise NotImplementedError('dynamic_pressure ' + NOT_IMPLEMENTED)

    def orbital_velocity(self, x, y, z, t):
        """Calculate the orbital velocity of the wave particles.

        Parameters
        ----------
        x : float
            Position (in meters) at which we want the elevation. Projected on
            the X-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        y : float
            Position (in meters) at which we want the elevation. Projected on
            the Y-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        z : float
            Position (in meters) at which we want the elevation. Projected on
            the Z-axis of the Earth-centered, Earth-fixed North-East-Down
            reference frame.
        t : float
            Simulation time (in seconds). All values are calculated at that
            instant. The documentation of each waves model should describe how
            the phases are defined.

        Returns
        -------
        dict
            Should contain the following fields:
            - vx (float): Projection on the X-axis of the Earth-centered,
                          Earth-fixed North-East-Down reference frame of the
                          velocity of each wave partical relative to the
                          ground. In meters per second.
            - vy (float): Projection on the Y-axis of the Earth-centered,
                          Earth-fixed North-East-Down reference frame of the
                          velocity of each wave partical relative to the
                          ground. In meters per second.
            - vz (float): Projection on the Z-axis of the Earth-centered,
                          Earth-fixed North-East-Down reference frame of the
                          velocity of each wave partical relative to the
                          ground. In meters per second.

        """
        raise NotImplementedError('orbital_velocity ' + NOT_IMPLEMENTED)

    def spectrum(self, x, y, t):
        """Linear spectrum that can be used by xdyn's diffraction module.

        Parameters
        ----------
        x : float
            Position (in meters) at which we want the (linearized) spectrum.
            Projected on the X-axis of the Earth-centered, Earth-fixed
            North-East-Down reference frame.
        y : float
            Position (in meters) at which we want the (linearized) spectrum.
            Projected on the Y-axis of the Earth-centered, Earth-fixed
            North-East-Down reference frame.
        t : float
            Simulation time (in seconds). Date at which we want the
            (linearized) spectrum.

        Returns
        -------
        list of dict
            Each one should contain the following fields:
            - si (list of floats): Discretized spectral density for each
              omega (should therefore be the same size as omega).
              In s m^2/rad.
            - dj (list of floats): Spatial spreading for each psi (should
              therefore be the same size as psi. In 1/rad.
            - omega (list of floats): Angular frequencies the spectrum was
              discretized at. In rad/s.
            - psi (list of floats): Directions between 0 & 2pi the spatial
              spreading was discretized at. In rad.
            - k (list of floats): Discretized wave number for each
              frequency (should therefore be the same size as omega).
              In rad/m.
            - phase (list of dict): Random phases, for each
              (direction,frequency) couple (but time invariant), should
              have the same size as psi. In radian.
              Each element is a dict containing the following fields:
              - phase (list of floats): Random phase. Should have the same
                size as omega in parent dict. In radian.

        """
        raise NotImplementedError('spectrum ' + NOT_IMPLEMENTED)

    def angular_frequencies_for_rao(self):
        """Get angular frequencies the wave spectrum is discretized at.

        Returns
        -------
        list of floats
            Angular frequencies the spectrum was discretized at (in rad/s).
            Used, for example, when interpolating the wave RAOs.

        """
        raise NotImplementedError('angular_frequencies_for_rao '
                                  + NOT_IMPLEMENTED)

    def directions_for_rao(self):
        """Get the incidences the directional spreading is discretized at.

        Returns
        -------
        list of floats
          Wave incidences the spectrum was discretized at (in rad).
          0° is for waves coming from the North.
          90° for waves coming from the East.
          Used, for example, when interpolating the wave RAOs.

        """
        raise NotImplementedError('angular_frequencies_for_rao '
                                  + NOT_IMPLEMENTED)


class WavesServicer(wave_grpc_pb2_grpc.WavesServicer):
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
            match = closest_match(list(yaml.safe_load(request.parameters)),
                                  str(exception).replace("'", ""))
            context.set_details("Unable to find key "
                                + str(exception)
                                + " in the YAML. " + match)
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return wave_types_pb2.SetParameterResponse(error_message=repr(exception))
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return wave_types_pb2.SetParameterResponse(error_message=repr(exception))
        return wave_types_pb2.SetParameterResponse(error_message='')

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
        response = wave_types_pb2.XYZTGrid()
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
        response = wave_types_pb2.DynamicPressuresResponse()
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
        response = wave_types_pb2.OrbitalVelocitiesResponse()
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
            spectra = self.model.spectrum(request.x, request.y, request.t)
        except NotImplementedError as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        except Exception as exception:
            context.set_details(repr(exception))
            context.set_code(grpc.StatusCode.UNKNOWN)
        response = wave_types_pb2.SpectrumResponse()
        for spectrum in spectra:
            s = wave_types_pb2.Spectrum()
            s.si[:] = spectrum.si
            s.dj[:] = spectrum.dj
            s.omega[:] = spectrum.omega
            s.psi[:] = spectrum.psi
            s.phase[:] = spectrum.phase
            response.spectrum.append(s)
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
        response = wave_types_pb2.AngularFrequencies()
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
        response = wave_types_pb2.Directions()
        response.psis[:] = psis
        return response


_ONE_DAY_IN_SECONDS = 60 * 60 * 24


def serve(model):
    """Launch the gRPC server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    wave_grpc_pb2_grpc.add_WavesServicer_to_server(
        WavesServicer(model), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    try:
        while True:
            time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server.stop(0)
