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

void arbitrage::arbitrage_order_trade(const uint64_t& market_id,
									  const name& order_type,
									  const uint64_t& order_id,
									  const asset& amount,
									  const symbol_code& mindswap_pool) {
	check(is_valid_order_type(order_type), "arbitrage_order_trade: invalid order type");
	check(is_valid_market_id(market_id), "arbitrage_order_trade: invalid market id");
	check(is_valid_order_id(order_type, market_id, order_id), "arbitrage_order_trade: invalid order id");
	check(is_valid_amount(order_type, market_id, order_id, amount), "arbitrage_order_trade: invalid amount");
	check(is_pool_exist(mindswap_pool), "arbitrage_order_trade: mindswap pool is not exist");

	auto [amount_from, amount_to, memo] = count_swap_amounts(mindswap_pool, order_type, market_id, order_id, amount);
	auto arbitrage_balance_from_before = get_balance(get_self(), amount_from.get_extended_symbol());
	auto arbitrage_balance_to_before = get_balance(get_self(), amount_to.get_extended_symbol());
	auto limit_balance_before = get_balance(LIMIT_ACCOUNT, amount_to.get_extended_symbol());

	//Send request to mindswap and validate income amount
	send_transfer(amount_from.contract, MINDSWAP_ACCOUNT, amount_from.quantity, memo);
	send_validate(name("swap"), get_self(), arbitrage_balance_to_before + amount_to);

	//Fill order and validate limit balance
	if(order_type == BUY_TYPE)
	{
		send_part_fill_buy_order(market_id, order_id, amount);
	}
	else {
		send_part_fill_sell_order(market_id, order_id, amount);
	}
	
	send_transfer(amount_to.contract, LIMIT_ACCOUNT, amount_to.quantity, "");
	send_validate(name("trade"), LIMIT_ACCOUNT, limit_balance_before + amount_to);

	//Final validation of arbitrage started balance
	send_validate(name("final"), get_self(), arbitrage_balance_from_before);
}

void arbitrage::arbitrage_pair_trade(const uint64_t& market_id,
									 const name& orders_type,
									 const std::vector<uint64_t>& orders_ids,
									 const symbol_code& mindswap_pool) {
	check(is_valid_order_type(orders_type), "arbitrage_pair_trade: invalid orders type");
	check(is_valid_market_id(market_id), "arbitrage_pair_trade: invalid market id");
	check(is_valid_orders_ids(orders_type, market_id, orders_ids), "arbitrage_order_trade: invalid orders ids");
	check(is_pool_exist(mindswap_pool), "arbitrage_pair_trade: mindswap pool is not exist");

	for (const auto& id : orders_ids) {
		//request swap to MIND_SWAP
		auto [amount_from, amount_to, memo] = count_swap_amounts(mindswap_pool, orders_type, market_id, id);
		auto arbitrage_balance_from_before = get_balance(get_self(), amount_from.get_extended_symbol());
		auto arbitrage_balance_to_before = get_balance(get_self(), amount_to.get_extended_symbol());
		auto limit_balance_before = get_balance(LIMIT_ACCOUNT, amount_to.get_extended_symbol());

		//Send request to mindswap and validate income amount
		send_transfer(amount_from.contract, MINDSWAP_ACCOUNT, amount_from.quantity, memo);
		send_validate(name("swap"), get_self(), arbitrage_balance_to_before + amount_to);

		//Fill order and validate limit balance
		if(orders_type == BUY_TYPE)
		{
			send_fill_buy_order(market_id, id);
		}
		else {
			send_fill_sell_order(market_id, id);
		}
		
		send_transfer(amount_to.contract, LIMIT_ACCOUNT, amount_to.quantity, "");
		send_validate(name("trade"), LIMIT_ACCOUNT, limit_balance_before + amount_to);

		//Final validation of arbitrage started balance
		send_validate(name("final"), get_self(), arbitrage_balance_from_before);
	}
}

void arbitrage::validate(const name& type, const name& account, const extended_asset& expected_balance) {
	require_auth(get_self());
	check(is_account(account), "validate: account is not exist");
	check(expected_balance.quantity.is_valid(), "validate: expected_balance is not valid");
	check(expected_balance.quantity.amount > 0, "validate: expected_balance should be positive");
	auto current_balance = get_balance(account, expected_balance.get_extended_symbol());
	check(current_balance >= expected_balance, "validate: " + type.to_string() + " validation failed for " + account.to_string() + " " + current_balance.quantity.to_string() + "!>=" + expected_balance.quantity.to_string());
}
									
