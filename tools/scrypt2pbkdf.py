import json,sys
from cryptography.hazmat.primitives.kdf.scrypt import Scrypt
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC


password = bytes(sys.argv[1],"utf-8")
in_filename = sys.argv[2]
out_filename = sys.argv[3]

f = open(in_filename)

keystore_in = json.load(f)
keystore_out =keystore_in.copy()

#import scrypt params from keystore file
salt = bytes.fromhex(keystore_in['crypto']['kdf']['params']['salt'])
n= int(keystore_in['crypto']['kdf']['params']['n'])
p= int(keystore_in['crypto']['kdf']['params']['p'])
r= int(keystore_in['crypto']['kdf']['params']['r'])
dklen= int(keystore_in['crypto']['kdf']['params']['dklen'])
iv = bytes.fromhex(keystore_in['crypto']['cipher']['params']['iv'])
enc_private_key = bytes.fromhex(keystore_in['crypto']['cipher']['message'])
checksum = bytes.fromhex(keystore_in['crypto']['checksum']['message'])
# derive
kdf = Scrypt(
    salt=salt,
    length=dklen,
    n=n,
    r=r,
    p=p,
)
key = kdf.derive(password)

digest = hashes.Hash(hashes.SHA256())
digest.update(key[16:32])
digest.update(enc_private_key)
if digest.finalize() == checksum:
  cipher = Cipher(algorithms.AES(key[0:16]), modes.CTR(iv))
  decryptor = cipher.decryptor()
  private_key = decryptor.update(enc_private_key) + decryptor.finalize()
  keystore_out['crypto']['kdf'].clear()
  keystore_out['crypto']['kdf']['function']="pbkdf2"
  keystore_out['crypto']['kdf']['message']=""
  keystore_out['crypto']['kdf']['params'] ={}
  keystore_out['crypto']['kdf']['params']['dklen'] = dklen
  keystore_out['crypto']['kdf']['params']['c'] = n
  keystore_out['crypto']['kdf']['params']['prf'] = "hmac-sha256"
  keystore_out['crypto']['kdf']['params']['salt'] = salt.hex()
  
  kdf = PBKDF2HMAC(
    algorithm=hashes.SHA256(),
    length=dklen,
    salt=salt,
    iterations=n,
  )
  key = kdf.derive(password)
  
  cipher = Cipher(algorithms.AES(key[0:16]), modes.CTR(iv))
  encryptor = cipher.encryptor()
  enc_private_key = encryptor.update(private_key) + encryptor.finalize()
  keystore_out['crypto']['cipher']['message'] = enc_private_key.hex()

  digest = hashes.Hash(hashes.SHA256())
  digest.update(key[16:32])
  digest.update(enc_private_key)
  checksum = digest.finalize()
  keystore_out['crypto']['checksum']['message'] = checksum.hex()

  with open(out_filename, 'w') as outfile:
    json.dump(keystore_out, outfile)
  
