#pragma once
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

class [[eosio::contract]] limit : public eosio::contract {
public:
	limit(name receiver, name code, datastream<const char*> ds);

	//For managing markets
	[[eosio::action("addmarket")]] void add_market(const uint64_t& id, const extended_symbol& token1, const extended_symbol& token2);

	[[eosio::action("rmvmarket")]] void remove_market(const uint64_t& id, const extended_symbol& token1, const extended_symbol& token2);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& symbol, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& symbol);

	[[eosio::action("deposit")]] void deposit(const name& to, const extended_asset& quantity, const std::string& memo);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	//For creating orders
	[[eosio::action("crtlmtbuy")]] void create_limit_buy(const name& owner, const uint64_t market_id, const asset& volume, const asset& price);

	[[eosio::action("crtlmtsell")]] void create_limit_sell(const name& owner, const uint64_t market_id, const asset& volume, const asset& price);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);

	void sub_balance_in_orders(const name& owner, const extended_asset& value);
	void add_balance_in_orders(const name& owner, const extended_asset& value, const name& ram_payer);
};