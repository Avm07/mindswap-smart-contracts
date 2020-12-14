#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] deposit {
	uint64_t id;
	name contract;
	asset balance;
	asset balance_in_orders;

	uint64_t primary_key() const {
		return id;
	}

	checksum256 token_hash_key() const {
		std::string str = balance.symbol.code().to_string() + "@" + contract.to_string();
		return sha256(str.data(), str.size());
	}

	EOSLIB_SERIALIZE(deposit, (id)(contract)(balance)(balance_in_orders))
};
using deposits = multi_index<name("deposits"), deposit>;
