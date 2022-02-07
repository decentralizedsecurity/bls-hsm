# Ethereum Keystore 
Utility to extract private key from Ethereum KeyStore file

<h2 style="border-bottom: 1px solid greigh6;">Module Usage</h2>

```typescript
import EthereumKeyStore from 'eth2-keystore-js';

fs.readFile(filePath, 'utf8', async (err: ErrnoException, data: string) => {
    if (err) {
        reject(err);
    } else {
        const keyStore = new EthereumKeyStore(JSON.parse(data));
        
        const privateKey = await keyStore.getPrivateKey('<keystorePassword>');
    }
});
```

## Dependencies

- <a href="https://www.npmjs.com/package/scrypt-js">scrypt-js</a> - The scrypt password-base key derivation function
- <a href="https://github.com/ethereumjs/ethereumjs-wallet">ethereumjs-wallet</a> - A lightweight wallet implementation
- <a href="https://www.npmjs.com/package/typescript">typescript</a> - TypeScript is a language for application-scale JavaScript
