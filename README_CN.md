# GD32W51x Wi-Fi SDK

**中文** | [English](README_EN.md)

面向 GD32W51x 系列 MCU 的 Wi-Fi SDK，当前发布版本为 **V1.0.5**。SDK 提供从启动加载、RTOS操作系统、外设驱动，到 Wi-Fi协议栈、网络协议栈、应用框架及示例工程的完整开发基础。

GD32W51x 系列集成 2.4 GHz Wi-Fi 4（IEEE 802.11b/g/n）20/40M 带宽，采用最高主频 180 MHz 的 Arm Cortex-M33 内核，并集成 2 MB Flash 与 448 KB SRAM。

## 快速开始

```bat
git submodule update --init --recursive
.\gcc_build_ns.bat
.\gcc_download_ns.bat DAPLINK
```

烧录后用串口工具以 **115200 8N1** 连接 LOG UART，即可看到启动日志，并可输入“help”查看命令。

## 目录说明

| 目录或文件 | 说明 |
| --- | --- |
| `config/` | Flash/SRAM 布局、板级定义（`platform_def.h`）、固件版本及全局构建配置。 |
| `docs/` | 应用笔记（`EN/`、`CN/`）与开发板原理图（`Schematic/`）。 |
| `GD32W51x_Addon/` | OpenOCD 等随包附带的下载调试工具。 |
| `MBL/` | 主引导加载程序，`source/` 为 TrustZone 版本，`source_ns/` 为非 TrustZone 版本。 |
| `NSPE/` | 非安全侧固件：CMSIS、标准外设库、Wi-Fi 驱动、网络协议栈、应用与示例。 |
| `NSPE/Firmware/` | 标准外设库和 Wi-Fi 驱动。 |
| `NSPE/Example/` | 外设和 Wi-Fi 移植示例。 |
| `NSPE/WIFI_IOT/` | 默认 Wi-Fi IoT 应用（`app/`、`bsp/`、`network/`、`wifi/`、`cloud/` 等）。 |
| `NSPE/Project/WIFI_IOT/` | Wi-Fi IoT 固件的 GCC/Keil/IAR 工程与后处理脚本。 |
| `PROT/` | 安全与协议组件：Mbed TLS、TF-M 等。 |
| `ROM-EXPORT/` | ROM 导出符号、平台头文件及 ROM 版 Mbed TLS 接口。 |
| `scripts/` | 工具链配置、镜像生成（`imgtool/`）、FLM 算法、证书和产物输出目录（`images/`）。 |
| `gcc_build_ns.bat` | GCC 非 TrustZone 固件构建入口。 |
| `gcc_download_ns.bat` | 使用 J-Link、GD-Link 或 DAP-Link 烧录组合镜像。 |
| `MultiProject*.uvmpw` / `MultiProject*.eww` | Keil / IAR 多工程工作区（`_NS` 后缀为非 TrustZone 版本）。 |

> `MBL`、`NSPE`、`PROT` 和 `ROM-EXPORT` 由 Git 子模块管理。克隆仓库后请先初始化子模块：

```bash
git submodule update --init --recursive
```

## 文档索引

`docs/EN/` 提供英文版应用笔记，`docs/CN/` 提供对应的中文版本：

| 文档 | 内容 |
| --- | --- |
| AN066 | 硬件开发指南 |
| AN079 / AN097 | 快速开发指南（开发板、工程结构、编译与下载流程） |
| AN080 | AT 指令用户指南 |
| AN081 | 基本指令（串口命令行）用户指南 |
| AN082 | 安全启动用户指南 |
| AN083 | 认证测试指南 |
| AN084 | 射频指标及收发功耗测试指南 |
| AN085 | 吞吐量及场景功耗测试指南 |
| AN100 | Wi-Fi 开发指南 |
| AN103 | TrustZone 开发指南 |

`docs/Schematic/` 提供 `GD32W515P-EVAL` 与 `GD32W515T-START` 开发板原理图。

