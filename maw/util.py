import math

def dirac_delta(x=0, a=1):
    return math.exp(-(x**2 * a**2))