#pragma once
#include "contracts.hpp"
#include "arbitrage_test_api.hpp"
#include "token_test_api.hpp"
#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

#define SUCCESS(call) BOOST_REQUIRE_EQUAL(success(), call)
#define WASM_ASSERT(msg, call) BOOST_REQUIRE_EQUAL(wasm_assert_msg(msg), call)
#define ERROR(msg, call) BOOST_REQUIRE_EQUAL(error(msg), call)

class arbitrage_tester : public tester {
protected:
	arbitrage_test_api arb;
	token_test_api iq;

public:
	arbitrage_tester();

private:
	void init_token();
	void init_traders_accounts();
};