#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(pair_trade_tests)

BOOST_FIXTURE_TEST_CASE(asserts_pair_trade_test, arbitrage_tester)
try {
    WASM_ASSERT("arbitrage_pair_trade: invalid orders type",
                arb.arbitrage_pair_trade(N(alice), N(alice), 0, N(invalid), std::vector<uint64_t>{1000}, symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_pair_trade: invalid market id",
                arb.arbitrage_pair_trade(N(alice), N(alice), 999, N(sell), std::vector<uint64_t>{1000}, symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_order_trade: invalid orders ids",
                arb.arbitrage_pair_trade(N(alice), N(alice), 0, N(sell), std::vector<uint64_t>{999}, symbol::from_string("3,SYSIQ").to_symbol_code()));

    WASM_ASSERT("arbitrage_pair_trade: mindswap pool is not exist",
                arb.arbitrage_pair_trade(N(alice), N(alice), 0, N(sell), std::vector<uint64_t>{1000}, symbol::from_string("3,NOEXIST").to_symbol_code()));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(pair_trade_test, arbitrage_tester)
try {
    auto arb_iq_balance = iq.get_account(arb.contract, "3,IQ");
    auto arb_sys_balance = iq.get_account(arb.contract, "3,SYS");
	REQUIRE_MATCHING_OBJECT(arb_iq_balance, mvo()("balance", "200.000 IQ"));
    REQUIRE_MATCHING_OBJECT(arb_sys_balance, mvo()("balance", "200.000 SYS"));

    auto alice_iq_balance = iq.get_account(N(alice), "3,IQ");
	REQUIRE_MATCHING_OBJECT(alice_iq_balance, mvo()("balance", "100.000 IQ"));

    SUCCESS(arb.arbitrage_pair_trade(N(alice), N(alice), 0, N(sell), std::vector<uint64_t>{1000}, symbol::from_string("3,SYSIQ").to_symbol_code()));

    arb_iq_balance = iq.get_account(arb.contract, "3,IQ");
    arb_sys_balance = iq.get_account(arb.contract, "3,SYS");
	REQUIRE_MATCHING_OBJECT(arb_iq_balance, mvo()("balance", "200.000 IQ"));
    REQUIRE_MATCHING_OBJECT(arb_sys_balance, mvo()("balance", "200.000 SYS"));

    alice_iq_balance = iq.get_account(N(alice), "3,IQ");
	REQUIRE_MATCHING_OBJECT(alice_iq_balance, mvo()("balance", "100.097 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
