import * as bip39 from 'bip39';
import {
    deriveMasterSK,
    pathToIndices,
    deriveChildSKMultiple
} from '@chainsafe/bls-hd-key';
import { SecretKey, init } from '@chainsafe/bls';
import generateKeystore from './generateKeystore';
import verifyKeystore from './verifyKeystore';
class Keystore {
    constructor({
        mnemonic = '',
        password = '',
        bits = 256,
        lang = 'english'
    } = {}) {
        bip39.setDefaultWordlist(lang);
        if (mnemonic === '') {
            mnemonic = bip39.generateMnemonic(bits);
        }
        this.mnemonic = mnemonic;
        this.bits = bits;
        this.seed = bip39.mnemonicToSeedSync(mnemonic, password);
        this.masterKey = deriveMasterSK(this.seed);
    }
    async getEntropy() {
        return bip39.mnemonicToEntropy(this.mnemonic);
    }
    async getSeed() {
        return this.seed;
    }
    async getMnemonic() {
        return this.mnemonic;
    }
    async getPath(idx = 0, isSigning = true) {
        const withdrawalPath = `m/12381/3600/${idx}/0`;
        return isSigning ? withdrawalPath + '/0' : withdrawalPath;
    }
    async getChildKey(idx = 0, isSigning = true) {
        const path = await this.getPath(idx, isSigning);
        return deriveChildSKMultiple(this.masterKey, pathToIndices(path));
    }
    async getPublicKey(idx = 0, isSigning = true) {
        await init('herumi');
        return Buffer.from(
            SecretKey.fromBytes(await this.getChildKey(idx, isSigning))
                .toPublicKey()
                .toBytes()
        );
    }
    async toSigningKeystore(password, idx = 0, params = {}) {
        const childKey = await this.getChildKey(idx, true);
        const path = await this.getPath(idx, true);
        return generateKeystore(childKey, password, params, path);
    }
    async toWithdrawalKeystore(password, idx = 0, params = {}) {
        const childKey = await this.getChildKey(idx, false);
        const path = await this.getPath(idx, false);
        return generateKeystore(childKey, password, params, path);
    }
}
export { generateKeystore, verifyKeystore };
export default Keystore;
