#include <stdio.h>
#include <string.h>
#include "xtcp.h"
#include "buff.h"
#include "uart.h"
#include "cmd_type.h"


//#undef DEBUG
#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) printf(x)
#else
#define DPRINTK(x...)
#endif

//全局变量
volatile U16 pk_time_out = 0;	  //收包超时定时器，对其设置一个初值，定时中断会每隔1毫秒减1，为0表示超时
packet   pk_buf;					//包接收缓冲区
xsocket  xsck[1];

extern int uart_fd;

/*************************************************************************
 内部函数
 ************************************************************************/
static U16 pk_poll(void);
static void send_ack(void);
static void send_pk(void *pk);



const U8 crc_table[256];


/**
  * @brief :初始化通道套接字
	* @param :None
	* @retval:None
	*/
void xsocket_init(void)
{
	memset(&xsck[0],0,sizeof(xsocket));
}

/**
  * @brief :
	* @param :None
	* @retval:None
	*/
static void send_ack(void)
{
    U8 i;
    xys_tcp_hdr ack_pk = pk_buf.hdr;
    U8 *buf = (U8 *)&ack_pk;
    ack_pk.flag &= 0x0f;
    ack_pk.flag |= ACK_PK;  //设置回复标志
    ack_pk.len[0] = 0x00;
    ack_pk.len[1] = 0x00;
    ack_pk.crc = crc_cal(&ack_pk, sizeof(xys_tcp_hdr)-1);
    for ( i = 0; i < sizeof(ack_pk); i++)
    {
        //uart_send(&uart_fd,buf[i]);
    }
}

static void send_heart(void)
{
	U8 i;
	xys_tcp_hdr heart_pk = pk_buf.hdr;
	U8 *buf = (U8 *)&heart_pk;
	heart_pk.flag &= 0x0f;
	heart_pk.flag |= HEART_PK;
	heart_pk.len[0] = 0x0a;
	heart_pk.len[1] = 0x00;
	heart_pk.crc = crc_cal(&heart_pk, sizeof(xys_tcp_hdr)-1);
	for (i=0; i<sizeof(heart_pk); i++) {
		//uart_send(&uart_fd,buf[i]);
	}
}

/**
  * @brief :提取串口接收队列数据，并进行组包
	* @param : None
	* @retval: 返回包长   收到一包数据
	                0   没有收满一包数据
	*///没有处理校验
U8  xystcp_state = 0;
extern int do_smthing;
static U16 pk_poll(void)
{	
	static U16 bus_len  = 0;//业务数据长度，需要静态
    //(分成四个状态 0 1 2 3->收到SYN1,SYN2，包头数据处理,业务数据处理)需要静态
	U8  rcv_char = 0;
	U16 pk_len   = 0;

	if (!buff_dlen(&cbuff))		 //buff无数据，直接返回
		return 0;

	switch (xystcp_state) {
		case 0://检测同步字段SYN1，若检测到，则转向state 1
			buff_out(&cbuff,&rcv_char,1);
			if (rcv_char == SYN1) {
			    xystcp_state = 1;
				pk_time_out = PK_RCV_TIMEOUT;   // = 10 in xtcp.h
			}
			break;
		case 1://检测同步字段SYN2，若检测到，则转向state 2
			buff_out(&cbuff,&rcv_char,1);
			if ((rcv_char == SYN2)) {
			    xystcp_state = 2;
				pk_buf.hdr.sync[0] = 0x55;
				pk_buf.hdr.sync[1] = 0xaa;
			} else {
				pk_time_out = 0;
				xystcp_state = 0;
			}
			break;
		case 2://找到同步字段，接收包头，若包头正确，且有业务数据，转向state3
			if (buff_dlen(&cbuff) >= sizeof(xys_tcp_hdr)-2 )
			{//收到至少一个包头，采用数据块出队，增加效率
				buff_try(&cbuff, ((U8 *)(&pk_buf))+2, sizeof(xys_tcp_hdr)-2);//包头的55AA已经处理
				if (pk_buf.hdr.crc == crc_cal(&pk_buf, sizeof(xys_tcp_hdr)-1))
				{
				    buff_out(&cbuff,(BYTE *)0,sizeof(xys_tcp_hdr)-2);
					bus_len = pk_buf.hdr.len[0] + pk_buf.hdr.len[1] * 256;
					DPRINTK("rec bus_len=%d\n",bus_len);
					if (bus_len > 0)
					{
						if (bus_len <= OP_CODE_LEN){
						    xystcp_state = 3;
						}//有业务数据，数据两小于定义的最大长度 转向state3接收业务数据
						else{
						    xystcp_state = 0;
						}
					} else {
						pk_len = sizeof(xys_tcp_hdr);	//可能是ACK包，转向state0
						xystcp_state = 0;
						//do_smthing = 0;
						DPRINTK("transfer rec ack!\n");
					}
				}
				else {
					DPRINTK("crc cheak error\n");
					xystcp_state = 0;//若校验错误，丢弃该包，转向state0s
					pk_time_out = 0;
				}
			}
			break;
		case 3://接收业务数据，若业务数据校验错误，丢弃该包，并转向state0
			if (buff_dlen(&cbuff) >= (bus_len + 1) ) {
				buff_out( &cbuff, ((U8 *)(&pk_buf))+sizeof(xys_tcp_hdr), bus_len + 1);
				DPRINTK("real_rcv_len=%d\n",bus_len);
				//业务数据校验在传输层校验吗？  由于所有业务数据都有和校验，建议在传输层校验
				pk_len = bus_len + sizeof(xys_tcp_hdr) + 1;
				xystcp_state = 0;
				pk_time_out = 0;
			}
			break;
		default:
			break;
	}
	return pk_len;
}




