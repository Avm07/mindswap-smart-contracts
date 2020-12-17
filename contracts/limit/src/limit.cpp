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

void limit::withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo) {
	require_auth(from);
	check(is_withdraw_account_exist(to, quantity.get_extended_symbol()), "withdraw: withdraw account is not exist");
	sub_balance(from, quantity);
	send_transfer(quantity.contract, to, quantity.quantity, memo);
}

void limit::create_limit_buy(const name& owner, const extended_asset& volume, const extended_asset& price) {
	require_auth(owner);

	extended_asset amount(price.quantity * volume.quantity.amount, price.contract);
	sub_balance(owner, amount);
	add_balance_in_orders(owner, amount, owner);

	auto market_id = add_market(volume.get_extended_symbol(), price.get_extended_symbol(), owner);
	check(market_id != 0, "add_market: cannot add market");
	auto ord_id = get_new_ord_id(market_id);

	buy_orders _buy_orders(get_self(), market_id);

	_buy_orders.emplace(owner, [&](auto& a) {
		a.id = ord_id;
		a.owner = owner;
		a.volume = volume.quantity;
		a.balance = volume.quantity;
		a.price = price.quantity;
		a.creation_date = current_time_point();
	});
}

void limit::create_limit_sell(const name& owner, const extended_asset& volume, const extended_asset& price) {
	require_auth(owner);

	sub_balance(owner, volume);
	add_balance_in_orders(owner, volume, owner);

	auto market_id = add_market(volume.get_extended_symbol(), price.get_extended_symbol(), owner);
	auto ord_id = get_new_ord_id(market_id);

	sell_orders _sell_orders(get_self(), market_id);

	_sell_orders.emplace(owner, [&](auto& a) {
		a.id = ord_id;
		a.owner = owner;
		a.volume = volume.quantity;
		a.balance = volume.quantity;
		a.price = price.quantity;
		a.creation_date = current_time_point();
	});
}

void limit::on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo) {
	if (to == get_self()) {
		extended_asset value(quantity, get_first_receiver());
		check(is_deposit_account_exist(from, value.get_extended_symbol()), "on_transfer: deposit account is not exist");
		add_balance(from, value, same_payer);
	}
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

uint64_t limit::add_market(const extended_symbol& token1, const extended_symbol& token2, const name& ram_payer) {
	markets _markets(get_self(), get_self().value);

	auto index = _markets.get_index<name("bypair")>();
	auto hash1 = to_pair_hash_key(token1, token2);
	auto hash2 = to_pair_hash_key(token2, token1);

	auto it1 = index.find(hash1);
	auto it2 = index.find(hash2);

	if (it1 == index.end() && it2 == index.end()) {
		auto id = _markets.available_primary_key();

		_markets.emplace(ram_payer, [&](auto& a) {
			a.id = id;
			a.token1 = token1;
			a.token2 = token2;
			a.available_ord_id = 1000;
		});

		return id;
	} else if (it1 != index.end()) {
		return it1->id;
	} else if (it2 != index.end()) {
		check(false, "add_market: wrong trade pair");
	}
	return 0;
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

uint64_t limit::get_new_ord_id(const uint64_t& market_id) {
	markets _markets(get_self(), get_self().value);
	const auto& obj = _markets.get(market_id, "get_new_ord_id: no market object found");
	uint64_t id = obj.available_ord_id;

	_markets.modify(obj, same_payer, [&](auto& a) {
		if (a.available_ord_id == std::numeric_limits<uint64_t>::max()) {
			a.available_ord_id = 1000;
		} else {
			a.available_ord_id++;
		}
	});

	return id;
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

bool limit::is_deposit_account_exist(const name& owner, const extended_symbol& token) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);
	return it != index.end() ? true : false;
}

bool limit::is_withdraw_account_exist(const name& owner, const extended_symbol& token) {
	accounts _accounts(token.get_contract(), owner.value);
	auto it = _accounts.find(token.get_symbol().code().raw());
	return it != _accounts.end() ? true : false;
}

void limit::send_transfer(const name &contract, const name &to, const asset &quantity, const std::string &memo)
{
    action(
        permission_level{get_self(), name("active")},
        contract,
        name("transfer"),
        std::make_tuple(get_self(), to, quantity, memo))
        .send();
}