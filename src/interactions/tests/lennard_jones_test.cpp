#define BOOST_TEST_MODULE interactions lennard_jones
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <limits>

#include <interactions/lennard_jones.hpp>

BOOST_AUTO_TEST_CASE(lennard_jones) {
  auto constexpr eps = std::numeric_limits<double>::epsilon();
  auto const lj = Interactions::LennardJones<double>{1.0, 2.0};
  BOOST_CHECK_SMALL(lj.energy(2.0 * std::pow(2.0, 1. / 6.)) + 1.0,
                    eps);                 // minimum
  BOOST_CHECK_SMALL(lj.energy(2.0), eps); // at sigma
}