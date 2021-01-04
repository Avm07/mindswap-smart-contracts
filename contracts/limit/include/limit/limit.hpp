#pragma once
#include <cmath>
#include <eosio/eosio.hpp>
#include <token/tables/account.hpp>
#include <token/tables/stats.hpp>
#include <limit/tables/deposit.hpp>
#include <limit/tables/market.hpp>
#include <limit/tables/order.hpp>

using namespace eosio;
using trade_pair = std::pair<extended_symbol, extended_symbol>;
using price_n_date = std::pair<asset, time_point>;

class [[eosio::contract("limit")]] limit : public contract {
public:
	limit(name receiver, name code, datastream<const char*> ds);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& token, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& token);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	//For creating orders
	[[eosio::action("crtlmtbuy")]] void create_limit_buy(const name& owner, const extended_asset& volume, const extended_asset& price);

	[[eosio::action("crtlmtsell")]] void create_limit_sell(const name& owner, const extended_asset& volume, const extended_asset& price);

	[[eosio::action("clslmtbuy")]] void close_limit_buy(const uint64_t& market_id, const uint64_t& id);

	[[eosio::action("clslmtsell")]] void close_limit_sell(const uint64_t& market_id, const uint64_t& id);

	[[eosio::on_notify("*::transfer")]] void on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);
	void sub_balance_in_orders(const name& owner, const extended_asset& value);
	void add_balance_in_orders(const name& owner, const extended_asset& value, const name& ram_payer);

	std::pair<bool, uint64_t> add_market(const extended_symbol& token1, const extended_symbol& token2, const name& ram_payer);
	void remove_market(const extended_symbol& token1, const extended_symbol& token2);

	template <typename B, typename S>
	void manage_lmt_orders(B & lmt_buy, S & lmt_sell);

	template <typename T>
	uint64_t find_first_buy(T & lmt_buy);
	template <typename T>
	uint64_t find_first_sell(T & lmt_sell);

	void exec_lmt_buy(const extended_asset& amount, const extended_asset& deal_volume, const name& owner);
	void exec_lmt_sell(const extended_asset& amount, const extended_asset& deal_volume, const name& owner);

	price_n_date get_deal_price_n_date(const order& buy_order, const order& sell_order);
	uint64_t get_new_ord_id(const uint64_t& market_id);
	market get_market(const uint64_t& market_id);

	extended_asset count_amount(const extended_asset& volume, const extended_asset& price);

	std::string to_string(const extended_symbol& token);
	checksum256 to_token_hash_key(const extended_symbol& token);
	checksum256 to_pair_hash_key(const extended_symbol& token1, const extended_symbol& token2);

	bool is_deposit_account_exist(const name& owner, const extended_symbol& token);
	bool is_withdraw_account_exist(const name& owner, const extended_symbol& token);

	trade_pair is_market_exist(const uint64_t& market_id);

	template <typename B, typename S>
	bool is_limit_deal_exist(B& lmt_buy, S& lmt_sell);
	template <typename T>
	bool is_orders_exist(T& lmt);
	template <typename B, typename S>
	bool is_prices_match(B& buy, S& sell);

	bool is_open_orders_exist(const name& owner, const checksum256& token_hash);
	bool is_open_buy_orders_exist(const name& owner, const uint64_t& market_id);
	bool is_open_sell_orders_exist(const name& owner, const uint64_t& market_id);

	void send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo);
};