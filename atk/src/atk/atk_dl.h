#ifndef __ATK_DL_H__
#define __ATK_DL_H__

#define UART_PORT		"UART"
#define UART_PORT_LEN	4

#define USB_PORT		"USB"
#define USB_PORT_LEN	3

extern void atk_download_task_init(void);
extern void atk_download_task_clk(void);
extern unsigned char atk_uart_init(int *secure);
extern void atk_download_task_start(int security);

#endif
