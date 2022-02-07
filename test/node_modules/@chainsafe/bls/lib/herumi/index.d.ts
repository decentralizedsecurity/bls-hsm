import { SecretKey } from "./secretKey";
import { PublicKey } from "./publicKey";
import { Signature } from "./signature";
import { init, destroy } from "./context";
import { IBls } from "../interface";
export * from "../constants";
export { SecretKey, PublicKey, Signature, init, destroy };
export declare const bls: IBls;
export default bls;
