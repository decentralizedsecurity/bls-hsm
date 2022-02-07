import { expect } from 'chai';
import { deriveChildSK, deriveMasterSK } from '@chainsafe/bls-hd-key';
import BN from 'bn.js';
import testVectorsJson from './vectors/tree_kdf.json';

describe('key derivation', function () {
    const testVectors = testVectorsJson.kdf_tests;
    describe('master key derivation', function () {
        testVectors.forEach((testVector, index) => {
            it(`test vector #${index}`, function () {
                const seed = Buffer.from(
                    testVector.seed.replace('0x', ''),
                    'hex'
                );
                const expectedMasterSK = new BN(
                    testVector.master_SK
                ).toArrayLike(Buffer, 'be');
                const masterSK = deriveMasterSK(seed);
                expect(masterSK.toString('hex')).to.be.deep.equal(
                    expectedMasterSK.toString('hex')
                );
            });
        });
    });

    describe('child key derivation', function () {
        testVectors.forEach((testVector, index) => {
            it(`test vector #${index}`, function () {
                const parentSK = new BN(testVector.master_SK).toArrayLike(
                    Buffer,
                    'be'
                );
                const index = testVector.child_index;
                const expectedChildSK = new BN(testVector.child_SK).toArrayLike(
                    Buffer,
                    'be'
                );
                const childSK = deriveChildSK(parentSK, index);
                expect(childSK.toString('hex')).to.be.deep.equal(
                    expectedChildSK.toString('hex')
                );
            });
        });
    });
});
