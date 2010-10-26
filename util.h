/*
 *  util.h
 *  maw
 *
 *  Created by Nick Stenning on 22/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef UTIL_H
#define UTIL_H

// How to print a list
template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& list) {
  typename std::vector<T>::const_iterator i = list.begin();
  
  if (i == list.end()) {
    os << "[ ]";
    return os;
  }
  
  os << "[ " << *i << "\n";
  
  ++i;
  for (; i != list.end(); ++i) {
    os << ", " << *i << "\n";
  }
  
  os << "]";
  return os;
}

#endif // UTIL_H