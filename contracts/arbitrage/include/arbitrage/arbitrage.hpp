#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract("arbitrage")]] arbitrage : public contract {
public:
	arbitrage(name receiver, name code, datastream<const char*> ds);

	[[eosio::action("arbitrage")]] void arbitrage_action(const extended_asset& token1, const extended_asset& token2, const name& owner);

private:

};