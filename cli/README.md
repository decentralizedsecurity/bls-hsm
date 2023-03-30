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
- *getsize*: returns the number of keys that have been stored.
  ```
  uart:~$ getsize
  2
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

## CLI tests

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
