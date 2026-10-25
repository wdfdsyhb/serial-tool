# serial-tool · 纯 C 串口调试助手

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![C99](https://img.shields.io/badge/C-C99-blue)

> 一条 `serial-tool list` 列出所有 COM 口，并按 VID/PID 识别 USB 串口芯片（CH340/CP210x/FTDI/ESP32 JTAG）——纯 C、零依赖，正在长成完整的串口调试工具。
>
> English: A pure-C CLI serial tool: list COM ports and identify USB-serial chips (CH340/CP210x/FTDI/ESP32 JTAG) by VID/PID — zero dependencies, growing into a full serial console.

> C 语言实现的串口调试助手（开发中）——我的第一个正式仓库，嵌入式路线的第一块砖。

## 简介

用纯 C 实现的命令行串口调试工具：

- [x] 串口枚举 + USB 芯片识别（v0.1）：`serial-tool list`
      通过 SetupDi 枚举设备、解析 USB VID/PID 查表识别芯片，
      覆盖 ESP32 原生 USB-Serial-JTAG、CH340/CH343、CP210x、FTDI、PL2303
- [ ] 串口打开/关闭（v0.2，学习路线见 `docs/v0.1-checklist.md`）
- [ ] 收发数据（文本 / HEX 双模式）
- [ ] 收发日志落盘
- [ ] （规划中）简单协议帧解析

实测输出示例（ESP32-S3-N16R8 接 USB 口）：

```
COM4      Espressif USB-Serial-JTAG (ESP32-S2/S3/C3/C6 native)  (VID_303A&PID_1001)
```

## 构建

Windows + MSVC（Visual Studio 2022）：

```bat
build.bat
```

内容即 `cl /W4 /O2 /utf-8 /D_CRT_SECURE_NO_WARNINGS src\main.c src\devices.c`。

## 目录结构

```
src/        源码（main.c 入口 / devices.c 设备枚举与识别）
docs/       学习笔记与踩坑记录
build.bat   MSVC 一键编译
```

## 踩坑记录

- 中文 Windows 下 cl 默认按 GBK 解源码，UTF-8 注释会报 C4819 → 编译加 `/utf-8`
- `sscanf` 在 MSVC 触发 C4996 安全警告 → 定义 `_CRT_SECURE_NO_WARNINGS`
- 枚举 COM 口用 `SetupDiGetClassDevs(GUID_DEVCLASS_PORTS)` 比注册表 `SERIALCOMM` 多拿到硬件 ID（VID/PID），识别芯片全靠它

## License

MIT

---

## English

Full English documentation: [README_EN.md](README_EN.md)
