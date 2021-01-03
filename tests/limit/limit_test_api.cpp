#include "limit_test_api.hpp"

limit_test_api::limit_test_api(name acnt, tester* tester) {
	contract = acnt;
	_tester = tester;

	_tester->create_accounts({contract});
	_tester->set_code(contract, contracts::limit_wasm());
	_tester->set_abi(contract, contracts::limit_abi().data());

	const auto& accnt = _tester->control->db().get<account_object, by_name>(contract);

	abi_def abi;
	BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
	abi_ser.set_abi(abi, base_tester::abi_serializer_max_time);

	auto auth = authority(
		1, {key_weight{_tester->get_public_key(contract, "active"), 1}}, {permission_level_weight{{contract, config::eosio_code_name}, 1}});

	_tester->set_authority(contract, config::active_name, auth);
}

fc::variant limit_test_api::get_deposit(const name& owner, const uint64_t& id)
{
    vector<char> data = _tester->get_row_by_account(contract, owner, N(deposits), account_name(id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("deposit", data, base_tester::abi_serializer_max_time);
}

fc::variant limit_test_api::get_market(const uint64_t& id)
{
    vector<char> data = _tester->get_row_by_account(contract, contract, N(markets), account_name(id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("market", data, base_tester::abi_serializer_max_time);
}

fc::variant limit_test_api::get_buy_order(const uint64_t& market_id, const uint64_t& order_id)
{
	vector<char> data = _tester->get_row_by_account(contract, name(market_id), N(buyorders), account_name(order_id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("order", data, base_tester::abi_serializer_max_time);
}
	
fc::variant limit_test_api::get_sell_order(const uint64_t& market_id, const uint64_t& order_id)
{
	vector<char> data = _tester->get_row_by_account(contract, name(market_id), N(sellorders), account_name(order_id));
    return data.empty() ? fc::variant() : abi_ser.binary_to_variant("order", data, base_tester::abi_serializer_max_time);
}

action_result limit_test_api::open(const name& signer, const name& owner, const extended_symbol& token, const name& ram_payer)
{
	return push_action(signer, contract, N(open), mvo()("owner", owner)("token", token)("ram_payer", ram_payer));
}

action_result limit_test_api::close(const name& signer, const name& owner, const extended_symbol& token)
{
	return push_action(signer, contract, N(close), mvo()("owner", owner)("token", token));
}

action_result limit_test_api::withdraw(const name& signer, const name& from, const name& to, const extended_asset& quantity, const std::string& memo)
{
	return push_action(signer, contract, N(withdraw), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
}

action_result limit_test_api::create_limit_buy(const name& signer, const name& owner, const extended_asset& volume, const extended_asset& price)
{
	return push_action(signer, contract, N(crtlmtbuy), mvo()("owner", owner)("volume", volume)("price", price));
}

action_result limit_test_api::create_limit_sell(const name& signer, const name& owner, const extended_asset& volume, const extended_asset& price)
{
	return push_action(signer, contract, N(crtlmtsell), mvo()("owner", owner)("volume", volume)("price", price));
}

action_result limit_test_api::close_limit_buy(const name& signer, const uint64_t& market_id, const uint64_t& id)
{
	return push_action(signer, contract, N(clslmtbuy), mvo()("market_id", market_id)("id", id));
}

action_result limit_test_api::close_limit_sell(const name& signer, const uint64_t& market_id, const uint64_t& id)
{
	return push_action(signer, contract, N(clslmtsell), mvo()("market_id", market_id)("id", id));
}

action_result limit_test_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}