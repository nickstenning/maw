from __future__ import print_function

import sys
import signal
import argparse

from maw.ga import GA, ELITISM
import maw.ga.fitness.unicycle as ff

parser = argparse.ArgumentParser(description='Evolve a population of NNs.')
parser.add_argument('-g', '--generations', dest='generations', default=50,
                    type=int, help='number of generations', metavar='NUM')
parser.add_argument('-p', '--popsize', dest='popsize', default=50,
                    type=int, help='size of population', metavar='SIZE')
parser.add_argument('-r', '--recurrent', dest='recurrent', action='store_true', default=False,
                    help='evolve a recurrent network')

evaluator = ff.Evaluator()

exit_early = False

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

def catch_usr1(signo, frame):
    global exit_early
    exit_early = True

signal.signal(signal.SIGUSR1, catch_usr1)

def main():
    args = parser.parse_args()

    if args.popsize < 1:
        print('Must specify a popsize of at least 1.', file=sys.stderr)
        return 1

    if args.recurrent:
        from maw.rbrain import RBrain, brain_constructor
        klass = RBrain
    else:
        from maw.brain import Brain, brain_constructor
        klass = Brain

    print('Reading ' + str(klass) + ' from stdin...', file=sys.stderr)

    model_brain = klass.from_string(sys.stdin.read())

    ga = GA(brain_constructor(model_brain.spec()), evaluator.evaluate)

    prepop = int(ELITISM * args.popsize)

    ga.add_individual(model_brain, prepop)
    ga.add_individual(count=args.popsize - prepop) # Random remaining brains

    print("# gen      minFit     maxFit     meanFit    sdFit", file=sys.stderr)
    while ga.generation < args.generations and not exit_early:
        ga.step()
        print_stats(ga)

    sorted_pop = sorted(ga.population, lambda x, y: cmp(x.fitness, y.fitness))
    fittest = sorted_pop[-1]
    print(fittest)

    return 0

if __name__ == '__main__':
    sys.exit(main())

