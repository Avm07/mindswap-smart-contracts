#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

using namespace eosio;

namespace token {
struct [[eosio::table]] currency_stats {
	asset supply;
	asset max_supply;
	name issuer;

	uint64_t primary_key() const {
		return supply.symbol.code().raw();
	}
};

using stats = multi_index<name("stat"), currency_stats>;
} // namespace token
