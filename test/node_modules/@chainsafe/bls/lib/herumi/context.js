"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
/* eslint-disable require-atomic-updates */
const bls_eth_wasm_1 = __importDefault(require("bls-eth-wasm"));
const errors_1 = require("../errors");
let blsGlobal = null;
let blsGlobalPromise = null;
async function setupBls() {
    if (!blsGlobal) {
        await bls_eth_wasm_1.default.init(bls_eth_wasm_1.default.BLS12_381);
        // Patch to fix multiVerify() calls on a browser with polyfilled NodeJS crypto
        if (typeof window === "object") {
            const crypto = window.crypto || window.msCrypto;
            // getRandomValues is not typed in `bls-eth-wasm` because it's not meant to be exposed
            // @ts-ignore
            bls_eth_wasm_1.default.getRandomValues = (x) => crypto.getRandomValues(x);
        }
        blsGlobal = bls_eth_wasm_1.default;
    }
}
exports.setupBls = setupBls;
// Cache a promise for Bls instead of Bls to make sure it is initialized only once
async function init() {
    if (!blsGlobalPromise) {
        blsGlobalPromise = setupBls();
    }
    return blsGlobalPromise;
}
exports.init = init;
function destroy() {
    blsGlobal = null;
    blsGlobalPromise = null;
}
exports.destroy = destroy;
function getContext() {
    if (!blsGlobal) {
        throw new errors_1.NotInitializedError("herumi");
    }
    return blsGlobal;
}
exports.getContext = getContext;
