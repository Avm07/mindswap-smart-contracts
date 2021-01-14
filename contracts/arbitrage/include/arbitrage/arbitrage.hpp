#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <token/tables/account.hpp>
#include <token/tables/stats.hpp>
#include <limit/tables/market.hpp>
#include <limit/tables/order.hpp>
#include <arbitrage/tables/deposit.hpp>
#include <arbitrage/tables/total.hpp>
#include <resources/resources.hpp>

using namespace eosio;

class [[eosio::contract("arbitrage")]] arbitrage : public contract {
public:
	arbitrage(name receiver, name code, datastream<const char*> ds);

	//For managing virtual balances
	[[eosio::action("open")]] void open_account(const name& owner, const extended_symbol& token, const name& ram_payer);

	[[eosio::action("close")]] void close_account(const name& owner, const extended_symbol& token);

	[[eosio::action("withdraw")]] void withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo);

	[[eosio::action("arbordtrade")]] void arbitrage_order_trade(const name& owner, const uint64_t& market_id,
																const name& order_type, const uint64_t& order_id);

	[[eosio::action("arbpairtrade")]] void arbitrage_pair_trade(const name& owner, const uint64_t& market_id);

	[[eosio::on_notify("*::transfer")]] void on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo);

private:
	void sub_balance(const name& owner, const extended_asset& value);
	void add_balance(const name& owner, const extended_asset& value, const name& ram_payer);

	std::string create_request_memo(const symbol_code& sym1, const symbol_code& sym2, const asset& amount);
	std::pair<extended_asset, std::string> count_swap_request(const name& order_type, const uint64_t& market_id, const uint64_t& id);

	std::string to_string(const extended_symbol& token);
	checksum256 to_token_hash_key(const extended_symbol& token);

	bool is_deposit_account_exist(const name& owner, const extended_symbol& token);
	bool is_withdraw_account_exist(const name& owner, const extended_symbol& token);
	bool is_valid_order_type(const name& type);
	bool is_valid_market_id(const uint64_t& id);
	bool is_valid_order_id(const name& order_type, const uint64_t& market_id, const uint64_t& id);


	void send_fillorder(const name& order_type, const uint64_t& market_id, const uint64_t& id);
	void send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo);
};