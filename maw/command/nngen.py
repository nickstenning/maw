from __future__ import print_function

import sys

from maw.nn import NN

def main():
    random = False

    if sys.argv[1] == '-r':
        sys.argv.pop(1)
        random = True

    if len(sys.argv) < 2:
        print("Usage: nngen <LAYER_1_SIZE>, <LAYER_2_SIZE>, ...", file=sys.stderr)
    else:
        layer_sizes = [int(i) for i in sys.argv[1:]]
        nn = NN(layer_sizes)

        if random:
            nn.set_weights_random()

        print(nn)

if __name__ == '__main__':
    sys.exit(main())
