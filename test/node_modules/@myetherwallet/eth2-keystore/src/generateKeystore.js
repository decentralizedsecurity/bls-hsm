import { SecretKey, init } from '@chainsafe/bls';
import { randomBytes, pbkdf2Sync, createCipheriv } from 'crypto';
import { scrypt } from 'scrypt-js';
import { sha256 } from 'ethereumjs-util';
import { v4 as uuidv4 } from 'uuid';
import {
    normalizePassword,
    KDFFunctions,
    runCipherBuffer,
    kdfParamsForScrypt,
    kdfParamsForPBKDF
} from './utils';

const generate = async (privateKey, password, params = {}, path = '') => {
    password = normalizePassword(password);
    const v4Defaults = {
        cipher: 'aes-128-ctr',
        kdf: 'scrypt',
        salt: randomBytes(32),
        iv: randomBytes(16),
        dklen: 32,
        c: 262144,
        n: 262144,
        r: 8,
        p: 1,
        uuid: randomBytes(16)
    };
    Object.assign(v4Defaults, params);
    let derivedKey, kdfParams;
    if (v4Defaults.kdf === KDFFunctions.PBKDF) {
        kdfParams = kdfParamsForPBKDF(v4Defaults);
        derivedKey = pbkdf2Sync(
            Buffer.from(password),
            kdfParams.salt,
            kdfParams.c,
            kdfParams.dklen,
            'sha256'
        );
    } else {
        kdfParams = kdfParamsForScrypt(v4Defaults);
        derivedKey = await scrypt(
            Buffer.from(password),
            kdfParams.salt,
            kdfParams.n,
            kdfParams.r,
            kdfParams.p,
            kdfParams.dklen
        );
        derivedKey = Buffer.from(derivedKey);
    }
    const cipher = createCipheriv(
        v4Defaults.cipher,
        derivedKey.slice(0, 16),
        v4Defaults.iv
    );
    const ciphertext = runCipherBuffer(cipher, privateKey);
    await init('herumi');
    return {
        crypto: {
            kdf: {
                function: v4Defaults.kdf,
                params: {
                    ...kdfParams,
                    salt: kdfParams.salt.toString('hex')
                },
                message: ''
            },
            checksum: {
                function: 'sha256',
                params: {},
                message: sha256(
                    Buffer.concat([derivedKey.slice(16), ciphertext])
                ).toString('hex')
            },
            cipher: {
                function: v4Defaults.cipher,
                params: {
                    iv: v4Defaults.iv.toString('hex')
                },
                message: ciphertext.toString('hex')
            }
        },
        description: '',
        pubkey: Buffer.from(
            SecretKey.fromBytes(privateKey).toPublicKey().toBytes()
        ).toString('hex'),
        path: path,
        uuid: uuidv4({ random: v4Defaults.uuid }),
        version: 4
    };
};
export default generate;
