from __future__ import print_function

import sys
import textwrap

from maw.nn import NN

COLOURS = [
    "#1f77b4",
    "#ff7f0e",
    "#2ca02c",
    "#d62728",
    "#9467bd",
    "#8c564b",
    "#e377c2",
    "#7f7f7f",
    "#bcbd22",
    "#17becf",
    "#aec7e8",
    "#ffbb78",
    "#98df8a",
    "#ff9896",
    "#c5b0d5",
    "#c49c94",
    "#f7b6d2",
    "#c7c7c7",
    "#dbdb8d",
    "#9edae5",
]

def nn2dot(nn):
    dot = textwrap.dedent("""
    digraph brain {{
      splines=false; nodesep=1; ranksep="1.5 equally"; rankdir=LR;
    {ranks}
    {weights}
    }}
    """)

    ranks = ""
    weights = ""

    for k in range(len(nn.layers)):
        layer = nn.layers[k]

        ranks += "  { rank=same; "
        ranks += " ".join("L{l}_{n}".format(l=k, n=i) for i in range(len(layer)))
        ranks += " }\n"

    for k in range(len(nn.weights)):
        mx = nn.weights[k]
        send = nn.layers[k]
        recv = nn.layers[k + 1]

        for i in range(len(send)):
            for j in range(len(recv)):
                weights += _weight(k, i, k + 1, j, mx[i][j])

    return dot.format(ranks=ranks, weights=weights)

def _weight(layer_i, i, layer_j, j, val):
    if abs(val) < 0.01:
        return ''

    dot = '  L{0}_{1} -> L{2}_{3} [headlabel={4},color="{5}",fontcolor="{5}",fontsize=10,labeldistance=3];\n'

    colour = COLOURS[i % len(COLOURS)]
    return dot.format(layer_i, i, layer_j, j, val, colour)


def main():
    print('Reading NN specification from stdin...', file=sys.stderr)

    nn = NN.from_string(sys.stdin.read())

    print(nn2dot(nn))

if __name__ == '__main__':
    sys.exit(main())
