limit
--------

Actions:
The naming convention is account::action followed by a list of paramters.

Open deposit balance
## limit::open owner token ram_payer
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
      "account": "mindswaplimt",
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
## limit::close owner token
- **owner** owner of deposit and authority provider
- **token** token symbol with precision and contract name

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
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
## limit::withdraw from to quantity memo
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
      "account": "mindswaplimt",
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

Create limit buy order
## limit::crtlmtbuy owner volume price
- **owner** order owner and authority provider
- **volume** volume of order
- **price** price of order

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "crtlmtbuy",
      "data": {
        "owner": "usera",
        "volume": {
            "contract": "eosio.token",
            "asset": "0.0100 EOS"
        },
        "price": {
            "contract": "everipediaiq",
            "asset": "0.0010 IQ"
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

Create limit sell order
## limit::crtlmtsell owner volume price
- **owner** order owner and authority provider
- **volume** volume of order
- **price** price of order

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "crtlmtsell",
      "data": {
        "owner": "usera",
        "volume": {
            "contract": "eosio.token",
            "asset": "0.0100 EOS"
        },
        "price": {
            "contract": "everipediaiq",
            "asset": "0.0010 IQ"
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

Close limit buy order
## limit::clslmtbuy market_id id
- **market_id** market id of trade pair
- **id** order id

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "clslmtbuy",
      "data": {
        "market_id": "0",
        "id": "1000"
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

Close limit sell order
## limit::clslmtsell market_id id
- **market_id** market id of trade pair
- **id** order id

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "clslmtsell",
      "data": {
        "market_id": "0",
        "id": "1000"
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

Part fill buy order
## limit::ptfillbuyord market_id order_id amount
- **market_id** market id of trade pair
- **order_id** order id
- **amount** amount to fill

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "ptfillbuyord",
      "data": {
        "market_id": "0",
        "order_id": "1000",
        "amount": "100.0000 EOS"
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

Part fill sell order
## limit::ptfillsellord market_id order_id amount
- **market_id** market id of trade pair
- **order_id** order id
- **amount** amount to fill

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "ptfillsellord",
      "data": {
        "market_id": "0",
        "order_id": "1000",
        "amount": "100.0000 EOS"
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

Full fill buy order
## limit::fillbuyord market_id order_id
- **market_id** market id of trade pair
- **order_id** order id

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "fillbuyord",
      "data": {
        "market_id": "0",
        "order_id": "1000"
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

Full fill sell order
## limit::fillsellord market_id order_id
- **market_id** market id of trade pair
- **order_id** order id

```bash
cleos -u https://eos.greymass.com/ push transaction
{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "mindswaplimt",
      "name": "fillsellord",
      "data": {
        "market_id": "0",
        "order_id": "1000"
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



    