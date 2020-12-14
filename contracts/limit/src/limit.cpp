#include "limit/limit.hpp"

limit::limit(name receiver, name code, datastream<const char *> ds)
    : contract::contract(receiver, code, ds)
{
}

void limit::add_market(const uint64_t& id, const extended_symbol& token1, const extended_symbol& token2)
{

}

void limit::remove_market(const uint64_t& id, const extended_symbol& token1, const extended_symbol& token2)
{

}

void limit::open_account(const name& owner, const extended_symbol& token, const name& ram_payer)
{

}

void limit::close_account(const name& owner, const extended_symbol& token)
{

}

void limit::deposit(const name& to, const extended_asset& quantity, const std::string& memo)
{

}

void limit::withdraw(const name& from, const name& to, const extended_asset& quantity, const std::string& memo)
{

}

void limit::create_limit_buy(const name& owner, const uint64_t market_id, const asset& volume, const asset& price)
{

}

void limit::create_limit_sell(const name& owner, const uint64_t market_id, const asset& volume, const asset& price)
{

}

void limit::sub_balance(const name& owner, const extended_asset& value)
{

}

void limit::add_balance(const name& owner, const extended_asset& value, const name& ram_payer)
{

}

void limit::sub_balance_in_orders(const name& owner, const extended_asset& value)
{

}

void limit::add_balance_in_orders(const name& owner, const extended_asset& value, const name& ram_payer)
{

}