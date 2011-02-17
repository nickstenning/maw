#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

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

NNDotPrinter::~NNDotPrinter()
{
  m_os = NULL;
  delete m_os;
}

std::string const& NNDotPrinter::prefix() const
{
  return m_prefix;
}

NNDotPrinter& NNDotPrinter::prefix(const char p[])
{
  m_prefix = p;
  return *this;
}

NNDotPrinter& NNDotPrinter::operator= (NNDotPrinter const& p)
{
  m_os = p.m_os;
  m_colours = p.m_colours;
  m_prefix = p.m_prefix;
  return *this;
}

NNDotPrinter& NNDotPrinter::operator<< (NN const& nn)
{
  size_t k, i, j;

  std::ostream& os = *m_os;

  os << std::fixed;
  os << m_prefix << "digraph brain {\n";
  os << m_prefix << "  splines=false; nodesep=1; ranksep=\"1.5 equally\"; rankdir=LR;\n";

  for (k = 0; k < nn.m_weights.size(); k += 1) {
    NN::weight_matrix_t const& mx = nn.m_weights[k];
    NN::layer_t const& send = nn.m_layers[k];
    NN::layer_t const& recv = nn.m_layers[k + 1];

    std::ostringstream rankStr;
    rankStr << "  { rank=same;";

    for (i = 0; i < send.size(); i += 1) {
      rankStr << " L" << k << "_" << i;

      for (j = 0; j < recv.size(); j += 1) {
        os << m_prefix;
        printWeight(os, k, i, k + 1, j, mx[i][j]);
      }
    }

    os << m_prefix << rankStr.str() << " }\n";
  }

  std::ostringstream rankStr;
  rankStr << "  { rank=same;";
  NN::layer_t const& lastLayer = nn.m_layers[nn.m_layers.size() - 1];
  for (i = 0; i < lastLayer.size(); i += 1) {
    rankStr << " L" << nn.m_layers.size() - 1 << "_" << i;
  }
  os << m_prefix << rankStr.str() << " }\n";


  os << m_prefix << "}";

  return *this;
}

void NNDotPrinter::printWeight(std::ostream& os, size_t layer_i, size_t i, size_t layer_j, size_t j, double val)
{
  std::string& col = m_colours[i % m_colours.size()];

  os << "  L" << layer_i << "_" << i << " -> L" << layer_j << "_" << j;
  os << " [headlabel=" << val << ",";
  os << "color=\"" << col << "\",fontcolor=\"" << col << "\",";
  os << "fontsize=10,labeldistance=3];\n";
}

void NNDotPrinter::initColours()
{
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