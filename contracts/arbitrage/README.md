arbitrage
--------

Actions:
The naming convention is account::action followed by a list of paramters.

Open deposit balance
## arbitrage::open owner token ram_payer
- **owner** owner of deposit
- **token** token symbol with precision and contract name
- **ram_payer** ram payer and authority provider

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "open",
      "data": {
        "owner": "usera",
        "token": {
            "contract": "everipediaiq",
            "sym": "3,IQ"
        },
        "ram_payer": "userb"
      },
      "authorization": [
        {
          "actor": "usera",
          "permission": "active"
        }
      ]
    }
  ]
}
```

Close deposit balance
## arbitrage::close owner token
- **owner** owner of deposit and authority provider
- **token** token symbol with precision and contract name

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "close",
      "data": {
        "owner": "usera",
        "token": {
            "contract": "everipediaiq",
            "sym": "3,IQ"
        }
      },
      "authorization": [
        {
          "actor": "usera",
          "permission": "active"
        }
      ]
    }
  ]
}
```

Withdraw founds, transfer from platform
## arbitrage::withdraw from to quantity memo
- **from** owner of deposit and authority provider
- **to** recipient 
- **quantity** amount of tokens with contract name
- **memo** memo for transfer

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "withdraw",
      "data": {
        "from": "usera",
        "to": "userb",
        "quantity": {
            "contract": "eosio.token",
            "asset": "0.0100 EOS"
        },
        "memo": ""
      },
      "authorization": [
        {
          "actor": "usera",
          "permission": "active"
        }
      ]
    }
  ]
}
```

Execute arbitrage order trade
## arbitrage::arbordtrade account market_id order_type order_id amount mindswap_pool
- **account** earnings receiver and authority provider
- **market_id** market id on limit contract
- **order_type** type of order(buy/sell)
- **order_id** order id
- **amount** amount of tokens to fill
- **mindswap_pool** mindswap pool code

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "arbordtrade",
      "data": {
        "account": "usera",
        "market_id": "0",
        "order_type": "buy",
        "order_id": "1000",
        "amount": "1.0000 EOS",
        "mindswap_pool": "EOSIQ"
      },
      "authorization": [
        {
          "actor": "usera",
          "permission": "active"
        }
      ]
    }
  ]
}
```

Execute arbitrage pair trade
## arbitrage::arbpairtrade account market_id orders_type orders_ids mindswap_pool
- **account** earnings receiver and authority provider
- **market_id** market id on limit contract
- **orders_type** type of orders(buy/sell)
- **orders_ids** array of orders ids
- **mindswap_pool** mindswap pool code

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "arbpairtrade",
      "data": {
        "account": "usera",
        "market_id": "0",
        "orders_type": "buy",
        "orders_ids": "1000",
        "mindswap_pool": "EOSIQ"
      },
      "authorization": [
        {
          "actor": "usera",
          "permission": "active"
        }
      ]
    }
  ]
}
```

Execute arbitrage validate
## arbitrage::validate type account expected_balance recipient
- **type** operation of current validation
- **account** name of validated account
- **expected_balance** expected balance for account
- **recipient** earnings recepient

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaparbi",
      "name": "validate",
      "data": {
        "type": "swap",
        "account": "mindswaparbi",
        "expected_balance": {
            "contract": "eosio.token",
            "asset": "0.0100 EOS"
        }
        "recipient": "usera"
      },
      "authorization": [
        {
          "actor": "mindswaparbi",
          "permission": "active"
        }
      ]
    }
  ]
}
```



    