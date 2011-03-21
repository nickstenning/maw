from bindings.unicycle import Unicycle

class Unicycle(Unicycle):
    def __init__(self,
                 fork_width=0.2,
                 fork_length=4.0,
                 wheel_width=0.25,
                 wheel_radius=2.0,
                 seat_mass=10.0,
                 wheel_mass=2.5,
                 *args):
        super(Unicycle, self).__init__(fork_width,
                                       fork_length,
                                       wheel_width,
                                       wheel_radius,
                                       seat_mass,
                                       wheel_mass,
                                       *args)
