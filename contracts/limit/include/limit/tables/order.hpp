#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

using namespace eosio;

struct [[eosio::contract("limit"), eosio::table]] order {
	uint64_t id;
	name owner;
	asset volume;
	asset balance;
	asset price;
	time_point creation_date;

	uint64_t primary_key() const {
		return id;
	}

	uint64_t owner_key() const {
		return owner.value;
	}

	uint64_t price_key() const {
		return price.amount;
	}

	EOSLIB_SERIALIZE(order, (id)(owner)(volume)(balance)(price)(creation_date))
};
using by_price = indexed_by<name("byprice"), const_mem_fun<order, uint64_t, &order::price_key>>;
using by_owner = indexed_by<name("byowner"), const_mem_fun<order, uint64_t, &order::owner_key>>;

using buy_orders = multi_index<name("buyorders"), order, by_price, by_owner>;
using sell_orders = multi_index<name("sellorders"), order, by_price, by_owner>;
