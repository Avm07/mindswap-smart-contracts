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

action_result limit_test_api::hello(const name& user, const string& text) {
	return push_action(user, contract, N(hello), mvo()("user", user)("text", text));
}

action_result limit_test_api::push_action(const name& signer, const name& cnt, const action_name& name, const variant_object& data) {
	string action_type_name = abi_ser.get_action_type(name);
	action act;
	act.account = cnt;
	act.name = name;
	act.data = abi_ser.variant_to_binary(action_type_name, data, base_tester::abi_serializer_max_time);

	return _tester->push_action(std::move(act), signer.to_uint64_t());
}