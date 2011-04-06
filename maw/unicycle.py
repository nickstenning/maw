from bindings.unicycle import Unicycle

class Unicycle(Unicycle):
    def __init__(self,
                 fork_length=4.0,
                 wheel_radius=2.0,
                 drive_radius=1.8,
                 rider_mass=5.00,
                 wheel_mass=2.5,
                 drive_mass=10.00,
                 wheel_impulse=5.0,
                 drive_impulse=2.0,
                 drive_limits=False,
                 *args):
        super(Unicycle, self).__init__(fork_length,
                                       wheel_radius,
                                       drive_radius,
                                       rider_mass,
                                       wheel_mass,
                                       drive_mass,
                                       wheel_impulse,
                                       drive_impulse,
                                       drive_limits,
                                       *args)
