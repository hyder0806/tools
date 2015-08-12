
#include <stdio.h>
#include "task.h"
#include "buff.h"
#include "uart.h"
#include "atk.h"
#include "atk_dl.h"

#undef ATK_DEBUG
//#define ATK_DEBUG

#ifdef ATK_DEBUG
#define ATK_DBG(x...) printf(x)
#else
#define ATK_DBG(x...)
#endif
		
#define __ATK_CMD_LEN__ 			16	
#define __ATK_FILE_BUFF_SIZE__		64
#define __ATK_FILE_BLK_SIZE__		16
#define __ATK_READ_BLK__			(__ATK_FILE_BUFF_SIZE__/__ATK_FILE_BLK_SIZE__)
#define __RECHECK__					0
#define __CHECK_MSK__				0xff00
#define DOWNLOAD_TIMEOUT			10
	
#define FILE_NAME					"./boot_csf.bin"


FILE *fpFile = NULL;

typedef struct _TaskParam
{
    unsigned char *Src;
    unsigned int src_len;
    unsigned char *Dst;
    unsigned int dst_len;
	unsigned int setting;//TimeOut;
				  /* setting will be store:
				   * CMD group [31:24]
				   * Time ETA [7:0]
				   *
				   */
}TskPrm;

struct _TaskParam TaskParam[] = {
    {ATK_RST_IMX,ATK_RST_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_INIT_IMX,ATK_CLKCTL_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_CLKCTL_IMX,ATK_CLKCTL_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_CS5CRU_IMX,ATK_CS5CRU_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_CS5CRL_IMX,ATK_CS5CRL_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_CS5CRA_IMX,ATK_CS5CRA_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_MDDR_IMX,ATK_MDDR_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_ESDMISC_IMX,ATK_ESDMISC_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_ESDCTL0_IMX,ATK_ESDCTL0_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_AUTOFRESH_IMX,ATK_AUTOFRESH_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_W_RE1_IMX,ATK_W_RE1_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_W_RE2_IMX,ATK_W_RE2_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_LOAD_IMX,ATK_LOAD_LEN,NULL,0, TASK_TIME_OUT},
    {ATK_WR_ADDR_IMX,ATK_WR_ADDR_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_WR_DATA_IMX,ATK_WR_DATA_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_COUNTER_IMX,ATK_COUNTER_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_W_RE3_IMX,ATK_W_RE3_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_W_RE4_IMX,ATK_W_RE4_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_SET_SDRAM_IMX,ATK_SET_SDRAM_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_W_RE5_IMX,ATK_W_RE5_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_END_IMX,ATK_END_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_RECHECK_IMX,ATK_RECHECK_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_RECHECK2_IMX,ATK_RECHECK2_LEN,NULL, 0, TASK_TIME_OUT},
    {ATK_GET_STATUS,__ATK_CMD_LEN__,NULL, 0, TASK_TIME_OUT},
    {0},
};

static volatile unsigned int offset = 0;
int atk_download_time_out = 0;
static TASK Task = {0};
unsigned char compare_state = 0;
int security = 0;
extern double p_value;
int dnl_sum;
void atk_download_task_start(int security);
int atk_download_file_init(void);


static unsigned char atk_compare(int off, int *secure)
{
	unsigned char *dst_sec = NULL;
	unsigned char *dst_unsec = NULL;
	unsigned char *dst_status = NULL;
	unsigned char len;
	unsigned char rcv_char;
	int length,ret = 1;

	length = buff_dlen(&cbuff);
	if(!length)
		return 1;	
	
	dst_sec = ATK_IS_SECURE;
	dst_unsec = ATK_UN_SECURE;
	dst_status = ATK_STATUS;
	len = ATK_RES_LEN;
	
	if(0 == compare_state)
	{
			buff_out(&cbuff,&rcv_char,1);
			ATK_DBG("[%x]\n",rcv_char);
			if((rcv_char == *dst_sec) || (rcv_char == *dst_unsec) || (rcv_char == *dst_status))
			{
				compare_state = 1;
			}
			ret  = 2;
	}
	else if(1 == compare_state)
	{
			offset++;
			buff_out(&cbuff,&rcv_char,1);
			ATK_DBG("-[%x]\n",rcv_char);
			if(rcv_char == *(dst_sec+offset))
			{
				*secure = SECURE;
			}
			else if(rcv_char == *(dst_unsec+offset))
			{
				*secure = UNSECURE;
			}
			else if(rcv_char == *(dst_status+offset))
			{
				;
			}
			else
			{
				offset = 0;
				compare_state = 0;
				ret = 3;
			}
				
			if(offset == (len-1))
			{
				ret = 0;
				offset = 0;
			}
	}
	return ret;
}

