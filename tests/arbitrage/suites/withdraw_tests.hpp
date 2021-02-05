#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(withdraw_tests)

BOOST_FIXTURE_TEST_CASE(asserts_withdraw_test, arbitrage_tester)
try {
	extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

	ERROR("missing authority of bob", arb.withdraw(N(alice), N(bob), N(alice), iq_token_asset, ""));

	extended_asset negative_iq_token_asset{asset::from_string("-1.000 IQ"), N(everipediaiq)};
	WASM_ASSERT("withdraw: quantity should be positive", arb.withdraw(N(alice), N(alice), N(carol), negative_iq_token_asset, ""));

	WASM_ASSERT("withdraw: withdraw account is not exist", arb.withdraw(N(alice), N(alice), N(carol), iq_token_asset, ""));

	extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
	SUCCESS(arb.open(N(alice), N(alice), iq_token, N(alice)));

	extended_asset iq_token_asset2{asset::from_string("1000.000 IQ"), N(everipediaiq)};
	WASM_ASSERT("overdrawn balance", arb.withdraw(N(alice), N(alice), N(alice), iq_token_asset2, ""));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(withdraw_test, arbitrage_tester)
try {
	extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
	SUCCESS(arb.open(N(alice), N(alice), iq_token, N(alice)));
	SUCCESS(iq.transfer(N(alice), arb.contract, asset::from_string("1.000 IQ"), ""));

	extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

	SUCCESS(arb.withdraw(N(alice), N(alice), N(alice), iq_token_asset, ""));

	auto alice_deposit = arb.get_deposit(N(alice), 0);
	auto balance = alice_deposit["balance"].get_object();
    
    BOOST_REQUIRE_EQUAL(alice_deposit["id"], fc::variant(0)); 
    REQUIRE_MATCHING_OBJECT(balance, mvo()("quantity", "0.000 IQ")("contract", "everipediaiq"));

	auto alice_balance = iq.get_account(N(alice), "3,IQ");

	REQUIRE_MATCHING_OBJECT(alice_balance, mvo()
    ("balance", "100.000 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
