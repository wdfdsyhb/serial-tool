/* devices.h - 串口设备枚举与 USB 芯片识别 */
#ifndef ST_DEVICES_H
#define ST_DEVICES_H

/* 枚举本机所有串口，打印 COM 口号 + 芯片型号 + VID:PID。
 * 返回发现的设备数，失败返回 -1。 */
int list_serial_ports(void);

#endif
