import SHA256 from "bcrypto/lib/sha256";
import HKDF from "bcrypto/lib/hkdf";
import BN from "bn.js";
import {Buffer} from "buffer";

function ikmToLamportSK(ikm: Buffer, salt: Buffer): Buffer[] {
  const bIKM = Buffer.from(ikm);
  const prk = HKDF.extract(SHA256, bIKM, salt);
  const okm = HKDF.expand(SHA256, prk, Buffer.alloc(0), 8160); // 8160 = 255 * 32
  return Array.from({length: 255}, (_, i) => okm.slice(i*32, (i+1)*32));
}

function parentSKToLamportPK(parentSK: Buffer, index: number): Buffer {
  const salt = (new BN(index)).toArrayLike(Buffer, "be", 4);
  const ikm = Buffer.from(parentSK);
  const lamport0 = ikmToLamportSK(ikm, salt);
  const notIkm = Buffer.from(ikm.map((value) => ~value));
  const lamport1 = ikmToLamportSK(notIkm, salt);
  const lamportPK = lamport0.concat(lamport1).map((value) => SHA256.digest(value));
  return SHA256.digest(Buffer.concat(lamportPK));
}

function hkdfModR(ikm: Buffer, keyInfo: Buffer = Buffer.alloc(0)): Buffer {
  let salt = Buffer.from("BLS-SIG-KEYGEN-SALT-", "ascii");
  let sk = new BN(0);
  while (sk.eqn(0)) {
    salt = SHA256.digest(salt);
    const prk = HKDF.extract(
      SHA256,
      Buffer.concat([ikm, Buffer.alloc(1)]),
      salt
    );
    const okm = HKDF.expand(SHA256, prk, Buffer.concat([keyInfo, Buffer.from([0, 48])]), 48);
    const okmBN = new BN(okm, "hex", "be");
    const r = new BN("52435875175126190479447740508185965837690552500527637822603658699938581184513");
    sk = okmBN.mod(r);
  }
  return Buffer.from(sk.toArray("be", 32));
}

export function deriveChildSK(parentSK: Buffer, index: number): Buffer {
  if (!Buffer.isBuffer(parentSK) || parentSK.length !== 32) {
    throw new Error("parentSK must be a Buffer of 32 bytes");
  }
  if (!Number.isSafeInteger(index) || index < 0 || index >= 2 ** 32) {
    throw new Error("index must be 0 <= i < 2**32");
  }
  const compressedLamportPK = parentSKToLamportPK(parentSK, index);
  return hkdfModR(compressedLamportPK);
}

export function deriveMasterSK(ikm: Buffer): Buffer {
  if (!Buffer.isBuffer(ikm)) {
    throw new Error("ikm must be a Buffer");
  }
  if (ikm.length < 32) {
    throw new Error("ikm must be >= 32 bytes");
  }
  return hkdfModR(ikm);
}

export function deriveChildSKMultiple(parentSK: Buffer, indices: number[]): Buffer {
  let key = parentSK;
  indices.forEach(i => key = deriveChildSK(key, i));
  return key;
}
