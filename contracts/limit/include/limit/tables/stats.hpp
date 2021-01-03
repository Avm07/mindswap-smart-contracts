#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::table]] currency_stats {
	asset supply;
	asset max_supply;
	name issuer;

	uint64_t primary_key() const {
		return supply.symbol.code().raw();
	}
};
using stats = multi_index<name("stat"), currency_stats>;