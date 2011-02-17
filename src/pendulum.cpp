#include <cmath>

#include "pendulum.h"

const double Pendulum::pi      = 3.141592653589793238462643;
const double Pendulum::gravity = 9.81; // m s^-2
const double Pendulum::mass    = 1;    // kg
const double Pendulum::length  = 1;    // m
const double Pendulum::dt      = 0.01; // s

std::vector<double> physics (double ang_0, double vel_0, double externalTorque)
{
  std::vector<double> result;

  // d_ang / d_t = vel
  double d_ang = vel_0;
  result.push_back(d_ang);

  // d_vel / d_t = torque
  double d_vel = 0.0;

  // NB: the angle is the angle to the *upper* vertical, measured clockwise.
  d_vel += Pendulum::gravity / Pendulum::length * sin(ang_0);
  d_vel += externalTorque / (Pendulum::mass * Pendulum::length);
  result.push_back(d_vel);

  return result;
}

Pendulum::Pendulum()
  : m_time(0.0)
  , m_ang(0.0)
  , m_vel(0.0)
{}

Pendulum::~Pendulum() {}

double const& Pendulum::time() const
{
  return m_time;
}

double const& Pendulum::ang() const
{
  return m_ang;
}

double const& Pendulum::vel() const
{
  return m_vel;
}

Pendulum& Pendulum::time(double const& t)
{
  m_time = t;
  return *this;
}

Pendulum& Pendulum::ang(double const& a)
{
  m_ang = a;
  return *this;
}

Pendulum& Pendulum::vel(double const& v)
{
  m_vel = v;
  return *this;
}

void Pendulum::step(double externalTorque, double ext_dt)
{
  // Pendulum::dt provides a minimum physics resolution, but ext_dt can specify
  // that it wants the simulation to run for longer in a single call to step().
  while (ext_dt > dt) {
    ext_dt -= dt;
    rk_step(dt, externalTorque, physics);
  }
  rk_step(ext_dt, externalTorque, physics);

  // Angular wrap-round
  while (m_ang > pi) {
    m_ang -= 2 * pi;
  }
  while (m_ang < -pi) {
    m_ang += 2 * pi;
  }

  // Maximum speed
  if (m_vel > 10 * pi) {
    m_vel = 10 * pi;
  }
  if (m_vel < -10 * pi) {
    m_vel = -10 * pi;
  }

  m_time += dt;
}

void Pendulum::rk_step(double h, double torque, rkFunc func)
{
  typedef std::vector<double> state;

  state c1_v = func(m_ang, m_vel, torque);
  double c1_ang = h * c1_v[0];
  double c1_vel = h * c1_v[1];

  state c2_v = func(m_ang + c1_ang / 2, m_vel + c1_vel / 2, torque);
  double c2_ang = h * c2_v[0];
  double c2_vel = h * c2_v[1];

  state c3_v = func(m_ang + c2_ang / 2, m_vel + c2_vel / 2, torque);
  double c3_ang = h * c3_v[0];
  double c3_vel = h * c3_v[1];

  state c4_v = func(m_ang + c3_ang, m_vel + c3_vel, torque);
  double c4_ang = h * c4_v[0];
  double c4_vel = h * c4_v[1];

  m_ang = m_ang + (c1_ang + 2 * c2_ang + 2 * c3_ang + c4_ang) / 6;
  m_vel = m_vel + (c1_vel + 2 * c2_vel + 2 * c3_vel + c4_vel) / 6;
}