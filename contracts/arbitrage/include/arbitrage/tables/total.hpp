#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::contract("arbitrage"), eosio::table]] currency_total {
    uint64_t id;
	extended_asset total_supply;
	
	uint64_t primary_key() const {
		return id;
	}

    std::string to_string(const extended_asset &token) const {
		std::string str = token.quantity.symbol.code().to_string() + "@" + token.contract.to_string();
		return str;
	}

	checksum256 token_hash_key() const {
		std::string str = to_string(total_supply);
		return sha256(str.data(), str.size());
	}
};
using by_supply_token_hash = indexed_by<name("bytoken"), const_mem_fun<currency_total, checksum256, &currency_total::token_hash_key>>;
using totals = multi_index<name("totals"), currency_total, by_supply_token_hash>;