#ifndef UNICYCLE_2D_H
#define UNICYCLE_2D_H

#include "vector.h"

class Unicycle2D
{
public:
  typedef Vector<4> state;
  typedef state (*rkFunc)(double const& t, state const& s, double const& F_w, double const& F_p);

  Unicycle2D();

  void step(double ext_dt = Unicycle2D::dt, double F_w = 0.0, double F_p = 0.0);

  double const& t() const;
  double const& p() const;
  double const& dpdt() const;
  double const& w() const;
  double const& dwdt() const;

  double const T() const;
  double const V() const;

  Unicycle2D& t(double const& t);
  Unicycle2D& p(double const& p);
  Unicycle2D& dpdt(double const& dpdt);
  Unicycle2D& w(double const& w);
  Unicycle2D& dwdt(double const& dwdt);

  static const double postLength;
  static const double seatMass;
  static const double wheelRadius;
  static const double wheelMass;
  static const double dt;

protected:
  void rk_step(double h, double F_w, double F_p, rkFunc func);

private:
  double m_t;
  double m_p;    /**< Seatpost angle */
  double m_dpdt; /**< Seatpost angular velocity */
  double m_w;    /**< Wheel angle */
  double m_dwdt; /**< Wheel angular velocity */
};

#endif // UNICYCLE_2D_H
