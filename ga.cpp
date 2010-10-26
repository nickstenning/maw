/*
 *  ga.cpp
 *  maw
 *
 *  Created by Nick Stenning on 22/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "ga.h"

namespace ga {
  
  std::vector<size_t> piePick (std::vector<double> const& pie, size_t numToPick) {
    double pieSize = std::accumulate(pie.begin(), pie.end(), 0);
    
    std::vector<size_t> picks;
    
    // Pick numToPick indices
    for (size_t i = 0; i < numToPick; i += 1) {
      
      double subTotal = 0.0;
      double pickPoint = (static_cast<double>(rand()) / RAND_MAX) * pieSize;
      
      // Find the index that that point in the pie corresponds to. 
      for (size_t j = 0; j < pie.size(); j += 1) {
        subTotal += pie[j];
        if (subTotal >= pickPoint) {
          picks.push_back(j);
          break;
        }
      }
    }
    
    return picks;
  }
  
  Brain const& recombine (Brain const& parentA, Brain const& parentB) {
    return parentA;
  }
  
  Brain const& mutate (Brain& inputBrain) {
    inputBrain.fitness(inputBrain.fitness() + 1);
    return inputBrain;
  }

}



