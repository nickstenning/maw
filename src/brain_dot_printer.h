#ifndef BRAIN_DOT_PRINTER_H
#define BRAIN_DOT_PRINTER_H

#include <iostream>
#include <string>
#include <vector>

class Brain;

class BrainDotPrinter {
public:
  BrainDotPrinter(std::ostream& os);
  BrainDotPrinter(BrainDotPrinter const& bp);
  ~BrainDotPrinter();

  std::string const& prefix() const;
  BrainDotPrinter& prefix(const char prefix[]);

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
  std::string m_prefix;

  void printWeight(std::ostream&, std::string, size_t, std::string, size_t, double);
  void initColours();
};

#endif // BRAIN_DOT_PRINTER_H