void arbitrage::on_transfer(const name& from, const name& to, const asset& quantity, const std::string& memo) {
	if (to == get_self()) {
		if (from == MINDSWAP_ACCOUNT || from == LIMIT_ACCOUNT) {
			return;
		} else {
			extended_asset value(quantity, get_first_receiver());
			check(is_deposit_account_exist(from, value.get_extended_symbol()), "on_transfer: deposit account is not exist");
			add_balance(from, value, same_payer);
		}
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

std::string arbitrage::create_request_memo(const symbol_code& mindswap_pool, const asset& amount_to) {
	auto req_amount = asset(0, amount_to.symbol);
	std::string str = "exchange: " + mindswap_pool.to_string() + "," + req_amount.to_string();
	return str;
}

std::tuple<extended_asset, extended_asset, std::string>
arbitrage::count_swap_amounts(const symbol_code& mindswap_pool, const name& order_type, const uint64_t& market_id, const uint64_t& id) {
	if (order_type == BUY_TYPE) {
		buy_orders _buy_orders(LIMIT_ACCOUNT, market_id);
		const auto& order = _buy_orders.get(id, "count_swap_amounts: order not found");
		check(is_valid_pool(mindswap_pool, order.balance.symbol.code(), order.price.symbol.code()), "count_swap_amounts: pool is not valid");
		auto market = get_market(market_id);
		auto amount_to = extended_asset(order.balance, market.token1.get_contract());
		auto amount_from = count_amount(amount_to, extended_asset(order.price, market.token2.get_contract()));
		auto memo = create_request_memo(mindswap_pool, order.balance);
		return std::make_tuple(amount_from, amount_to, memo);
	} else {
		sell_orders _sell_orders(LIMIT_ACCOUNT, market_id);
		const auto& order = _sell_orders.get(id, "count_swap_amounts: order not found");
		check(is_valid_pool(mindswap_pool, order.price.symbol.code(), order.balance.symbol.code()), "count_swap_amounts: pool is not valid");
		auto market = get_market(market_id);
		auto amount_from = extended_asset(order.balance, market.token1.get_contract());
		auto amount_to = count_amount(amount_from, extended_asset(order.price, market.token2.get_contract()));
		auto memo = create_request_memo(mindswap_pool, amount_to.quantity);
		return std::make_tuple(amount_from, amount_to, memo);
	}
}

std::tuple<extended_asset, extended_asset, std::string>
arbitrage::count_swap_amounts(const symbol_code& mindswap_pool, const name& order_type, const uint64_t& market_id, const uint64_t& id, const asset& amount) {
	if (order_type == BUY_TYPE) {
		buy_orders _buy_orders(LIMIT_ACCOUNT, market_id);
		const auto& order = _buy_orders.get(id, "count_swap_amounts: order not found");
		check(is_valid_pool(mindswap_pool, order.balance.symbol.code(), order.price.symbol.code()), "count_swap_amounts: pool is not valid");
		auto market = get_market(market_id);
		auto amount_to = extended_asset(amount, market.token1.get_contract());
		auto amount_from = count_amount(amount_to, extended_asset(order.price, market.token2.get_contract()));
		auto memo = create_request_memo(mindswap_pool, amount);
		return std::make_tuple(amount_from, amount_to, memo);
	} else {
		sell_orders _sell_orders(LIMIT_ACCOUNT, market_id);
		const auto& order = _sell_orders.get(id, "count_swap_amounts: order not found");
		check(is_valid_pool(mindswap_pool, order.price.symbol.code(), order.balance.symbol.code()), "count_swap_amounts: pool is not valid");
		auto market = get_market(market_id);
		auto amount_from = extended_asset(amount, market.token1.get_contract());
		auto amount_to = count_amount(amount_from, extended_asset(order.price, market.token2.get_contract()));
		auto memo = create_request_memo(mindswap_pool, amount_to.quantity);
		return std::make_tuple(amount_from, amount_to, memo);
	}
}

extended_asset arbitrage::count_amount(const extended_asset& volume, const extended_asset& price) {
	asset value(price.quantity * volume.quantity.amount / std::pow(10, volume.quantity.symbol.precision()));
	return extended_asset(value, price.contract);
}

extended_asset arbitrage::get_balance(const name& owner, const extended_symbol& token) {
	accounts _accounts(token.get_contract(), owner.value);
	const auto& obj = _accounts.get(token.get_symbol().code().raw(), "no balance object found");
	return extended_asset(obj.balance.amount, token);
}

market arbitrage::get_market(const uint64_t& market_id) {
	markets _markets(LIMIT_ACCOUNT, LIMIT_ACCOUNT.value);
	const auto obj = _markets.get(market_id, "get_market: no market object found");
	return obj;
}

std::string arbitrage::to_pool_name(const symbol_code& symb1, const symbol_code& symb2) {
	auto str = symb1.to_string() + symb2.to_string();
	return str;
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

bool arbitrage::is_valid_order_type(const name& type) {
	return type == BUY_TYPE || type == SELL_TYPE ? true : false;
}

bool arbitrage::is_valid_market_id(const uint64_t& id) {
	markets _markets(LIMIT_ACCOUNT, LIMIT_ACCOUNT.value);
	auto it = _markets.find(id);
	return it != _markets.end() ? true : false;
}

bool arbitrage::is_valid_order_id(const name& order_type, const uint64_t& market_id, const uint64_t& id) {
	if (order_type == BUY_TYPE) {
		buy_orders _buy_orders(LIMIT_ACCOUNT, market_id);
		auto itb = _buy_orders.find(id);
		return itb != _buy_orders.end() ? true : false;
	} else {
		sell_orders _sell_orders(LIMIT_ACCOUNT, market_id);
		auto its = _sell_orders.find(id);
		return its != _sell_orders.end() ? true : false;
	}
}

bool arbitrage::is_valid_amount(const name& order_type, const uint64_t& market_id, const uint64_t& id, const asset& amount) {
	if (order_type == BUY_TYPE) {
		buy_orders _buy_orders(LIMIT_ACCOUNT, market_id);
		const auto& ord = _buy_orders.get(id, "is_valid_amount: order not found");
		return (amount.is_valid() && amount.amount > 0 && ord.balance.symbol == amount.symbol && ord.balance >= amount) ? true : false;
	} else {
		sell_orders _sell_orders(LIMIT_ACCOUNT, market_id);
		const auto& ord = _sell_orders.get(id, "is_valid_amount: order not found");
		return (amount.is_valid() && amount.amount > 0 && ord.balance.symbol == amount.symbol && ord.balance >= amount) ? true : false;
	}
}

bool arbitrage::is_valid_orders_ids(const name& order_type, const uint64_t& market_id, const std::vector<uint64_t>& ids) {
	for (const auto& id : ids) {
		if (!is_valid_order_id(order_type, market_id, id)) {
			return false;
		}
	}
	return true;
}

bool arbitrage::is_pool_exist(const symbol_code& mindswap_pool) {
	mindswap_stats _mindswap_stats(MINDSWAP_ACCOUNT, mindswap_pool.raw());
	auto it = _mindswap_stats.find(mindswap_pool.raw());
	return it != _mindswap_stats.end() ? true : false;
}

bool arbitrage::is_valid_pool(const symbol_code& mindswap_pool, const symbol_code& symb1, const symbol_code& symb2) {
	mindswap_stats _mindswap_stats(MINDSWAP_ACCOUNT, mindswap_pool.raw());
	const auto& obj = _mindswap_stats.get(mindswap_pool.raw());
	auto pool = obj.supply.symbol.code().to_string();
	return (pool == to_pool_name(symb1, symb2) || pool == to_pool_name(symb2, symb1)) ? true : false;
}

void arbitrage::send_part_fill_buy_order(const uint64_t& market_id, const uint64_t& id, const asset& amount) {
	action(permission_level{get_self(), name("active")}, LIMIT_ACCOUNT, name("ptfillbuyord"), std::make_tuple(market_id, id, amount))
		.send();
}

void arbitrage::send_part_fill_sell_order(const uint64_t& market_id, const uint64_t& id, const asset& amount) {
	action(permission_level{get_self(), name("active")}, LIMIT_ACCOUNT, name("ptfillsellord"), std::make_tuple(market_id, id, amount))
		.send();
}

void arbitrage::send_fill_buy_order(const uint64_t& market_id, const uint64_t& id) {
	action(permission_level{get_self(), name("active")}, LIMIT_ACCOUNT, name("fillbuyord"), std::make_tuple(market_id, id))
		.send();
}

void arbitrage::send_fill_sell_order(const uint64_t& market_id, const uint64_t& id) {
	action(permission_level{get_self(), name("active")}, LIMIT_ACCOUNT, name("fillsellord"), std::make_tuple(market_id, id))
		.send();
}

void arbitrage::send_transfer(const name& contract, const name& to, const asset& quantity, const std::string& memo) {
	action(permission_level{get_self(), name("active")}, contract, name("transfer"), std::make_tuple(get_self(), to, quantity, memo))
		.send();
}

void arbitrage::send_validate(const name& type, const name& account, const extended_asset& expected_balance) {
	action(permission_level{get_self(), name("active")}, get_self(), name("validate"), std::make_tuple(type, account, expected_balance))
		.send();
}