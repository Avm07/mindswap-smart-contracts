#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::table]] mindaccount {
	extended_asset balance;
	uint64_t id;

	uint64_t primary_key() const {
		return id;
	}

	uint128_t secondary_key() const {
		return make128key(balance.contract.value, balance.quantity.symbol.raw());
	}
};

static uint128_t make128key(uint64_t a, uint64_t b);

using by_extended_key = indexed_by<"extended"_n, const_mem_fun<mindaccount, uint128_t, &mindaccount::secondary_key>>;
using mindswap_mindaccounts = multi_index<"mindacnts"_n, mindaccount, by_extended_key>;