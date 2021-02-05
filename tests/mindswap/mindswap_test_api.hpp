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

class mindswap_test_api {
public:
	mindswap_test_api(name acnt, tester* tester);

	action_result openext(const name& user, const name& payer, const extended_symbol& extended_symbol);
	action_result inittoken(const name& user, const symbol& new_symbol, const extended_asset& initial_pool1,
							const extended_asset& initial_pool2, const int& initial_fee, const name& fee_contract, const uint64_t& curve);

	name contract;
private:
	action_result push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data);

	abi_serializer abi_ser;
	tester* _tester;
};
