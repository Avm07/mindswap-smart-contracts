limit
--------

Actions:
The naming convention is account::action followed by a list of paramters.

Open deposit balance
## limit::open owner token ram_payer
- **owner** owner of deposit
- **token** token symbol with precision and contract name
- **ram_payer** ram payer and authority provider

Close deposit balance
## limit::close owner token
- **owner** owner of deposit and authority provider
- **token** token symbol with precision and contract name

Withdraw founds, transfer from platform
## limit::withdraw from to quantity memo
- **from** owner of deposit and authority provider
- **to** recipient 
- **quantity** amount of tokens with contract name
- **memo** memo for transfer

Create limit buy order
## limit::crtlmtbuy owner volume price
- **owner** order owner and authority provider
- **volume** volume of order
- **price** price of order

Create limit sell order
## limit::crtlmtsell owner volume price
- **owner** order owner and authority provider
- **volume** volume of order
- **price** price of order

Close limit buy order
## limit::clslmtbuy market_id id
- **market_id** market id of trade pair
- **id** order id

Close limit sell order
## limit::clslmtsell market_id id
- **market_id** market id of trade pair
- **id** order id

Part fill buy order
## limit::ptfillbuyord market_id order_id amount
- **market_id** market id of trade pair
- **order_id** order id
- **amount** amount to fill

Part fill sell order
## limit::ptfillsellord market_id order_id amount
- **market_id** market id of trade pair
- **order_id** order id
- **amount** amount to fill

Full fill buy order
## limit::fillbuyord market_id order_id
- **market_id** market id of trade pair
- **order_id** order id

Full fill sell order
## limit::fillsellord market_id order_id
- **market_id** market id of trade pair
- **order_id** order id



    