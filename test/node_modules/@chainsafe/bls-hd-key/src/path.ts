import assert from "assert";

const ETH2_COIN_TYPE = 3600;

/**
 * Convert a derivation path to an array of indices,
 * verifying that the path conforms to [EIP-2334](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-2334.md)
 * @param coinType - set to null to disable coin type verification
 */
export function pathToIndices(path: string, coinType: number | null = ETH2_COIN_TYPE): number[] {
  assert(path.indexOf("'") === -1, "path should not contain \"'\" - hardened keys not supported");
  const components = path.split("/");
  assert(components.length >= 5, "path should contain at least 5 levels separated by '/'");
  assert(components[0] === "m", "root should be \"m\" - root of tree");
  // remove "m"
  components.shift();
  // parse levels
  const indices = components.map((level) => Number.parseInt(level));
  assert(!indices.some(Number.isNaN), "every level must be a number (except the master node)");
  assert(indices.every((i) => i >= 0 && i < 4294967296), "every level must be within bounds 0 <= i < 2**32");
  assert(indices[0] === 12381, "purpose should be '12381' - bls12-381");
  if (coinType) {
    assert(indices[1] === coinType, "coin type should match");
  }
  return indices;
}
