#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("arbitrage")]] arbitrage : public contract {
public:
	arbitrage(name receiver, name code, datastream<const char*> ds);

private:

};