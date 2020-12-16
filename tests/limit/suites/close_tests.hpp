#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(close_tests)

BOOST_FIXTURE_TEST_CASE(asserts_close_test, limit_tester)
try {
	SUCCESS(api.hello(N(cesar), "hola"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
