from __future__ import print_function

import sys

from maw.nn import NN
from maw.simulate.unicycle import simulate

def main():
    print('Reading NN specification from stdin...', file=sys.stderr)

    nn = NN.from_string(sys.stdin.read())
    simulate(nn)

if __name__ == '__main__':
    sys.exit(main())


