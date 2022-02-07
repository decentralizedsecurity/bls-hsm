import { normalizePassword, KDFFunctions } from './utils';
import { pbkdf2Sync } from 'crypto';
import { sha256 } from 'ethereumjs-util';
import { scrypt } from 'scrypt-js';
const verify = async (keystore, password) => {
    password = normalizePassword(password);
    let derivedKey;
    if (keystore.crypto.kdf.function === KDFFunctions.Scrypt) {
        const kdfparams = keystore.crypto.kdf.params;
        derivedKey = await scrypt(
            Buffer.from(password),
            Buffer.from(kdfparams.salt, 'hex'),
            kdfparams.n,
            kdfparams.r,
            kdfparams.p,
            kdfparams.dklen
        );
        derivedKey = Buffer.from(derivedKey);
    } else if (keystore.crypto.kdf.function === KDFFunctions.PBKDF) {
        const kdfparams = keystore.crypto.kdf.params;
        derivedKey = pbkdf2Sync(
            Buffer.from(password),
            Buffer.from(kdfparams.salt, 'hex'),
            kdfparams.c,
            kdfparams.dklen,
            'sha256'
        );
    }
    const checksum = sha256(
        Buffer.concat([
            derivedKey.slice(16),
            Buffer.from(keystore.crypto.cipher.message, 'hex')
        ])
    ).toString('hex');
    return checksum === keystore.crypto.checksum.message;
};
export default verify;
