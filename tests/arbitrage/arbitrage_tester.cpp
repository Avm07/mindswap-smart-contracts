#include "arbitrage_tester.hpp"

arbitrage_tester::arbitrage_tester()
	: arb(N(arbitrage), this),
	iq(N(everipediaiq), this),
	swap(N(mindswapswap), this)	
{
	create_accounts({N(alice), N(bob), N(cesar), N(carol), N(trader1), N(trader2)});
	produce_blocks(2);
	init_token();
	init_traders_accounts();
}

void arbitrage_tester::init_token() {
	SUCCESS(iq.create(iq.contract, asset::from_string("1000000000.000 IQ")));
	SUCCESS(iq.issue(iq.contract, asset::from_string("1000000000.000 IQ"), ""));

	SUCCESS(iq.create(iq.contract, asset::from_string("1000000000.000 SYS")));
	SUCCESS(iq.issue(iq.contract, asset::from_string("1000000000.000 SYS"), ""));

	SUCCESS(iq.transfer(iq.contract, N(alice), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(bob), asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(cesar), asset::from_string("500.000 IQ"), ""));

	SUCCESS(iq.transfer(iq.contract, N(trader1), asset::from_string("500.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(trader2), asset::from_string("500.000 IQ"), ""));

	SUCCESS(iq.transfer(iq.contract, N(trader1), asset::from_string("500.000 SYS"), ""));
	SUCCESS(iq.transfer(iq.contract, N(trader2), asset::from_string("500.000 SYS"), ""));

	SUCCESS(iq.transfer(iq.contract, N(prediqtteam1), asset::from_string("1000.000 IQ"), ""));
	SUCCESS(iq.transfer(iq.contract, N(prediqtteam1), asset::from_string("1000.000 SYS"), ""));
}

void arbitrage_tester::init_traders_accounts() {
	extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
	extended_symbol sys_token{symbol(SY(3, SYS)), N(everipediaiq)};

	//Open virtual balances
	SUCCESS(arb.open(N(trader1), N(trader1), iq_token, N(trader1)));
	SUCCESS(arb.open(N(trader1), N(trader1), sys_token, N(trader1)));

	SUCCESS(arb.open(N(trader2), N(trader2), iq_token, N(trader2)));
	SUCCESS(arb.open(N(trader2), N(trader2), sys_token, N(trader2)));

	//Make deposits
	SUCCESS(iq.transfer(N(trader1), arb.contract, asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(N(trader1), arb.contract, asset::from_string("100.000 SYS"), ""));

	SUCCESS(iq.transfer(N(trader2), arb.contract, asset::from_string("100.000 IQ"), ""));
	SUCCESS(iq.transfer(N(trader2), arb.contract, asset::from_string("100.000 SYS"), ""));
}

void arbitrage_tester::init_mindswap()
{
	extended_symbol iq_token{symbol(SY(3, IQ)), N(everipediaiq)};
	extended_symbol sys_token{symbol(SY(3, SYS)), N(everipediaiq)};

	SUCCESS(swap.openext(N(prediqtteam1), N(prediqtteam1), iq_token));
	SUCCESS(swap.openext(N(prediqtteam1), N(prediqtteam1), sys_token));
	SUCCESS(iq.transfer(N(prediqtteam1), N(mindswapswap), asset::from_string("1000.000 IQ"), "liquidity"));
	SUCCESS(iq.transfer(N(prediqtteam1), N(mindswapswap), asset::from_string("1000.000 SYS"), "liquidity"));

	extended_asset iq_token_asset{asset::from_string("1000.000 IQ"), N(everipediaiq)};
	extended_asset sys_token_asset{asset::from_string("1000.000 SYS"), N(everipediaiq)};
	
	SUCCESS(swap.inittoken(N(prediqtteam1), symbol(SY(3, SYSIQ)), iq_token_asset, sys_token_asset, 70, N(), 1));
}
