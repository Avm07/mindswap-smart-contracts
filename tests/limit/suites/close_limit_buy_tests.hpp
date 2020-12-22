#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(close_limit_buy_tests)

BOOST_FIXTURE_TEST_CASE(asserts_close_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

    WASM_ASSERT("close_limit_buy: market is not exist",
				limit.close_limit_buy(N(trader1), 666, 1000));

    SUCCESS(limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, iq_token_asset)); 
    
    WASM_ASSERT("close_limit_buy: order is not exist",
				limit.close_limit_buy(N(trader1), 0, 666));

    ERROR("missing authority of trader1", limit.close_limit_buy(N(trader2), 0, 1000));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(close_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, iq_token_asset)); 

    SUCCESS(limit.close_limit_buy(N(trader1), 0, 1000)); 

    auto buy_order = limit.get_buy_order(0, 1000);
    BOOST_REQUIRE_EQUAL(true, buy_order.is_null());

    auto deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "100.000 IQ")
    ("balance_in_orders", "0.000 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
