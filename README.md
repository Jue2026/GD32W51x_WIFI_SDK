# Welcom to the GD32W51x Wi-Fi SDK

[中文](README_CN.md) | **English**

Wi-Fi SDK for the GD32W51x MCU family. The current release is **V1.0.5**. The SDK provides a complete development base, from the bootloader, RTOS and peripheral drivers up to the Wi-Fi stack, network stack, application framework and example projects.

The GD32W51x family integrates 2.4 GHz Wi-Fi 4 (IEEE 802.11b/g/n) with 20/40 MHz bandwidth, an Arm Cortex-M33 core running at up to 180 MHz, 2 MB Flash and 448 KB SRAM.

## Quick start

```bat
git submodule update --init --recursive
.\gcc_build_ns.bat
.\gcc_download_ns.bat DAPLINK
```

After flashing, connect a serial terminal to the LOG UART at **115200 8N1** to see the boot log, and type `help` to list the available commands.

## Repository layout

| Directory or file | Description |
| --- | --- |
| `config/` | Flash/SRAM layout, board definitions (`platform_def.h`), firmware version and global build configuration. |
| `docs/` | Application notes (`EN/`, `CN/`) and board schematics (`Schematic/`). |
| `GD32W51x_Addon/` | Bundled download and debug tools such as OpenOCD. |
| `MBL/` | Main bootloader. `source/` is the TrustZone version, `source_ns/` is the non-TrustZone version. |
| `NSPE/` | Non-secure firmware: CMSIS, standard peripheral library, Wi-Fi driver, network stack, applications and examples. |
| `NSPE/Firmware/` | Standard peripheral library and Wi-Fi driver. |
| `NSPE/Example/` | Peripheral and Wi-Fi porting examples. |
| `NSPE/WIFI_IOT/` | Default Wi-Fi IoT application (`app/`, `bsp/`, `network/`, `wifi/`, `cloud/`, ...). |
| `NSPE/Project/WIFI_IOT/` | GCC/Keil/IAR projects and post-build scripts for the Wi-Fi IoT firmware. |
| `PROT/` | Security and protocol components: Mbed TLS, TF-M, etc. |
| `ROM-EXPORT/` | ROM exported symbols, platform headers and the ROM Mbed TLS interface. |
| `scripts/` | Toolchain configuration, image generation (`imgtool/`), FLM algorithms, certificates and the artifact output directory (`images/`). |
| `gcc_build_ns.bat` | Entry point for the GCC non-TrustZone firmware build. |
| `gcc_download_ns.bat` | Flashes the combined image with J-Link, GD-Link or DAP-Link. |
| `MultiProject*.uvmpw` / `MultiProject*.eww` | Keil / IAR multi-project workspaces (the `_NS` suffix is the non-TrustZone variant). |

> `MBL`, `NSPE`, `PROT` and `ROM-EXPORT` are managed as Git submodules. Initialize them after cloning:

```bash
git submodule update --init --recursive
```

## Documentation index

`docs/EN/` contains the English application notes; `docs/CN/` contains the Chinese counterparts:

| Document | Content |
| --- | --- |
| AN066 | Hardware Development Guide |
| AN079 / AN097 | Rapid Development Guide (boards, project structure, build and download flow) |
| AN080 | AT Command User Guide |
| AN081 | Base Command (serial console) User Guide |
| AN082 | Secure Boot User Guide |
| AN083 | Certification Test Guide |
| AN084 | Testing Guidelines for RF Indexes and Transmitting/Receiving Power |
| AN085 | Throughput and Power Consumption Test Guide |
| AN100 | Wi-Fi Development Guide |
| AN103 | TrustZone Development Guide |

`docs/Schematic/` contains the schematics for the `GD32W515P-EVAL` and `GD32W515T-START` boards.

## Hardware and development boards

The SDK supports two official boards. Both expose a reset button, a BOOT pin, an SWD debug port and a LOG UART:

| Board | Debug interface | Power |
| --- | --- | --- |
| GD32W515P-EVAL | J-Link SWD | J-Link / GD-Link |
| GD32W515T-START | J-Link SWD, on-board DAP-Link | USB (shared with the LOG UART) |

- **SWD connection**: the J-Link SWD header exposes `AGND`, `JCLK`, `JTWS` and `W3V3`. Wire them to `GND`, `SWCLK`, `SWDIO` and `VCC` on the debug probe to download and debug.
- **BOOT pin**: selects the run mode at startup; used together with the reset button for production programming or recovery.
- **LOG UART**: 115200 baud, 8 data bits, no parity, 1 stop bit. Used for log output and serial commands.

The board is selected with `CONFIG_BOARD` in `config/platform_def.h`. The default is `PLATFORM_BOARD_32W515T_START`:

