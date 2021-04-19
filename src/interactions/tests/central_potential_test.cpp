#define BOOST_TEST_MODULE interactions test
#define BOOST_TEST_DYN_LINK
#include <array>
#include <boost/test/unit_test.hpp>
#include <limits>

#include <interactions/central_potential.hpp>

struct Dummy {
  using value_type = double;
  using vector_type = std::array<value_type, 3>;
  vector_type force(value_type, vector_type const &r12) const { return r12; }
  value_type energy(value_type) const { return 1.0; }
};

BOOST_AUTO_TEST_CASE(central_potential) {
  auto constexpr eps = std::numeric_limits<double>::epsilon();
  auto const cp = Interactions::CentralPotential<Dummy>(1.0, 2.0, 3.0);
  BOOST_CHECK_SMALL(cp.energy(.5), eps); // smaller than offset
  BOOST_TEST(cp.force(.5, Dummy::vector_type{}) == Dummy::vector_type{});
  BOOST_CHECK_SMALL(cp.energy(2.0), eps); // at offset (excluded)
  BOOST_TEST(cp.force(2.0, Dummy::vector_type{}) == Dummy::vector_type{});
  BOOST_CHECK_SMALL(cp.energy(3.0) - 4.0,
                    eps); // at offset plus cutoff (included)
  BOOST_TEST((cp.force(3.0, Dummy::vector_type{1, 2, 3}) ==
              Dummy::vector_type{1, 2, 3}));
}