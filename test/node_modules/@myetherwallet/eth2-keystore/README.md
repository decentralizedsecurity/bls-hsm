# Eth2 Keystore

![npm (tag)](https://img.shields.io/npm/v/@myetherwallet/eth2-keystore/latest)
![GitHub](https://img.shields.io/github/license/MyEtherWallet/eth2-keystore)
![Node Version](https://img.shields.io/badge/node-12.x-green)

Utility functions for generating BLS secret keys, built for NodeJs and Browser.

-   Create keystore from BIP-39 mnemonic.
-   Create a derived child key from BIP-39 mnemonic.
-   Create Eth2 validator keystore from mnemonic.

Implementation follows EIPS: [EIP-2334](https://github.com/ethereum/EIPs/pull/2334), [EIP-2333](https://github.com/ethereum/EIPs/pull/2333)

For low-level [EIP-2333](https://github.com/ethereum/EIPs/pull/2333) and [EIP-2334](https://github.com/ethereum/EIPs/pull/2334) functionality, see [@chainsafe/bls-hd-key](https://github.com/chainsafe/bls-hd-key).

### Examples

```javascript
//New mnemonic
import KeyStore, { verifyKeystore } from '@myetherwallet/eth2-keystore';
const PASSWORD = 'testwallet';
const ks = new KeyStore();
console.log(await ks.getMnemonic());
const genSigningKeystore = await ks.toSigningKeystore(PASSWORD); //generates the keystore json
const res = await verifyKeystore(genSigningKeystore, PASSWORD); //verify generated keystore
//res === true
const genWithdrawalKeystore = await ks.toWithdrawalKeystore(PASSWORD); //generates the keystore json
const res2 = await verifyKeystore(genWithdrawalKeystore, PASSWORD); //verify generated keystore
//res2 === true

//Existing mnemonic
import KeyStore, { verifyKeystore } from '@myetherwallet/eth2-keystore';
const MNEMONIC =
    'tenant glimpse solve letter chest ankle jealous movie subway exhibit cream garden scene grunt below patrol hurt fatigue escape trap phrase mandate feature one';
const PASSWORD = 'testwallet';
const ks = new KeyStore({
    mnemonic: MNEMONIC
});
console.log(ks.getMnemonic());
//tenant glimpse solve letter chest ankle jealous movie subway exhibit cream garden scene grunt below patrol hurt fatigue escape trap phrase mandate feature one
const genSigningKeystore = await ks.toSigningKeystore(PASSWORD); //generates the keystore json
const res = await verifyKeystore(genSigningKeystore, PASSWORD); //verify generated keystore
//res === true
const genWithdrawalKeystore = await ks.toWithdrawalKeystore(PASSWORD); //generates the keystore json
const res2 = await verifyKeystore(genWithdrawalKeystore, PASSWORD); //verify generated keystore
//res2 === true
```

### Contribution

Requirements:

-   nodejs
-   npm

```bash
    npm install
    npm run test
```

### Audit

This repo is using the library audited by [this security audit](https://github.com/ChainSafe/lodestar/blob/master/audits/2020-03-23_UTILITY_LIBRARIES.pdf), released 2020-03-23. Commit [`32b068`](https://github.com/ChainSafe/bls-hd-key/commit/32b068) verified in the report.

### Special Thanks

to [@chainsafe](https://github.com/ChainSafe) for providing necessary libraries

### License

MIT
