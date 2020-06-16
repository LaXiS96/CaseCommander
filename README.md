# CaseCommander
Computer case environmental controller with USB control interface

## Planned features
- Fan speed control
    - Classic 3pin fans via variable supply voltage
    - PWM 4pin fans via direct PWM signaling
    - Various modes (static, threshold, curve, target [PID], TBD)
- Temperature sensor readout and feedback
- RGB LED strip control
    - SK9822 support
    - Various modes (TBD)
- All functions should be inspectable and controllable from some form of PC (GUI) software

## State of the art (as of 2020/06/16)
- Design of supporting circuitry for variable voltage and PWM control is complete, yet to be tested and validated
- Implemented firmware features:
    - USB communication
    - fan tachometer reading
    - SK9822 initial test implementation using SPI peripheral

## Hardware
The chosen MCU is an ARM Cortex-M3 STM32F103C8, which is readily available from chinese resellers on a prebuilt board known as Blue/Black Pill.

There is also some supporting circuitry, consisting of a N-channel MOSFET (IRF3205 or IRF520N or equivalent) and gate driver (TC4420 or equivalent), one combo to drive each variable voltage control channel.

## Development environment
### Cross-platform? (Linux on Windows)
First try, Windows Subsystem for Linux: worked well up to the point of actually trying to flash the MCU -> WSL does not expose USB devices.

Second try, MSYS2: same issue as WSL.

My current environment is a Ubuntu 20.04 Server virtual machine which I SSH into from Visual Studio Code using its "Remote - SSH" extension (works really well). As a debugger probe I am using a SEGGER J-Link.

- Ubuntu 20.04 with these packages: `build-essential python-is-python3 libncurses5` (also `openocd` if you don't want to use proprietary software, but read on...)
- Visual Studio Code with these extensions: C/C++, Cortex-Debug, Cortex-Debug: Device Support Pack - STM32F1
- [gnu-arm-embedded](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) toolchain
- [J-Link Software](https://www.segger.com/downloads/jlink/) for Linux 64-bit

    gnu-arm-embedded and J-Link software binaries paths must be set in the PATH (eg. by setting them in `~/.profile` like so: `export PATH=~/gcc-arm-none-eabi-9-2020-q2-update/bin:~/JLink_Linux_V680b_x86_64:$PATH`)

As far as the debugging software is concerned, I first tried using OpenOCD (as you can see from the included debug launch configuration and `openocd.cfg` file) and it worked for simple flashing and debugging, but started crashing once I set up SWO trace in Cortex-Debug. The error was `jaylink_swd_io() failed: JAYLINK_ERR_DEV_NO_MEMORY` followed by a USB reenumeration of the probe and numerous other errors; I did not bother troubleshooting the cause (was it because it's running in a VM? who knows...), so I simply switched to SEGGER's official software, which is available and well supported for Linux.

## Development utilities
### JLink connections (SWD mode)
| JLink  | Pin | STM32 | Pin
| ------ | --- | ----- | -
| VTref  | 1   | 3.3V  |
| GND    | 4   | GND   |
| SWDIO  | 7   | PA13  | 34
| SWCLK  | 9   | PA14  | 37
| SWO    | 13  | PB3   | 39
| nRESET | 15  | NRST  | 7

## Used libraries
- [libopencm3](https://github.com/libopencm3/libopencm3)
- [FreeRTOS](https://www.freertos.org/)

## Building
1. Clone repository including submodules

        git clone --recurse-submodules https://github.com/LaXiS96/CaseCommander.git
        cd firmware/

2. Build libopencm3 (needed only once after cloning)

        make -C lib/libopencm3

3. Build

        make

4. Flash

        make flash

5. Debug

        # Launch any "Cortex-Debug ..." configuration in VSCode
        # Configurations ending in "... Launch" will flash and reset the MCU; those ending in "... Attach" will simply attach the debugger without flashing nor resetting

## Useful links
- https://github.com/ve3wwg/stm32f103c8t6
- https://github.com/libopencm3/libopencm3-examples/tree/master/examples/stm32/f1
