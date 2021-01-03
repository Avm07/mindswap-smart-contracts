#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] deposit {
	uint64_t id;
	extended_asset balance;

	uint64_t primary_key() const {
		return id;
	}

	std::string to_string(const extended_asset &token) const {
		std::string str = token.quantity.symbol.code().to_string() + "@" + token.contract.to_string();
		return str;
	}

	checksum256 token_hash_key() const {
		std::string str = to_string(balance);
		return sha256(str.data(), str.size());
	}

	EOSLIB_SERIALIZE(deposit, (id)(balance))
};
using by_token_hash = indexed_by<name("bytoken"), const_mem_fun<deposit, checksum256, &deposit::token_hash_key>>;
using deposits = multi_index<name("deposits"), deposit, by_token_hash>;