int hfc_cpy(unsigned char *dst, int off)
{
	int i;
		
	for(i = 0; i < __ATK_CMD_LEN__; i++)
	{
		*(dst+i) = *(TaskParam[off].Src+i);
	}
}


unsigned char atk_task_func(TASK *task)
{
	int ret;
	int length;
	static int off = -1;
	unsigned char cmd[__ATK_CMD_LEN__];
	
	int i;

	if(task->Loop == TASK_SEVICE)
	{
		if(!atk_download_time_out)
		{
			ATK_DBG("Task time out\n");
			task->Loop = TASK_TIMEOUT;
		}
		else
		{
			//if(TaskParam[off].Dst != NULL)
			//{
				ret = atk_compare(off,&security);
				if(!ret)
				{
					ATK_DBG("ATK TASK Success\n");
					task->Loop = TASK_DESTROY;
				}
			//}
			/*else
			{
				ret = atk_compare(off,&security);
				if(!ret)
				{
					ATK_DBG("ATK TASK init setting,security = %d\n",security);
					atk_download_task_start(security);
					task->Loop = TASK_DESTROY;
				}
			}*/
		}
	}
	else if(task->Loop == TASK_INIT)
	{
		task->Loop = TASK_SEVICE;
		
		buff_init(&cbuff,0);
			
		off = task->ParamH; 
		ATK_DBG("atk task offset is :%d\n",off);

		if(off != -1)
		{
			memset(cmd,0,__ATK_CMD_LEN__);
			ATK_DBG("Send len is %d\n",TaskParam[off].src_len);
			hfc_cpy(cmd,off);
		}

#ifdef ATK_DEBUG 
		ATK_DBG("\n");
		for(i = 0; i < __ATK_CMD_LEN__; i++)
		{
			ATK_DBG("%x ",cmd[i]);
		}
		ATK_DBG("\n");
#endif

		ret = dev_uart_ioctl(DBG_PORT,NULL,GET_UART_FD);
		if(ret > 0)
		{
			ATK_DBG("write data to uart port\n");
			dev_uart_write(DBG_PORT,cmd,__ATK_CMD_LEN__);
		}
		
		atk_download_time_out = task->ParamL;

		compare_state = 0;
		offset = 0;
		
		ATK_DBG("atk connect init\n");
	}
	else if(task->Loop == TASK_DESTROY)
	{
		ATK_DBG("atk task destroy\n");
		
		if( LAST_ONE == task->ParamH)
		{
			printf("atk file download start\n");
			atk_download_file_init();
		}
		atk_download_time_out = 0;
		task->Loop = TASK_QUIT;
	}
	else if(task->Loop == TASK_TIMEOUT)
	{
		ATK_DBG("atk task time out\n");
		task->Loop = TASK_QUIT;
	}
	else
	{
		ATK_DBG("atk connect param error\n");
	}
	
	return task->Loop;
}

/*
 *	In ATK function
 *  ParamH is use to direct which TaskParam is active.
 *	ParamL is use to calculate the time clipse
 *  Loop is the task status
 */
