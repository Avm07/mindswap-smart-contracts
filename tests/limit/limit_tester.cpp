#include "limit_tester.hpp"

limit_tester::limit_tester()
	: limit(N(limit), this)
	, iq(N(everipediaiq), this) {
	create_accounts({N(alice), N(bob), N(cesar), N(carol), N(trader1), N(trader2)});
	produce_blocks(2);
	init_token();
	init_traders_accounts();
}

void limit_tester::init_token() {
	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(create), mvo()("issuer", iq.contract)("maximum_supply", asset::from_string("1000000000.000 IQ"))));

	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(issue), mvo()("to", iq.contract)("quantity", asset::from_string("1000000000.000 IQ"))("memo", "")));

	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(create), mvo()("issuer", iq.contract)("maximum_supply", asset::from_string("1000000000.000 SYS"))));

	SUCCESS(iq.push_action(
		iq.contract, iq.contract, N(issue), mvo()("to", iq.contract)("quantity", asset::from_string("1000000000.000 SYS"))("memo", "")));

	SUCCESS(iq.transfer(iq.contract, N(alice), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(bob), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(cesar), asset::from_string("500.000 IQ"), ""));

	SUCCESS(iq.transfer(iq.contract, N(trader1), asset::from_string("500.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(trader2), asset::from_string("500.000 IQ"), ""));

	SUCCESS(iq.transfer(iq.contract, N(trader1), asset::from_string("500.000 SYS"), ""));
	SUCCESS(iq.transfer(iq.contract, N(trader2), asset::from_string("500.000 SYS"), ""));
}

void limit_tester::init_traders_accounts() {
	extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
	extended_symbol sys_token{symbol(SY(3, SYS)), N(everipediaiq)};

	//Open virtual balances
	SUCCESS(limit.open(N(trader1), N(trader1), iq_token, N(trader1)));
	SUCCESS(limit.open(N(trader1), N(trader1), sys_token, N(trader1)));

	SUCCESS(limit.open(N(trader2), N(trader2), iq_token, N(trader2)));
	SUCCESS(limit.open(N(trader2), N(trader2), sys_token, N(trader2)));

	//Make deposits
	SUCCESS(iq.transfer(N(trader1), limit.contract, asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(N(trader1), limit.contract, asset::from_string("100.000 SYS"), ""));

	SUCCESS(iq.transfer(N(trader2), limit.contract, asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(N(trader2), limit.contract, asset::from_string("100.000 SYS"), ""));
}
