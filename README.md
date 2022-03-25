# ETH 2.0 Hardware Remote Signer 

The aim of this project is to develop a working prototype for an Eth 2.0 remote signer that follows [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) for the ARM Cortex-M33 architecture. We take advantage of ARM TrustZone to protect the private keys of the validator, critical parts of the code that require access to private keys are run in the secure world. Our prototype is developed specifically for the [nRF9160DK](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk) and [nRF5340DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) boards, although it should be easy to port to other boards using an ARM Cortex-M33. We use the [blst](https://github.com/supranational/blst) library by suprational to implement the BLS signature and related methods

This project is founded by the [Eth2 staking community grants](https://blog.ethereum.org/2021/02/09/esp-staking-community-grantee-announcement/), from [Etherum Fundation](https://ethereum.foundation/)

We have implemented the following tools:

- `cli`. A command line interface to create BLS keypairs, sign and verify messages.
- `remote-signer`. An Eth2.0 remote signer that protects private keys using ARM TrustZone.

Those two tools are provided as separate nRF Connect projects and requiere a nRF5340DK or a nRF9160DK board to run. They relay in the `secure_module` project, that is run the secure world and contains blst function calls that involve private keys, using Secure Partition Manager (SPM).

Apart from those tools, we also provide a port of them that can be run without the need of ARM TrustZone and instead of exposing the functionality over the serial port, use a TCP socket:

- `cli-socket`.
- `remote-signer-socket`.

Those two tools can be build and run in Linux, MacOS and Windows directly, and would allow anyone to test our project before considering buying a supported hardware.

Lastly, we have also implemented a simple remote signer in Go that uses our CLI as backend.

- `remote-signer-go`.

## Dependencies :link:

We have developed this project using the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK), which is based on the Zephyr RTOS. In order to install the required dependencies it is recommended to follow this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html). Currently, only 1.8.0 version of the toolchain or lower is supported. If you are using Linux, you can try the script `linux_dependencies.sh` to install all the required dependencies, although we recommend following the official guide.

