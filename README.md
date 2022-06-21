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

## Tests

### CLI tests

[cli/test](./cli/test) folder contains a test coded in [Go](https://golang.org/) language. In order to run it, you must install Go and run `go mod init test`, `go mod tidy` and then either `go run ./main.go ./utils.go [-v] COMport` if you want to run it right away or `go build` and then `./test [-v] COMport` if you want to generate an executable. Optional argument `-v` will show a detailed output of the tests. `COMport` is the board's serial port name (e.g. COM4, /dev/ttyS3).
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

### Remote Signer tests

[remote-signer/test_rs](./remote-signer/test_rs) also contains some test in Go to try some basic remote signer API methods. You need to run, as in the previous tests, `go mod init test_rs`, `go mod tidy` and then `go test -com=COMPORT`. You can add the `-v` flag to show detailed info of each test.
Output example:

```
go test -com=com7 -v
=== RUN   TestImportKeystore
--- PASS: TestImportKeystore (74.15s)
=== RUN   TestSignBlock
--- PASS: TestSignBlock (1.04s)
=== RUN   TestSignAggregateAndProof
--- PASS: TestSignAggregateAndProof (1.03s)
=== RUN   TestSignAggregationSlot
--- PASS: TestSignAggregationSlot (1.03s)
=== RUN   TestSignAttestation
--- PASS: TestSignAttestation (1.03s)
PASS
ok      test_rs 78.576s
```

##  Trying out the CLI without the board

It is also possible to test the project in Linux and MacOS directly without the board. The "cli-socket" directory contains a simple socket server that by default exports all the functionality of the cli project over the 8080 port, if you need to change the port simply modify the value of PORT in [main.c](cli-socket/main.c). It also contains a simple socket client to consume this API and do some testing ([client.c](cli-socket/client.c)).

In order to compile those files you can use the script [build_cli-socket.sh](build_cli-socket.sh). Just run `./build_emu.sh`, start the server by running `./cli-socket/build/server` and then in another terminal run `./cli-socket/build/client`. You should see a prompt like this and will be able to enter any command supported by the cli project (see [Usage](#Usage)):

```
Socket successfully created..
connected to the server..
Enter command: 
```

# Remote signer :warning: (Work in progress)

We are currently in the process of implementing a full C/C++ implementation of the [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) that runs entirely in the nRF9160DK and nRF5340DK boards. 

## Usage

This is still work in progress and there are still some missing features. 
One of the limitations of running a remote signer in an ARM Cortex-M33 cpu is the inability to run scrypt in a reasonable time. For that reason we only support PBKDFv2 keystores. The keystore import requires 1 minute so be patient. In order to convert scrypt keystores, we have also included in the repo a very simple python script [scrypt2pbkdf.py](scrypt2pbkdf.py). You need to pass three arguments: the password of the scrypt keystore, the path to the scrypt keystore and the path for the output keystore in pbkdfv2 format. The output keystore will use the same password as the input keystore.

```
python scrypt2pbkdf.py 123456789 scrypt_keystore.json pbkdf2keystore.json
```

The remote signer listen for HTTP requests over a serial port, process them and sends the response over serial too. If you want to test it with a Eth 2.0 client you first need to setup a Socket to Serial bridge, which is provided by [bridge.go](remote-signer/bridge.go). In order to build it you have to run `go mod init bridge`, `go mod tidy` and `go build`. You must specify the serial port when launching it (`./bridge [-port=SOCKETPORT] [-v] <COMport>`). Here are some output examples:

<details>
  <summary>Expand</summary>
  
```
./bridge -v com7
Connected to serial port com7
Listening on port 8080
REQUEST
POST /eth/v1/keystores HTTP/1.1
Content-Type: application/json
User-Agent: PostmanRuntime/7.29.0
Accept: */*
Postman-Token: 3970b383-070e-4d04-9990-0d755d9a0c2f
Host: localhost:8080
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
Content-Length: 1212

{
    "keystores": [
        "{
    \"crypto\": {
        \"kdf\": {
            \"function\": \"pbkdf2\",
            \"message\": \"\",
            \"params\": {
                \"dklen\": 32,
                \"c\": 262144,
                \"prf\": \"hmac-sha256\",
                \"salt\": \"8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900\"
            }
        },
        \"checksum\": {
            \"function\": \"sha256\",
            \"params\": {},
            \"message\": \"e5b9369f4f60f6c8bf4982254a7c1989e243d1ce372af4a6b0ad50a20f33bfa4\"
        },
        \"cipher\": {
            \"function\": \"aes-128-ctr\",
            \"params\": {
                \"iv\": \"7ea5abd19a7747ddac97b3951ade63a5\"
            },
            \"message\": \"ac099b2371f36ed9356e03918ddbcb231b3d018ef5d3b5b5041ecc2b83d56a2a\"
        }
    },
    \"description\": \"\",
    \"pubkey\": \"ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2\",
    \"path\": \"m/12381/3600/0/0/0\",
    \"uuid\": \"cc260592-1cf5-40d7-bc5a-44eaaa298d06\",
    \"version\": 4
}"
    ],
    "passwords": [
        "123456789"
    ]
}

RESPONSE
HTTP/1.1 200 Success response
Content-Type: application/json
Content-Length: 48

{"data": [{"status": "imported","message": ""}]}

REQUEST
POST /api/v1/eth2/sign/0xae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2 HTTP/1.1
Content-Type: application/json
User-Agent: PostmanRuntime/7.29.0
Accept: */*
Postman-Token: 72df661d-c1c5-4fe5-ba40-91321a31f6a4
Host: localhost:8080
Accept-Encoding: gzip, deflate, br
Connection: keep-alive
Content-Length: 1636

{
    "signingRoot": "0x041d15309fb66fc416428e429a771628e3e4fb62b24a8c02a5ce1661115e24c6",
    "type": "aggregate_and_proof",
    "fork_info": {
        "genesis_validators_root": "0x043db0d9a83813551ee2f33450d23797757d430911a9320530ad8a0eabc43efb",
        "fork": {
            "previous_version": "0x00001020",
            "current_version": "0x01001020",
            "epoch": "36660"
        }
    },
    "aggregate_and_proof": {
        "aggregator_index": "273938",
        "aggregate": {
            "aggregation_bits": "0xb64dfc7effa97fb4ef95e77cff7362ed3d",
            "data": {
                "slot": "2180640",
                "index": "41",
                "beacon_block_root": "0x86e3699b0baa4861dd33cb5a40ba046e085e6b4822a37f47d63f9d46143bae58",
                "source": {
                    "epoch": "68144",
                    "root": "0x94a27b264cb3ff7173259df70c393a259cb7685d39af4f8f3c5ff4e64d589f18"
                },
                "target": {
                    "epoch": "68145",
                    "root": "0x86e3699b0baa4861dd33cb5a40ba046e085e6b4822a37f47d63f9d46143bae58"
                }
            },
            "signature": "0x820378465e6de17bf7d2e88ec2737475ba1bde918b8ad087ff12dee12290c7021c88a054fd4fcef936df4d11f46fe6bf08a69b7c3969c6b31efd664c762587dfc428c34f55155bbacddfc0e3e03e97a6a435597af5fa4fb95f74e0835f0a8fcb"
        },
        "selection_proof": "0x8753799e8ea4981df0af22ff40b6fa54d6c4f73a8766d585f58917585253ba587c4074c5e835448a3a885755776639ea08eb42b70d6f391c756eb13612d4b9ba11a3ae31d5d855bec6a6fb1ba8df8b1e9f10795eb1fed268569cde801ae5276a"
    }
}

RESPONSE
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 211

{"signature": "0xa73365dc1a4054dccb12fae44dc93b697c2425241bb28eeb2e74c1562ff210f9687a122f3849e7eb60925c0ea7f7d68006b887c9bea436665d443c780cb386dd39ac3ede509506eb5f40535f4a16c94d1f5ddb9b3291149885de83f8f508b6b4"}
```
</details>

## Remote signer in GO using CLI backend

This module provides remote signer functionality using CLI as backend. See [remote-go](remote-go) README.
