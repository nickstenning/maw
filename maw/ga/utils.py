import random

def roulette_pick(population):
    wheel_size = reduce(lambda sum, x: sum + x.fitness, population, 0);
    pick_point = random.uniform(0, wheel_size)

    # Find the index that that point in the pie corresponds to.
    subtotal = 0
    for individual in population:
      subtotal += individual.fitness
      if subtotal >= pick_point:
        return individual

if __name__ == '__main__':
    from evolvable import Evolvable

    pop = [Evolvable() for i in range(10)]

    for i in range(10):
        pop[i].fitness = i

    print [roulette_pick(pop).fitness for i in range(20)]