## 硬件与开发板

SDK 支持两款官方开发板，均引出复位按键、BOOT 引脚、SWD 调试口和 LOG UART：

| 开发板 | 调试接口 | 供电 |
| --- | --- | --- |
| GD32W515P-EVAL | J-Link SWD | J-Link / GD-Link |
| GD32W515T-START | J-Link SWD、板载 DAP-Link | USB（与 LOG UART 复用） |

- **SWD 连接**：板上 J-Link SWD 引出 `AGND`、`JCLK`、`JTWS`、`W3V3` 四个引脚，使用杜邦线分别接到仿真器的 `GND`、`SWCLK`、`SWDIO`、`VCC` 即可下载与调试。
- **BOOT 引脚**：用于在启动时选择运行模式，量产烧录或恢复时需要配合复位按键使用。
- **LOG UART**：波特率 115200、8 数据位、无校验、1 停止位，用于输出日志并接收串口命令。

开发板通过 `config/platform_def.h` 中的 `CONFIG_BOARD` 选择，默认值为 `PLATFORM_BOARD_32W515T_START`：

| `CONFIG_BOARD` | LOG UART | TX 引脚 | RX 引脚 |
| --- | --- | --- | --- |
| `PLATFORM_BOARD_32W515T_START` | `USART1` | `PB15` | `PA8` |
| `PLATFORM_BOARD_32W515P_EVAL` | `USART2` | `PB10` | `PB11` |
| `PLATFORM_BOARD_32W515P_OTHER` | `USART0` | `PA0` | `PA1` |

同一文件中还可配置晶振频率（`PLATFORM_CRYSTAL`，默认 40 MHz）和 XIP Flash 来源（`CONFIG_XIP_FLASH`，默认使用 SiP Flash）。

## 开发环境

### Windows

安装并加入系统 `PATH`：

- CMake 3.15 或更高版本。
- GNU Make（脚本使用 `Unix Makefiles` 生成器）。
- Arm GNU Toolchain，需提供 `arm-none-eabi-gcc`、`arm-none-eabi-objcopy` 和 `arm-none-eabi-objdump`。
- 7-Zip，用于工具或发布包处理。

如使用 J-Link 下载，还需安装 SEGGER J-Link 软件。首次使用可执行：

```bat
scripts\setup_jlink.bat
```

该脚本会将本仓库提供的 GD32W51x FLM 与设备描述复制到 J-Link 安装目录。使用 DAP-Link 时可执行 `scripts\setup_daplink.bat`；GD-Link 与 DAP-Link 的下载依赖 `GD32W51x_Addon/OpenOCD`，无需额外安装。

### Ubuntu

支持 Ubuntu 18.04 及更高版本。安装 Arm GNU Toolchain 后，将其 `bin` 目录加入 `PATH`，并安装构建依赖：

```bash
sudo apt update
sudo apt install -y build-essential srecord libncurses5 libncursesw5
```

验证工具链可用：

```bash
arm-none-eabi-gcc --version
cmake --version
make --version
```

## 构建固件

### Windows

在仓库根目录执行：

```bat
gcc_build_ns.bat
```

该脚本会删除并重新创建 `cmake_build/`，因此不要在该目录保存手工修改。

### Linux

```bash
rm -rf cmake_build && mkdir cmake_build && cd cmake_build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH=./scripts/cmake/toolchain.cmake ..
make -j
```

### 构建选项

默认配置为：

- 非 TrustZone 构建（`CONFIG_TZ_ENABLED=OFF`）。当前 GCC 工具链配置不支持启用 TrustZone，TrustZone 工程请使用 Keil/IAR，参见 AN103。
- FreeRTOS（`CONFIG_OS=FREERTOS`，另可选 `RTTHREAD`）。
- Flash 版 Mbed TLS 3.x（`CONFIG_MBEDTLS_VERSION=3.x`，另可选 `2.17.0` 的 ROM 版本）。
- 链接预编译 Wi-Fi 静态库（`CONFIG_STATIC_LIB_BUILDED=ON`）。

