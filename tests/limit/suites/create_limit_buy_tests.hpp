#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(create_limit_buy_tests)

BOOST_FIXTURE_TEST_CASE(asserts_create_limit_buy_test, limit_tester)
try {
	extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};

	ERROR("missing authority of trader1", limit.create_limit_buy(N(trader2), N(trader1), sys_token_asset, iq_token_asset));

    extended_asset iq_1000token_asset{asset::from_string("1000.000 IQ"), N(everipediaiq)};

	WASM_ASSERT("overdrawn balance",
				limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, iq_1000token_asset));

    SUCCESS(limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, iq_token_asset));

    WASM_ASSERT("add_market: wrong trade pair",
				limit.create_limit_buy(N(trader1), N(trader1), iq_token_asset, sys_token_asset));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};

    SUCCESS(limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, iq_token_asset));  

    auto market = limit.get_market(0);
    auto token1 = market["token1"].get_object();
    auto token2 = market["token2"].get_object();

    BOOST_REQUIRE_EQUAL(market["id"], fc::variant(0)); 
    REQUIRE_MATCHING_OBJECT(token1, mvo()("sym", "3,SYS")("contract", "everipediaiq"));
    REQUIRE_MATCHING_OBJECT(token2, mvo()("sym", "3,IQ")("contract", "everipediaiq"));
    BOOST_REQUIRE_EQUAL(market["available_ord_id"], fc::variant(1001)); 

    auto buy_order = limit.get_buy_order(0, 1000);

    REQUIRE_MATCHING_OBJECT(buy_order, mvo()
    ("id", 1000)
    ("owner", "trader1")
    ("volume", "1.000 SYS")
    ("balance", "1.000 SYS")
    ("price", "100.000 IQ")
    ("creation_date", "2020-01-01T00:00:12.000"));

    auto deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "0.000 IQ")
    ("balance_in_orders", "100.000 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
