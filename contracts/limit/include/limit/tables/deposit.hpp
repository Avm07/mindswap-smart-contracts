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

	std::string to_string(const asset &token, const name& contract) const {
		std::string str = token.symbol.code().to_string() + "@" + contract.to_string();
		return str;
	}

	checksum256 token_hash_key() const {
		std::string str = to_string(balance, contract);
		return sha256(str.data(), str.size());
	}

	EOSLIB_SERIALIZE(deposit, (id)(contract)(balance)(balance_in_orders))
};
using by_token_hash = indexed_by<name("bytoken"), const_mem_fun<deposit, checksum256, &deposit::token_hash_key>>;
using deposits = multi_index<name("deposits"), deposit, by_token_hash>;
