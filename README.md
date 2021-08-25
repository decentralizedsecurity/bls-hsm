# bls-hsm

## Design :page_with_curl:
Both projects emulate the behaviour of a command API that internally uses [blst library](https://github.com/supranational/blst#blst). They have been implemented with Segger Embedded Studio for the execution in Nordic Semiconductor nRF9160 board.


## Installation
In order to build the project, it's recommended to use nRF Connect SDK as it is explained in this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html).



## Usage
The commands that are supported are:
- *keygen*: generates a 32-bytes secret key and a 48-bytes public key randomly.
  ```
  uart:~$ keygen
  Public key:
  0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e
  ```
- *publickey*: shows the last public key that has been generated.
  ```
  uart:~$ publickey
   Public key:
   0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e
  ```
- *signature "publickey" "message"*: produces a 96-bytes signature with the message that has been introduced and after choosing a public key that has had to be generated before.
  ```
  uart:~$ signature 0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05c
  c25f4d8069a79b002ae665b9ad35ce800a0e 5656565656565656565656565656565656565656565
  656565656565656565656
  Signature:
  0xa60e2f24827943379f8377d6bf7a126ef8def56b08ca4cdd958954b5f56091f7cd49d251481f4cd6316396a2a4f4398c09e2cfda6ea16416dffdac687cf06db0e4d3a0ec83b4016c835b27f84325342199f724abd092cb7957177f5f30dcbe19
  ```
- *verify*: signature verification.
  ```
  uart:~$ verify
  Success
  ```
- *getkeys*: returns the public keys that have been generated.
  ```
  uart:~$ getkeys
  {'keys':['a2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e']}
  ```
- *benchmark*: runs the code from the key generation to the signature and it outputs the time needed to perform each operation.
  ```
  uart:~$ benchmark
  Public key:
  0xb924ae791b157e2e29017d814b016d33d8af39855a4be477a8e7050fb8a851b645f56a6366332926595d3c3c6955e085

  Benchmark for key generation: 229 ms

  Message:
  0x5656565656565656565656565656565656565656565656565656565656565656

  Benchmark for message hash: 430 ms

  Signature:
  0x8af4ffeb87eb37a38f6b261dfdcfb3f7eff33b5d55a520c50e79c3c5687ab12f0389962dac4378930a8ecccf10c993cb0e0e83476295f6dc45a7548c35d583ddb6b4d2d2bd7921462030a334c625fb4b7cad7c1c2ad046fe8878bb61bf3b154f

  Benchmark for signature: 519 ms
  ```


## Implementations :pick:
**1. shell_module**: This project uses blst static library that has been compiled for Cortex-M33 architecture with the following command:
```
./build.sh CC=/some/where/toolchain/opt/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gcc-10.2.1 -mcpu=cortex-m33 flavour=elf -fno-pie
```

**2. shell_module_basic**: This project uses source files from blst library. They can be found in *src* folder and they have not been modified expect [line 70 in vect.h file](https://github.com/supranational/blst/blob/master/src/vect.h#L370) which has been commented in order to avoid errors during compilation.


PS. The *prj.conf* file has been modified in both projects because default size caused stack overflow from the UART thread. Current size is 49152 bytes.

**3. secure_module**: This module contains blst function calls that involve usage and storage of secret keys, using Secure Partition Manager (SPM).

## Benchmarks :hourglass_flowing_sand:
**1. shell_module**:
```
uart:~$ benchmark
Secret key:
0x2f0455fe024bcf4232c044248106e3964a13043a8a16e5798d0461850a11b475 (no longer shown, as it is generated in ARM TrustZone)
Public key:
0x813f4f84cf1663c4252c63215b2b8edae2ae5d5e062a24b27752a7f6e0462bde0b256ace4cd9e9dd7e4c0b6507d3f20f

Benchmark for key generation: 242 ms

Message:
0x5656565656565656565656565656565656565656565656565656565656565656

Benchmark for message hash: 453 ms

Signature:
0xb8a1012432413ad2d2f5fed7a38ef48f0954ddb32ddbc081b42326644ddee66ec851e2c173a28c218fe4c4b5f7e533cb14c81d78f2ca315f428b66e73572526fe1ba9ec56736c04dd76c80188d49d2facd3f15b12dcf6f5afcf156f5b2746920

Benchmark for signature: 532 ms
```

**2. shell_module_basic**
```
uart:~$ benchmark
Secret key:
0x696396a6b9ea18323bd08c869e68cd74c69f84acfac71a1b2473ac29d6879625
Public key:
0xa63e9ac221fee7f8111ad86ab3fd215a703974fb4cc0868b96aea70265a5475f26116de9b07f2340d2b2fa6817c63109

Benchmark for key generation: 294 ms

Message:
0x5656565656565656565656565656565656565656565656565656565656565656

Benchmark for message hash: 582 ms

Signature:
0x910864ac1b0f0417ba29db4a678948b9e6d7f3542e7215017bbf28846449269e130b8d1ce633c3a094165dabed6b46d913cc0a855f1567b78ebc12c45d5a178bdd164859a598e844a622da88ef71c0d41b9561d2bc55eb0284b011d6f0531e0c

Benchmark for signature: 631 ms
```

## To Do: :ballot_box_with_check:
- [x] Benchmark command.
- [ ] Shell script for setting up the work environment.
- [ ] Include Nordic Security Backend in order to take advantage of the 144-byte random number used for the 32-byte vector in secret key generation function.
- [x] Execution of the code in ARM TrustZone.
