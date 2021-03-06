#include <cmath>
#include <algorithm>

#include "util.h"
#include "vector.h"
#include "unicycle_2d.h"

const double Unicycle2D::dt          = 0.01; // s
const double Unicycle2D::postLength  = 1;   // m
const double Unicycle2D::seatMass    = 20;   // kg
const double Unicycle2D::wheelRadius = 0.2; // m
const double Unicycle2D::wheelMass   = 5;   // kg

static const double pi = 3.141592653589793238462643;
static const double g  = 9.81; // m s^-2
static const double m = Unicycle2D::wheelMass;
static const double M = Unicycle2D::seatMass;
static const double l = Unicycle2D::postLength;
static const double r = Unicycle2D::wheelRadius;

// Hard limit to seat motion
static const double angMax = (pi / 2.0) + asin(r / l);

// Rolling friction accel maximum
static const double fricRoll = (0.03 * m* g) / (r* m);
// Seat sliding friction force maximum
static const double fricSlide = (0.5 * M* g) / m;

// MoI of wheel
static const double I = (m* r* r) / 2.0;

// KE of wheel, and of seat due to linear motion
static const double A = (I + m* r* r + M* r* r) / 2.0;
// KE of seat
static const double B = M * l * l / 2.0;
// Coupling
static const double C = M * r * l;
// Gravitational PE
static const double D = M * g * l;

// 2D unicycle equations of motion.
//
// Note no explicit dependence on angle of wheel (as we'd expect from symmetry).
Unicycle2D::state physics (double const& /*t*/, Unicycle2D::state const& s, double const& F_w, double const& F_p)
{
  double p    = s[0], ddt_p;
  double dpdt = s[1], ddt_dpdt;
  double /*w = s[2],*/ ddt_w;
  double dwdt = s[3], ddt_dwdt;

  // \frac{d}{dt} p = \dot{p}
  ddt_p = dpdt;

  // \frac{d}{dt} \dot{p} = \ddot{p} = \frac{(C \dot{p})^2 \cos{p} - 2 A D}{(C \cos{p})^2 - 4 A B} \sin{p}
  double ddt_dpdt_numer = C * F_w * cos(p) + pow(C * dpdt, 2) * cos(p) * sin(p) - 2.0 * A * (F_p + D * sin(p));
  double ddt_dpdt_denom = pow(C * cos(p), 2) - 4.0 * A * B;
  ddt_dpdt = ddt_dpdt_numer / ddt_dpdt_denom;

  // \frac{d}{dt} w = \dot{w}
  ddt_w = dwdt;

  // \frac{d}{dt} \dot{w} = \ddot{w} = \frac{D \sin{p} - 2 B \ddot{p}}{C \cos {p}}
  ddt_dwdt = (F_p + D * sin(p) - 2.0 * B * ddt_dpdt) / C * cos(p);

  // Simple static friction model
  double wheelAccel = std::abs(ddt_dwdt);

  if (dwdt < 0) {
    ddt_dwdt += std::min(wheelAccel, fricRoll);
  } else if (dwdt > 0) {
    ddt_dwdt -= std::min(wheelAccel, fricRoll);
  }

  if (std::abs(p) >= angMax) {
    if (dwdt < 0) {
      ddt_dwdt += std::min(wheelAccel, fricSlide);
    } else if (dwdt > 0) {
      ddt_dwdt -= std::min(wheelAccel, fricSlide);
    }
  }

  Unicycle2D::state ddt_s;
  ddt_s[0] = ddt_p;
  ddt_s[1] = ddt_dpdt;
  ddt_s[2] = ddt_w;
  ddt_s[3] = ddt_dwdt;

  return ddt_s;
}

Unicycle2D::Unicycle2D()
  : m_t(0.0)
  , m_p(0.0)
  , m_dpdt(0.0)
  , m_w(0.0)
  , m_dwdt(0.0)
{}

void Unicycle2D::step(double ext_dt, double F_w, double F_p)
{
  // Unicycle2D::dt provides a minimum physics resolution, but ext_dt can specify
  // that it wants the simulation to run for longer in a single call to step().
  while (ext_dt > dt) {
    ext_dt -= dt;
    rk_step(dt, F_w, F_p, physics);
  }
  rk_step(ext_dt, F_w, F_p, physics);
}

void Unicycle2D::rk_step(double h, double F_w, double F_p, rkFunc func)
{
  state s;
  s[0] = m_p;
  s[1] = m_dpdt;
  s[2] = m_w;
  s[3] = m_dwdt;

  state c1 = func(m_t,           s, F_w, F_p)            * h;
  state c2 = func(m_t + h / 2.0, s + c1 / 2.0, F_w, F_p) * h;
  state c3 = func(m_t + h / 2.0, s + c2 / 2.0, F_w, F_p) * h;
  state c4 = func(m_t + h,       s + c3, F_w, F_p)       * h;

  s   += (c1 + 2.0 * c2 + 2.0 * c3 + c4) / 6.0;
  m_t += h;

  m_p      = s[0];
  m_dpdt   = s[1];
  m_w      = s[2];
  m_dwdt   = s[3];

  // Angular limit -- don't allow seat below floor.
  if (std::abs(m_p) >= angMax) {
    if (m_p > 0.0) {
      m_p = angMax;
      m_dpdt = std::min(0.0, m_dpdt);
    } else {
      m_p = -angMax;
      m_dpdt = std::max(0.0, m_dpdt);
    }
  }
}

double const Unicycle2D::T() const
{
  return A * m_dwdt * m_dwdt + B * m_dpdt * m_dpdt + C * m_dwdt * m_dpdt * cos(m_p);
}

double const Unicycle2D::V() const
{
  return D * cos(m_p);
}

double const& Unicycle2D::t() const
{
  return m_t;
}

Unicycle2D& Unicycle2D::t(double const& t_)
{
  m_t = t_;
  return *this;
}

double const& Unicycle2D::p() const
{
  return m_p;
}
double const& Unicycle2D::dpdt() const
{
  return m_dpdt;
}
double const& Unicycle2D::w() const
{
  return m_w;
}
double const& Unicycle2D::dwdt() const
{
  return m_dwdt;
}

Unicycle2D& Unicycle2D::p(double const& p_)
{
  m_p = p_;
  return *this;
}

Unicycle2D& Unicycle2D::dpdt(double const& dpdt_)
{
  m_dpdt = dpdt_;
  return *this;
}

Unicycle2D& Unicycle2D::w(double const& w_)
{
  m_w = w_;
  return *this;
}

Unicycle2D& Unicycle2D::dwdt(double const& dwdt_)
{
  m_dwdt = dwdt_;
  return *this;
}