# ETH 2.0 Hardware Remote Signer 

The aim of this project is to develop a working prototype for an Eth 2.0 remote signer that follows [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) for the ARM Cortex-M33 architecture. We take advantage of ARM TrustZone to protect the private keys of the validator, critical parts of the code that require access to private keys are run in the secure world. Our prototype is developed specifically for the [nRF9160DK](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk) and [nRF5340DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) boards, although it should be easy to port to other boards using an ARM Cortex-M33. We use the [blst](https://github.com/supranational/blst) library by suprational to implement the BLS signature and related methods

This project is founded by the [Eth2 staking community grants](https://blog.ethereum.org/2021/02/09/esp-staking-community-grantee-announcement/), from [Etherum Fundation](https://ethereum.foundation/)

We have implemented the following tools:

- [`cli`](./cli/README.md). A command line interface to create BLS keypairs, sign and verify messages.
- `remote-signer`. An Eth2.0 remote signer that protects private keys using ARM TrustZone.

Those two tools are provided as separate nRF Connect projects and requiere a nRF5340DK or a nRF9160DK board to run. They relay in the `secure_partition` project, that is run the secure world and contains blst function calls that involve private keys, using Trusted Firmware-M (TF-M).

Apart from those tools, we also provide a port of them that can be run without the need of ARM TrustZone and instead of exposing the functionality over the serial port, use a TCP socket:

- `cli-socket`.
- `remote-signer-socket`.

Those two tools can be build and run in Linux, MacOS and Windows directly, and would allow anyone to test our project before considering buying a supported hardware.

Lastly, we have also implemented a simple remote signer in Go that uses our CLI as backend.

- `remote-signer-go`.

## Dependencies :link:

We have developed this project using the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK), which is based on the Zephyr RTOS. In order to install the required dependencies it is recommended to follow this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html). Currently, only 2.0.0 version of the toolchain is supported. If you are using Linux, you can try the script `linux_dependencies.sh` to install all the required dependencies, although we recommend following the official guide.

In order to build the [Blst](https://github.com/supranational/blst) library, we provide the script `build_libs.sh`, that clones and builds the library in Linux providing the ARM compiler as a parameter. If you are using other operating system you can try building the library using the scripts provided by suprational in their repo.

## Building the tools :pick:

In order to build the nRF Connect projects you can either follow these [steps](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_programming.html#gs-programming-cmd) or use the `build.sh` script. There is also a `flash.sh` script to flash the board with a previously built project.

Another way to build the tools and flash the board with the project is to use the [nRF Connect for VS Code](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-vs-code) extension pack for [VS Code](https://code.visualstudio.com) which allows the user to build, flash and debug the tools, among other features.

You can find more information about building and running the other tools in their respective folders

## Testing and debugging tools:bug:

### Send commands

Commands are sent to the board through a serial port. In the same way, information is received through the same serial port. To see the UART output and send commands, you can connect the development kit with a terminal emulator, like PuTTY, or use the one provided by VS Code.

The connection to the serial port must be made with the following configuration:
- speed: 115200 baud
- data bits: 8
- stop bits: 1
- parity: none
- flow control: none

### Debug secure-partition

The serial port is needed to use `cli` and `remote-signer` projects. In addition, there is the option to view error and debug messages from the `secure partition` (TF-M).
- nRF5340DK: Connect the pin P0.25 with your computer using a serial-USB converter.
- nRF9160DK: ?

# Remote signer

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

## Remote Signer tests

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