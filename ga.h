/*
 *  ga.h
 *  maw
 *
 *  Created by Nick Stenning on 22/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef GA_H
#define GA_H

#include <vector>
#include <numeric>

#include "Brain.h"
#include "util.h"

namespace ga {

  std::vector<size_t> piePick (std::vector<double> const& pie, size_t numToPick);

  Brain const& recombine (Brain const&, Brain const&);
  void mutate (Brain&);

}

#endif // GA_H