/**
  * @brief : 在指定超时时间内收取一个数据包
	* @param : timeout   超时时间ms 一般一包数据小于500ms
	* @retval: 返回包长度：收到一包数据
	                   0:超时
	*/
U16 pk_read(void)
{
//	char get_pk_len;

#if 0
	else
	{
		pk_time_out = 0;	//初始设置超时为0，在pk_poll（）中检测到包开始时，才启动超时定时器
		do {
			get_pk_len = pk_poll();
			if (get_pk_len >= 10) {	 //包长合法才接收
				return get_pk_len;
			}
		} while (pk_time_out);
	}
#endif
	return 0;		
}

unsigned char check_business(unsigned char *p, unsigned int len) 
{
	unsigned int i;
	unsigned char tmp = 0;
	for (i = 0; i < len; i++) {
		tmp ^= *(p+i);
	}
	//DPRINTK("len=%d\n",len);
	return tmp;
}
/**
  * @brief :收到数据包后调用，处理回复，若有数据，提交给应用层
	* @param :None
	* @retval:None
	*/
void xtcp_in(void)
{
    U16 business_size;
    U8 check_business_flag;
	xsocket *sck = &xsck[0];
	static unsigned int err_times = 0;
	if (sck != NULL) {
		switch (pk_buf.hdr.flag & 0xf0) {
	      case DATA_PK: //数据包
	        business_size = pk_buf.hdr.len[0]+pk_buf.hdr.len[1]*256;
	        check_business_flag = check_business(pk_buf.business, business_size);
	        send_ack();
	        if( check_business_flag == pk_buf.business[business_size])
	        {
                DPRINTK("we get data_pk\n");
                sck->flag |= SCK_NEWDATA;
                sck->sn_d = pk_buf.hdr.sn; //更新通道套接字 目标序号
                sck->sn_d++;
                printf("console sn:%d\n",sck->sn_d);
                business_deal();            //若有数据，提交给应用层
	        }
	        else
	        {
                err_times++;
                DPRINTK("err_times:%d\n", err_times);
	        }
	        break;
		case ACK_PK: 	//应答包
			DPRINTK("we get ack_pk\n");
			if (sck->flag & SCK_REMIT_FLAG){  //若未收到应答，包重传标志为1
				if (pk_buf.hdr.sn == sck->sn_s){
					sck->sn_s++;
					sck->flag &= ~SCK_REMIT_FLAG; //收到指定序号的应答包后，表示传输成功
					printf("local sn:%d\n",sck->sn_s);
				}
			}
			break;
		case SHAKEHAND_PK:
			send_ack();
			sck->sn_s = 0;
			sck->sn_d = 0;
			break;
		case HEART_PK:
			send_heart();
			break;
		defaule: break;
		}
	}
}

