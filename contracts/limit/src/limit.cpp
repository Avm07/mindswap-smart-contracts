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

	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);

	if (it == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) {
			a.id = _deposits.available_primary_key();
			a.contract = token.get_contract();
			a.balance = asset{0, token.get_symbol()};
			a.balance_in_orders = asset{0, token.get_symbol()};
		});
	}
}

void limit::close_account(const name& owner, const extended_symbol& token) {
	require_auth(owner);
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);
	check(it != index.end(), "Balance row already deleted or never existed. Action won't have any effect.");
	check(it->balance.amount == 0, "Cannot close because the balance is not zero.");
	check(it->balance_in_orders.amount == 0, "Cannot close because the balance_in_orders is not zero.");
	_deposits.erase(*it);
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
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	const auto& from = index.get(hash, "no balance object found");
	check(from.balance.amount >= value.quantity.amount, "overdrawn balance");

	_deposits.modify(from, owner, [&](auto& a) { a.balance -= value.quantity; });
}

void limit::add_balance(const name& owner, const extended_asset& value, const name& ram_payer) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	auto it = index.find(hash);
	if (it == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) {
			a.id = _deposits.available_primary_key();
			a.contract = value.contract;
			a.balance = value.quantity;
			a.balance_in_orders = asset{0, value.quantity.symbol};
		});
	} else {
		_deposits.modify(*it, same_payer, [&](auto& a) { a.balance += value.quantity; });
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

	auto it = index.find(hash);
	if (it == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) {
			a.id = _deposits.available_primary_key();
			a.contract = value.contract;
			a.balance = asset{0, value.quantity.symbol};
			a.balance_in_orders = value.quantity;
		});
	} else {
		_deposits.modify(*it, same_payer, [&](auto& a) { a.balance_in_orders += value.quantity; });
	}
}

void limit::add_market(const extended_symbol& token1, const extended_symbol& token2, const name& ram_payer) {
	markets _markets(get_self(), get_self().value);

	auto index = _markets.get_index<name("bypair")>();
    auto hash1 = to_pair_hash_key(token1, token2);
    auto hash2 = to_pair_hash_key(token2, token1);

	auto it1 = index.find(hash1);
    auto it2 = index.find(hash2);

	if (it1 == index.end() && it2 == index.end()) {
		_markets.emplace(ram_payer, [&](auto& a) {
			a.id = _markets.available_primary_key();
			a.token1 = token1;
			a.token2 = token2;
			a.availiable_ord_id = 1000;
		});
	}
    else if (it1 != index.end()) {
        return;
    }
    else if(it2 != index.end()) {
        check(false, "add_market: wrong trade pair");
    }
}

void limit::remove_market(const extended_symbol& token1, const extended_symbol& token2) {
	markets _markets(get_self(), get_self().value);
	auto index = _markets.get_index<name("bypair")>();
    auto hash = to_pair_hash_key(token1, token2);
	auto it = index.find(hash);

	if (it != index.end()) {
		_markets.erase(*it);
	}
}

std::string limit::to_string(const extended_symbol& token) {
	std::string str = token.get_symbol().code().to_string() + "@" + token.get_contract().to_string();
	return str;
}

checksum256 limit::to_token_hash_key(const extended_symbol& token) {
	std::string str = to_string(token);
	return sha256(str.data(), str.size());
}

checksum256 limit::to_pair_hash_key(const extended_symbol& token1, const extended_symbol& token2) {
	std::string str = to_string(token1) + "/" + to_string(token2);
	return sha256(str.data(), str.size());
}
