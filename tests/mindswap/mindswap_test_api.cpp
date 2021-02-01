#include "mindswap_test_api.hpp"

mindswap_test_api::mindswap_test_api(name acnt, tester* tester) {
	contract = acnt;
	_tester = tester;

	_tester->create_accounts({contract});
	_tester->set_code(contract, contracts::mindswapswap_wasm());
	_tester->set_abi(contract, contracts::mindswapswap_abi().data());

	const auto& accnt = _tester->control->db().get<account_object, by_name>(contract);

	abi_def abi;
	BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
	abi_ser.set_abi(abi, base_tester::abi_serializer_max_time);

	auto auth = authority(
		1, {key_weight{_tester->get_public_key(contract, "active"), 1}}, {permission_level_weight{{contract, config::eosio_code_name}, 1}});

	_tester->set_authority(contract, config::active_name, auth);
}

action_result mindswap_test_api::openext(const name& user, const name& payer, const extended_symbol& extended_symbol) {
	return push_action(payer, contract, N(openext), mvo()("user", user)("payer", payer)("ext_symbol", extended_symbol));
}

action_result mindswap_test_api::inittoken(const name& user,
										   const symbol& new_symbol,
										   const extended_asset& initial_pool1,
										   const extended_asset& initial_pool2,
										   const int& initial_fee,
										   const name& fee_contract,
										   const uint64_t& curve) {
	return push_action(user,
					   contract,
					   N(inittoken),
					   mvo()("user", user)("new_symbol", new_symbol)("initial_pool1", initial_pool1)("initial_pool2", initial_pool2)(
						   "initial_fee", initial_fee)("fee_contract", fee_contract)("curve", curve));
}

action_result mindswap_test_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}