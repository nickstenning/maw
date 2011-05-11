import random
from util import roulette_pick

ELITISM = 0.1
CROSSOVER_PROB = 0.3

class GA(object):
    generation = 0
    population = []

    def __init__(self, ctor, fitness_function=lambda x: 1.0):
        self.ctor = ctor
        self.fitness_function = fitness_function

    def add_individual(self, obj=None, count=1):
        """Add an individual"""
        for _ in xrange(count):
            if obj:
                ind = obj
            else:
                ind = self.ctor()

            ind.fitness = self.fitness_function(ind)
            self.population.append(ind)

    def step(self):
        self.generation += 1

        self.population = sorted(self.population, lambda x, y: cmp(x.fitness, y.fitness))

        new_population = []

        # Elitism step. Preserve some proportion of the population untouched
        # for the next generation.
        num_elite = int(round(ELITISM * len(self.population)))

        for i in range(-num_elite, 0):
            individual = self.population[i].clone()
            new_population.append(individual)

        # Replace the rest of the population with roulette-picked individuals
        for i in range(len(self.population) - num_elite):
            # Copy-construct new individual
            individual = roulette_pick(self.population).clone()

            if random.random() < CROSSOVER_PROB:
                individual.crossover(roulette_pick(self.population))
            else:
                individual.mutate()

            new_population.append(individual)

        for individual in self.population:
            del individual # Need to do this to trigger C++ destructors?

        self.population = new_population

        # Compute fitness for next generation
        for ind in self.population:
            ind.fitness = self.fitness_function(ind)
