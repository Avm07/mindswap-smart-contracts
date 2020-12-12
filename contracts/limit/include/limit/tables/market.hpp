#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] market {
	uint64_t id;
	extended_symbol token1;
	extended_symbol token2;
	uint64_t availiable_ord_id;

	uint64_t primary_key() const {
		return id;
	}

    EOSLIB_SERIALIZE(market, (id)(token1)(token2)(availiable_ord_id))
};
using markets = multi_index<name("markets"), market>;
