class AbstractWaveModel(object):

    def set_parameters(self, parameters):
        pass
        raise(NotImplementedError('Method not implemented!'))

    def elevation(self, x, y, t):
        pass
        raise(NotImplementedError('Method not implemented!'))

    def dynamic_pressure(self, x, y, z, t):
        pass
        raise(NotImplementedError('Method not implemented!'))

    def orbital_velocity(self, x, y, z, t):
        pass
        raise(NotImplementedError('Method not implemented!'))
