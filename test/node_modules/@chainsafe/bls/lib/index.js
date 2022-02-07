"use strict";
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
const herumi_1 = require("./herumi");
__export(require("./interface"));
// TODO: Use a Proxy for example to throw an error if it's not initialized yet
exports.bls = {};
exports.default = exports.bls;
async function getImplementation(impl = "herumi") {
    switch (impl) {
        case "herumi":
            await herumi_1.bls.init();
            return herumi_1.bls;
        case "blst-native":
            // Lazy import native bindings to prevent automatically importing binding.node files
            if (typeof require !== "function") {
                throw Error("blst-native is only supported in NodeJS");
            }
            // eslint-disable-next-line @typescript-eslint/no-require-imports
            return require("./blst").bls;
        default:
            throw new Error(`Unsupported implementation - ${impl}`);
    }
}
async function init(impl) {
    // Using Object.assign instead of just bls = getImplementation()
    // because otherwise the default import breaks. The reference is lost
    // and the imported object is still undefined after calling init()
    const blsImpl = await getImplementation(impl);
    Object.assign(exports.bls, blsImpl);
    Object.assign(exports, blsImpl);
}
exports.init = init;
