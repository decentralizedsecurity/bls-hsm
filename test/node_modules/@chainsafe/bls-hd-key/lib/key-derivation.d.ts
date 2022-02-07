/// <reference types="node" />
export declare function deriveChildSK(parentSK: Buffer, index: number): Buffer;
export declare function deriveMasterSK(ikm: Buffer): Buffer;
export declare function deriveChildSKMultiple(parentSK: Buffer, indices: number[]): Buffer;
