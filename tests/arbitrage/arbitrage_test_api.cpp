#include "arbitrage_test_api.hpp"

arbitrage_test_api::arbitrage_test_api(name acnt, tester* tester) {
	contract = acnt;
	_tester = tester;

	_tester->create_accounts({contract});
	_tester->set_code(contract, contracts::arbitrage_wasm());
	_tester->set_abi(contract, contracts::arbitrage_abi().data());

	const auto& accnt = _tester->control->db().get<account_object, by_name>(contract);

	abi_def abi;
	BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
	abi_ser.set_abi(abi, base_tester::abi_serializer_max_time);

	auto auth = authority(
		1, {key_weight{_tester->get_public_key(contract, "active"), 1}}, {permission_level_weight{{contract, config::eosio_code_name}, 1}});

	_tester->set_authority(contract, config::active_name, auth);
}

fc::variant arbitrage_test_api::get_deposit(const name& owner, const uint64_t& id)
{
    vector<char> data = _tester->get_row_by_account(contract, owner, N(deposits), account_name(id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("deposit", data, base_tester::abi_serializer_max_time);
}

action_result arbitrage_test_api::open(const name& signer, const name& owner, const extended_symbol& token, const name& ram_payer)
{
	return push_action(signer, contract, N(open), mvo()("owner", owner)("token", token)("ram_payer", ram_payer));
}

action_result arbitrage_test_api::close(const name& signer, const name& owner, const extended_symbol& token)
{
	return push_action(signer, contract, N(close), mvo()("owner", owner)("token", token));
}

action_result arbitrage_test_api::withdraw(const name& signer, const name& from, const name& to, const extended_asset& quantity, const std::string& memo)
{
	return push_action(signer, contract, N(withdraw), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
}

action_result arbitrage_test_api::arbitrage_order_trade(const name& signer, const uint64_t& market_id, const name& order_type, const uint64_t& order_id,  const symbol_code& mindswap_pool)
{
	return push_action(signer, contract, N(arbordtrade), mvo()("market_id", market_id)("order_type", order_type)("order_id", order_id)("mindswap_pool", mindswap_pool));
}

action_result arbitrage_test_api::arbitrage_pair_trade(const name& signer, const uint64_t& market_id, const name& orders_type, const std::vector<uint64_t>& orders_ids,  const symbol_code& mindswap_pool)
{
	return push_action(signer, contract, N(arbpairtrade), mvo()("market_id", market_id)("orders_type", orders_type)("orders_ids", orders_ids)("mindswap_pool", mindswap_pool));
}

action_result arbitrage_test_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}