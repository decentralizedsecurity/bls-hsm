# ETH 2.0 Hardware Remote Signer 

The aim of this project is to develop a working prototype for an Eth 2.0 remote signer that follows [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) for the ARM Cortex-M33 architecture. We take advantage of ARM TrustZone to protect the private keys of the validator, critical parts of the code that require access to private keys are run in the secure world. Our prototype is developed specifically for the [nRF9160DK](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk) and [nRF5340DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) boards, although it should be easy to port to other boards using an ARM Cortex-M33. We use the [blst](https://github.com/supranational/blst) library by suprational to implement the BLS signature and related methods

This project is founded by the [Eth2 staking community grants](https://blog.ethereum.org/2021/02/09/esp-staking-community-grantee-announcement/), from [Etherum Fundation](https://ethereum.foundation/)

We have implemented the following tools:

- [`cli`](./cli). A command line interface to create BLS keypairs, sign and verify messages.
- [`remote-signer`](./remote-signer). An Eth2.0 remote signer that protects private keys using ARM TrustZone.

Those two tools are provided as separate nRF Connect projects and requiere a nRF5340DK or a nRF9160DK board to run. They relay in the `secure_partition` project, that is run the secure world and contains blst function calls that involve private keys, using Trusted Firmware-M (TF-M).

Lastly, we have also implemented a simple remote signer in Go that uses our CLI as backend.

- `remote-signer-go`.

## Boards compatibility :electric_plug:
| | nRF9160DK | Thingy:91 | nRF5340DK | Thingy:53 |
| --- | --- | --- | --- | --- |
|**Tools**|
| [cli](./cli) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: |
| [remote-signer](./remote-signer) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: |
|**Features**|
| [Trust-Zone](#trust-zone) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: |
| [Serial communication](#serial) | :x: | :x: | :microscope: | :microscope: |
| [LTE](#lte) | :heavy_check_mark: | :microscope: | :x: | :x: |
| [Bluetooth](#bluetooth) | :soon: | :soon: | :x: | :x: |
| [Touchpad-screen](#touchpad-screen) | :soon: | :soon: | :microscope: | :soon: |

 - :heavy_check_mark: Full support
 - :large_orange_diamond: Partial support
 - :x: No support
 - :soon: On the to-do list
 - :microscope: Experimental

## Features :page_with_curl: <span id="features"><span>

### ARM Trust-Zone <span id="trust-zone"><span>
A secure partition is used to store sensitive data (keys) and allows acces to them. For more information on how it works, you can check out [`secure_partition` documentation](./secure_partition).

### Serial communication <span id="serial"><span>
TODO

### LTE <span id="lte"><span>
TODO

### Bluetooth <span id="bluetooth"><span>
TODO

### Touchpad screen <span id="touchpad-screen"><span>
[LVGL](https://docs.lvgl.io/8.3/) is used to add a touchpad screen to the board. It allows the user to interact with the board without using a terminal on their computer.

The development has been done using an Adafruit 2.8" TFT Touch Shield.

To make a build with this feature, `display.conf` kconfig fragment must be added to the build configuration.

## Dependencies :link:

We have developed this project using the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK), which is based on the Zephyr RTOS. In order to install the required dependencies it is recommended to follow this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html). Currently, only 2.2.0 version of the toolchain is supported. If you are using Linux, you can try the script `linux_dependencies.sh` to install all the required dependencies, although we recommend following the official guide.

In order to build the [Blst](https://github.com/supranational/blst) library, we provide the script `build_libs.sh`, that clones and builds the library in Linux providing the ARM compiler as a parameter. If you are using other operating system you can try building the library using the scripts provided by suprational in their repo.

## Building the tools :pick:

In order to build the nRF Connect projects you can either follow these [steps](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_programming.html#gs-programming-cmd) or use the `build.sh` script. There is also a `flash.sh` script to flash the board with a previously built project.

Another way to build the tools and flash the board with the project is to use the [nRF Connect for VS Code](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-vs-code) extension pack for [VS Code](https://code.visualstudio.com) which allows the user to build, flash and debug the tools, among other features.

You can find more information about building and running the other tools in their respective folders

## Testing and debugging tools :bug:

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