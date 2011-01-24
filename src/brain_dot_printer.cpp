#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include "brain.h"
#include "brain_dot_printer.h"

BrainDotPrinter::BrainDotPrinter(std::ostream& os)
: m_os(&os)
, m_colours()
, m_prefix("")
{
  initColours();
}

BrainDotPrinter::BrainDotPrinter(BrainDotPrinter const& bp)
: m_os(bp.m_os)
, m_colours(bp.m_colours)
, m_prefix(bp.m_prefix)
{}

BrainDotPrinter::~BrainDotPrinter() {
  m_os = NULL;
}

std::string const& BrainDotPrinter::prefix() const {
  return m_prefix;
}

BrainDotPrinter& BrainDotPrinter::prefix(const char p[]) {
  m_prefix = p;
  return *this;
}

BrainDotPrinter& BrainDotPrinter::operator= (BrainDotPrinter const& bp) {
  m_os = bp.m_os;
  return *this;
}

BrainDotPrinter& BrainDotPrinter::operator<< (Brain const& b) {
  size_t i, j;

  std::ostream& os = *m_os;
  os << std::setprecision(3);

  os << m_prefix << "digraph brain {\n";
  os << m_prefix << "  splines=false; nodesep=1; ranksep=\"1.5 equally\"; rankdir=LR;\n";

  std::ostringstream rankStrI, rankStrH, rankStrO;

  rankStrI << "  { rank=same;";
  rankStrH << "  { rank=same;";
  rankStrO << "  { rank=same;";

  for (i = 0; i < b.m_numInput; i += 1) {
    rankStrI << " I" << i;
    for (j = 0; j < b.m_numHidden; j += 1) {
      os << m_prefix;
      printWeight(os, "I", i, "H", j, b.m_weightsIH[i][j]);
    }
  }

  for (i = 0; i < b.m_numHidden; i += 1) {
    rankStrH << " H" << i;
    for (j = 0; j < b.m_numOutput; j += 1) {
      os << m_prefix;
      printWeight(os, "H", i, "O", j, b.m_weightsHO[i][j]);
    }
  }

  for (i = 0; i < b.m_numOutput; i += 1) {
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

void BrainDotPrinter::printWeight(std::ostream& os, std::string lab_i, size_t i, std::string lab_j, size_t j, double val) {
  std::string& col = m_colours[i % m_colours.size()];

  os << "  " << lab_i << i << " -> " << lab_j << j;
  os << " [headlabel=" << val << ",";
  os << "color=\"" << col << "\",fontcolor=\"" << col << "\",";
  os << "fontsize=10,labeldistance=3];\n";
}


void BrainDotPrinter::initColours() {
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