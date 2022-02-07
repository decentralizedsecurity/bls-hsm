"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const bls_keygen_1 = require("@chainsafe/bls-keygen");
const constants_1 = require("../constants");
const context_1 = require("./context");
const publicKey_1 = require("./publicKey");
const signature_1 = require("./signature");
const helpers_1 = require("../helpers");
const errors_1 = require("../errors");
class SecretKey {
    constructor(value) {
        if (value.isZero()) {
            throw new errors_1.ZeroSecretKeyError();
        }
        this.value = value;
    }
    static fromBytes(bytes) {
        if (bytes.length !== constants_1.SECRET_KEY_LENGTH) {
            throw new errors_1.InvalidLengthError("SecretKey", constants_1.SECRET_KEY_LENGTH);
        }
        const context = context_1.getContext();
        const secretKey = new context.SecretKey();
        secretKey.deserialize(bytes);
        return new SecretKey(secretKey);
    }
    static fromHex(hex) {
        return this.fromBytes(helpers_1.hexToBytes(hex));
    }
    static fromKeygen(entropy) {
        const sk = bls_keygen_1.generateRandomSecretKey(entropy && Buffer.from(entropy));
        return this.fromBytes(sk);
    }
    sign(message) {
        return new signature_1.Signature(this.value.sign(message));
    }
    toPublicKey() {
        return new publicKey_1.PublicKey(this.value.getPublicKey());
    }
    toBytes() {
        return this.value.serialize();
    }
    toHex() {
        return helpers_1.bytesToHex(this.toBytes());
    }
}
exports.SecretKey = SecretKey;
