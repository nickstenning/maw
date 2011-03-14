class Evolvable(object):
    fitness = 0

    def ga_init(self):
        pass

    def crossover(self, other):
        return self

    def mutate(self):
        return self