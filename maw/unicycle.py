from bindings.unicycle import Unicycle

class Unicycle(Unicycle):
    drive_impulse = 4.0
    wheel_impulse = 2.0

    def __init__(self,
                 fork_length=4.0,
                 wheel_radius=2.0,
                 drive_radius=1.5,
                 rider_mass=5.00,
                 wheel_mass=2.5,
                 drive_mass=5.00,
                 drive_limits=False,
                 *args):
        super(Unicycle, self).__init__(fork_length,
                                       wheel_radius,
                                       drive_radius,
                                       rider_mass,
                                       wheel_mass,
                                       drive_mass,
                                       drive_limits,
                                       *args)
