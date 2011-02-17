#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <ostream>

template<size_t D = 3, typename T = double>
class Vector
{
public:
  Vector<D, T> ()
    : m_vec(D)
  {}

  Vector<D, T>& operator=(Vector<D, T> const& rhs) {
    for(size_t i = 0; i < D; i += 1) {
      m_vec[i] = T(rhs.m_vec[i]);
    }
    return *this;
  }

  T const operator[](size_t const& index) const {
    return m_vec[index];
  }

  T& operator[](size_t const& index)  {
    return m_vec[index];
  }

  Vector<D, T>& operator+=(Vector<D, T> const& rhs) {
    for(size_t i = 0; i < D; i += 1) {
      m_vec[i] += T(rhs.m_vec[i]);
    }
    return *this;
  }

  Vector<D, T>& operator-=(Vector<D, T> const& rhs) {
    for(size_t i = 0; i < D; i += 1) {
      m_vec[i] -= T(rhs.m_vec[i]);
    }
    return *this;
  }

  Vector<D, T>& operator*=(T const& rhs) {
    for(size_t i = 0; i < D; i += 1) {
      m_vec[i] *= rhs;
    }
    return *this;
  }

  Vector<D, T>& operator/=(T const& rhs) {
    for(size_t i = 0; i < D; i += 1) {
      m_vec[i] /= rhs;
    }
    return *this;
  }

  Vector<D, T> operator*(T const& factor) const {
    return Vector<D, T>(*this) *= factor;
  }

  Vector<D, T> operator/(T const& divisor) const {
    return Vector<D, T>(*this) /= divisor;
  }

  Vector<D, T> operator+(Vector<D, T> const& rhs) const {
    return Vector<D, T>(*this) += rhs;
  }

  Vector<D, T> operator-(Vector<D, T> const& rhs) const {
    return Vector<D, T>(*this) -= rhs;
  }
private:
  typename std::vector<T> m_vec;
};

template<size_t D, typename T>
Vector<D, T> operator*(T factor, Vector<D, T> const& rhs)
{
  return Vector<D, T>(rhs) *= factor;
}

#endif // VECTOR_H
