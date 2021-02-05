#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(close_tests)

BOOST_FIXTURE_TEST_CASE(asserts_close_test, arbitrage_tester)
try {
    extended_symbol iq_token_sym{symbol(SY(3, IQ)), N(everipediaiq)};

    SUCCESS(arb.open(N(alice), N(alice), iq_token_sym, N(alice)));

    ERROR("missing authority of alice", arb.close(N(bob), N(alice), iq_token_sym));
    
    WASM_ASSERT("Balance row already deleted or never existed. Action won't have any effect.",
                arb.close(N(bob), N(bob), iq_token_sym));

    SUCCESS(iq.transfer(N(alice), arb.contract, asset::from_string("10.000 IQ"), ""));

    WASM_ASSERT("Cannot close because the balance is not zero.",
                arb.close(N(alice), N(alice), iq_token_sym));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(close_test, arbitrage_tester)
try {
    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
    SUCCESS(arb.open(N(alice), N(alice), iq_token, N(alice)));

    SUCCESS(arb.close(N(alice), N(alice), iq_token));

    auto alice_deposit = arb.get_deposit(N(alice), 0);
    BOOST_REQUIRE_EQUAL(true, alice_deposit.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
