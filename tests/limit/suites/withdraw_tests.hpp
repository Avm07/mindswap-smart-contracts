#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(withdraw_tests)

BOOST_FIXTURE_TEST_CASE(asserts_withdraw_test, limit_tester)
try {
    extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

    ERROR("missing authority of bob", limit.withdraw(N(alice), N(bob), N(alice), iq_token_asset, ""));
    
    WASM_ASSERT("withdraw: withdraw account is not exist", limit.withdraw(N(alice), N(alice), N(carol), iq_token_asset, ""));

    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
    SUCCESS(limit.open(N(alice), N(alice), iq_token, N(alice)));

    extended_asset iq_token_asset2{asset::from_string("1000.000 IQ"), N(everipediaiq)};
    WASM_ASSERT("overdrawn balance", limit.withdraw(N(alice), N(alice), N(alice), iq_token_asset2, ""));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(withdraw_test, limit_tester)
try {
    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
    SUCCESS(limit.open(N(alice), N(alice), iq_token, N(alice)));
    SUCCESS(iq.transfer(N(alice), limit.contract, asset::from_string("1.000 IQ"), ""));

    extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

    SUCCESS(limit.withdraw(N(alice), N(alice), N(alice), iq_token_asset, ""));

    auto alice_deposit = limit.get_deposit(N(alice), 0);

    REQUIRE_MATCHING_OBJECT(alice_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "0.000 IQ")
    ("balance_in_orders", "0.000 IQ"));

    auto alice_balance = iq.get_account(N(alice), "3,IQ");

    REQUIRE_MATCHING_OBJECT(alice_balance, mvo()
    ("balance", "100.000 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
