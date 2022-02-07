/**
 * Convert a derivation path to an array of indices,
 * verifying that the path conforms to [EIP-2334](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-2334.md)
 * @param coinType - set to null to disable coin type verification
 */
export declare function pathToIndices(path: string, coinType?: number | null): number[];
