import * as blst from "@chainsafe/blst";
import { SecretKey as ISecretKey } from "../interface";
import { PublicKey } from "./publicKey";
import { Signature } from "./signature";
export declare class SecretKey implements ISecretKey {
    readonly value: blst.SecretKey;
    constructor(value: blst.SecretKey);
    static fromBytes(bytes: Uint8Array): SecretKey;
    static fromHex(hex: string): SecretKey;
    static fromKeygen(entropy?: Uint8Array): SecretKey;
    sign(message: Uint8Array): Signature;
    toPublicKey(): PublicKey;
    toBytes(): Uint8Array;
    toHex(): string;
}
