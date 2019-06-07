from wave_model_interface import AbstractWaveModel 

class WaveModelStub(AbstractWaveModel):

  def set_parameters(self, parameters):
    self.parameters = parameters

  def elevation(self, x, y, t):
    return x+y+t

  def dynamic_pressure(self, x, y, z, t):
    return x+y+z+t

  def orbital_velocity(self, x, y, z, t):
    return {vx: x, vy: y, vz: z}


