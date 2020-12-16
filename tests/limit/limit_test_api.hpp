#pragma once
#include "contracts.hpp"
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;
using action_result = base_tester::action_result;

class limit_test_api {
public:
	name contract;

	limit_test_api(name acnt, tester* tester);

	action_result hello(const name& user, const string& text);

private:
	action_result push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data);

	abi_serializer abi_ser;
	tester* _tester;
};