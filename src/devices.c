/* devices.c - 枚举串口设备，通过 USB VID/PID 识别芯片型号。
 * 覆盖 ESP32 系列常见芯片：原生 USB-Serial-JTAG、CH34x、CP210x、FTDI。
 * API: SetupDiGetClassDevs + SetupDiEnumDeviceInfo (setupapi.lib)
 */
#include "devices.h"

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "setupapi.lib")

typedef struct {
    unsigned short vid;
    unsigned short pid;
    const char    *chip;
} ChipEntry;

/* VID/PID 表：新芯片加一行即可 */
static const ChipEntry kChips[] = {
    {0x303A, 0x1001, "Espressif USB-Serial-JTAG (ESP32-S2/S3/C3/C6 native)"},
    {0x1A86, 0x55D3, "WCH CH343"},
    {0x1A86, 0x55D4, "WCH CH343"},
    {0x1A86, 0x55D5, "WCH CH343"},
    {0x1A86, 0x7523, "WCH CH340"},
    {0x10C4, 0xEA60, "Silicon Labs CP210x"},
    {0x0403, 0x6001, "FTDI FT232"},
    {0x0403, 0x6010, "FTDI FT2232"},
    {0x0403, 0x6011, "FTDI FT4232"},
    {0x0403, 0x6014, "FTDI FT232H"},
    {0x067B, 0x2303, "Prolific PL2303"},
};

static const char *lookup_chip(unsigned vid, unsigned pid)
{
    size_t i;
    for (i = 0; i < sizeof(kChips) / sizeof(kChips[0]); i++) {
        if (kChips[i].vid == vid && kChips[i].pid == pid)
            return kChips[i].chip;
    }
    return NULL;
}

/* 从 hardware ID 字符串里解析 VID/PID，格式如 USB\VID_303A&PID_1001
 * 返回 0 表示解析成功 */
static int parse_vid_pid(const char *hwid, unsigned *vid, unsigned *pid)
{
    const char *v = strstr(hwid, "VID_");
    const char *p = strstr(hwid, "PID_");
    if (!v || !p)
        return -1;
    return (sscanf(v + 4, "%4x", vid) == 1 && sscanf(p + 4, "%4x", pid) == 1) ? 0 : -1;
}

/* 从 friendly name 提取 COM 口号，如 "USB 串行设备 (COM4)" -> COM4
 * 返回 out（复用入参），找不到 COM 字样返回 NULL */
static char *extract_com_name(const char *friendly, char *out, size_t outsz)
{
    const char *c = strstr(friendly, "COM");
    const char *end;
    size_t len;

    if (!c)
        return NULL;
    /* COM 后面必须紧跟数字，且到右括号/结尾为止 */
    if (c[3] < '0' || c[3] > '9')
        return NULL;
    end = c + 3;
    while (*end >= '0' && *end <= '9')
        end++;
    len = (size_t)(end - c);
    if (len + 1 > outsz)
        return NULL;
    memcpy(out, c, len);
    out[len] = '\0';
    return out;
}

int list_serial_ports(void)
{
    HDEVINFO hSet;
    SP_DEVINFO_DATA did;
    int count = 0;

    hSet = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, NULL, NULL,
                                DIGCF_PRESENT);
    if (hSet == INVALID_HANDLE_VALUE)
        return -1;

    did.cbSize = sizeof(did);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hSet, i, &did); i++) {
        char friendly[256];
        char hwids[2048];
        DWORD regtype, needed;
        char comname[16];
        const char *chip = NULL;
        unsigned vid = 0, pid = 0;

        if (!SetupDiGetDeviceRegistryPropertyA(hSet, &did, SPDRP_FRIENDLYNAME,
                                               &regtype, (BYTE *)friendly,
                                               sizeof(friendly), &needed))
            continue;

        /* hardware ID 是 REG_MULTI_SZ：多个 \0 分隔的串 */
        if (SetupDiGetDeviceRegistryPropertyA(hSet, &did, SPDRP_HARDWAREID,
                                              &regtype, (BYTE *)hwids,
                                              sizeof(hwids), &needed)) {
            /* 遍历多串，任何一个 ID 能解析出芯片即命中 */
            for (const char *h = hwids; *h; h += strlen(h) + 1) {
                if (parse_vid_pid(h, &vid, &pid) == 0) {
                    chip = lookup_chip(vid, pid);
                    if (chip)
                        break;
                }
            }
        }

        if (extract_com_name(friendly, comname, sizeof(comname))) {
            if (chip)
                printf("%-8s  %s  (VID_%04X&PID_%04X)\n",
                       comname, chip, vid, pid);
            else if (vid || pid)
                printf("%-8s  未知 USB 串口芯片  (VID_%04X&PID_%04X)\n",
                       comname, vid, pid);
            else
                printf("%-8s  系统内建/虚拟串口\n", comname);
            count++;
        }
    }

    SetupDiDestroyDeviceInfoList(hSet);
    return count;
}
