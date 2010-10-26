/*
 *  Pendulum.h
 *  maw
 *
 *  Created by Nick Stenning on 21/10/2010.
 *  Copyright 2010 Nick Stenning. All rights reserved.
 *
 */

#ifndef PENDULUM_H
#define PENDULUM_H

#include <vector>
#include <cmath>

typedef std::vector<double> (*rkBlock)(double, double, double);

class Pendulum {
public:
  Pendulum();
  ~Pendulum();

  void step(double externalTorque);

  double const& time() const;
  double const& ang() const;
  double const& vel() const;

  Pendulum const& ang(double const& a);
  Pendulum const& vel(double const& v);

  static const double pi;
  static const double gravity;
  static const double length;
  static const double mass;
  static const double dt;

protected:
  double m_time;
  double m_ang;
  double m_vel;

  void rk_step(double h, double t, rkBlock func);
};

#endif // PENDULUM_H