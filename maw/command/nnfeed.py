from __future__ import print_function

import sys
import argparse

from maw.nn import NN

parser = argparse.ArgumentParser(description='Feed forward values through a NN specification.')
parser.add_argument('inputs', metavar='VALUE', type=float, nargs='+',
                    help='input values')

def main():
    args = parser.parse_args()

    print('Reading NN specification from stdin...', file=sys.stderr)

    nn = NN.from_string(sys.stdin.read())

    num_given = len(args.inputs)
    num_needed = len(nn.layers[0])

    if num_given != num_needed:
        print(
          "Error: supplied {0} input values to a {1}-input network.".format(num_given, num_needed),
          file=sys.stderr
        )
        return 1

    print(nn.feed(list(args.inputs)))

if __name__ == '__main__':
    sys.exit(main())


