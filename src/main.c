/* serial-tool - 命令行串口调试助手
 * 主入口。设备枚举见 devices.c；
 * 学习路线（自己练）：QueryDosDeviceA 枚举 -> CreateFile 打开 -> 收发循环，见 docs/v0.1-checklist.md
 */
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "devices.h"

static void usage(void)
{
    printf("serial-tool v0.1.0 - 命令行串口调试助手\n\n");
    printf("用法:\n");
    printf("  serial-tool list    列出本机串口并识别 USB 芯片\n");
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "list") == 0) {
        int n = list_serial_ports();
        if (n < 0) {
            fprintf(stderr, "错误: 设备枚举失败 (GetLastError=%lu)\n",
                    (unsigned long)GetLastError());
            return 1;
        }
        if (n == 0)
            printf("未发现串口设备\n");
        return 0;
    }

    usage();
    return 0;
}