static void atk_download_func_init(unsigned char (*pFun)(struct _TASK *task), int off, int security)
{
	int i;

		Task.Loop = 0x01;
		Task.ParamH = off;
		Task.ParamL = TaskParam[off].setting;
		if(SECURE == security)
		{
			TaskParam[off].Dst = ATK_IS_SECURE;
			TaskParam[off].dst_len = ATK_RES_LEN;
		}
		else if(UNSECURE == security)
		{
			TaskParam[off].Dst = ATK_UN_SECURE;
			TaskParam[off].dst_len = ATK_RES_LEN;
		}
		else if(STATUS == security)
		{
			TaskParam[off].Dst = ATK_STATUS;
			TaskParam[off].dst_len = ATK_STATUS_LEN;
		}
#ifdef ATK_DEBUG	
		for(i = 0; i < __ATK_CMD_LEN__; i++)
		{
			ATK_DBG("%x ",*(TaskParam[off].Src+i));
		}
		ATK_DBG("\n");
#endif
	task_queue_add(pFun,&Task);
}

static void atk_file_download_init(unsigned char (*pFun)(struct _TASK *task), int off)
{

	Task.Loop = 0x01;
	Task.ParamH = off;
	Task.ParamL = 0xff;
	task_queue_add(pFun,&Task);
}

void atk_download_task_clk(void)
{
	if(atk_download_time_out)
	{
		ATK_DBG("atk time:%d\n",atk_download_time_out);
		atk_download_time_out--;
	}
}

void atk_download_task_start(int security)
{
	atk_download_func_init(atk_task_func,ATK_INIT_TASK, security);
	atk_download_func_init(atk_task_func,ATK_CLKCTL_TASK, security);
	atk_download_func_init(atk_task_func,ATK_CS5CRU_TASK, security);
	atk_download_func_init(atk_task_func,ATK_CS5CRL_TASK, security);
	atk_download_func_init(atk_task_func,ATK_CS5CRA_TASK, security);
	atk_download_func_init(atk_task_func,ATK_MDDR_TASK, security);
	atk_download_func_init(atk_task_func,ATK_ESDMISC_TASK, security);
	atk_download_func_init(atk_task_func,ATK_ESDCTL0_TASK, security);
	atk_download_func_init(atk_task_func,ATK_AUTOFRESH_TASK, security);
	atk_download_func_init(atk_task_func,ATK_W_RE1_TASK, security);
	atk_download_func_init(atk_task_func,ATK_W_RE2_TASK, security);
	atk_download_func_init(atk_task_func,ATK_LOAD_TASK, security);
	atk_download_func_init(atk_task_func,ATK_WR_ADDR_TASK, security);
	atk_download_func_init(atk_task_func,ATK_WR_DATA_TASK, security);
	atk_download_func_init(atk_task_func,ATK_COUNTER_TASK, security);
	atk_download_func_init(atk_task_func,ATK_W_RE3_TASK, security);
	atk_download_func_init(atk_task_func,ATK_W_RE4_TASK, security);
	atk_download_func_init(atk_task_func,ATK_SET_SDRAM_TASK, security);
	atk_download_func_init(atk_task_func,ATK_W_RE5_TASK, security);
	atk_download_func_init(atk_task_func,ATK_END_TASK, security);
}

void atk_download_task_init(void)
{
	atk_download_func_init(atk_task_func,ATK_RST_CMD,0);
}


int atk_download_file_start(unsigned char *FileName)
{
	fpFile = fopen(FILE_NAME,"rb");
	
	if(fpFile == NULL)
	{
		ATK_DBG("Open File Err\n");
		return ATK_ERR;
	}
	return ATK_OK;
}

int atk_close_download_file(void)
{
	if(fpFile != NULL)
	{
		ATK_DBG("Close File\n");
		fclose(fpFile);
	}
}

int atk_get_file_size(void)
{
	int len = -1;

	if(fpFile != NULL)
	{
		fseek(fpFile,0,SEEK_END);
		len = ftell(fpFile);
		fseek(fpFile,0,SEEK_SET);
	}
	return len;
}

