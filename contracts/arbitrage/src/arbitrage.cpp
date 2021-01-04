#include <arbitrage/arbitrage.hpp>

arbitrage::arbitrage(name receiver, name code, datastream<const char*> ds)
	: contract::contract(receiver, code, ds) { }

void arbitrage::open_account(const name& owner, const extended_symbol& token, const name& ram_payer) {
	require_auth(ram_payer);
	check(is_account(owner), "owner account does not exist");
	check(token.get_symbol().is_valid(), "token symbol is not valid");

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
			a.balance = extended_asset{0, token};
		});
	}
}

void arbitrage::close_account(const name& owner, const extended_symbol& token) {
	require_auth(owner);
	check(token.get_symbol().is_valid(), "token symbol is not valid");
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);
	check(it != index.end(), "Balance row already deleted or never existed. Action won't have any effect.");
	check(it->balance.quantity.amount == 0, "Cannot close because the balance is not zero.");
	_deposits.erase(*it);
}

void arbitrage::withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo) {
	require_auth(from);
	check(quantity.quantity.symbol.is_valid(), "withdraw: quantity symbol is not valid");
	check(quantity.quantity.amount > 0, "withdraw: quantity should be positive");
	check(is_withdraw_account_exist(to, quantity.get_extended_symbol()), "withdraw: withdraw account is not exist");
	sub_balance(from, quantity);
	send_transfer(quantity.contract, to, quantity.quantity, memo);
}

void arbitrage::arbitrage_action(const extended_asset& token1, const extended_asset& token2, const name& owner) {

}

void arbitrage::on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo) {
	if (to == get_self()) {
		extended_asset value(quantity, get_first_receiver());
		check(is_deposit_account_exist(from, value.get_extended_symbol()), "on_transfer: deposit account is not exist");
		add_balance(from, value, same_payer);
	}
}

void arbitrage::sub_balance(const name& owner, const extended_asset& value) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	const auto& from = index.get(hash, "no balance object found");
	check(from.balance >= value, "overdrawn balance");

	_deposits.modify(from, owner, [&](auto& a) { a.balance -= value; });
}

void arbitrage::add_balance(const name& owner, const extended_asset& value, const name& ram_payer) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(value.get_extended_symbol());

	auto it = index.find(hash);
	if (it == index.end()) {
		_deposits.emplace(ram_payer, [&](auto& a) {
			a.id = _deposits.available_primary_key();
			a.balance = value;
		});
	} else {
		_deposits.modify(*it, same_payer, [&](auto& a) { a.balance += value; });
	}
}

std::string arbitrage::to_string(const extended_symbol& token) {
	std::string str = token.get_symbol().code().to_string() + "@" + token.get_contract().to_string();
	return str;
}

checksum256 arbitrage::to_token_hash_key(const extended_symbol& token) {
	std::string str = to_string(token);
	return sha256(str.data(), str.size());
}

bool arbitrage::is_deposit_account_exist(const name& owner, const extended_symbol& token) {
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);
	return it != index.end() ? true : false;
}

bool arbitrage::is_withdraw_account_exist(const name& owner, const extended_symbol& token) {
	accounts _accounts(token.get_contract(), owner.value);
	auto it = _accounts.find(token.get_symbol().code().raw());
	return it != _accounts.end() ? true : false;
}

void arbitrage::send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo) {
	action(permission_level{get_self(), name("active")}, contract, name("transfer"), std::make_tuple(get_self(), to, quantity, memo))
		.send();
}