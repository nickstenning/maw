/*
 *  Brain.h
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef BRAIN_H
#define BRAIN_H

#include <iostream>
#include <vector>

class Brain {
public:
  Brain(int id)
  : m_id(id)
  , m_fitness(0.0)
  {};
  
  ~Brain() {};
  
  int const& id() const;
  
  double const& fitness() const;
  Brain const& fitness(double const&);
    
  friend std::ostream& operator<<(std::ostream&, Brain const&);
  
private:
  int m_id;
  double m_fitness;
};

std::ostream& operator<<(std::ostream&, std::vector<Brain> const&);

#endif // BRAIN_H