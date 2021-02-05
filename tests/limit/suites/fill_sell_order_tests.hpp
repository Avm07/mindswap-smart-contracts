#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(fill_sell_order_tests)

BOOST_FIXTURE_TEST_CASE(asserts_fill_sell_order_test, limit_tester)
try {
	ERROR("missing authority of mindswaparbi",
          limit.fill_sell_order(N(alice), 0, 1000));

    WASM_ASSERT("fill_sell_order: order is not exist",
                 limit.fill_sell_order(N(mindswaparbi), 0, 999));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(fill_sell_order_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("0.010 SYS"), iq.contract};
	extended_asset iq_token_asset{asset::from_string("0.100 IQ"), iq.contract};
	SUCCESS(limit.create_limit_sell(N(trader1), N(trader1), sys_token_asset, iq_token_asset));

    auto limit_iq_balance = iq.get_account(limit.contract, "3,IQ");
    auto limit_sys_balance = iq.get_account(limit.contract, "3,SYS");

	REQUIRE_MATCHING_OBJECT(limit_iq_balance, mvo()("balance", "200.000 IQ"));
    REQUIRE_MATCHING_OBJECT(limit_sys_balance, mvo()("balance", "200.000 SYS"));

    SUCCESS(limit.fill_sell_order(N(mindswaparbi), 0, 1000));

    limit_iq_balance = iq.get_account(limit.contract, "3,IQ");
    limit_sys_balance = iq.get_account(limit.contract, "3,SYS");
    
	REQUIRE_MATCHING_OBJECT(limit_iq_balance, mvo()("balance", "200.000 IQ"));
    REQUIRE_MATCHING_OBJECT(limit_sys_balance, mvo()("balance", "199.990 SYS"));
    
    auto buy_order = limit.get_sell_order(0, 1000);
    BOOST_REQUIRE_EQUAL(true, buy_order.is_null());

    auto deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "100.001 IQ")
    ("balance_in_orders", "0.000 IQ"));

    deposit = limit.get_deposit(N(trader1), 1);

    REQUIRE_MATCHING_OBJECT(deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "99.990 SYS")
    ("balance_in_orders", "0.000 SYS"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
