# serial-tool · Pure-C Serial Console

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![C99](https://img.shields.io/badge/C-C99-blue)

> A pure-C CLI serial tool: list COM ports and identify USB-serial chips (CH340/CP210x/FTDI/ESP32 JTAG) by VID/PID — zero dependencies, growing into a full serial console.

> A serial debugging assistant written in plain C (in development) — my first proper repository, and the first brick on the embedded path.

## Overview

A command-line serial debugging tool implemented in pure C:

- [x] Port enumeration + USB chip identification (v0.1): `serial-tool list`
      Enumerates devices via SetupDi and identifies chips by parsing USB VID/PID
      against a lookup table, covering ESP32 native USB-Serial-JTAG, CH340/CH343,
      CP210x, FTDI and PL2303
- [ ] Port open/close (v0.2; learning path in `docs/v0.1-checklist.md`)
- [ ] Send/receive data (text / HEX dual mode)
- [ ] Log send/receive traffic to disk
- [ ] (planned) Simple protocol frame parsing

Sample output on real hardware (ESP32-S3-N16R8 plugged into USB):

```
COM4      Espressif USB-Serial-JTAG (ESP32-S2/S3/C3/C6 native)  (VID_303A&PID_1001)
```

## Build

Windows + MSVC (Visual Studio 2022):

```bat
build.bat
```

Which is just `cl /W4 /O2 /utf-8 /D_CRT_SECURE_NO_WARNINGS src\main.c src\devices.c`.

## Layout

```
src/        Sources (main.c entry point / devices.c device enumeration & identification)
docs/       Learning notes and pitfalls
build.bat   One-click MSVC build
```

## Pitfalls

- On Chinese-locale Windows, cl defaults to GBK source decoding, so UTF-8 comments trigger C4819 → add `/utf-8` to the compile line
- `sscanf` on MSVC raises the C4996 security warning → define `_CRT_SECURE_NO_WARNINGS`
- Enumerating COM ports with `SetupDiGetClassDevs(GUID_DEVCLASS_PORTS)` yields hardware IDs (VID/PID) that the registry's `SERIALCOMM` key lacks — chip identification depends entirely on them

## License

MIT

---

> 🇨🇳 中文文档: [README.md](README.md)
