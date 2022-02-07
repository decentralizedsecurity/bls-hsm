"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.deriveChildSK = deriveChildSK;
exports.deriveMasterSK = deriveMasterSK;
exports.deriveChildSKMultiple = deriveChildSKMultiple;

var _sha = _interopRequireDefault(require("bcrypto/lib/sha256"));

var _hkdf = _interopRequireDefault(require("bcrypto/lib/hkdf"));

var _bn = _interopRequireDefault(require("bn.js"));

var _buffer = require("buffer");

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function ikmToLamportSK(ikm, salt) {
  const bIKM = _buffer.Buffer.from(ikm);

  const prk = _hkdf.default.extract(_sha.default, bIKM, salt);

  const okm = _hkdf.default.expand(_sha.default, prk, _buffer.Buffer.alloc(0), 8160); // 8160 = 255 * 32


  return Array.from({
    length: 255
  }, (_, i) => okm.slice(i * 32, (i + 1) * 32));
}

function parentSKToLamportPK(parentSK, index) {
  const salt = new _bn.default(index).toArrayLike(_buffer.Buffer, "be", 4);

  const ikm = _buffer.Buffer.from(parentSK);

  const lamport0 = ikmToLamportSK(ikm, salt);

  const notIkm = _buffer.Buffer.from(ikm.map(value => ~value));

  const lamport1 = ikmToLamportSK(notIkm, salt);
  const lamportPK = lamport0.concat(lamport1).map(value => _sha.default.digest(value));
  return _sha.default.digest(_buffer.Buffer.concat(lamportPK));
}

function hkdfModR(ikm, keyInfo = _buffer.Buffer.alloc(0)) {
  let salt = _buffer.Buffer.from("BLS-SIG-KEYGEN-SALT-", "ascii");

  let sk = new _bn.default(0);

  while (sk.eqn(0)) {
    salt = _sha.default.digest(salt);

    const prk = _hkdf.default.extract(_sha.default, _buffer.Buffer.concat([ikm, _buffer.Buffer.alloc(1)]), salt);

    const okm = _hkdf.default.expand(_sha.default, prk, _buffer.Buffer.concat([keyInfo, _buffer.Buffer.from([0, 48])]), 48);

    const okmBN = new _bn.default(okm, "hex", "be");
    const r = new _bn.default("52435875175126190479447740508185965837690552500527637822603658699938581184513");
    sk = okmBN.mod(r);
  }

  return _buffer.Buffer.from(sk.toArray("be", 32));
}

function deriveChildSK(parentSK, index) {
  if (!_buffer.Buffer.isBuffer(parentSK) || parentSK.length !== 32) {
    throw new Error("parentSK must be a Buffer of 32 bytes");
  }

  if (!Number.isSafeInteger(index) || index < 0 || index >= 2 ** 32) {
    throw new Error("index must be 0 <= i < 2**32");
  }

  const compressedLamportPK = parentSKToLamportPK(parentSK, index);
  return hkdfModR(compressedLamportPK);
}

function deriveMasterSK(ikm) {
  if (!_buffer.Buffer.isBuffer(ikm)) {
    throw new Error("ikm must be a Buffer");
  }

  if (ikm.length < 32) {
    throw new Error("ikm must be >= 32 bytes");
  }

  return hkdfModR(ikm);
}

function deriveChildSKMultiple(parentSK, indices) {
  let key = parentSK;
  indices.forEach(i => key = deriveChildSK(key, i));
  return key;
}
//# sourceMappingURL=key-derivation.js.map