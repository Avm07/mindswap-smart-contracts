#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] deposit {
	extended_asset balance;
	extended_asset balance_in_orders;

	uint64_t primary_key() const {
		return balance.quantity.symbol.code().raw();
	}

	EOSLIB_SERIALIZE(deposit, (balance)(balance_in_orders))
};
using deposits = multi_index<name("deposits"), deposit>;
