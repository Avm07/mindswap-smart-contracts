#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(order_trade_tests)

BOOST_FIXTURE_TEST_CASE(asserts_order_trade_test, arbitrage_tester)
try {
    WASM_ASSERT("arbitrage_order_trade: invalid order type",
                arb.arbitrage_order_trade(N(alice), N(alice), 1, N(invalid), 1000, asset::from_string("0.010 SYS"), symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_order_trade: invalid market id",
                arb.arbitrage_order_trade(N(alice), N(alice), 1000, N(buy), 1000, asset::from_string("0.010 SYS"), symbol::from_string("3,SYSIQ").to_symbol_code()));
            
    WASM_ASSERT("arbitrage_order_trade: invalid order id",
                arb.arbitrage_order_trade(N(alice), N(alice), 0, N(buy), 999, asset::from_string("0.010 SYS"), symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_order_trade: invalid amount",
                arb.arbitrage_order_trade(N(alice), N(alice), 0, N(sell), 1000, asset::from_string("-0.010 SYS"), symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_order_trade: invalid amount",
                arb.arbitrage_order_trade(N(alice), N(alice), 0, N(sell), 1000, asset::from_string("0.010 EOS"), symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_order_trade: mindswap pool is not exist",
                arb.arbitrage_order_trade(N(alice), N(alice), 0, N(sell), 1000, asset::from_string("0.010 SYS"), symbol::from_string("3,NOEXIST").to_symbol_code()));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(order_trade_test, arbitrage_tester)
try {
    auto arb_iq_balance = iq.get_account(arb.contract, "3,IQ");
    auto arb_sys_balance = iq.get_account(arb.contract, "3,SYS");
	REQUIRE_MATCHING_OBJECT(arb_iq_balance, mvo()("balance", "200.000 IQ"));
    REQUIRE_MATCHING_OBJECT(arb_sys_balance, mvo()("balance", "200.000 SYS"));

    SUCCESS(arb.arbitrage_order_trade(N(alice), N(alice), 0, N(sell), 1000, asset::from_string("0.010 SYS"), symbol::from_string("3,SYSIQ").to_symbol_code()));

    arb_iq_balance = iq.get_account(arb.contract, "3,IQ");
    arb_sys_balance = iq.get_account(arb.contract, "3,SYS");
	REQUIRE_MATCHING_OBJECT(arb_iq_balance, mvo()("balance", "200.097 IQ"));
    REQUIRE_MATCHING_OBJECT(arb_sys_balance, mvo()("balance", "200.000 SYS"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
