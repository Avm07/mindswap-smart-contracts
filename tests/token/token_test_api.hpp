#pragma once
#include "contracts.hpp"
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;

using mvo = fc::mutable_variant_object;
using action_result = base_tester::action_result;

class token_test_api {
public:
	token_test_api(account_name acnt, tester* tester);

	fc::variant get_account(const account_name& acc, const string& symbolname);

	action_result create(const name& issuer, const asset& maximum_supply);
	action_result issue(const name& to, const asset& quantity, const std::string& memo);
	action_result transfer(const name& from, const name& to, const asset& amount, const string& memo);

	name contract;
private:
	action_result push_action(const account_name& signer, const account_name& cnt, const action_name& name, const variant_object& data);

	abi_serializer abi_ser;
	tester* _tester;
};