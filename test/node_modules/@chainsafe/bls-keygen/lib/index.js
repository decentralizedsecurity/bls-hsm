"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.generateRandomSecretKey = generateRandomSecretKey;
exports.deriveKeyFromMnemonic = deriveKeyFromMnemonic;
exports.deriveKeyFromEntropy = deriveKeyFromEntropy;
exports.deriveKeyFromMaster = deriveKeyFromMaster;
exports.eth2ValidatorPaths = eth2ValidatorPaths;
exports.deriveEth2ValidatorKeys = deriveEth2ValidatorKeys;

var _bip = require("bip39");

var _buffer = require("buffer");

var _randombytes = _interopRequireDefault(require("randombytes"));

var _blsHdKey = require("@chainsafe/bls-hd-key");

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

/**
 *
 * @param entropy optional additional entropy
 */
function generateRandomSecretKey(entropy) {
  let ikm = (0, _randombytes.default)(32);

  if (entropy) {
    ikm = _buffer.Buffer.concat([entropy, ikm]);
  }

  return deriveKeyFromEntropy(ikm);
}
/**
 * Derive a secret key from a BIP39 mnemonic seed and optionally an EIP-2334 path.
 * If path is included, the derived key will be the child secret key at that path,
 * otherwise, the derived key will be the master secret key
 */


function deriveKeyFromMnemonic(mnemonic, path) {
  if (!(0, _bip.validateMnemonic)(mnemonic)) {
    throw new Error("invalid mnemonic");
  }

  const ikm = _buffer.Buffer.from((0, _bip.mnemonicToSeedSync)(mnemonic));

  return deriveKeyFromEntropy(ikm, path);
}
/**
 * Derive a secret key from entropy and optionally an EIP-2334 path.
 * If path is included, the derived key will be the child secret key at that path,
 * otherwise, the derived key will be the master secret key
 */


function deriveKeyFromEntropy(entropy, path) {
  const masterKey = (0, _blsHdKey.deriveMasterSK)(_buffer.Buffer.from(entropy));

  if (path) {
    return deriveKeyFromMaster(masterKey, path);
  }

  return masterKey;
}
/**
 * Derive a child secret key from a master secret key
 * @param masterKey master secret key
 * @param path EIP-2334 path to child
 */


function deriveKeyFromMaster(masterKey, path) {
  return (0, _blsHdKey.deriveChildSKMultiple)(masterKey, (0, _blsHdKey.pathToIndices)(path));
}

/**
 * Return Eth2 validator HD paths
 */
function eth2ValidatorPaths(validatorIndex) {
  return {
    withdrawal: `m/12381/3600/${validatorIndex}/0`,
    signing: `m/12381/3600/${validatorIndex}/0/0`
  };
}
/**
 * Derive Eth2 validator secret keys from a single master secret key
 * @param masterKey master secret key
 */


function deriveEth2ValidatorKeys(masterKey, validatorIndex) {
  const paths = eth2ValidatorPaths(validatorIndex);
  return {
    withdrawal: deriveKeyFromMaster(masterKey, paths.withdrawal),
    signing: deriveKeyFromMaster(masterKey, paths.signing)
  };
}
//# sourceMappingURL=index.js.map