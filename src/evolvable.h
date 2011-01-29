#ifndef EVOLVABLE_H
#define EVOLVABLE_H

/**
 * Base class for Evolvable objects.
**/
class Evolvable
{
public:
  Evolvable()
  : m_fitness(0)
  {};

  virtual ~Evolvable() {};

  virtual void gaInit() {};

  virtual Evolvable* crossover(Evolvable const*) {
    return this;
  };
  virtual Evolvable* mutate() {
    return this;
  };
  virtual Evolvable* clone();
  virtual Evolvable* clone(Evolvable*);

  Evolvable& fitness(double f);
  double fitness() const;

  friend double operator+ (double const& lhs, Evolvable const& rhs);
  friend double operator< (Evolvable const& lhs, Evolvable const& rhs);

private:
  double m_fitness;
};

#endif // EVOLVABLE_H
