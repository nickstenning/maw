from bindings.unicycle import Unicycle

DT = 0.01

sf = 1
person = 10.0

class Unicycle(Unicycle):
    drive_impulse = 10 * DT
    wheel_impulse = 15 * DT

    def __init__(self,
                 fork_length=1.2,
                 wheel_radius=0.5,
                 drive_radius=0.3,
                 rider_mass=person/2.0,
                 wheel_mass=1.0,
                 drive_mass=person/2.0,
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
