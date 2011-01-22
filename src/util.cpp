/*
 *  util.cpp
 *  maw
 *
 *  Created by Nick Stenning on 26/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#include "util.h"

#include <iostream>

double util::rand (double a, double b) {
  double normed = static_cast<double>(::rand()) / RAND_MAX;
  return (b - a) * normed + a;
}