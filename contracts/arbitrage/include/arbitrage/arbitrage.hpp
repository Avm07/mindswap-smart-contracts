#pragma once
#include <arbitrage/tables/deposit.hpp>
#include <cmath>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <limit/tables/market.hpp>
#include <limit/tables/order.hpp>
#include <mindswap/tables/stat.hpp>
#include <resources/resources.hpp>
#include <token/tables/account.hpp>
#include <token/tables/stats.hpp>

using namespace eosio;
using namespace token;

class [[eosio::contract("arbitrage")]] arbitrage : public contract {
public:
	arbitrage(name receiver, name code, datastream<const char*> ds);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& token, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& token);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	[[eosio::action("arbordtrade")]] void arbitrage_order_trade(
		const uint64_t& market_id, const name& order_type, const uint64_t& order_id, const asset& amount, const symbol_code& mindswap_pool);

	[[eosio::action("arbpairtrade")]] void arbitrage_pair_trade(
		const uint64_t& market_id, const name& orders_type, const std::vector<uint64_t>& orders_ids, const symbol_code& mindswap_pool);

	[[eosio::action("validate")]] void validate(const name& type, const name& account, const extended_asset& expected_balance);

	[[eosio::on_notify("*::transfer")]] void on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);

	std::string create_request_memo(const symbol_code& mindswap_pool, const asset& amount_to);
	extended_asset count_amount(const extended_asset& volume, const extended_asset& price);
	std::tuple<extended_asset, extended_asset, std::string> count_swap_amounts(const symbol_code& mindswap_pool, const name& order_type,
																			   const uint64_t& market_id, const uint64_t& id);
	std::tuple<extended_asset, extended_asset, std::string> count_swap_amounts(const symbol_code& mindswap_pool, const name& order_type,
																			   const uint64_t& market_id, const uint64_t& id, const asset& amount);

	extended_asset get_balance(const name& owner, const extended_symbol& token);
	market get_market(const uint64_t& market_id);

	std::string to_pool_name(const symbol_code& symb1, const symbol_code& symb2);
	std::string to_string(const extended_symbol& token);
	checksum256 to_token_hash_key(const extended_symbol& token);

	bool is_deposit_account_exist(const name& owner, const extended_symbol& token);
	bool is_withdraw_account_exist(const name& owner, const extended_symbol& token);
	bool is_valid_order_type(const name& type);
	bool is_valid_market_id(const uint64_t& id);
	bool is_valid_order_id(const name& order_type, const uint64_t& market_id, const uint64_t& id);
	bool is_valid_amount(const name& order_type, const uint64_t& market_id, const uint64_t& id, const asset& amount);
	bool is_valid_orders_ids(const name& order_type, const uint64_t& market_id, const std::vector<uint64_t>& ids);
	bool is_pool_exist(const symbol_code& mindswap_pool);
	bool is_valid_pool(const symbol_code& mindswap_pool, const symbol_code& symb1, const symbol_code& symb2);

	void send_part_fill_buy_order(const uint64_t& market_id, const uint64_t& id, const asset& amount);
	void send_part_fill_sell_order(const uint64_t& market_id, const uint64_t& id, const asset& amount);
	void send_fill_buy_order(const uint64_t& market_id, const uint64_t& id);
	void send_fill_sell_order(const uint64_t& market_id, const uint64_t& id);
	void send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo);
	void send_validate(const name& type, const name& contract, const extended_asset& expected_balance);
};