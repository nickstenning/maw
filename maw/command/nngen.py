from __future__ import print_function

import sys
import argparse

parser = argparse.ArgumentParser(description='Generate a NN specification.')
parser.add_argument('sizes', metavar='SIZE', type=int, nargs='+',
                    help='number of neurons in each layer')
parser.add_argument('-z', '--zeros', dest='zeros', action='store_true', default=False,
                    help='blank connection weights')
parser.add_argument('-r', '--recurrent', dest='recurrent', action='store_true', default=False,
                    help='make a recurrent network')

def main():
    args = parser.parse_args()

    if args.recurrent:
        from maw.rbrain import brain_constructor
    else:
        from maw.brain import brain_constructor

    ctor = brain_constructor(args.sizes, randomize=not args.zeros)

    print(ctor(), end='')

if __name__ == '__main__':
    sys.exit(main())
