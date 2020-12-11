#pragma once

#include <eosio/eosio.hpp>

using eosio::name;
using std::string;

class [[eosio::contract]] demo : public eosio::contract {
public:
	using contract::contract;

	ACTION hello(const name& user, const string& text);
};
