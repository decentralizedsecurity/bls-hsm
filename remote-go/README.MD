## Remote signer in GO using CLI backend

[remote](../remote) folder implements a HTTP server which follows the same spec as [Web3Signer](https://github.com/ConsenSys/web3signer), which is based on [EIP-3030 spec](https://eips.ethereum.org/EIPS/eip-3030). This module is currently in development and only supports signing of [`Phase0 Beacon Blocks`](https://github.com/ethereum/consensus-specs/blob/dev/specs/phase0/beacon-chain.md#beacon-blocks), [`AttestationData`](https://github.com/ethereum/consensus-specs/blob/dev/specs/phase0/beacon-chain.md#attestationdata), `Aggregation Slot` and `Aggregate and Proof` (info about these two in [Web3Signer API REST](https://consensys.github.io/web3signer/web3signer-eth2.html)). The client doesn't need to send the signing root, it is computed in the server.

To run the server use `go mod init remote`, `go mod tidy` and `go run ./main.go <comPort> <keystore_path> <keystore_password> [-v]` to run it directly or, if you prefer to build it first, run `go build` and then launch it by running `./remote <comPort> <keystore_path> <keystore_password> [-v]`. This will import the secret key obtained from the given keystore in `keystore_path` and wait for requests. `[-v]` parameter will give information about each signing request received.
It can be tested using [Postman](https://www.postman.com/).
Supported HTTP requests are `/upcheck`, `/api/v1/eth2/sign/{identifier}` and `/api/v1/eth2/publicKeys`.

Example in terminal:
```
go run ./main.go ../test/eip2335/keystore-m_12381_3600_0_0_0-1642162977.json 123456789 -v
Key imported
Starting server at port 80
Received signing request
Signing successful
```
Output using curl:
```
curl -X POST localhost:80/sign/ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 -H 'Content-Type: application/json' -d @block.json
{
        "signature": "0xb7131dbfc2d3b867751d419665402d1a1f06c7f52f83c3cc2af9c7b940bfdb30d8c4e21e72b71e7908406adefcf902ea18bec2326348c1de635dc8728d46e3f56531cc29dc5fb951032d2d9db26fafcd5e2b04cb759bf2c8cd5dcc9de77dcfce"
}
```
The body used in the HTTP request is the block json found in [samples](../samples) folder.
