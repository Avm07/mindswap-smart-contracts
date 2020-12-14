#include "limit/limit.hpp"

limit::limit(name receiver, name code, datastream<const char*> ds)
	: contract::contract(receiver, code, ds) { }

void limit::open_account(const name& owner, const extended_symbol& token, const name& ram_payer) {
	require_auth(ram_payer);
	check(is_account(owner), "owner account does not exist");

	auto sym_code_raw = token.get_symbol().code().raw();
	stats statstable(token.get_contract(), sym_code_raw);
	const auto& st = statstable.get(sym_code_raw, "symbol does not exist");
	check(st.supply.symbol == token.get_symbol(), "symbol precision mismatch");

	deposits deposits(get_self(), owner.value);
	auto index = deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);

	if (it == index.end()) {
		deposits.emplace(ram_payer, [&](auto& a) {
			a.id = deposits.available_primary_key();
			a.contract = token.get_contract();
			a.balance = asset{0, token.get_symbol()};
			a.balance_in_orders = asset{0, token.get_symbol()};
		});
	}
}

void limit::close_account(const name& owner, const extended_symbol& token) {
	require_auth(owner);
	deposits deposits(get_self(), owner.value);
	auto it = deposits.find(token.get_symbol().code().raw());
	check(it != deposits.end(), "Balance row already deleted or never existed. Action won't have any effect.");
	check(it->balance.amount == 0, "Cannot close because the balance is not zero.");
	check(it->balance_in_orders.amount == 0, "Cannot close because the balance_in_orders is not zero.");
	deposits.erase(it);
}

void limit::deposit(const name& to, const extended_asset& quantity, const std::string& memo) {
	require_auth(get_self());
	add_balance(to, quantity, same_payer);
}

void limit::withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo) {
	require_auth(get_self());
	sub_balance(from, quantity);
}

void limit::create_limit_buy(const name& owner, const extended_asset& volume, const extended_asset& price) {
    require_auth(owner);
 }

void limit::create_limit_sell(const name& owner, const extended_asset& volume, const extended_asset& price) {
    require_auth(owner);
 }

void limit::sub_balance(const name& owner, const extended_asset& value) {
	deposits deposits(get_self(), owner.value);
	auto index = deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	const auto& from = index.get(hash, "no balance object found");
	check(from.balance.amount >= value.quantity.amount, "overdrawn balance");

	deposits.modify(from, owner, [&](auto& a) { a.balance -= value.quantity; });
}

void limit::add_balance(const name& owner, const extended_asset& value, const name& ram_payer) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	auto to = index.find(hash);
	if (to == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) { a.balance = value.quantity; });
	} else {
		_deposits.modify(to, same_payer, [&](auto& a) { a.balance += value.quantity; });
	}
}

void limit::sub_balance_in_orders(const name& owner, const extended_asset& value) {
    deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	const auto& from = index.get(hash, "no balance object found");
	check(from.balance_in_orders.amount >= value.quantity.amount, "overdrawn balance");

	_deposits.modify(from, owner, [&](auto& a) { a.balance_in_orders -= value.quantity; });
}

void limit::add_balance_in_orders(const name& owner, const extended_asset& value, const name& ram_payer) {
    deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	auto to = index.find(hash);
	if (to == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) { a.balance_in_orders = value.quantity; });
	} else {
		_deposits.modify(to, same_payer, [&](auto& a) { a.balance_in_orders += value.quantity; });
	}
}

void limit::add_market(const extended_symbol& token1, const extended_symbol& token2) { }

void limit::remove_market(const extended_symbol& token1, const extended_symbol& token2) { }

checksum256 limit::to_token_hash_key(const extended_symbol& token) {
	std::string str = token.get_symbol().code().to_string() + "@" + token.get_contract().to_string();
	return sha256(str.data(), str.size());
}
