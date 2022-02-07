/* var verifyKeystore = require('@myetherwallet/eth2-keystore');
const PASSWORD = 'testwallet';
const ks = new KeyStore();
console.log(await ks.getMnemonic());
const genSigningKeystore = await ks.toSigningKeystore(PASSWORD); //generates the keystore json
const res = await verifyKeystore(genSigningKeystore, PASSWORD); //verify generated keystore
//res === true
const genWithdrawalKeystore = await ks.toWithdrawalKeystore(PASSWORD); //generates the keystore json
const res2 = await verifyKeystore(genWithdrawalKeystore, PASSWORD); //verify generated keystore
//res2 === true */

//Existing mnemonic
var KeyStore = require('@myetherwallet/eth2-keystore');
const MNEMONIC =
    'tenant glimpse solve letter chest ankle jealous movie subway exhibit cream garden scene grunt below patrol hurt fatigue escape trap phrase mandate feature one';
const PASSWORD = 'testwallet';
const ks = new KeyStore.default({
    mnemonic: MNEMONIC
});
console.log(ks.getMnemonic());
//console.log(ks.getPrivKey());
//tenant glimpse solve letter chest ankle jealous movie subway exhibit cream garden scene grunt below patrol hurt fatigue escape trap phrase mandate feature one
const genSigningKeystore =  ks.toSigningKeystore(PASSWORD); //generates the keystore json
//const res = await verifyKeystore(genSigningKeystore, PASSWORD); //verify generated keystore
//res === true
//const genWithdrawalKeystore = await ks.toWithdrawalKeystore(PASSWORD); //generates the keystore json
//const res2 = await verifyKeystore(genWithdrawalKeystore, PASSWORD); //verify generated keystore
//res2 === true 