#include "limit/limit.hpp"

limit::limit(name receiver, name code, datastream<const char*> ds)
	: contract::contract(receiver, code, ds) { }

void limit::open_account(const name& owner, const extended_symbol& token, const name& ram_payer) {
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
			a.contract = token.get_contract();
			a.balance = asset{0, token.get_symbol()};
			a.balance_in_orders = asset{0, token.get_symbol()};
		});
	}
}

void limit::close_account(const name& owner, const extended_symbol& token) {
	require_auth(owner);
	check(token.get_symbol().is_valid(), "token symbol is not valid");
	deposits _deposits(get_self(), owner.value);
	auto index = _deposits.get_index<name("bytoken")>();
	auto hash = to_token_hash_key(token);
	auto it = index.find(hash);
	check(it != index.end(), "Balance row already deleted or never existed. Action won't have any effect.");
	check(it->balance.amount == 0, "Cannot close because the balance is not zero.");
	check(it->balance_in_orders.amount == 0, "Cannot close because the balance_in_orders is not zero.");
	check(!is_open_orders_exist(owner, hash), "close_account: can not close because open orders exist");
	_deposits.erase(*it);
}

void limit::withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo) {
	require_auth(from);
	check(quantity.quantity.symbol.is_valid(), "withdraw: quantity symbol is not valid");
	check(quantity.quantity.amount > 0, "withdraw: quantity should be positive");
	check(is_withdraw_account_exist(to, quantity.get_extended_symbol()), "withdraw: withdraw account is not exist");
	sub_balance(from, quantity);
	send_transfer(quantity.contract, to, quantity.quantity, memo);
}

void limit::create_limit_buy(const name& owner, const extended_asset& volume, const extended_asset& price) {
	require_auth(owner);
	check(volume.quantity.symbol.is_valid(), "create_limit_buy: volume symbol is not valid");
	check(price.quantity.symbol.is_valid(), "create_limit_buy: price symbol is not valid");
	check(volume.quantity.amount > 0, "create_limit_buy: volume should be positive");
	check(price.quantity.amount > 0, "create_limit_buy: price should be positive");
	check(is_deposit_account_exist(owner, volume.get_extended_symbol()), to_string(volume.get_extended_symbol()) + " deposit is not opened");
	check(is_deposit_account_exist(owner, price.get_extended_symbol()), to_string(price.get_extended_symbol()) + " deposit is not opened");

	auto amount = count_amount(volume, price);
	sub_balance(owner, amount);
	add_balance_in_orders(owner, amount, owner);

	auto [status, market_id] = add_market(volume.get_extended_symbol(), price.get_extended_symbol(), owner);
	check(status != false, "add_market: wrong trade pair");
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

	sell_orders _sell_orders(get_self(), market_id);
	manage_lmt_orders(_buy_orders, _sell_orders);
}

void limit::create_limit_sell(const name& owner, const extended_asset& volume, const extended_asset& price) {
	require_auth(owner);
	check(volume.quantity.symbol.is_valid(), "create_limit_sell: volume symbol is not valid");
	check(price.quantity.symbol.is_valid(), "create_limit_sell: price symbol is not valid");
	check(volume.quantity.amount > 0, "create_limit_sell: volume should be positive");
	check(price.quantity.amount > 0, "create_limit_sell: price should be positive");
	check(is_deposit_account_exist(owner, volume.get_extended_symbol()), to_string(volume.get_extended_symbol()) + " deposit is not opened");
	check(is_deposit_account_exist(owner, price.get_extended_symbol()), to_string(price.get_extended_symbol()) + " deposit is not opened");

	sub_balance(owner, volume);
	add_balance_in_orders(owner, volume, owner);

	auto [status, market_id] = add_market(volume.get_extended_symbol(), price.get_extended_symbol(), owner);
	check(status != false, "add_market: wrong trade pair");
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

	buy_orders _buy_orders(get_self(), market_id);
	manage_lmt_orders(_buy_orders, _sell_orders);
}

