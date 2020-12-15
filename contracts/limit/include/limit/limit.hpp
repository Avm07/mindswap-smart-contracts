#pragma once
#include <eosio/eosio.hpp>
#include <limit/tables/account.hpp>
#include <limit/tables/deposit.hpp>
#include <limit/tables/market.hpp>
#include <limit/tables/order.hpp>
#include <limit/tables/stats.hpp>

using namespace eosio;

class [[eosio::contract("limit")]] limit : public contract {
public:
	limit(name receiver, name code, datastream<const char*> ds);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& token, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& token);

	[[eosio::action("deposit")]] void deposit(const name& to, const extended_asset& quantity, const std::string& memo);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	//For creating orders
	[[eosio::action("crtlmtbuy")]] void create_limit_buy(const name& owner, const extended_asset& volume, const extended_asset& price);

	[[eosio::action("crtlmtsell")]] void create_limit_sell(const name& owner, const extended_asset& volume, const extended_asset& price);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);

	void sub_balance_in_orders(const name& owner, const extended_asset& value);
	void add_balance_in_orders(const name& owner, const extended_asset& value, const name& ram_payer);

	void add_market(const extended_symbol& token1, const extended_symbol& token2, const name& ram_payer);
	void remove_market(const extended_symbol& token1, const extended_symbol& token2);

	std::string to_string(const extended_symbol& token);
	checksum256 to_token_hash_key(const extended_symbol& token);
	checksum256 to_pair_hash_key(const extended_symbol& token1, const extended_symbol& token2);
};