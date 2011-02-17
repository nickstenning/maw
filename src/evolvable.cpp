#include "evolvable.h"

Evolvable& Evolvable::fitness(double f)
{
  m_fitness = f;
  return *this;
}

Evolvable* Evolvable::clone()
{
  return new Evolvable();
}

Evolvable* Evolvable::clone(Evolvable* toCopy)
{
  return new Evolvable(*toCopy);
}

double Evolvable::fitness() const
{
  return m_fitness;
}

double operator+ (double const& lhs, Evolvable const& rhs)
{
  return lhs + rhs.fitness();
}

double operator< (Evolvable const& lhs, Evolvable const& rhs)
{
  return lhs.fitness() < rhs.fitness();
}