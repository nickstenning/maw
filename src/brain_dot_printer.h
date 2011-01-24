#ifndef BRAIN_DOT_PRINTER_H
#define BRAIN_DOT_PRINTER_H

#include <iostream>
#include <string>
#include <vector>

typedef unsigned int uint;

class Brain;

class BrainDotPrinter {
public:
  BrainDotPrinter(std::ostream& os);
  BrainDotPrinter(BrainDotPrinter const& bp);
  ~BrainDotPrinter();

  BrainDotPrinter& operator=(BrainDotPrinter const&);

  BrainDotPrinter& operator<<(Brain const& b);

  template<typename T>
  BrainDotPrinter& operator<<(T const& obj) {
    *m_os << obj;
    return *this;
  }
protected:
  std::ostream* m_os;
  std::vector<std::string> m_colours;

  void printWeight(std::ostream&, std::string, uint, std::string, uint, double);
  void initColours();
};

#endif // BRAIN_DOT_PRINTER_H