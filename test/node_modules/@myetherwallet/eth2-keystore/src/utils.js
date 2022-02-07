const KDFFunctions = {
    PBKDF: 'pbkdf2',
    Scrypt: 'scrypt'
};
const kdfParamsForPBKDF = opts => {
    return {
        dklen: opts.dklen,
        salt: opts.salt,
        c: opts.c,
        prf: 'hmac-sha256'
    };
};
const kdfParamsForScrypt = opts => {
    return {
        dklen: opts.dklen,
        salt: opts.salt,
        n: opts.n,
        r: opts.r,
        p: opts.p
    };
};
const runCipherBuffer = (cipher, data) => {
    return Buffer.concat([cipher.update(data), cipher.final()]);
};
const escapeRegExp = string => {
    return string.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
};
const normalizePassword = password => {
    password = password.normalize('NFKD');
    for (let i = 0x00; i < 0x20; i++)
        password = password.replace(
            new RegExp(escapeRegExp(String.fromCharCode(i)), 'g'),
            ''
        );
    for (let i = 0x7f; i < 0xa0; i++)
        password = password.replace(
            new RegExp(escapeRegExp(String.fromCharCode(i)), 'g'),
            ''
        );
    return password;
};
export {
    normalizePassword,
    KDFFunctions,
    runCipherBuffer,
    kdfParamsForScrypt,
    kdfParamsForPBKDF
};