void limit::close_limit_buy(const uint64_t& market_id, const uint64_t& id) {
	auto [token1, token2] = is_market_exist(market_id);
	check(token1 != extended_symbol() && token2 != extended_symbol(), "close_limit_buy: market is not exist");

	buy_orders _buy_orders(get_self(), market_id);
	auto it = _buy_orders.find(id);
	check(it != _buy_orders.end(), "close_limit_buy: order is not exist");

	require_auth(it->owner);

	auto amount = count_amount({it->balance, token1.get_contract()}, {it->price, token2.get_contract()});
	sub_balance_in_orders(it->owner, amount);
	add_balance(it->owner, amount, same_payer);

	_buy_orders.erase(it);
}

void limit::close_limit_sell(const uint64_t& market_id, const uint64_t& id) {
	auto [token1, token2] = is_market_exist(market_id);
	check(token1 != extended_symbol() && token2 != extended_symbol(), "close_limit_sell: market is not exist");

	sell_orders _sell_orders(get_self(), market_id);
	auto it = _sell_orders.find(id);
	check(it != _sell_orders.end(), "close_limit_sell: order is not exist");

	require_auth(it->owner);

	extended_asset amount(it->balance, token2.get_contract());

	sub_balance_in_orders(it->owner, amount);
	add_balance(it->owner, amount, same_payer);

	_sell_orders.erase(it);
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

std::pair<bool, uint64_t> limit::add_market(const extended_symbol& token1, const extended_symbol& token2, const name& ram_payer) {
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

		return std::make_pair(true, id);
	} else if (it1 != index.end()) {
		return std::make_pair(true, it1->id);
	}

	return std::make_pair(false, 0);
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

template <typename B, typename S>
void limit::manage_lmt_orders(B& lmt_buy, S& lmt_sell) {
	if (!is_limit_deal_exist<B, S>(lmt_buy, lmt_sell)) {
		return;
	}

	auto& buy_order = lmt_buy.get(find_first_buy<B>(lmt_buy));
	auto& sell_order = lmt_sell.get(find_first_sell<S>(lmt_sell));

	const auto market = get_market(lmt_buy.get_scope());

	auto [price, date] = get_deal_price_n_date(buy_order, sell_order);
	extended_asset deal_price(price, market.token2.get_contract());
	extended_asset deal_vol(buy_order.balance, market.token1.get_contract());
	extended_asset amount = count_amount(deal_vol, deal_price);

	if (buy_order.balance == sell_order.balance) {
		exec_lmt_buy(amount, deal_vol, buy_order.owner);
		exec_lmt_sell(amount, deal_vol, sell_order.owner);

		lmt_sell.erase(sell_order);
		lmt_buy.erase(buy_order);
	} else if (buy_order.balance < sell_order.balance) {
		exec_lmt_buy(amount, deal_vol, buy_order.owner);
		exec_lmt_sell(amount, deal_vol, sell_order.owner);

		lmt_sell.modify(sell_order, get_self(), [&](auto& r) { r.balance -= buy_order.balance; });

		lmt_buy.erase(buy_order);
		manage_lmt_orders<B, S>(lmt_buy, lmt_sell);
	} else if (buy_order.balance > sell_order.balance) {
		deal_vol = extended_asset(sell_order.balance, market.token1.get_contract());
		amount = count_amount(deal_vol, deal_price);

		exec_lmt_buy(amount, deal_vol, buy_order.owner);
		exec_lmt_sell(amount, deal_vol, sell_order.owner);

		lmt_buy.modify(buy_order, get_self(), [&](auto& r) { r.balance -= sell_order.balance; });

		lmt_sell.erase(sell_order);
		manage_lmt_orders<B, S>(lmt_buy, lmt_sell);
	}
}

template <typename T>
uint64_t limit::find_first_buy(T& lmt_buy) {
	auto indexbuy = lmt_buy.template get_index<name("byprice")>();
	auto itb = indexbuy.rbegin();

	for (auto itr = ++indexbuy.rbegin(); itr != indexbuy.rend(); ++itr) {
		if (itr->price != itb->price) {
			break;
		} else if (itr->price == itb->price && itr->id < itb->id) {
			itb = itr;
		}
	}
	return itb->id;
}

template <typename T>
uint64_t limit::find_first_sell(T& lmt_sell) {
	auto indexsell = lmt_sell.template get_index<name("byprice")>();
	auto its = indexsell.begin();

	for (auto itr = ++indexsell.begin(); itr != indexsell.end(); ++itr) {
		if (itr->price != its->price) {
			break;
		} else if (itr->price == its->price && itr->id < its->id) {
			its = itr;
		}
	}
	return its->id;
}

void limit::exec_lmt_buy(const extended_asset& amount, const extended_asset& deal_volume, const name& owner) {
	sub_balance_in_orders(owner, amount);
	add_balance(owner, deal_volume, owner);
}

void limit::exec_lmt_sell(const extended_asset& amount, const extended_asset& deal_volume, const name& owner) {
	sub_balance_in_orders(owner, deal_volume);
	add_balance(owner, amount, owner);
}

price_n_date limit::get_deal_price_n_date(const order& buy_order, const order& sell_order) {
	return (sell_order.id > buy_order.id) ? std::make_pair(buy_order.price, sell_order.creation_date)
										  : std::make_pair(sell_order.price, buy_order.creation_date);
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

market limit::get_market(const uint64_t& market_id) {
	markets _markets(get_self(), get_self().value);
	const auto obj = _markets.get(market_id, "get_market: no market object found");
	return obj;
}

extended_asset limit::count_amount(const extended_asset& volume, const extended_asset& price) {
	asset value(price.quantity * volume.quantity.amount / std::pow(10, volume.quantity.symbol.precision()));
	return extended_asset(value, price.contract);
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

trade_pair limit::is_market_exist(const uint64_t& market_id) {
	markets _markets(get_self(), get_self().value);
	auto it = _markets.find(market_id);
	return it != _markets.end() ? std::make_pair(it->token1, it->token2) : std::make_pair(extended_symbol(), extended_symbol());
}

template <typename B, typename S>
bool limit::is_limit_deal_exist(B& lmt_buy, S& lmt_sell) {
	return (is_orders_exist<B>(lmt_buy) && is_orders_exist<S>(lmt_sell) && is_prices_match<B, S>(lmt_buy, lmt_sell)) ? true : false;
}

template <typename T>
bool limit::is_orders_exist(T& lmt) {
	return lmt.begin() != lmt.end() ? true : false;
}

template <typename B, typename S>
bool limit::is_prices_match(B& buy, S& sell) {
	auto indexbuy = buy.template get_index<name("byprice")>();
	auto indexsell = sell.template get_index<name("byprice")>();
	return indexbuy.rbegin()->price >= indexsell.begin()->price ? true : false;
}

bool limit::is_open_orders_exist(const name& owner, const checksum256& token_hash) {
	markets _markets(get_self(), get_self().value);

	auto bytoken1_index = _markets.get_index<name("bytoken1")>();

	for (auto itr1 = bytoken1_index.find(token_hash); itr1 != bytoken1_index.end(); itr1++) {
		if (itr1->token1_hash_key() != token_hash)
			break;

		if (is_open_buy_orders_exist(owner, itr1->primary_key()))
			return true;
	}

	auto bytoken2_index = _markets.get_index<name("bytoken2")>();

	for (auto itr2 = bytoken2_index.find(token_hash); itr2 != bytoken2_index.end(); itr2++) {
		if (itr2->token2_hash_key() != token_hash)
			break;

		if (is_open_sell_orders_exist(owner, itr2->primary_key()))
			return true;
	}

	return false;
}

bool limit::is_open_buy_orders_exist(const name& owner, const uint64_t& market_id) {
	buy_orders _buy_orders(get_self(), market_id);
	auto index = _buy_orders.get_index<name("byowner")>();
	auto it = index.find(owner.value);
	return it != index.end() ? true : false;
}

bool limit::is_open_sell_orders_exist(const name& owner, const uint64_t& market_id) {
	sell_orders _sell_orders(get_self(), market_id);
	auto index = _sell_orders.get_index<name("byowner")>();
	auto it = index.find(owner.value);
	return it != index.end() ? true : false;
}

void limit::send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo) {
	action(permission_level{get_self(), name("active")}, contract, name("transfer"), std::make_tuple(get_self(), to, quantity, memo))
		.send();
}