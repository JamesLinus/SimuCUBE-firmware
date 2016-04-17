# SimuCUBE firmware
This is a Firmware for [SimuCUBE](http://granitedevices.com/simucube) hardware. SimuCUBE is a [IONI drive](http://granitedevices.com/miniature-servo-drive-ioni/) based motor control board where a single AC/DC/BLDC/Stepper motor may be controlled over USB and with the help of on-board STM32F4 microcontroller. 

This firmware attempts to implement racing game steering wheel controller on a SimuCUBE embedded processor. To see latest development version, check out the brankch named "develop" instead of "master".

## STATUS
The current status of firmware is a "test environment" without actual meaningful functionality.

## COMPILING

###Tools needed

a) GCC compiler for ARM

A GCC compiler toolchain for ARM is needed to compile the source code. Get GCC ARM toolchain from https://launchpad.net/gcc-arm-embedded

b) GNU Make

Source code of firmware and makefirmware utily is easily compiled by `make` command. For that, the GNU make must be installed in system. 

For Windows system, an easy way to obtain make is to install "msys-base" and "msys-make" packages from MinGW Installation Manager. MinGW site: http://www.mingw.org/ Download the installation manager at: http://sourceforge.net/projects/mingw/files/Installer/ . You may also add MSys binaries to system environment variable called PATH to have make command to work from command prompt without typing full executable path every time. If the default folder was used, make.exe is located at: C:\MinGW\msys\1.0\bin.

###Tips
1) Make sure that system path is set so that make and arm-none-eabi-gcc are found "globally" in the system. I.e. when you enter in command `make` or `arm-none-eabi-gcc` in a command prompt, you don't get any kind of "command not found" message.

2) If you're compiling firmware on Linux, you may need to build makefirmware utily before firmware compilation. You can do it by entering in subdirectory utils/makefirmware and running `make` command (assuming that native gcc toolchain in addition to ARM toolchain is installed). You can check installation of gcc by entering command `gcc` alone on command prompt and verify the response like you did in the step 1.

###Compiling firmware

- Open command prompt and go to project directory where Makefile lies.
- Run make by entering command: `make` (Compiler outputs lots of stuff)
- When comipation is succeed, something like following text should become as last output lines:

.

    arm-none-eabi-objcopy -O binary simucube.elf simucube.bin
    arm-none-eabi-size  simucube.elf
    text    data     bss     dec     hex filename
    48936     548    5296   54780    d5fc simucube.elf

If so, then just simucube.bin/.elf is the binary file going to SimuCUBE STM32F4 MCU. 

##INSTALLING FIRMWARE

The binary may be loaded to SimuCUBE with two methods:

1) By uploading through USB in DFU (Device Firmware Upgrade) mode: http://granitedevices.com/wiki/Installing_SimuCUBE_firmware

2) By using debugger device (such as comaptible JTAG or SWD interfaces like Segger J-Link or ST-LINK/V2). For development purposes, getting for example a [J-Link EDU](https://www.segger.com/j-link-edu.html) and setting up a [GDB based debug enfironment in Eclipse](https://www.segger.com/IDE_Integration_Eclipse.html) might pay back very fast in saved development time.

## LICENSE
This project has been licensed with Apache liecnse version 2.0. For full details, see http://www.apache.org/licenses/LICENSE-2.0.