unsigned char atk_download_func(TASK *task)
{
	int ret;
	int nBlk,fEnd;
	static long int length;
	int off = -1;
	static int resend;
	static loop = 0;
	unsigned char buff[__ATK_FILE_BUFF_SIZE__];
	int write_size;	
	int i;

	if(task->Loop == TASK_SEVICE)
	{
		ATK_DBG("atk download sevice\n");

		off = task->ParamH;
		if(off)	
		{
			nBlk = fread(buff,__ATK_FILE_BLK_SIZE__,__ATK_READ_BLK__,fpFile);
			if(nBlk == __ATK_READ_BLK__)
			{
				write_size = __ATK_FILE_BUFF_SIZE__;
			}
			if(nBlk < __ATK_READ_BLK__)
			{
				write_size = (length & (__ATK_FILE_BUFF_SIZE__ - 1));
			}
				
			loop++;
			ATK_DBG("File Write End Size:%d, blk:%d, Loop:%d\n",write_size,nBlk,loop);
			ATK_DBG("process:%f,dnl_sum:%d,len:%ld\n",p_value,dnl_sum,length);
	
#ifdef ATK_DEBUG	
			ATK_DBG("\n");
			for(i = 0; i < write_size; i++)
			{
				ATK_DBG("%02X ",buff[i]);
			}
			ATK_DBG("\n");
#endif

			ret = dev_uart_write(DBG_PORT,buff,write_size);
			if(ret != write_size)
			{
				printf("ERROR!\n");
			}
			
			dnl_sum += write_size;
			p_value = (double)dnl_sum/length;

			if(feof(fpFile))
			{
				p_value = 1;
				task->Loop = TASK_DESTROY;
				ATK_DBG("Send File End,param = %d\n",task->ParamH);
			}
		}
		else
		{
			ATK_DBG("start agian:%d\n",resend);
			nBlk = fread(buff,__ATK_FILE_BLK_SIZE__,__ATK_READ_BLK__,fpFile);
			if(nBlk == __ATK_READ_BLK__)
			{ 

#ifdef ATK_DEBUG	
				ATK_DBG("\n");
				for(i = 0; i < __ATK_FILE_BUFF_SIZE__; i++)
				{
					ATK_DBG("%02X ",buff[i]);
				}
				ATK_DBG("\n");
#endif	
				ret = dev_uart_write(DBG_PORT,buff,__ATK_FILE_BUFF_SIZE__);
				if(ret == __ATK_FILE_BUFF_SIZE__)
				{
					ATK_DBG("resend = %d\n",resend);
					resend -= __ATK_FILE_BUFF_SIZE__;
					if(0 == resend)
					{
						ATK_DBG("atk download finished\n");
						task->Loop = TASK_DESTROY;
					}
				}
			}
		}
	}
	else if(task->Loop == TASK_INIT)
	{
		ATK_DBG("atk download init\n");
		
		ret = atk_download_file_start(FILE_NAME);
		if(!ret)
		{
			off = task->ParamH;
			memset(buff,0,__ATK_FILE_BUFF_SIZE__);
			length = atk_get_file_size();
			ATK_DBG("File Size:%ld,buff_size:%d,read_blk:%d,off:%d\n",
					length,__ATK_FILE_BUFF_SIZE__,__ATK_READ_BLK__,off);
			
			dnl_sum = 0;
			if(!off)
			{
				resend = 1024;
			}

			task->Loop = TASK_SEVICE;
		}
		else
		{
			task->Loop = TASK_DESTROY;
		}
	}
	else if(task->Loop == TASK_DESTROY)
	{
		ATK_DBG("atk download destroy\n");
		atk_close_download_file();	
		task->Loop = TASK_QUIT;
	}
	else
	{
		ATK_DBG("atk download param error\n");
	}
	
	ATK_DBG("return:%d\n",task->Loop);

	return task->Loop;
}

int atk_download_file_init(void)
{
	atk_download_func_init(atk_task_func,ATK_RECHECK_TASK, 2);
	atk_file_download_init(atk_download_func,0x1);
	atk_download_func_init(atk_task_func,ATK_RECHECK2_TASK, 2);
	atk_file_download_init(atk_download_func,0x0);
	atk_download_func_init(atk_task_func,ATK_STATUS_TASK, 3);
}




