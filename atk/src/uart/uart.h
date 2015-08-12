#ifndef __UART_H__
#define __UART_H__

#define DBG_PORT		0

#define DBG_PORT_NAME	"/dev/ttyUSB0"

#define __COM_MAX__		16
		
#define USRT_SET_PARAM		0x1
#define GET_UART_FD			0x2
#define GET_UART_WR_LOCK	0x3
#define SET_UART_WR_LOCK	0x4
#define CLR_UART_WR_LOCK	0x5
#define GET_UART_RD_LOCK	0x6
#define SET_UART_RD_LOCK	0x7
#define CLR_UART_RD_LOCK	0x8


int dev_uart_open(unsigned char port);
void dev_uart_close(unsigned char port);
int dev_uart_ioctl(unsigned char port,unsigned char *argv, unsigned int argc);
int dev_uart_read(unsigned char port, unsigned char *buf, unsigned int len);
int dev_uart_write(unsigned char port, unsigned char *buf, unsigned int len);

#endif
