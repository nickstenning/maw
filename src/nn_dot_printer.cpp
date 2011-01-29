#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include "nn.h"
#include "nn_dot_printer.h"

NNDotPrinter::NNDotPrinter(std::ostream& os)
: m_os(&os)
, m_colours()
, m_prefix("")
{
  initColours();
}

NNDotPrinter::NNDotPrinter(NNDotPrinter const& p)
: m_os(p.m_os) // We don't own this pointer, so this is fine.
, m_colours(p.m_colours)
, m_prefix(p.m_prefix)
{}

NNDotPrinter::~NNDotPrinter() {
  m_os = NULL;
  delete m_os;
}

std::string const& NNDotPrinter::prefix() const {
  return m_prefix;
}

NNDotPrinter& NNDotPrinter::prefix(const char p[]) {
  m_prefix = p;
  return *this;
}

NNDotPrinter& NNDotPrinter::operator= (NNDotPrinter const& p) {
  m_os = p.m_os;
  m_colours = p.m_colours;
  m_prefix = p.m_prefix;
  return *this;
}

NNDotPrinter& NNDotPrinter::operator<< (NN const& nn) {
  size_t i, j;

  std::ostream& os = *m_os;
  os << std::setprecision(3);

  os << m_prefix << "digraph brain {\n";
  os << m_prefix << "  splines=false; nodesep=1; ranksep=\"1.5 equally\"; rankdir=LR;\n";

  std::ostringstream rankStrI, rankStrH, rankStrO;

  rankStrI << "  { rank=same;";
  rankStrH << "  { rank=same;";
  rankStrO << "  { rank=same;";

  for (i = 0; i < nn.m_numInput; i += 1) {
    rankStrI << " I" << i;
    for (j = 0; j < nn.m_numHidden; j += 1) {
      os << m_prefix;
      printWeight(os, "I", i, "H", j, nn.m_weightsIH[i][j]);
    }
  }

  for (i = 0; i < nn.m_numHidden; i += 1) {
    rankStrH << " H" << i;
    for (j = 0; j < nn.m_numOutput; j += 1) {
      os << m_prefix;
      printWeight(os, "H", i, "O", j, nn.m_weightsHO[i][j]);
    }
  }

  for (i = 0; i < nn.m_numOutput; i += 1) {
    rankStrO << " O" << i;
  }

  rankStrI << " }\n";
  rankStrH << " }\n";
  rankStrO << " }\n";

  os << m_prefix << rankStrI.str()
     << m_prefix << rankStrH.str()
     << m_prefix << rankStrO.str();

  os << m_prefix << "}";

  return *this;
}

void NNDotPrinter::printWeight(std::ostream& os, std::string lab_i, size_t i, std::string lab_j, size_t j, double val) {
  std::string& col = m_colours[i % m_colours.size()];

  os << "  " << lab_i << i << " -> " << lab_j << j;
  os << " [headlabel=" << val << ",";
  os << "color=\"" << col << "\",fontcolor=\"" << col << "\",";
  os << "fontsize=10,labeldistance=3];\n";
}

void NNDotPrinter::initColours() {
  m_colours.push_back("#1f77b4");
  m_colours.push_back("#ff7f0e");
  m_colours.push_back("#2ca02c");
  m_colours.push_back("#d62728");
  m_colours.push_back("#9467bd");
  m_colours.push_back("#8c564b");
  m_colours.push_back("#e377c2");
  m_colours.push_back("#7f7f7f");
  m_colours.push_back("#bcbd22");
  m_colours.push_back("#17becf");
  m_colours.push_back("#aec7e8");
  m_colours.push_back("#ffbb78");
  m_colours.push_back("#98df8a");
  m_colours.push_back("#ff9896");
  m_colours.push_back("#c5b0d5");
  m_colours.push_back("#c49c94");
  m_colours.push_back("#f7b6d2");
  m_colours.push_back("#c7c7c7");
  m_colours.push_back("#dbdb8d");
  m_colours.push_back("#9edae5");
}