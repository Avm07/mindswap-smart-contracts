#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(pair_trade_tests)

BOOST_FIXTURE_TEST_CASE(asserts_pair_trade_test, arbitrage_tester)
try {
    // WASM_ASSERT("arbitrage_pair_trade: invalid orders type",
    //             arb.arbitrage_pair_trade());

    // WASM_ASSERT("arbitrage_pair_trade: invalid market id",
    //             arb.arbitrage_pair_trade());

    // WASM_ASSERT("arbitrage_order_trade: invalid orders ids",
    //             arb.arbitrage_pair_trade());

    // WASM_ASSERT("arbitrage_pair_trade: mindswap pool is not exist",
    //             arb.arbitrage_pair_trade());
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(pair_trade_test, arbitrage_tester)
try {
    
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
