#ifndef __XTCP_H__
#define __XTCP_H__

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long



/*************************************************************************
配置宏定义
 ************************************************************************/
//业务数据长度(不包括包头的长度)
//来自手机的业务数据一般不会太长
#define OP_CODE_LEN 	(513)

//发送数据包缓冲大小
#define BUF_SIZE 		(OP_CODE_LEN+10)

#define PK_RCV_TIMEOUT   10
#define PK_REMIT_TIMEOUT 10
#define PK_REMIT_COUNT   6


/*************************************************************************
 接口函数定义
 ************************************************************************/
extern void business_deal(void);
/*************************************************************************
 
 ************************************************************************/
//同步字段定义
#define SYN1 		(0x55)
#define SYN2 		(0xAA)
#define BUS_ID      (0x08)


//数据包类型字段定义
#define DATA_PK 	(0x01<<4)
#define ACK_PK	 	(0x02<<4)
#define HEART_PK 	(0x03<<4)
#define DISCONNECT_PK	(0x04<<4)
#define SHAKEHAND_PK	(0x05<<4)

//串口接收缓冲区，要求大于包长即可
#define PK_BUF_SIZE 			(sizeof(packet))	

//xsocket.flag标志定义
#define SCK_NEWDATA       (1<<0)
#define SCK_REMIT_TIMEOUT (1<<1)
#define SCK_REMIT_FLAG    (1<<2)

/*************************************************************************
 
 ************************************************************************/
//包头结构
typedef struct {
	U8 sync[2];	//该字段交由收包程序标识包开始，不必加入传输层
	U8 port;
	U8 flag;
	U8 id;
	U8 sn;
	U8 len[2];
	U8 reserve;
	U8 crc;
}xys_tcp_hdr;


//数据包结构
typedef struct{
	xys_tcp_hdr hdr;
	U8 business[OP_CODE_LEN];
}packet;

//端口套接字
typedef struct{
	U8  sn_d;	    //终端包序号
	U8  sn_s;	    //本地包序号
	U16 time_out;	//重传超时定时器
	U8  flag;       //标识 新数据到来、重传、或者重传超时等

	U8  remit_count;			//重传次数
	U8  buf[BUF_SIZE];	 		//缓冲区，可用于重传
}xsocket;


/*************************************************************************
 全局变量
 ************************************************************************/
//收包超时定时器，对其设置一个初值，定时中断会每隔1毫秒减1，为0表示超时
extern volatile U16 pk_time_out;	

//包接收缓冲区
extern packet pk_buf;
extern xsocket  xsck[1];


/*************************************************************************
 接口函数
 ************************************************************************/
void xsocket_init(void);

U16 pk_read(void);

void xtcp_in(void);
void xtcp_out(U8 channel);
void xtcp_remit(void);
unsigned char check_business(unsigned char *p, unsigned int len);
U8 crc_cal(void *pt, U32 cnt);
void tcp_send(unsigned char port, unsigned int len);
#endif


