# BLS HD Key Utilities

![npm (tag)](https://img.shields.io/npm/v/@chainsafe/bls-hd-key/latest)
[![Discord](https://img.shields.io/discord/593655374469660673.svg?label=Discord&logo=discord)](https://discord.gg/aMxzVcr)
![GitHub](https://img.shields.io/github/license/chainsafe/bls-hd-key)
![ES Version](https://img.shields.io/badge/ES-2015-yellow)
![Node Version](https://img.shields.io/badge/node-10.x-green)

Utility functions for managing BLS heirarchical deterministic accounts.

1. Create a master private key from entropy
2. Create a child private key from a private key and index
3. Convert a standard path into an array of indices
4. Create a decendent private key from a private key and array of indices

Implementation is following EIPS: [EIP-2334](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-2334.md), [EIP-2333](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-2333.md)

For a higher-level interface, see [@chainsafe/bls-keygen](https://github.com/chainsafe/bls-keygen).

### Example
```typescript

import {deriveMasterSK, deriveChildSK, deriveChildSKMultiple, pathToIndices} from "@chainsafe/bls-hd-key";

// Create master private key (according to EIP-2333)

const entropy: Buffer = Buffer.from(...);
const masterKey: Buffer = deriveMasterSK(entropy);

// Create child private key one level deep

const childIndex: number = ...;
const childKey: Buffer = deriveChildSK(masterKey, childIndex);

// Convert a "path" into an array of indices (using validation rules of EIP-2334)

cont path = "m/12381/3600/0/0";
const childIndices: number[] = pathToIndices(path);

// Convert a "path" into an array of indices (validating an alternate key type)

const eth1KeyType = 60;
cont path = "m/12381/60/0/0";
const childIndices: number[] = pathToIndices(path, eth1KeyType);

// Create a child private key `childIndices.length` levels deep

const childIndices: number[] = [...];
const childKey = deriveChildSKMultiple(masterKey, childIndices);

// Compose functions to derive a child key from entropy and a path

const entropy: Buffer = Buffer.from(...);
cont path = "m/12381/3600/0/0";
const childKey = deriveChildSKMultiple(deriveMasterSK(entropy), pathToIndices(path));

```

### Contrubuting

Requirements:
- nodejs
- yarn

```bash
yarn install
yarn run test
```

### Audit

This repo was audited by Least Authority as part of [this security audit](https://github.com/ChainSafe/lodestar/blob/master/audits/2020-03-23_UTILITY_LIBRARIES.pdf), released 2020-03-23. Commit [`767c998`](https://github.com/ChainSafe/bls-hd-key/commit/767c998) verified in the report.

### License

Apache-2.0
