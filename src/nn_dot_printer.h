#ifndef NN_DOT_PRINTER_H
#define NN_DOT_PRINTER_H

#include <iostream>
#include <string>
#include <vector>

class NN;

class NNDotPrinter {
public:
  NNDotPrinter(std::ostream& os);
  NNDotPrinter(NNDotPrinter const& bp);
  ~NNDotPrinter();

  std::string const& prefix() const;
  NNDotPrinter& prefix(const char prefix[]);

  NNDotPrinter& operator=(NNDotPrinter const&);
  NNDotPrinter& operator<<(NN const&);

  template<typename T>
  NNDotPrinter& operator<<(T const& obj) {
    *m_os << obj;
    return *this;
  }
protected:
  std::ostream* m_os;
  std::vector<std::string> m_colours;
  std::string m_prefix;

  void printWeight(std::ostream&, size_t, size_t, size_t, size_t, double);
  void initColours();
};

#endif // NN_DOT_PRINTER_H