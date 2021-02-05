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

class arbitrage_test_api {
public:
	arbitrage_test_api(name acnt, tester* tester);

	fc::variant get_deposit(const name& owner, const uint64_t& id);

	action_result open(const name& signer, const name& owner, const extended_symbol& token, const name& ram_payer);
	action_result close(const name& signer, const name& owner, const extended_symbol& token);
	action_result withdraw(const name& signer, const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	action_result arbitrage_order_trade(const name& signer, const uint64_t& market_id, const name& order_type,
										const uint64_t& order_id, const symbol_code& mindswap_pool);
										
	action_result arbitrage_pair_trade(const name& signer, const uint64_t& market_id, const name& orders_type, 
									   const std::vector<uint64_t>& orders_ids,  const symbol_code& mindswap_pool);

	action_result validate(const name& signer, const name& type, const name& account, const extended_asset& expected_balance);

	name contract;
private:
	action_result push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data);

	abi_serializer abi_ser;
	tester* _tester;
};