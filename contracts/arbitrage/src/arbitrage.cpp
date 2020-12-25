#include <arbitrage/arbitrage.hpp>

arbitrage::arbitrage(name receiver, name code, datastream<const char*> ds)
	: contract::contract(receiver, code, ds) { }

void arbitrage::arbitrage_action(const extended_asset& token1, const extended_asset& token2, const name& owner) {

}

