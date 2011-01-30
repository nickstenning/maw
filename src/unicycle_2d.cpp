#include <cmath>
#include <cstdlib>

#include "util.h"
#include "unicycle_2d.h"

const double Unicycle2D::pi      = 3.141592653589793238462643;
const double Unicycle2D::g       = 9.81; // m s^-2
const double Unicycle2D::dt      = 0.01; // s

const double Unicycle2D::postLength  = 1;   // m
const double Unicycle2D::seatMass    = 10;  // kg
const double Unicycle2D::wheelRadius = 0.2; // m
const double Unicycle2D::wheelMass   = 10;   // kg

static const double M = Unicycle2D::wheelMass;
static const double m = Unicycle2D::seatMass;
static const double l = Unicycle2D::postLength;
static const double r = Unicycle2D::wheelRadius;

// MoI of wheel
static const double I = (M*r*r) / 2.0;

// KE of wheel, and of seat due to linear motion
static const double A = (I + M*r*r + m*r*r) / 2.0;
// KE of seat
static const double B = m * l * l / 2.0;
// Coupling
static const double C = m * r * l;
// Gravitational PE
static const double D = m*l*Unicycle2D::g;

// 2D unicycle equations of motion.
//
// Note no explicit dependence on angle of wheel (as we'd expect from symmetry).
std::vector<double> physics (double p_0, double dp_0, double /*w_0*/, double dw_0) {
  std::vector<double> result;

  double C_cos_p = C * cos(p_0);

  double d_p = dp_0;
  result.push_back(d_p);

  double d_dp_numer = sin(p_0) * ((C * dp_0 - D) * dp_0 - (2.0 * A * D)/C_cos_p);
  double d_dp_denom = C_cos_p - (4.0 * A * B)/C_cos_p;
  double d_dp = d_dp_denom / d_dp_numer;
  result.push_back(d_dp);

  double d_w = dw_0;
  result.push_back(d_w);

  double d_dw = (D * sin(p_0) - 2.0 * B * d_dp) / C_cos_p;
  result.push_back(d_dw);

  return result;
}

Unicycle2D::Unicycle2D()
: m_t(0.0)
, m_p(0.0)
, m_dp(0.0)
, m_w(0.0)
, m_dw(0.0)
{}

void Unicycle2D::step(double ext_dt) {
  // Unicycle2D::dt provides a minimum physics resolution, but ext_dt can specify
  // that it wants the simulation to run for longer in a single call to step().
  while (ext_dt > dt) {
    ext_dt -= dt;
    rk_step(dt, physics);
  }
  rk_step(ext_dt, physics);

  double limit = (pi / 2) + asin(r / l);
  // Angular limit -- don't allow seat below floor:
  if (m_p > limit) {
    std::cout << "hit limit pos\n";
    m_p = limit;
  }
  if (m_p < -limit) {
    std::cout << "hit limit neg\n";
    m_p = -limit;
  }

  // Maximum speed
  // if (m_dp > 10 * pi)  m_dp = 10 * pi;
  // if (m_dp < -10 * pi) m_dp = -10 * pi;
  // if (m_dw > 10 * pi)  m_dw = 10 * pi;
  // if (m_dw < -10 * pi) m_dw = -10 * pi;

  m_t += dt;
}

void Unicycle2D::rk_step(double h, rkBlock func) {
  typedef std::vector<double> state;

  state c1_v = func(m_p, m_dp, m_w, m_dw);
  double c1_p  = h * c1_v[0];
  double c1_w  = h * c1_v[2];
  double c1_dp = h * c1_v[1];
  double c1_dw = h * c1_v[3];

  state c2_v = func(m_p + c1_p/2, m_dp + c1_dp/2, m_w + c1_w/2, m_dw + c1_dw/2);
  double c2_p  = h * c2_v[0];
  double c2_w  = h * c2_v[2];
  double c2_dp = h * c2_v[1];
  double c2_dw = h * c2_v[3];

  state c3_v = func(m_p + c2_p/2, m_dp + c2_dp/2, m_w + c2_w/2, m_dw + c2_dw/2);
  double c3_p  = h * c3_v[0];
  double c3_w  = h * c3_v[2];
  double c3_dp = h * c3_v[1];
  double c3_dw = h * c3_v[3];

  state c4_v = func(m_p + c3_p, m_dp + c3_dp, m_w + c3_w, m_dw + c3_dw);
  double c4_p  = h * c4_v[0];
  double c4_w  = h * c4_v[2];
  double c4_dp = h * c4_v[1];
  double c4_dw = h * c4_v[3];

  m_p += (c1_p + 2*c2_p + 2*c3_p + c4_p)/6;
  m_w += (c1_w + 2*c2_w + 2*c3_w + c4_w)/6;
  m_dp = (c1_dp + 2*c2_dp + 2*c3_dp + c4_dp)/6;
  m_dw = (c1_dw + 2*c2_dw + 2*c3_dw + c4_dw)/6;
}

double const Unicycle2D::T() const {
  double T_wheel = ((I + M * r * r) * m_dw * m_dw) / 2.0;
  double T_seat = (m * (r * r * m_dw * m_dw + l * l * m_dp * m_dp)) / 2.0;
  double T_coupling = m * r * l * m_dw * m_dp * cos(m_p);
  return T_wheel + T_seat + T_coupling;
}

double const Unicycle2D::V() const {
  return m * g * l * cos(m_p);
}

double const& Unicycle2D::t() const { return m_t; }

double const& Unicycle2D::p() const { return m_p; }
double const& Unicycle2D::dp() const { return m_dp; }

double const& Unicycle2D::w() const { return m_w; }
double const& Unicycle2D::dw() const { return m_dw; }

Unicycle2D& Unicycle2D::t(double const& t_) {
  m_t = t_;
  return *this;
}

Unicycle2D& Unicycle2D::p(double const& p_) {
  m_p = p_;
  return *this;
}

Unicycle2D& Unicycle2D::dp(double const& dp_) {
  m_dp = dp_;
  return *this;
}

Unicycle2D& Unicycle2D::w(double const& w_) {
  m_w = w_;
  return *this;
}

Unicycle2D& Unicycle2D::dw(double const& dw_) {
  m_dw = dw_;
  return *this;
}