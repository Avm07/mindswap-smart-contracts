#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::table]] currency_stats {
	asset supply;
	asset max_supply;
	name issuer;
	extended_asset pool1;
	extended_asset pool2;
	int fee;
	name fee_contract;
	uint64_t curve;

	uint64_t primary_key() const {
		return supply.symbol.code().raw();
	}
};

using mindswap_stats = multi_index<"stat"_n, currency_stats>;