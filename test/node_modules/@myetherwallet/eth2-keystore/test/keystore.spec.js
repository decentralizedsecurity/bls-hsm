import { expect } from 'chai';
import keystore0 from './vectors/keystores/test0.json';
import keystore1 from './vectors/keystores/test1.json';
import Keystore, { generateKeystore, verifyKeystore } from '../src/index';
const PASSWORD =
    '\ud835\udd31\ud835\udd22\ud835\udd30\ud835\udd31\ud835\udd2d\ud835\udd1e\ud835\udd30\ud835\udd30\ud835\udd34\ud835\udd2c\ud835\udd2f\ud835\udd21\ud83d\udd11';
const PRIV_KEY = Buffer.from(
    '000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f',
    'hex'
);
describe('Keystore Test', function () {
    this.timeout(30000);
    it('should pass ' + keystore0.crypto.kdf.function, async () => {
        delete keystore0['uuid'];
        delete keystore0['description'];
        const params = {
            salt: Buffer.from(keystore0.crypto.kdf.params.salt, 'hex'),
            iv: Buffer.from(keystore0.crypto.cipher.params.iv, 'hex'),
            kdf: keystore0.crypto.kdf.function
        };
        const genKeystore = await generateKeystore(
            PRIV_KEY,
            PASSWORD,
            params,
            keystore0.path
        );
        delete genKeystore['uuid'];
        delete genKeystore['description'];
        expect(genKeystore).to.deep.equal(keystore0);
    });
    it('should pass ' + keystore1.crypto.kdf.function, async () => {
        delete keystore1['uuid'];
        delete keystore1['description'];
        const params = {
            salt: Buffer.from(keystore1.crypto.kdf.params.salt, 'hex'),
            iv: Buffer.from(keystore1.crypto.cipher.params.iv, 'hex'),
            kdf: keystore1.crypto.kdf.function
        };
        const genKeystore = await generateKeystore(
            PRIV_KEY,
            PASSWORD,
            params,
            keystore1.path
        );
        delete genKeystore['uuid'];
        delete genKeystore['description'];
        expect(genKeystore).to.deep.equal(keystore1);
        const res = await verifyKeystore(genKeystore, PASSWORD);
        expect(res).to.equal(true);
    });
    it('should create new keystore', async () => {
        const ks = new Keystore();
        const keystore = await ks.toWithdrawalKeystore(PASSWORD);
        const res = await verifyKeystore(keystore, PASSWORD);
        expect(res).to.equal(true);
    });
});
