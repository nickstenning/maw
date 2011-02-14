#ifndef PENDULUM_H
#define PENDULUM_H

#include <vector>

typedef std::vector<double> (*rkFunc)(double, double, double);

class Pendulum {
public:
  Pendulum();
  ~Pendulum();

  void step(double externalTorque, double ext_dt = Pendulum::dt);

  double const& time() const;
  double const& ang() const;
  double const& vel() const;

  Pendulum& time(double const& t);
  Pendulum& ang(double const& a);
  Pendulum& vel(double const& v);

  static const double pi;
  static const double gravity;
  static const double length;
  static const double mass;
  static const double dt;

protected:
  double m_time;
  double m_ang;
  double m_vel;

  void rk_step(double h, double t, rkFunc func);
};

#endif // PENDULUM_H