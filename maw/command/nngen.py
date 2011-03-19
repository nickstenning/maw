from __future__ import print_function

import sys
import argparse

from maw.nn import NN

import argparse

parser = argparse.ArgumentParser(description='Generate a feedforward NN specification.')
parser.add_argument('sizes', metavar='SIZE', type=int, nargs='+',
                    help='number of neurons in each layer')
parser.add_argument('-r', '--random', dest='random', action='store_true', default=False,
                    help='randomize connection weights')

def main():
    args = parser.parse_args()

    nn = NN(args.sizes)

    if args.random:
        nn.set_weights_random()

    print(nn, end='')

if __name__ == '__main__':
    sys.exit(main())