| `CONFIG_BOARD` | LOG UART | TX pin | RX pin |
| --- | --- | --- | --- |
| `PLATFORM_BOARD_32W515T_START` | `USART1` | `PB15` | `PA8` |
| `PLATFORM_BOARD_32W515P_EVAL` | `USART2` | `PB10` | `PB11` |
| `PLATFORM_BOARD_32W515P_OTHER` | `USART0` | `PA0` | `PA1` |

The same file also configures the crystal frequency (`PLATFORM_CRYSTAL`, 40 MHz by default) and the XIP Flash source (`CONFIG_XIP_FLASH`, SiP Flash by default).

## Development environment

### Windows

Install the following and add them to the system `PATH`:

- CMake 3.15 or later.
- GNU Make (the scripts use the `Unix Makefiles` generator).
- Arm GNU Toolchain, providing `arm-none-eabi-gcc`, `arm-none-eabi-objcopy` and `arm-none-eabi-objdump`.
- 7-Zip, used by the tooling and release packaging.

To download with J-Link, also install the SEGGER J-Link software and run once:

```bat
scripts\setup_jlink.bat
```

This copies the GD32W51x FLM and device description shipped with this repository into the J-Link installation directory. For DAP-Link, run `scripts\setup_daplink.bat`. GD-Link and DAP-Link downloads rely on `GD32W51x_Addon/OpenOCD` and need no extra installation.

### Ubuntu

Ubuntu 18.04 and later are supported. Install the Arm GNU Toolchain, add its `bin` directory to `PATH`, and install the build dependencies:

```bash
sudo apt update
sudo apt install -y build-essential srecord libncurses5 libncursesw5
```

Verify the toolchain:

```bash
arm-none-eabi-gcc --version
cmake --version
make --version
```

## Building the firmware

### Windows

From the repository root:

```bat
gcc_build_ns.bat
```

The script deletes and recreates `cmake_build/`, so do not keep manual changes there.

### Linux

```bash
rm -rf cmake_build && mkdir cmake_build && cd cmake_build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH=./scripts/cmake/toolchain.cmake ..
make -j
```

### Build options

The default configuration is:

- Non-TrustZone build (`CONFIG_TZ_ENABLED=OFF`). The current GCC toolchain configuration does not support enabling TrustZone; use Keil/IAR for TrustZone projects, see AN103.
- FreeRTOS (`CONFIG_OS=FREERTOS`, `RTTHREAD` is also available).
- Flash-based Mbed TLS 3.x (`CONFIG_MBEDTLS_VERSION=3.x`, the ROM-based `2.17.0` is also available).
- Linking against the prebuilt Wi-Fi static library (`CONFIG_STATIC_LIB_BUILDED=ON`).

These cache variables are defined in the top-level `CMakeLists.txt`. Edit them there, or override them with `-D` on the `cmake` command line, and rebuild.

Application-level feature switches live in `NSPE/WIFI_IOT/app/app_cfg.h`:

| Macro | Default | Description |
| --- | --- | --- |
| `CONFIG_CONSOLE_ENABLE` | On | Master switch for the serial console. |
| `CONFIG_BASECMD` | On | Base command set (see AN081). |
| `CONFIG_ATCMD` | Off | AT command set (see AN080). |
| `CONFIG_IPERF_TEST` | On | iperf2 / iperf3 throughput test commands. |
| `CONFIG_SOFTAP_PROVISIONING` | Off | SoftAP provisioning. |
| `CONFIG_ALICLOUD_SUPPORT` | Off | Alibaba Cloud Link Kit; requires `CONFIG_EXTEND_MEMORY` to be enabled as well. |
| `CONFIG_MQTT` / `CONFIG_SSL_TEST` / `CONFIG_FATFS_SUPPORT` / `CONFIG_IPV6_SUPPORT` | Off | Corresponding optional components. |

Firmware addresses, image layout and component versions are defined in `config/config_gdm32.h` and `config/config_gdm32_ntz.h`. Before changing these layout parameters, make sure the bootloader and the upgrade flow remain compatible.

## Build artifacts

Debug artifacts are written to the GCC output directory of each project:

```text
MBL/project/GCC/output/bin/mbl-ns.axf         # Bootloader
NSPE/Project/WIFI_IOT/GCC/output/bin/nspe.axf # Application firmware
NSPE/Project/WIFI_IOT/GCC/output/bin/nspe.map # Linker map file
```

Post-build processing produces the flashable and OTA images:

```text
scripts/images/image-all.bin  # Full flash image, base address 0x08000000
scripts/images/image-all.hex  # Full image in Intel HEX format
scripts/images/image-ota.bin  # NSPE image used for OTA
```

## Flashing

After a successful build, run the following from the repository root (the argument must be upper case):

```bat
gcc_download_ns.bat JLINK
gcc_download_ns.bat GDLINK
gcc_download_ns.bat DAPLINK
```

