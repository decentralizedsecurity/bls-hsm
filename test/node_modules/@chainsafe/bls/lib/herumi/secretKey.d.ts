import { SecretKeyType } from "bls-eth-wasm";
import { PublicKey } from "./publicKey";
import { Signature } from "./signature";
import { SecretKey as ISecretKey } from "../interface";
export declare class SecretKey implements ISecretKey {
    readonly value: SecretKeyType;
    constructor(value: SecretKeyType);
    static fromBytes(bytes: Uint8Array): SecretKey;
    static fromHex(hex: string): SecretKey;
    static fromKeygen(entropy?: Uint8Array): SecretKey;
    sign(message: Uint8Array): Signature;
    toPublicKey(): PublicKey;
    toBytes(): Uint8Array;
    toHex(): string;
}
