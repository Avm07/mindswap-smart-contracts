# template.Contracts 1.0.0

## Dependencies:
* [eosio v2.0.7](https://github.com/EOSIO/eos/releases/tag/v2.0.7)
* [eosio.cdt v1.7.0](https://github.com/EOSIO/eosio.cdt/releases/tag/v1.7.0)

To build the contracts and the unit tests:
* First, ensure that your __eosio__ is compiled to the core symbol for the EOSIO blockchain that intend to deploy to.
* Second, make sure that you have ```sudo make install```ed __eosio__.

After build:
* The unit tests executable is placed in the _build/tests_ and is named __unit_test__.
* The contracts are built into a _bin/\<contract name\>_ folder in their respective directories.
* Finally, simply use __cleos__ to _set contract_ by pointing to the previously mentioned directory.

Build & Run tests:
```
./build.sh -y && ./build/tests/unit_test
```

From docker:
```
docker run -t -i -v $(CWD):/dapp kesar/eos-dev:207-170 -c "cd /dapp && /bin/bash /dapp/build.sh -y -e /root/eosio/2.0 && /dapp/build/tests/unit_test"
```

From makefile (docker needs to be running):
```
make build-test 
```
