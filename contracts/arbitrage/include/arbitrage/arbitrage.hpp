#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract("arbitrage")]] arbitrage : public contract {
public:
	arbitrage(name receiver, name code, datastream<const char*> ds);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& token, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& token);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	[[eosio::action("arbitrage")]] void arbitrage_action(const extended_asset& token1, const extended_asset& token2, const name& owner);

	[[eosio::on_notify("*::transfer")]] void on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);

	std::string to_string(const extended_symbol& token);
	checksum256 to_token_hash_key(const extended_symbol& token);

	bool is_deposit_account_exist(const name& owner, const extended_symbol& token);
	bool is_withdraw_account_exist(const name& owner, const extended_symbol& token);
};