The script downloads `scripts/images/image-all.bin` to `0x08000000`. For J-Link, the default executable path is `C:\Program Files (x86)\SEGGER\JLink\Jlink.exe`; adjust `JLINK_PATH` in `gcc_download_ns.bat` if your installation differs. GD-Link and DAP-Link use `GD32W51x_Addon/OpenOCD` and reset the target automatically after flashing.

## Serial console

Once the LOG UART is connected at 115200 8N1, the firmware offers two command sets.

### Base commands (`CONFIG_BASECMD`, enabled by default)

Type `help` to list all commands. Common ones are:

| Category | Commands |
| --- | --- |
| Wi-Fi STA | `wifi_open`, `wifi_close`, `wifi_scan`, `wifi_connect`, `wifi_disconnect`, `wifi_status`, `wifi_rssi` |
| Wi-Fi SoftAP | `wifi_ap`, `wifi_ap_adv`, `wifi_stop_ap`, `wifi_ap_provisioning` |
| Network configuration | `wifi_set_ip`, `wifi_set_channel`, `wifi_set_bw`, `wifi_mac_addr`, `wifi_ps` |
| Test and diagnostics | `ping`, `iperf`, `iperf3`, `mem_status`, `rmem` |
| Optional components | `mqtt`, `ssl_client`, `ali_cloud`, `fatfs`, `telnet` |
| System | `reboot`, `exit`, `help` |

See AN081 for details.

### AT commands (`CONFIG_ATCMD`, disabled by default)

Uncomment `CONFIG_ATCMD` in `NSPE/WIFI_IOT/app/app_cfg.h` and rebuild to enable it. AT commands depend on `CONFIG_WIFI_MANAGEMENT_TASK`; both must be enabled or the build fails.

Once enabled, type `AT` on the console to enter AT mode and `ATQ` to leave it. Supported commands:

| Category | Commands |
| --- | --- |
| Base | `AT`, `ATQ`, `AT+HELP`, `AT+RST`, `AT+GMR`, `AT+TASK`, `AT+HEAP`, `AT+SYSRAM`, `AT+SYSSTATUS`, `AT+UART` |
| Wi-Fi | `AT+CWMODE_CUR`, `AT+CWJAP_CUR`, `AT+CWLAP`, `AT+CWSTATUS`, `AT+CWQAP`, `AT+CWSAP_CUR`, `AT+CWLIF`, `AT+CWAUTOCONN` |
| TCP/IP | `AT+PING`, `AT+CIPSTA`, `AT+CIPSTART`, `AT+CIPSEND`, `AT+CIPSERVER`, `AT+CIPCLOSE`, `AT+CIPSTATUS`, `AT+CIFSR` |

Commands follow four forms: `AT+<x>=?` (query the parameter range), `AT+<x>?` (query the current value), `AT+<x>=<...>` (set) and `AT+<x>` (execute). Responses end with `OK` or `ERROR`. See AN080 for the full parameter reference and examples.

## Examples and application development

- Peripheral examples are in `NSPE/Example/`, grouped by function (ADC, GPIO, I2C, SPI, USART, USBFS, TZPCU, ...). Each example directory contains a `readme.txt`.
- Wi-Fi porting examples are in `NSPE/Example/WIFI_PORTING/`.
- The default Wi-Fi IoT application source is in `NSPE/WIFI_IOT/app/`; the build targets are defined in `NSPE/Project/WIFI_IOT/`.
- Bare-metal and TrustZone template projects are in `NSPE/Template/` and `NSPE/Template_TrustZone/` respectively.

For Keil or IAR, open `MultiProject_NS.uvmpw` / `MultiProject_NS.eww` (non-TrustZone) or `MultiProject.uvmpw` / `MultiProject.eww` (TrustZone) from the repository root.

## Release history

| Version | Date | Highlights |
| --- | --- | --- |
| V1.0.5 | 2025-12-05 | Alibaba Cloud SDK upgraded to 1.6.6; added RT-Thread v5.0.1 and SoftAP provisioning support; CMSIS-ARM upgraded to v6.2.0 and DSP Library to v1.16.2; LwIP v2.1.2 → v2.2.0; Mbed TLS v2.17.0 (ROM) → v3.6.5 (Flash); FreeRTOS v10.3.1 → v10.5.1. |
| V1.0.4 | 2024-02-06 | Improved AP compatibility; MQTT 3.1.1 and MQTT 5.0 support; build support on Linux. |
| V1.0.3 | 2022-11-30 | Updated the firmware library; improved AP mode robustness; added the `switch_image` serial command. |
| V1.0.2 | 2022-03-18 | Simplified environment setup by removing the Python dependency and the automatic download after build; LwIP IPv6 support in STA mode; added a 40 MHz bandwidth command; RF parameter fine tuning. |
| V1.0.1 | 2022-01-20 | Added the UART configuration for GD32W515P-EVAL; support for more GD QSPI Flash types. |
| V1.0.0 | 2021-11-30 | First release. |

See `release_notes.txt` for the complete change log.
