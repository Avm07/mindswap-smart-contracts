#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(open_tests)

BOOST_FIXTURE_TEST_CASE(asserts_open_test, arbitrage_tester)
try {
    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};

    ERROR("missing authority of bob", arb.open(N(alice), N(alice), iq_token, N(bob)));
    
    WASM_ASSERT("owner account does not exist", arb.open(N(alice), N(notexisten), iq_token, N(alice)));

    extended_symbol invalid_sym_iq_token{symbol(SY(3, IQQ)), N(everipediaiq)};

    WASM_ASSERT("symbol does not exist", arb.open(N(alice), N(alice), invalid_sym_iq_token, N(alice)));

    extended_symbol invalid_precision_iq_token{symbol(SY(0, IQ)), N(everipediaiq)};

    WASM_ASSERT("symbol precision mismatch", arb.open(N(alice), N(alice), invalid_precision_iq_token, N(alice)));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(open_test, arbitrage_tester)
try {
    extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};

    SUCCESS(arb.open(N(alice), N(alice), iq_token, N(alice)));

    auto alice_deposit = arb.get_deposit(N(alice), 0);
    auto balance = alice_deposit["balance"].get_object();
    
    BOOST_REQUIRE_EQUAL(alice_deposit["id"], fc::variant(0)); 
    REQUIRE_MATCHING_OBJECT(balance, mvo()("quantity", "0.000 IQ")("contract", "everipediaiq"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
