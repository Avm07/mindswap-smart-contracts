#pragma once
#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include "arbitrage_test_api.hpp"
#include "token_test_api.hpp"
#include "mindswap_test_api.hpp"
#include "limit_test_api.hpp"

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
	limit_test_api limit;
	mindswap_test_api swap;

public:
	arbitrage_tester();

private:
	void init_token();
	void init_traders_accounts();
	void init_mindswap();
	void init_limit();
};