/**
  * @brief :发送数据包，填充通道buf后，调用该函数启动发送
	* @param :None
	* @retval:None
	*/
void xtcp_out(U8 channel)
{
	xsocket *sck = &xsck[0];
	if (sck)
	{
		if (!(sck->flag & SCK_REMIT_FLAG))
		{	//等待该通道数据发送完成
			//配置套接字信息，以启用发送
			sck->flag |= SCK_REMIT_FLAG;
			sck->flag &= ~SCK_REMIT_TIMEOUT;
			sck->time_out = PK_REMIT_TIMEOUT;
			sck->remit_count = PK_REMIT_COUNT;
			send_pk(sck->buf); //send packet data
		}
	}
}


/**
  * @brief :
	* @param :None
	* @retval:None
	*/
void xtcp_remit(void)
{
	U8 i;
	for (i=0;i<1;i++) {
		if (xsck[i].flag & SCK_REMIT_FLAG) {    //重传标志
			if (xsck[i].remit_count) {
				if (xsck[i].time_out == 0) {	 //若超时，启动发送，并将重传次数减一
					xsck[i].remit_count--;
					xsck[i].time_out = PK_REMIT_TIMEOUT;
					send_pk(xsck[i].buf);    //do remit
					DPRINTK("remit!\n");
				}
			} else {
				xsck[i].flag |= SCK_REMIT_TIMEOUT;  //重传超时，告知应用层
				DPRINTK("remit timeout!\n");
				//do_smthing = 0;
				xsck[i].flag &= ~(SCK_REMIT_FLAG | SCK_REMIT_TIMEOUT);
			}
		}
	}
}

/**
  * @brief :
	* @param :None
	* @retval:None
	*/
static void send_pk(void *pk)
{
	xys_tcp_hdr *hdr = (xys_tcp_hdr *)pk;
	U16 pk_len,i;
	U8 *ptr_tmp = pk;

	if (hdr) {
		pk_len = sizeof(xys_tcp_hdr) + hdr->len[0] + hdr->len[1] * 256 + 1;
		if (pk_len > 10) {
			for (i=0;i<pk_len;i++) {
				//uart_send(&uart_fd,ptr_tmp[i]);
			}
		}
	}
}

void tcp_send(unsigned char port, unsigned int len)
{
	xys_tcp_hdr hdr;
	hdr.sync[0] = 0x55;
	hdr.sync[1] = 0xaa;
	hdr.port = port;
	hdr.flag = DATA_PK;
	hdr.id = BUS_ID;
	hdr.sn = xsck[0].sn_s;
	hdr.len[0] = len & 0xff;
	hdr.len[1] = (len >> 8) & 0xff;
	hdr.reserve = 0;
	hdr.crc = crc_cal(&hdr, sizeof(xys_tcp_hdr)-1);
	xsck[0].buf[len+sizeof(xys_tcp_hdr)] = check_business(&(xsck[0].buf[sizeof(xys_tcp_hdr)]),len);
	memcpy(xsck[0].buf, (unsigned char *)&hdr, sizeof(xys_tcp_hdr));
	xtcp_out(port);
}
/**
  * @brief : CRC  X8+X2+X+1
	* @param :None
	* @retval:None
	*/
U8 crc_cal(void *pt, U32 cnt)
{
    U8 crc = 0;
    U8 *ptr = pt;
    while (cnt--) {
        crc = crc_table[crc ^ *ptr++];
    }
    return crc;
}
const U8 crc_table[256]=
{
    0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,
    0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
    0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,
    0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
    0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,
    0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
    0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,
    0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
    0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,
    0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
    0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,
    0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
    0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,
    0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
    0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,
    0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
    0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,
    0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
    0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,
    0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
    0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,
    0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
    0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,
    0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
    0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,
    0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
    0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,
    0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
    0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,
    0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
    0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,
    0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
};

