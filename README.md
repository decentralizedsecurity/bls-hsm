# bls-hsm

## Design :page_with_curl:
This project implements a command line interface (cli) that internally uses [blst library](https://github.com/supranational/blst#blst). It has been implemented using the Nordic Connect SDK for the execution in Nordic Semiconductor nRF9160DK and rRF5340DK boards. The cli can be accessed using the serial port of the board.
We take advantage of ARM TrustZone technology to protect cryptographic material. All critical parts of the code that require access to private keys are run in the secure world.

## Installation in Linux
You can run `./setup.sh [-c "compiler path"] [-i] -b "board identifier"` to build the project.
`-c "compiler path"` option will define the path of the arm compiler. `-i` option is used to automatically check if the compiler is installed and install it otherwise, using it in the building process.
`setup.sh` will run sequentially `build_blst.sh`, `dependencies.sh`, `build.sh` and `flash.sh`. After running `build_blst.sh` and `dependencies.sh` once, only `build.sh` and `flash.sh` are needed.

Output example:
```
user@user:~/bls-hsm$ ./setup.sh -c /usr/bin/arm-none-eabi-gcc -b nrf5340dk_nrf5340_cpuapp_ns
Compiler selected: /usr/bin/arm-none-eabi-gcc
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -c ./blst/src/server.c
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -c ./blst/build/assembly.S
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -nostdlib -r assembly.o server.o -o blst.o
+ /usr/bin/arm-none-eabi-objcopy --localize-symbols=/tmp/localize.blst.11736 blst.o
+ /usr/bin/arm-none-eabi-ar rc libblst.a blst.o
Blst library built
.
.
.
-- runners.nrfjprog: Board with serial number xxxxxxxxx flashed successfully.
user@user:~/bls-hsm$
```

It's also possible to install the nRF Connect SDK manually following this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html).

## Emulation
It is also possible to compile the project to run in Linux and MacOS directly without the board. The "emu" directory contains a simple socket server that by default exports all the funcionality of the cli project over the 8080 port, if you need to change the port simply modify the value of PORT in [main.c](emu/main.c). It also contains a simple socket client to comsume this API and do some testing ([client.c](emu/client.c)).

In order to compile those files you can use the script [build_emu.sh](build_emu.sh). Just run `./build_emu.sh`, start the server by running `./emu/build/server` and then in another terminal run `./emu/build/client`. You should see a prompt like this and will be able to enter any command supported by the cli project (see [Usage](#Usage)):

```
Socket successfully created..
connected to the server..
Enter command: 
```


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


## Implementations :pick:
**1. cli**: This project uses blst static library that has been compiled for Cortex-M33 architecture.

PS. The *prj.conf* file has been modified because default size caused stack overflow from the UART thread. Current size is 49152 bytes.

**2. secure_module**: This module contains blst function calls that involve usage and storage of secret keys, using Secure Partition Manager (SPM).

## Test
"test" folder contains a test coded in [Go](https://golang.org/) language. In order to run it, you must install Go and run `go mod init test`, `go mod tidy` and then either `go run ./main.go ./utils.go [-v] COMport` if you want to run it right away or `go build` and then `./test [-v] COMport` if you want to generate an executable. Optional argument `-v` will show a detailed output of the tests. `COMport` is the board's serial port name (e.g. COM4, /dev/ttyS3).
This test will do the following:
- Generate 10 keypairs (the maximum allowed by the board) and check that all keys are different.
- Attempt to generate an extra key pair and confirm the board refuses to do that.
- Perform a signature of a message with the wrong size an confirm the board refuses to do that.
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
## :warning:Remote signer interface:warning: (UNSTABLE)
[remote](remote) folder implements a HTTP server which follows the same spec as [Web3Signer](https://github.com/ConsenSys/web3signer), which is based on [EIP-3030 spec](https://eips.ethereum.org/EIPS/eip-3030). This module is currently in development and only supports signing of [Phase0 Beacon Blocks](https://github.com/ethereum/consensus-specs/blob/dev/specs/phase0/beacon-chain.md#beacon-blocks).

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
