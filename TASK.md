Description
Contract 1 (limit order contract)
Develop a contract that allow to store limit orders between 2 different extended assets. This contract should allow partial and full fill.

When an order is filled or partially-filled, the user can then withdraw the earned asset from the smart contract. (do not do an auto-transfer)

Contract 2 (arbitrage contract)
Develop a contract that has a method that can arbitrage between the previous limit order contract and mindswap.
It should have a public method that anyone can call for a desired pair, and it will succeed the call if that pair is arbitrable between mindswap and limit order contract.
It should allow as a field of the method a user that will receive the earnings of the arbitrage.

Example: mindswaparbitrage::arbitrage(extended_asset1, extended_asset2, your_account)

Inspiration: https://medium.com/@pine_eth/pine-finance-an-amm-orders-engine-525fe1f1b1eb

Contracts must have a test suite build with boost similar to eosio.contracts. Please use this project as template 
( https://github.com/kesar/eos-dev-template ) that uses docker + makefiles + boost unit tests.

If you complete this AND want to continue similar work, #3 is another bounty that comes after this.s