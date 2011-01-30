#ifndef UNICYCLE_2D_H
#define UNICYCLE_2D_H

#include <vector>
#include <cmath>

typedef std::vector<double> (*rkBlock)(double p, double dp, double w, double dw);

class Unicycle2D {
public:
  Unicycle2D();

  void step(double ext_dt = Unicycle2D::dt);

  double const& t() const;
  double const& p() const;
  double const& dp() const;
  double const& w() const;
  double const& dw() const;

  double const T() const;
  double const V() const;

  Unicycle2D& t(double const& t);
  Unicycle2D& p(double const& p);
  Unicycle2D& dp(double const& dp);
  Unicycle2D& w(double const& w);
  Unicycle2D& dw(double const& dw);

  static const double pi;
  static const double g;
  static const double postLength;
  static const double seatMass;
  static const double wheelRadius;
  static const double wheelMass;
  static const double dt;

protected:
  void rk_step(double h, rkBlock func);

private:
  double m_t;
  double m_p;  /**< Seatpost angle */
  double m_dp; /**< Seatpost angular velocity (per unit time) */
  double m_w;  /**< Wheel angle */
  double m_dw; /**< Wheel angular velocity (per unit time) */
};

#endif // UNICYCLE_2D_H
