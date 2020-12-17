#include "limit_tester.hpp"

limit_tester::limit_tester()
	: limit(N(limit), this)
	, iq(N(everipediaiq), this) {
	create_accounts({N(alice), N(bob), N(cesar), N(carol)});
	produce_blocks(2);
	init_token();
}

void limit_tester::init_token() {
	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(create), mvo()("issuer", iq.contract)("maximum_supply", asset::from_string("1000000000.000 IQ"))));

	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(issue), mvo()("to", iq.contract)("quantity", asset::from_string("1000000000.000 IQ"))("memo", "")));

	SUCCESS(iq.transfer(iq.contract, N(alice), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(bob), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(cesar), asset::from_string("500.000 IQ"), ""));
}