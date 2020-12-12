#include "limit/limit.hpp"

limit::limit(name receiver, name code, datastream<const char *> ds)
    : contract::contract(receiver, code, ds)
{
}