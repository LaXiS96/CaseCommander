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

## State of the art (as of 2020/05/28)
- Design of supporting circuitry for variable voltage and PWM control is complete, yet to be tested and validated
- Initial firmware project structure

## Development environment
I first tried Windows Subsystem for Linux and it worked well up to the point of actually trying to flash the MCU from inside WSL itself (TL;DR: WSL does not expose USB devices).
Then I tried MSYS2, but I didn't even bother once I saw it doesn't show USB devices either.
So I simply installed Ubuntu 20.04 Server inside a VM and set up VSCode with its Remote SSH extension, which by the way works really well.

- Ubuntu 20.04 with these packages: `build-essential python-is-python3 openocd libncurses5`
- Visual Studio Code with the following extensions:
    - C/C++
    - Cortex-Debug
    - Remote - SSH (since I'm running VSCode on Windows and remoting into the Ubuntu VM)
- [gnu-arm-embedded](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) 9-2020-q2-update

## Used libraries
- [libopencm3](https://github.com/libopencm3/libopencm3)
- [FreeRTOS](https://www.freertos.org/) 10.3.1

## Building
1. Clone repository including submodules

        git clone --recurse-submodules https://github.com/LaXiS96/CaseCommander.git
        cd firmware/

2. Edit these files as needed:
    - `env.sh` for toolchain location
    - `.vscode/c_cpp_properties.json` for VSCode IntelliSense
    - `.vscode/launch.json` for Cortex-Debug

3. Set up environment

        . env.sh

4. Build libopencm3 (needed only once)

        make -C lib/libopencm3

5. Build

        make

6. Debug

        # Run "Cortex Debug" configuration in VSCode

7. Flash

        make flash
