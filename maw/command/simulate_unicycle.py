from __future__ import print_function

import sys
import argparse

from maw.simulate.unicycle import simulate

parser = argparse.ArgumentParser(description='Simulate a unicycle')
parser.add_argument('-r', '--recurrent', dest='recurrent', action='store_true', default=False,
                    help='use a recurrent network')

def main():
    args = parser.parse_args()

    if args.recurrent:
        from maw.rnn import RNN as NNKlass
    else:
        from maw.nn import NN as NNKlass

    print('Reading ' + str(NNKlass) + ' from stdin...', file=sys.stderr)

    nn = NNKlass.from_string(sys.stdin.read())

    simulate(nn)

if __name__ == '__main__':
    sys.exit(main())


