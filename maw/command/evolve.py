from __future__ import print_function

import sys
from optparse import OptionParser

from maw.ga import GA
from maw.brain import Brain
import maw.ga.fitness.unicycle as ff

parser = OptionParser()
parser.add_option("-g", "--generations", dest="generations", default=50,
                  type="int", help="number of generations", metavar="NUM")
parser.add_option("-p", "--popsize", dest="popsize", default=50,
                  type="int", help="size of population", metavar="SIZE")

evaluator = ff.Evaluator()

def fitness_function(brain):
    return evaluator.evaluate(brain)

def print_stats(ga):
    sorted_pop = sorted(ga.population, lambda x, y: cmp(x.fitness, y.fitness))

    mean = reduce(lambda sum, x: sum + x.fitness, sorted_pop, 0) / len(sorted_pop);

    sumsqdev = reduce(lambda sum, x: sum + (x.fitness - mean) ** 2, sorted_pop, 0)
    stddev = (sumsqdev / len(sorted_pop)) ** 0.5

    stats = '{0:<10} {1:<10.2f} {2:<10.2f} {3:<10.2f} {4:<10.2f}'.format(
        ga.generation,
        sorted_pop[0].fitness,
        sorted_pop[-1].fitness,
        mean,
        stddev
    )

    print(stats, file=sys.stderr)

def main():
    (options, args) = parser.parse_args()

    if options.popsize < 1:
        print('Must specify a popsize of at least 1.', file=sys.stderr)
        return 1

    print('Reading brain from stdin...', file=sys.stderr)

    model_brain = Brain.from_string(sys.stdin.read())

    def ctor():
        b = Brain([len(l) for l in model_brain.layers])
        b.set_weights_random()
        return b

    ga = GA(ctor, fitness_function)

    ga.add_individual(model_brain)
    ga.add_individual(count=options.popsize - 1) # Random remaining brains

    print("# gen      minFit     maxFit     meanFit    sdFit", file=sys.stderr)
    while ga.generation < options.generations:
        ga.step()
        print_stats(ga)

    sorted_pop = sorted(ga.population, lambda x, y: cmp(x.fitness, y.fitness))
    fittest = sorted_pop[-1]
    print(fittest)

    return 0

if __name__ == '__main__':
    sys.exit(main())
