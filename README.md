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

## Development tools used
- Ubuntu Focal in Windows Subsystem for Linux (WSL). Required packages: build-essential, python-is-python3
- Visual Studio Code with the following extensions:
    - Remote - WSL
    - C/C++
    - Cortex-Debug
- [gnu-arm-embedded](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) 9-2019-q4-major
- [OpenOCD](http://openocd.org/)
- [libopencm3](https://github.com/libopencm3/libopencm3)
- [FreeRTOS](https://www.freertos.org/)

## Building
1. Clone repository including submodules

        git clone --recurse-submodules https://github.com/LaXiS96/CaseCommander.git
        cd firmware/

1. Set up environment (adds toolchain to PATH, edit for your environment)

        . env.sh

2. Build libopencm3 (needed only once)

        make -C lib/libopencm3

3. Build CaseCommander

        make
