#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] market {
	uint64_t id;
	extended_symbol token1;
	extended_symbol token2;
	uint64_t available_ord_id;

	uint64_t primary_key() const {
		return id;
	}

	std::string to_string(const extended_symbol &token) const {
		std::string str = token.get_symbol().code().to_string() + "@" + token.get_contract().to_string();
		return str;
	}

	checksum256 token1_hash_key() const {
		std::string str = to_string(token1);
		return sha256(str.data(), str.size());
	}

	checksum256 token2_hash_key() const {
		std::string str = to_string(token2);
		return sha256(str.data(), str.size());
	}

	checksum256 pair_hash_key() const {
		std::string str = to_string(token1) + "/" + to_string(token2);
		return sha256(str.data(), str.size());
	}

    EOSLIB_SERIALIZE(market, (id)(token1)(token2)(available_ord_id))
};
using by_token1_hash = indexed_by<name("bytoken1"), const_mem_fun<market, checksum256, &market::token1_hash_key>>;
using by_token2_hash = indexed_by<name("bytoken2"), const_mem_fun<market, checksum256, &market::token2_hash_key>>;
using by_pair_hash = indexed_by<name("bypair"), const_mem_fun<market, checksum256, &market::pair_hash_key>>;
using markets = multi_index<name("markets"), market, by_pair_hash>;