上述缓存变量定义在根目录 `CMakeLists.txt`，可直接修改或在 `cmake` 命令行用 `-D` 覆盖后重新构建。

应用层功能开关位于 `NSPE/WIFI_IOT/app/app_cfg.h`，常用宏包括：

| 宏 | 默认 | 说明 |
| --- | --- | --- |
| `CONFIG_CONSOLE_ENABLE` | 开 | 串口命令行总开关。 |
| `CONFIG_BASECMD` | 开 | 基本命令集（参见 AN081）。 |
| `CONFIG_ATCMD` | 关 | AT 指令集（参见 AN080）。 |
| `CONFIG_IPERF_TEST` | 开 | iperf2 / iperf3 吞吐测试命令。 |
| `CONFIG_SOFTAP_PROVISIONING` | 关 | SoftAP 配网。 |
| `CONFIG_ALICLOUD_SUPPORT` | 关 | 阿里云 Link Kit，需同时开启 `CONFIG_EXTEND_MEMORY`。 |
| `CONFIG_MQTT` / `CONFIG_SSL_TEST` / `CONFIG_FATFS_SUPPORT` / `CONFIG_IPV6_SUPPORT` | 关 | 对应可选组件。 |

固件地址、镜像布局和组件版本定义位于 `config/config_gdm32.h` 与 `config/config_gdm32_ntz.h`；修改这些布局参数前应确认启动加载程序和升级流程的兼容性。

## 构建产物

构建完成后，调试产物位于各工程的 GCC 输出目录：

```text
MBL/project/GCC/output/bin/mbl-ns.axf         # 启动加载程序
NSPE/Project/WIFI_IOT/GCC/output/bin/nspe.axf # 应用固件
NSPE/Project/WIFI_IOT/GCC/output/bin/nspe.map # 链接映射文件
```

后处理会生成可烧录及 OTA 使用的镜像：

```text
scripts/images/image-all.bin  # 全量烧录镜像，基地址 0x08000000
scripts/images/image-all.hex  # Intel HEX 格式全量镜像
scripts/images/image-ota.bin  # OTA 使用的 NSPE 镜像
```

## 烧录

构建成功后，在根目录执行（参数必须为大写）：

```bat
gcc_download_ns.bat JLINK
gcc_download_ns.bat GDLINK
gcc_download_ns.bat DAPLINK
```

脚本将 `scripts/images/image-all.bin` 下载到 `0x08000000`。使用 J-Link 时，默认程序路径为 `C:\Program Files (x86)\SEGGER\JLink\Jlink.exe`；若实际安装路径不同，请调整 `gcc_download_ns.bat` 中的 `JLINK_PATH`。GD-Link 与 DAP-Link 使用 `GD32W51x_Addon/OpenOCD` 完成烧录并自动复位运行。

## 串口命令行

以 115200 8N1 连接 LOG UART 后，固件提供两套命令集。

### 基本命令（`CONFIG_BASECMD`，默认开启）

输入 `help` 可列出全部命令。常用命令包括：

| 类别 | 命令 |
| --- | --- |
| Wi-Fi STA | `wifi_open`、`wifi_close`、`wifi_scan`、`wifi_connect`、`wifi_disconnect`、`wifi_status`、`wifi_rssi` |
| Wi-Fi SoftAP | `wifi_ap`、`wifi_ap_adv`、`wifi_stop_ap`、`wifi_ap_provisioning` |
| 网络配置 | `wifi_set_ip`、`wifi_set_channel`、`wifi_set_bw`、`wifi_mac_addr`、`wifi_ps` |
| 测试与诊断 | `ping`、`iperf`、`iperf3`、`mem_status`、`rmem` |
| 可选组件 | `mqtt`、`ssl_client`、`ali_cloud`、`fatfs`、`telnet` |
| 系统 | `reboot`、`exit`、`help` |

