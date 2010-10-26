/*
 *  Brain.cpp
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "Brain.h"

int const& Brain::id() const {
  return m_id;
}

double const& Brain::fitness() const {
  return m_fitness;
}

Brain const& Brain::fitness(double const& fitness) {
  m_fitness = fitness;
  return *this;
}

// How to print a brain
std::ostream& operator<< (std::ostream& os, Brain const& b) {
  os << "<Brain " << b.m_id << " ϕ:" << b.m_fitness << ">";
  return os;
}