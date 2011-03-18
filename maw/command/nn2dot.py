from __future__ import print_function

import sys
from maw.nn import NN

def main():
    print('Reading NN specification from stdin...', file=sys.stderr)

    nn = NN.from_string(sys.stdin.read())

    print(nn.to_dot())

if __name__ == '__main__':
    sys.exit(main())
