"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const randombytes_1 = __importDefault(require("randombytes"));
exports.randomBytes = randombytes_1.default;
/**
 * Validate bytes to prevent confusing WASM errors downstream if bytes is null
 */
function validateBytes(bytes, argName) {
    for (const item of Array.isArray(bytes) ? bytes : [bytes]) {
        if (item == null) {
            throw Error(`${argName || "bytes"} is null or undefined`);
        }
    }
}
exports.validateBytes = validateBytes;
function isZeroUint8Array(bytes) {
    return bytes.every((byte) => byte === 0);
}
exports.isZeroUint8Array = isZeroUint8Array;
function concatUint8Arrays(bytesArr) {
    const totalLen = bytesArr.reduce((total, bytes) => total + bytes.length, 0);
    const merged = new Uint8Array(totalLen);
    let mergedLen = 0;
    for (const bytes of bytesArr) {
        merged.set(bytes, mergedLen);
        mergedLen += bytes.length;
    }
    return merged;
}
exports.concatUint8Arrays = concatUint8Arrays;
