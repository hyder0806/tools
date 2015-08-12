
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include "uart.h"


#define BAUDRATE B115200

/**
  * @brief :
  * @param :None
  * @retval:None
static struct __com_cfg_param
{  
	unsigned char vaild;
	unsigned char baud;
	unsigned char bits;
	unsigned char stops;
	unsigned char parity;
	unsigned char flowctl;
};

struct __com_cfg_param cfg;
  */

int uart_fd[__COM_MAX__]={-1};
static struct termios newtio[__COM_MAX__], oldtio[__COM_MAX__];
int uart_write_lock = 0;
int uart_read_lock = 0;

int dev_uart_write(unsigned char port, unsigned char *buf, unsigned int len)
{
	int ret = -1;

	if(uart_fd[port])
	{
		ret = write(uart_fd[port], buf, len);
		if(ret == len)
		{
			ret = len;
		}
		else
		{
			tcflush(uart_fd[port], TCOFLUSH);
		}
	}
	return ret;
}

int dev_uart_read(unsigned char port, unsigned char *buf, unsigned int len)
{
	int ret = -1;

	if(uart_fd[port])
	{

		ret = read(uart_fd[port], buf, len);

	}
	return ret;
}

int dev_uart_ioctl(unsigned char port,unsigned char *argv, unsigned int argc)
{
	int ret = -1;

	switch(argc)
	{
		case USRT_SET_PARAM:
			if(uart_fd[port])
			{
    			// save current port settings
    			ret = tcgetattr(uart_fd[port],&oldtio[port]);
    			if( ret < 0)
    			{
    			    perror("Cannot get the attribution of the terminal");
    			    ret = 1;
    			}
    			memset(&newtio[port],0,sizeof(newtio[port]));
    			cfsetispeed(&newtio[port],BAUDRATE);
    			cfsetospeed(&newtio[port],BAUDRATE);

    			// set new port settings for canonical input processing
    			newtio[port].c_cflag = (CS8 | CLOCAL | CREAD);
    			newtio[port].c_iflag = (IGNPAR | ICRNL);
    			newtio[port].c_oflag = 0;
    			newtio[port].c_lflag = 0;
    			newtio[port].c_cc[VMIN] = 0;
    			newtio[port].c_cc[VTIME] = 0;
    			tcflush(uart_fd[port], TCIOFLUSH);
    			ret = tcsetattr(uart_fd[port],TCSANOW,&newtio[port]);
    			if(ret < 0)
    			{
    			    perror("Cannot set the attribution of the terminal");
    			    ret = 2;
    			}
				ret = 0;
			}
			break;

		case GET_UART_FD:
			ret = uart_fd[port];
			break;
		
		/*------------------------*/
		case GET_UART_WR_LOCK:
			ret = uart_write_lock;
			break;
		
		case SET_UART_WR_LOCK:
			ret = uart_fd[port];
			break;
		
		case CLR_UART_WR_LOCK:
			ret = uart_fd[port];
			break;
		/*------------------------*/
		case GET_UART_RD_LOCK:
			ret = uart_read_lock;
			break;
		
		case SET_UART_RD_LOCK:
			ret = uart_fd[port];
			break;
		
		case CLR_UART_RD_LOCK:
			ret = uart_fd[port];
			break;
		
		default:
			break;
	}

	return ret;
}

void dev_uart_close(unsigned char port)
{
	if(uart_fd[port])
	{
		close(uart_fd[port]);
	}
	uart_fd[port] = -1;
}

int dev_uart_open(unsigned char port)
{
    int temp;
	unsigned char dev_uart_name[20];
    
	if(uart_fd[port] < 0)
	{
		if(port == DBG_PORT)
		{
			strcpy(dev_uart_name, DBG_PORT_NAME);
		}
		else
		{
			g_print("port error\n");
			return 2;
		}

    	uart_fd[port] = open(DBG_PORT_NAME, O_RDWR | O_NOCTTY);// | O_NONBLOCK);
		if(uart_fd[port] < 0)
		{
			g_print("%s open error\n", dev_uart_name);
			return 3;
		}

		g_print("open device [%s] success, fd = %d\r\n", dev_uart_name,uart_fd[port]);

		dev_uart_ioctl(port,NULL,USRT_SET_PARAM);

		return 0;

	}
	
	g_print("port err open\n");
	return 1;
}
