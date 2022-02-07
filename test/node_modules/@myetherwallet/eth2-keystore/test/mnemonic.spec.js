import KeyStore from '../src/index';
import { expect } from 'chai';
import mnemonicVectors from './vectors/mnemonic.json';
describe('Mnemonic Test', function () {
    this.timeout(5000);
    const languages = Object.keys(mnemonicVectors);
    languages.forEach(lang => {
        it('should pass ' + lang, function () {
            mnemonicVectors[lang].forEach(async mn => {
                const keystore = new KeyStore({
                    mnemonic: mn[1],
                    password: 'TREZOR',
                    lang
                });
                const entropy = await keystore.getEntropy();
                const seed = await keystore.getSeed();
                expect(entropy.toString('hex')).to.equal(mn[0]);
                expect(seed.toString('hex')).to.equal(mn[2]);
            });
        });
    });
});