In order to build the [Blst](https://github.com/supranational/blst) library, we provide the script `build_blst.sh`, that clones and builds the library in Linux providing the ARM compiler as a parameter. If you are using other operating system you can try building the library using the scripts provided by suprational in their repo.

## Building the tools :pick:

In order to build the nRF Connect projects you can either follow these [steps](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_programming.html#gs-programming-cmd) or use the `build.sh` script. There is also a `flash.sh` script to flash the board with a previously built project.

You can find more information about building and running the other tools in their respective folders


# CLI

## Usage

The commands that are supported are:

- :warning:*import*:warning:: imports secrets key and derives public key (NOTE: this is currently an INSECURE implementation).
  ```
  uart:~$ import 31e8ff32b69aca39ce7ce789cff517cc9323cedec39eea9660d0dc3d4e8622cb
  0x86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3d12f937cf7104c9cd51bb4c62ff185d07f
  ```
- *signature "publickey" "message"*: produces a 96-bytes signature with the message that has been introduced and after choosing a public key that has had to be generated before.
  ```
  uart:~$ signature 86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3
  d12f937cf7104c9cd51bb4c62ff185d07f  56565656565656565656565656565656565656565656
  56565656565656565656
  Signature:
  0xb912c912616709f6a190b03db1a259ca21f535abe51f88d6c95407a81fd8648b067c5e0548587f6a84f2dea9afd2098812bb1d7fb188f1d04411a04f25042b627c5f8d60dcef6416072cfef40b799b3c89397bcddf69ae62611484bfc6e83689
  ```
- *verify "public_key" "message" "signature"*: signature verification.
  ```
  uart:~$ verify 0x86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3d
  12f937cf7104c9cd51bb4c62ff185d07f 5656565656565656565656565656565656565656565656
  565656565656565656 0xb912c912616709f6a190b03db1a259ca21f535abe51f88d6c95407a81fd
  8648b067c5e0548587f6a84f2dea9afd2098812bb1d7fb188f1d04411a04f25042b627c5f8d60dce
  f6416072cfef40b799b3c89397bcddf69ae62611484bfc6e83689
  Success
  ```
- *getkeys*: returns the public keys that have been generated.
  ```
  uart:~$ getkeys
  {'keys':['86a722b1f5c1cb1420ff0766cf5205b023de2e9c69efc65dbf976af2d710c3d12f937cf7104c9cd51bb4c62ff185d07f']}
  ```
- *reset*: deletes all the keys.
  ```
  uart:~$ reset
  Keys deleted
  ```
- *keygen*: generates a 32-bytes secret key and a 48-bytes public key randomly.
  ```
  uart:~$ keygen
  Public key:
  0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e
  ```

## Test

`test` folder contains a test coded in [Go](https://golang.org/) language. In order to run it, you must install Go and run `go mod init test`, `go mod tidy` and then either `go run ./main.go ./utils.go [-v] COMport` if you want to run it right away or `go build` and then `./test [-v] COMport` if you want to generate an executable. Optional argument `-v` will show a detailed output of the tests. `COMport` is the board's serial port name (e.g. COM4, /dev/ttyS3).
This test will do the following:
- Generate 10 keypairs (the maximum allowed by the board) and check that all keys are different.
- Attempt to generate an extra key pair and confirm the board refuses to do that.
- Perform a signature of a message with the wrong size and confirm the board refuses to do that.
- Perform a signature of a message with the right size and check that the signature is properly verified.
- Import key from both Web3 and EIP2335 sample keystores (only use them for testing purposes).
- Attempt to get key from keystore using wrong password.

Output example:

```
user@user:~/bls-hsm/test$ go mod init test
go: creating new go.mod: module test
user@user:~/bls-hsm/test$ go mod tidy
go: finding github.com/tarm/serial latest
go: downloading golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
go: extracting golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
user@user:~/bls-hsm/test$ go build
go: finding golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
user@user:~/bls-hsm/test$ ./test /dev/ttyACM2
Running tests...
Delete previous keys..........PASSED
Generate 10 keys..............PASSED
2.1233534s elapsed
Retrieve generated keys.......PASSED
Check keys are different......PASSED
Try to generate extra key.....PASSED
Sign msg......................PASSED
Verify signature..............PASSED
2.9392392s elapsed
Import from Web3 keystore.....PASSED
Import from EIP2335 keystore..PASSED
Try wrong pass in keystore....PASSED
Delete keys...................PASSED
RESULTS:
----------------------------------------
Total.........................11/11
----------------------------------------
```

##  Trying out the CLI without the board

It is also possible to test the project in Linux and MacOS directly without the board. The "emu" directory contains a simple socket server that by default exports all the functionality of the cli project over the 8080 port, if you need to change the port simply modify the value of PORT in [main.c](emu/main.c). It also contains a simple socket client to consume this API and do some testing ([client.c](emu/client.c)).

In order to compile those files you can use the script [build_emu.sh](build_emu.sh). Just run `./build_emu.sh`, start the server by running `./emu/build/server` and then in another terminal run `./emu/build/client`. You should see a prompt like this and will be able to enter any command supported by the cli project (see [Usage](#Usage)):

```
Socket successfully created..
connected to the server..
Enter command: 
```

# Remote signer :warning: (Work in progress)

We are currently in the process of implementing a full C/C++ implementation of the [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) that runs entirely in the nRF9160DK and nRF5340DK boards. 

## Usage

This is still work in progress and there are still some missing features. For example, the validator need to provide the signing root in all signing request. Apart from the signature method, we also implement the methods to retrieve keys and check the status. The remote signer listen for HTTP requests over a serial port, process them and sends the response over serial too. If you want to test it with a Eth 2.0 client you first need to setup a Socket to Serial bridge, so that the board's serial port can be accessed in a local port. In windows you can use [Comm Tunnel](https://www.serialporttool.com/GK/comm-tunnel/), in linux or MacOS you could use [socat](https://linux.die.net/man/1/socat). You could also implement a connector for your favorite ETH 2.0 client that instead of sending the requests over HTTP uses a serial port. Please contact us if you want to follow that direction.

## Remote signer in GO using CLI backend

[remote](remote) folder implements a HTTP server which follows the same spec as [Web3Signer](https://github.com/ConsenSys/web3signer), which is based on [EIP-3030 spec](https://eips.ethereum.org/EIPS/eip-3030). This module is currently in development and only supports signing of [`Phase0 Beacon Blocks`](https://github.com/ethereum/consensus-specs/blob/dev/specs/phase0/beacon-chain.md#beacon-blocks), [`AttestationData`](https://github.com/ethereum/consensus-specs/blob/dev/specs/phase0/beacon-chain.md#attestationdata), `Aggregation Slot` and `Aggregate and Proof` (info about these two in [Web3Signer API REST](https://consensys.github.io/web3signer/web3signer-eth2.html)). The client doesn't need to send the signing root, it is computed in the server.

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
The body used in the HTTP request is the block json found in [samples](samples) folder.




