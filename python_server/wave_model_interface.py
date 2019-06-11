"""Defines the AbstractWaveModel interface."""

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

