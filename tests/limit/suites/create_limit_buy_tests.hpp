#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(create_limit_buy_tests)

BOOST_FIXTURE_TEST_CASE(asserts_create_limit_buy_test, limit_tester)
try {
	extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};

	ERROR("missing authority of trader1", limit.create_limit_buy(N(trader2), N(trader1), sys_token_asset, iq_token_asset));

    extended_asset negative_sys_token_asset{asset::from_string("-1.000 SYS"), N(everipediaiq)};
	extended_asset negative_iq_token_asset{asset::from_string("-100.000 IQ"), N(everipediaiq)};

    WASM_ASSERT("create_limit_buy: volume should be positive",
				limit.create_limit_buy(N(trader1), N(trader1), negative_sys_token_asset, iq_token_asset));

    WASM_ASSERT("create_limit_buy: price should be positive",
				limit.create_limit_buy(N(trader1), N(trader1), sys_token_asset, negative_iq_token_asset));

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

BOOST_FIXTURE_TEST_CASE(create_full_fill_deal_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_sell(N(trader1), N(trader1), sys_token_asset, iq_token_asset));
    SUCCESS(limit.create_limit_buy(N(trader2), N(trader2), sys_token_asset, iq_token_asset));

    auto sell_order = limit.get_sell_order(0, 1000);
    BOOST_REQUIRE_EQUAL(true, sell_order.is_null());

    auto buy_order = limit.get_buy_order(0, 1001);
    BOOST_REQUIRE_EQUAL(true, buy_order.is_null());

    auto market = limit.get_market(0);
    BOOST_REQUIRE_EQUAL(market["available_ord_id"], fc::variant(1002)); 

    auto trader1_deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "200.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    trader1_deposit = limit.get_deposit(N(trader1), 1);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "99.000 SYS")
    ("balance_in_orders", "0.000 SYS"));

    auto trader2_deposit = limit.get_deposit(N(trader2), 0);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "0.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    trader2_deposit = limit.get_deposit(N(trader2), 1);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "101.000 SYS")
    ("balance_in_orders", "0.000 SYS"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_half_fill_deal_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("0.500 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_sell(N(trader1), N(trader1), sys_token_asset, iq_token_asset));

    extended_asset buyer_sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset seller_iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_buy(N(trader2), N(trader2), buyer_sys_token_asset, seller_iq_token_asset));

    auto sell_order = limit.get_sell_order(0, 1000);
    BOOST_REQUIRE_EQUAL(true, sell_order.is_null());

    auto buy_order = limit.get_buy_order(0, 1001);
    
    REQUIRE_MATCHING_OBJECT(buy_order, mvo()
    ("id", 1001)
    ("owner", "trader2")
    ("volume", "1.000 SYS")
    ("balance", "0.500 SYS")
    ("price", "100.000 IQ")
    ("creation_date", "2020-01-01T00:00:12.500"));

    auto market = limit.get_market(0);
    BOOST_REQUIRE_EQUAL(market["available_ord_id"], fc::variant(1002)); 

    auto trader1_deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "150.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    trader1_deposit = limit.get_deposit(N(trader1), 1);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "99.500 SYS")
    ("balance_in_orders", "0.000 SYS"));

    auto trader2_deposit = limit.get_deposit(N(trader2), 0);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "0.000 IQ")
    ("balance_in_orders", "50.000 IQ"));

    trader2_deposit = limit.get_deposit(N(trader2), 1);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "100.500 SYS")
    ("balance_in_orders", "0.000 SYS"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_over_fill_deal_limit_buy_test, limit_tester)
try {
    extended_asset sys_token_asset{asset::from_string("1.000 SYS"), N(everipediaiq)};
	extended_asset iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_sell(N(trader1), N(trader1), sys_token_asset, iq_token_asset));

    extended_asset buyer_sys_token_asset{asset::from_string("0.500 SYS"), N(everipediaiq)};
	extended_asset seller_iq_token_asset{asset::from_string("100.000 IQ"), N(everipediaiq)};
    SUCCESS(limit.create_limit_buy(N(trader2), N(trader2), buyer_sys_token_asset, seller_iq_token_asset));

    auto sell_order = limit.get_sell_order(0, 1000);
    REQUIRE_MATCHING_OBJECT(sell_order, mvo()
    ("id", 1000)
    ("owner", "trader1")
    ("volume", "1.000 SYS")
    ("balance", "0.500 SYS")
    ("price", "100.000 IQ")
    ("creation_date", "2020-01-01T00:00:12.000"));

    auto buy_order = limit.get_buy_order(0, 1001);
    BOOST_REQUIRE_EQUAL(true, buy_order.is_null());

    auto market = limit.get_market(0);
    BOOST_REQUIRE_EQUAL(market["available_ord_id"], fc::variant(1002)); 

    auto trader1_deposit = limit.get_deposit(N(trader1), 0);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "150.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    trader1_deposit = limit.get_deposit(N(trader1), 1);

    REQUIRE_MATCHING_OBJECT(trader1_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "99.000 SYS")
    ("balance_in_orders", "0.500 SYS"));

    auto trader2_deposit = limit.get_deposit(N(trader2), 0);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "50.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    trader2_deposit = limit.get_deposit(N(trader2), 1);

    REQUIRE_MATCHING_OBJECT(trader2_deposit, mvo()
    ("id", 1)
    ("contract", "everipediaiq")
    ("balance", "100.500 SYS")
    ("balance_in_orders", "0.000 SYS"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
