#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(close_tests)

BOOST_FIXTURE_TEST_CASE(asserts_close_test, limit_tester)
try {
    extended_symbol iq_token_sym{symbol(SY(3, IQ)), N(everipediaiq)};

    SUCCESS(limit.open(N(alice), N(alice), iq_token_sym, N(alice)));

    ERROR("missing authority of alice", limit.close(N(bob), N(alice), iq_token_sym));
    
    WASM_ASSERT("Balance row already deleted or never existed. Action won't have any effect.",
                limit.close(N(bob), N(bob), iq_token_sym));

    SUCCESS(iq.transfer(N(alice), limit.contract, asset::from_string("10.000 IQ"), ""));

    WASM_ASSERT("Cannot close because the balance is not zero.",
                limit.close(N(alice), N(alice), iq_token_sym));

    extended_asset eos_token_asset{asset::from_string("1.0000 EOS"), N(eosio.token)};
    extended_asset iq_token_asset{asset::from_string("1.000 IQ"), N(everipediaiq)};

    // SUCCESS(limit.create_limit_buy(N(alice), N(alice), eos_token_asset, iq_token_asset));

    // WASM_ASSERT("Cannot close because the balance_in_orders is not zero.",
    //             limit.close(N(alice), N(alice), iq_token_sym));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(close_test, limit_tester)
try {
    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
    SUCCESS(limit.open(N(alice), N(alice), iq_token, N(alice)));

    SUCCESS(limit.close(N(alice), N(alice), iq_token));

    auto alice_deposit = limit.get_deposit(N(alice), 0);
    BOOST_REQUIRE_EQUAL(true, alice_deposit.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