详见 AN081。

### AT 指令（`CONFIG_ATCMD`，默认关闭）

在 `NSPE/WIFI_IOT/app/app_cfg.h` 中取消 `CONFIG_ATCMD` 的注释并重新构建即可启用。AT 指令依赖 `CONFIG_WIFI_MANAGEMENT_TASK`，两者必须同时开启，否则编译报错。

启用后在命令行输入 `AT` 进入 AT 模式，`ATQ` 退出。支持的指令：

| 分类 | 指令 |
| --- | --- |
| 基础 | `AT`、`ATQ`、`AT+HELP`、`AT+RST`、`AT+GMR`、`AT+TASK`、`AT+HEAP`、`AT+SYSRAM`、`AT+SYSSTATUS`、`AT+UART` |
| Wi-Fi | `AT+CWMODE_CUR`、`AT+CWJAP_CUR`、`AT+CWLAP`、`AT+CWSTATUS`、`AT+CWQAP`、`AT+CWSAP_CUR`、`AT+CWLIF`、`AT+CWAUTOCONN` |
| TCP/IP | `AT+PING`、`AT+CIPSTA`、`AT+CIPSTART`、`AT+CIPSEND`、`AT+CIPSERVER`、`AT+CIPCLOSE`、`AT+CIPSTATUS`、`AT+CIFSR` |

指令格式遵循 `AT+<x>=?`（查询参数范围）、`AT+<x>?`（查询当前值）、`AT+<x>=<...>`（设置）、`AT+<x>`（执行）四种形式，响应以 `OK` 或 `ERROR` 结束。完整参数说明与示例见 AN080。

## 示例与应用开发

- 外设示例位于 `NSPE/Example/`，按 ADC、GPIO、I2C、SPI、USART、USBFS、TZPCU 等功能分类，每个示例目录附带 `readme.txt`。
- Wi-Fi 移植示例位于 `NSPE/Example/WIFI_PORTING/`。
- 默认 Wi-Fi IoT 应用源码位于 `NSPE/WIFI_IOT/app/`，构建目标定义在 `NSPE/Project/WIFI_IOT/`。
- 裸机与 TrustZone 模板工程分别位于 `NSPE/Template/` 和 `NSPE/Template_TrustZone/`。

使用 Keil 或 IAR 时，可打开根目录的 `MultiProject_NS.uvmpw` / `MultiProject_NS.eww`（非 TrustZone）或 `MultiProject.uvmpw` / `MultiProject.eww`（TrustZone）工作区。

## 版本信息

| 版本 | 日期 | 主要更新 |
| --- | --- | --- |
| V1.0.5 | 2025-12-05 | 阿里云 SDK 升级至 1.6.6；新增 RT-Thread v5.0.1 与 SoftAP 配网支持；CMSIS-ARM 升级至 v6.2.0、DSP Library 升级至 v1.16.2；LwIP v2.1.2 → v2.2.0；Mbed TLS v2.17.0（ROM）→ v3.6.5（Flash）；FreeRTOS v10.3.1 → v10.5.1。 |
| V1.0.4 | 2024-02-06 | 提升与 AP 的兼容性；支持 MQTT 3.1.1 与 MQTT 5.0；支持 Linux 下构建。 |
| V1.0.3 | 2022-11-30 | 更新固件库；增强 AP 模式健壮性；新增 `switch_image` 串口命令。 |
| V1.0.2 | 2022-03-18 | 简化开发环境搭建，移除 Python 依赖与编译后自动下载；STA 模式支持 LwIP IPv6；新增 40 MHz 带宽开关命令；射频参数调优。 |
| V1.0.1 | 2022-01-20 | 新增 GD32W515P-EVAL 串口配置；支持更多型号的 GD QSPI Flash。 |
| V1.0.0 | 2021-11-30 | 首次发布。 |

完整变更记录请参见 `release_notes.txt`。
