from bindings.unicycle import Unicycle

sf = 4
person = 10.0

class Unicycle(Unicycle):
    drive_impulse = sf * 75 * 0.02
    wheel_impulse = sf * 100 * 0.02

    def __init__(self,
                 fork_length=sf * 1.2,
                 wheel_radius=sf * 0.5,
                 drive_radius=sf * 0.3,
                 rider_mass=sf * 3 * person/4,
                 wheel_mass=sf * 2.5,
                 drive_mass=sf * person/4,
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
