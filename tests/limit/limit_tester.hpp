#pragma once
#include "contracts.hpp"
#include "limit_test_api.hpp"
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
#define ERROR(msg, call) BOOST_REQUIRE_EQUAL(wasm_assert_msg(msg), call)

class limit_tester : public tester {
protected:
	limit_test_api api;
	token_test_api iq;

public:
	limit_tester();

private:
	void init_token(const name& to);
};