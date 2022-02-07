/// <reference types="node" />
/**
 *
 * @param entropy optional additional entropy
 */
export declare function generateRandomSecretKey(entropy?: Buffer): Buffer;
/**
 * Derive a secret key from a BIP39 mnemonic seed and optionally an EIP-2334 path.
 * If path is included, the derived key will be the child secret key at that path,
 * otherwise, the derived key will be the master secret key
 */
export declare function deriveKeyFromMnemonic(mnemonic: string, path?: string): Buffer;
/**
 * Derive a secret key from entropy and optionally an EIP-2334 path.
 * If path is included, the derived key will be the child secret key at that path,
 * otherwise, the derived key will be the master secret key
 */
export declare function deriveKeyFromEntropy(entropy: Buffer, path?: string): Buffer;
/**
 * Derive a child secret key from a master secret key
 * @param masterKey master secret key
 * @param path EIP-2334 path to child
 */
export declare function deriveKeyFromMaster(masterKey: Buffer, path: string): Buffer;
export interface IEth2ValidatorKeys {
    withdrawal: Buffer;
    signing: Buffer;
}
/**
 * Return Eth2 validator HD paths
 */
export declare function eth2ValidatorPaths(validatorIndex: number): {
    withdrawal: string;
    signing: string;
};
/**
 * Derive Eth2 validator secret keys from a single master secret key
 * @param masterKey master secret key
 */
export declare function deriveEth2ValidatorKeys(masterKey: Buffer, validatorIndex: number): IEth2ValidatorKeys;
