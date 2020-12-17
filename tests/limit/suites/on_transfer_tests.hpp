#include "limit_tester.hpp"

BOOST_AUTO_TEST_SUITE(on_transfer_tests)

BOOST_FIXTURE_TEST_CASE(asserts_on_transfer_test, limit_tester)
try {
    WASM_ASSERT("on_transfer: deposit account is not exist",
                iq.transfer(N(alice), limit.contract, asset::from_string("1.000 IQ"), ""));

}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(on_transfer_test, limit_tester)
try {
    extended_symbol iq_token_sym{symbol(SY(3, IQ)), N(everipediaiq)};
    SUCCESS(limit.open(N(alice), N(alice), iq_token_sym, N(alice)));

    SUCCESS(iq.transfer(N(alice), limit.contract, asset::from_string("1.000 IQ"), ""));

    auto alice_deposit = limit.get_deposit(N(alice), 0);

    REQUIRE_MATCHING_OBJECT(alice_deposit, mvo()
    ("id", 0)
    ("contract", "everipediaiq")
    ("balance", "1.000 IQ")
    ("balance_in_orders", "0.000 IQ"));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
