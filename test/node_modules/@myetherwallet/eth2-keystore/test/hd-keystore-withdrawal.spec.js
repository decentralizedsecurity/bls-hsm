import { expect } from 'chai';
import KeyStore, { verifyKeystore } from '../src/index';
import KeyVectors from './vectors/keystores/hd-keystore-withdrawal.json';

const MNEMONIC =
    '정거장 여왕 설렁탕 그제서야 스튜디오 발전 페인트 수입 백화점 중순 시청 사흘 유산 항구 기침 씨앗 여인 당분간 지리산 믿음 닭고기 부족 밀리미터 배꼽';
const PASSWORD = 'testwallet';
describe('HD withdrawal keystore Test', function () {
    this.timeout(30000);
    Object.keys(KeyVectors).forEach(vIdx => {
        const _key = KeyVectors[vIdx];
        it('should pass path ' + _key.path, async () => {
            delete _key['uuid'];
            const params = {
                salt: Buffer.from(_key.crypto.kdf.params.salt, 'hex'),
                iv: Buffer.from(_key.crypto.cipher.params.iv, 'hex'),
                kdf: _key.crypto.kdf.function
            };
            const ks = new KeyStore({
                mnemonic: MNEMONIC,
                lang: 'korean'
            });
            const idx = parseInt(_key.path.split('/')[3]);
            const pubKey = await ks.getPublicKey(idx, false);
            const genKeystore = await ks.toWithdrawalKeystore(
                PASSWORD,
                idx,
                params
            );
            delete genKeystore['uuid'];
            expect(genKeystore).to.deep.equal(_key);
            expect(pubKey.toString('hex')).to.equal(_key.pubkey);
            const res = await verifyKeystore(genKeystore, PASSWORD);
            expect(res).to.equal(true);
        });
    });
});
