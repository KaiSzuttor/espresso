#ifndef VECTOREXPRESSION_HPP
#define VECTOREXPRESSION_HPP

namespace Utils {

template <typename T, std::size_t N> struct VectorExpression {
  T &operator~() { return static_cast<T &>(*this); }
  const T &operator~() const { return static_cast<const T &>(*this); }
};

template <typename LHS, typename RHS, std::size_t N>
class SumExpression : public VectorExpression<SumExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  using value_type = decltype(std::declval<LHS>()[0] + std::declval<RHS>()[0]);
  SumExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] + m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class DiffExpression : public VectorExpression<DiffExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  using value_type = decltype(std::declval<LHS>()[0] - std::declval<RHS>()[0]);
  DiffExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] - m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class HadamardExpression
    : public VectorExpression<HadamardExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  using value_type = decltype(std::declval<LHS>()[0] * std::declval<RHS>()[0]);
  HadamardExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] * m_rhs[i]; }
};

template <typename Scalar, typename RHS, std::size_t N,
          typename =
              std::enable_if_t<std::is_arithmetic<std::decay_t<Scalar>>::value>>
class ScalarMultExpression
    : public VectorExpression<ScalarMultExpression<Scalar, RHS, N>, N> {
  const Scalar m_lhs;
  const RHS &m_rhs;

public:
  using value_type = decltype(std::declval<Scalar>() * std::declval<RHS>()[0]);
  ScalarMultExpression(Scalar const &lhs, RHS const &rhs)
      : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs * m_rhs[i]; }
};

template <typename LHS, typename Scalar, std::size_t N,
          typename =
              std::enable_if_t<std::is_arithmetic<std::decay_t<Scalar>>::value>>
class ScalarDivExpression
    : public VectorExpression<ScalarDivExpression<LHS, Scalar, N>, N> {
  const LHS &m_lhs;
  const Scalar m_rhs;

public:
  using value_type = decltype(std::declval<LHS>()[0] / std::declval<Scalar>());
  ScalarDivExpression(LHS const &lhs, Scalar const &rhs)
      : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] / m_rhs; }
};

template <typename T, std::size_t N>
class MinusExpression : public VectorExpression<MinusExpression<T, N>, N> {
  const T &m_lhs;

public:
  using value_type = decltype(std::declval<T>()[0]);
  MinusExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return -m_lhs[i]; }
};

template <typename T, std::size_t N>
class NotExpression : public VectorExpression<NotExpression<T, N>, N> {
  const T &m_lhs;

public:
  using value_type = decltype(std::declval<T>()[0]);
  NotExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return not(m_lhs[i]); }
};

template <typename T, std::size_t N>
class SqrtExpression : public VectorExpression<SqrtExpression<T, N>, N> {
  const T &m_lhs;

public:
  using value_type = decltype(std::declval<T>()[0]);
  SqrtExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return std::sqrt(m_lhs[i]); }
};

} // namespace Utils

#endif
