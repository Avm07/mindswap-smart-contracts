#include "arbitrage_tester.hpp"

BOOST_AUTO_TEST_SUITE(validate_tests)

BOOST_FIXTURE_TEST_CASE(asserts_validate_test, arbitrage_tester)
try {
    extended_asset iq_token_asset{asset::from_string("1000.000 IQ"), iq.contract};

    ERROR("missing authority of mindswaparbi", arb.validate(N(alice), N(swap), N(mindswaparbi), iq_token_asset, N(alice)));
    
    WASM_ASSERT("validate: account is not exist",
                arb.validate(N(mindswaparbi), N(swap), N(notexisten), iq_token_asset, N(alice)));

    extended_asset negative_iq_token_asset{asset::from_string("-1000.000 IQ"), iq.contract};

    WASM_ASSERT("validate: expected_balance should be positive",
                arb.validate(N(mindswaparbi), N(swap), N(mindswaparbi), negative_iq_token_asset, N(alice)));

    WASM_ASSERT("validate: swap validation failed for mindswaparbi 200.000 IQ!>=1000.000 IQ",
                arb.validate(N(mindswaparbi), N(swap), N(mindswaparbi), iq_token_asset, N(alice)));

    extended_asset sys_token_asset{asset::from_string("199.970 SYS"), iq.contract};

    WASM_ASSERT("validate: recipient account is not exist",
                arb.validate(N(mindswaparbi), N(swap), N(mindswaparbi), sys_token_asset, N(notexisten)));

    WASM_ASSERT("validate: withdraw account is not exist",
                arb.validate(N(mindswaparbi), N(swap), N(mindswaparbi), sys_token_asset, N(alice)));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(validate_test, arbitrage_tester)
try {
    extended_asset iq_token_asset{asset::from_string("200.000 IQ"), iq.contract};

    SUCCESS(arb.validate(N(mindswaparbi), N(swap), N(mindswaparbi), iq_token_asset, N(alice)));
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
