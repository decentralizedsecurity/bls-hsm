# ETH 2.0 Hardware Remote Signer 

The aim of this project is to develop a working prototype for an Eth 2.0 remote signer that follows [EIP-3030](https://eips.ethereum.org/EIPS/eip-3030)/[Web3Signer ETH2 Api](https://consensys.github.io/web3signer/web3signer-eth2.html) for the ARM Cortex-M33 architecture. We take advantage of ARM TrustZone to protect the private keys of the validator, critical parts of the code that require access to private keys are run in the secure world. Our prototype is developed specifically for the [nRF9160DK](https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk) and [nRF5340DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) boards, although it should be easy to port to other boards using an ARM Cortex-M33. We use the [blst](https://github.com/supranational/blst) library by suprational to implement the BLS signature and related methods

This project is founded by the [Eth2 staking community grants](https://blog.ethereum.org/2021/02/09/esp-staking-community-grantee-announcement/), from [Etherum Fundation](https://ethereum.foundation/)

We have implemented the following tools:

- [`cli`](./cli). A command line interface to create BLS keypairs, sign and verify messages.
- [`remote-signer`](./remote-signer). An Eth2.0 remote signer that protects private keys using ARM TrustZone.

Those two tools are provided as separate nRF Connect projects and requiere a nRF5340DK or a nRF9160DK board to run. They relay in the [`secure_partition`](./secure_partition/) project, that is run the secure world and contains blst function calls that involve private keys, using Trusted Firmware-M (TF-M).

Lastly, we have also implemented a simple remote signer in Go that uses our CLI as backend.

- `remote-signer-go`.

## Boards compatibility :electric_plug:
| | nRF9160DK | Thingy:91 | nRF5340DK | Thingy:53 | nRF7002DK |
| --- | --- | --- | --- | --- | --- |
|**Tools**|
| [cli](./cli) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: |  |
| [remote-signer](./remote-signer) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: | :heavy_check_mark: |
|**Features**|
| [Trust-Zone](#trust-zone) | :microscope: | :microscope: | :heavy_check_mark: | :microscope: | :soon: |
| [Serial communication](#serial) | :x: | :x: | :heavy_check_mark: | :microscope: | :heavy_check_mark: | :heavy_check_mark: |
| [LTE](#lte) | :heavy_check_mark: | :microscope: | :x: | :x: | :x: |
| [WiFi](#wifi) | :x: | :x: | :x: | :x: | :heavy_check_mark: |
| [Bluetooth](#bluetooth) | :soon: | :soon: | :soon: | :x: | :x: |
| [Touchpad-screen](#touchpad-screen) | :soon: | :soon: | :heavy_check_mark: | :soon: | :soon: |

 - :heavy_check_mark: Full support
 - :large_orange_diamond: Partial support
 - :x: No support
 - :soon: On the to-do list
 - :microscope: Experimental

Note: the prototype is developed for the boards shown in the table. However, it should be easily ported to other boards having ARM Cortex-M33.

## Features :page_with_curl: <span id="features"><span>

### ARM Trust-Zone <span id="trust-zone"><span>
A secure partition is used to store sensitive data (keys) and allows acces to them. For more information on how it works, you can check out [`secure_partition` documentation](./secure_partition).

### Serial communication <span id="serial"><span>

You can send commands and receive information through a serial port. To do this, you can use the [browser application](#browser-app) or a [terminal emulator](#terminal-emulator).

### LTE <span id="lte"><span>
This feature is under development

### WiFi <span id="wifi"><span>
A version of the remote signer using WiFi is currently supported for nRF7002DK. To properly configure this version, you must specify the SSID and password the board should connect to by editing [CONFIG_STA_SAMPLE_SSID](./remote-signer-7002/socket.conf#L48) and [CONFIG_STA_SAMPLE_PASSWORD](./remote-signer-7002/socket.conf#L49) configurations inside [socket.conf](./remote-signer-7002/socket.conf) file, which must be used along with prj.conf in the building process. The connection status logs as well as IP and port assigned will be shown via serial communication in a terminal that must be previously connected. Once the board is connected to the desired SSID and the socket is listening, it is ready to handle requests.

### Bluetooth <span id="bluetooth"><span>
This feature is under development

### Touch screen <span id="touchpad-screen"><span>
[LVGL](https://docs.lvgl.io/8.3/) is used to add a touchpad screen to the board. It allows the user to interact with the board without using a terminal on their computer.

The development has been done using an Adafruit 2.8" TFT Touch Shield.

To enable this feature, uncomment the following line in [`CMakeLists.txt`](./remote-signer/CMakeLists.txt).
```
set(TOUCHSCREEN TRUE)
```
In addition, the configuration must be uncommented in [`prj.conf`](./remote-signer/prj.conf):
```
# Configure Touchscreen
CONFIG_LV_Z_MEM_POOL_NUMBER_BLOCKS=8
CONFIG_DISPLAY=y
CONFIG_DISPLAY_LOG_LEVEL_ERR=y

CONFIG_LVGL=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_USE_LOG=y
CONFIG_LV_USE_LABEL=y
CONFIG_LV_USE_MSGBOX=y
CONFIG_LV_USE_KEYBOARD=y
CONFIG_LV_FONT_MONTSERRAT_14=y
CONFIG_LV_LABEL_LONG_TXT_HINT=y

CONFIG_TFM_SECURE_UART1=n
CONFIG_TFM_LOG_LEVEL_SILENCE=y
```

## Dependencies :link:

We have developed this project using the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK), which is based on the Zephyr RTOS. In order to install the required dependencies it is recommended to follow this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html). Currently, only 2.2.0 version of the toolchain is supported. If you are using Linux, you can try the script `linux_dependencies.sh` to install all the required dependencies, although we recommend following the official guide.

In order to build the [Blst](https://github.com/supranational/blst) library, we provide the script `build_libs.sh`, that clones and builds the library in Linux providing the ARM compiler as a parameter. If you are using other operating system you can try building the library using the scripts provided by suprational in their repo.

## Building the tools :pick:

In order to build the nRF Connect projects you can either follow these [steps](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_programming.html#gs-programming-cmd) or use the `build.sh` script. There is also a `flash.sh` script to flash the board with a previously built project.

Another way to build the tools and flash the board with the project is to use the [nRF Connect for VS Code](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-vs-code) extension pack for [VS Code](https://code.visualstudio.com) which allows the user to build, flash and debug the tools, among other features.

You can find more information about building and running the other tools in their respective folders

## Testing and debugging tools :bug:

### Send commands
Commands are sent to the board through a serial port. In the same way, information is received through the same serial port.

#### Browser application <span id="browser-app"><span>
Browser application under development that allows an user-friendly interaction with remote-signer. It allows to send some predefined test commands. The user can also type his own commands. In addition, it displays the response given by remote-signer.

To use the application, open `remote-signer/test_rs/rs_user_interface.html` in Google Chrome.

#### Terminal emulator <span id="terminal-emulator"><span>
To see the UART output and send commands, you can connect the development kit with a terminal emulator, like PuTTY, or use the one provided by VS Code.

The connection to the serial port must be made with the following configuration:
- speed: 115200 baud
- data bits: 8
- stop bits: 1
- parity: none
- flow control: none

### Debug secure-partition

The serial port is needed to use `cli` and `remote-signer` projects. In addition, there is the option to view error and debug messages from the `secure partition` (TF-M).
- nRF5340DK: Connect the pin P0.25 with your computer using a serial-USB converter.
- nRF9160DK: Currently not implemented