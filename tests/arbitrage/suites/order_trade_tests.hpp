#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(order_trade_tests)

BOOST_FIXTURE_TEST_CASE(asserts_order_trade_test, arbitrage_tester)
try {
    // WASM_ASSERT("arbitrage_order_trade: invalid order type",
    //             arb.arbitrage_order_trade(N(alice), 1, N(invalid), 1000, symbol::from_string("4,SYSIQ").to_symbol_code()));

    //  WASM_ASSERT("arbitrage_order_trade: invalid market id",
    //             arb.arbitrage_order_trade(N(alice), 1000, N(buy), 1000, symbol::from_string("4,SYSIQ").to_symbol_code()));
            
    // WASM_ASSERT("arbitrage_order_trade: invalid order id",
    //             arb.arbitrage_order_trade(N(alice), 1, N(buy), 999, symbol::from_string("4,SYSIQ").to_symbol_code()));

    // WASM_ASSERT("arbitrage_order_trade: mindswap pool is not exist",
    //             arb.arbitrage_order_trade(N(alice), 1, N(buy), 1000, symbol::from_string("4,NOEXIST").to_symbol_code()));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(order_trade_test, arbitrage_tester)
try {
    
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
