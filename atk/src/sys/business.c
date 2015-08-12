#include <stdio.h>
#include <string.h>
#include "business.h"
#include "xtcp.h"


extern int do_smthing;
extern unsigned char test_flag;
extern unsigned char test_cmd;
extern unsigned char dis_flag;
extern void tcp_send(unsigned char port, unsigned int len);

int bin_fd;
int bin_size;
int pk_prg_sn;
int program_finish = 0;
//unsigned char program_bin[PROGRAM_SZ];

void send_update_cmd(void)
{
	xsocket *sck = &xsck[0];
	sck->buf[10] = 0xff;
	tcp_send(0,1);
}

void send_test_cmd(void)
{
    xsocket *sck = &xsck[0];
    sck->buf[10] = 0xff;
    tcp_send(0,1);
}

void send_test_iap(unsigned char data)
{
    xsocket *sck = &xsck[0];
    sck->buf[10] = 0xfa;
    sck->buf[11] = data;
    sck->buf[12] = 0x00;
    tcp_send(0,3);
}

void send_iap_data(unsigned char *ptr,int len)
{
    int i,temp;
    xsocket *sck = &xsck[0];
    unsigned char *cp;
    sck->buf[10] = 0xfd;
    cp = &sck->buf[11];
    memcpy(cp,ptr,len);
    /*for(i = 0; i < 523; i++)
    {
      if( (i % 16) == 0)
        printf("\n");
      temp = (sck->buf[i] & 0xff);
      if(temp < 16)
      {
          printf(":0%X",temp);
      }
      else
      {
          printf(":%2X",temp);
      }
    }
    printf("\ndisplay send package end!\n");
    do_smthing = 0;*/
    tcp_send(0,len+1);
}

void display_dev_msg(void)
{
  U16 business_len;
  urine_dev_msg *str_dev_msg;
  str_dev_msg = (urine_dev_msg *)pk_buf.business;
  business_len = pk_buf.hdr.len[0]+pk_buf.hdr.len[1]*256;
  if( business_len == sizeof(urine_dev_msg))
    {
      printf("get device message!\n");
      printf("device message:\n");
      printf("Dev hardware version:%d.%d\n",str_dev_msg->hareware[1],str_dev_msg->hareware[0]);
      printf("Dev software version:%d.%d\n",str_dev_msg->sofeware[1],str_dev_msg->sofeware[0]);
      printf("Flash capacity:%dK\n",(str_dev_msg->f_capacity[1]*256 + str_dev_msg->f_capacity[0]));
      printf("Flash page size:%d byte\n",(str_dev_msg->f_pagesz)*128);
    }
}


void display_iap_msg(void)
{
  U8 syc_cmd,SysChkSun;
  U16 business_len;
  urine_test_msg *str_iap_msg;
  str_iap_msg = (urine_test_msg *)pk_buf.business;
  business_len = pk_buf.hdr.len[0]+pk_buf.hdr.len[1]*256;
  if( business_len == sizeof(urine_test_msg))
  {
    syc_cmd = str_iap_msg->data;
    switch(syc_cmd)
    {
      case 0xfd:
        break;
        case 0xfe:
            printf("system reset!\n");
            break;
        default:
            printf("IAP in flash msg1:%d\n",str_iap_msg->data);
            printf("IAP in flash msg2:%d\n",str_iap_msg->reserved);
            break;
    }
  }
  printf("IAP test mode ok!\n");
}

/**
  * @brief :
	* @param :None
	* @retval:None
	*/
unsigned char g_sys_flag = 0;
void business_deal(void)
{
#if 0
  static int count_number = 0;
  xsocket *sck = &xsck[0];
  if(sck != NULL) {
    if (pk_buf.hdr.port == 0) {  //command pipe
      if (sck->flag & SCK_NEWDATA) {
      sck->flag &= ~SCK_NEWDATA;
      switch (pk_buf.business[0]) {
        case 0xfe:
          dis_flag |= DIS_DEV_MSG;
          break;
        case  0xfc:
          dis_flag |= DIS_PROGRAM;
          break;
        case  0xfb:
          printf("IAP mode finish!\n");
          break;
        case  0xfa:
          dis_flag |= DIS_IAP_MSG;
          printf("IAP test mode!\n");
          break;
        defaule:
          printf("rev_package data error!");
          break;
        }
      }
    }
  }
#endif
}


void handle(void)
{
#if 0
    int file_fd,nrd;

    int i,temp;

	if(test_flag & TEST_IAP_MODE)
	{
	    test_flag &= ~TEST_IAP_MODE;
		printf("test iap cmd.\n");
		if(test_cmd == CMD_SENDBIN)
		{
          if(pk_prg_sn > 0)
          {

              nrd = read(bin_fd,program_bin,sizeof(program_bin));
              if(nrd > 0)
              {
                  printf("send %d program package!\n",pk_prg_sn);
                  send_iap_data(program_bin,nrd);
                  if(nrd == 512)
                  {
                      lseek(bin_fd,0,SEEK_CUR);
                  }
              }
              else
              {
                  printf("read error!\n");
                  do_smthing = 0;
              }
              pk_prg_sn--;
              if(pk_prg_sn == 0)
              {
                  printf("IAP program package send over!\n");
                  program_finish = 1;
              }
              printf("next send %d program package!\n",pk_prg_sn);
          }
		}
		else
		{
		  printf("send test iap cmd!\n");
		  send_test_iap(test_cmd);
		}
	}
	else if(test_flag & TEST_DEV_MODE) //get message
    {
        test_flag &= ~TEST_DEV_MODE;
        printf("test cmd.\n");
        send_test_cmd();
    }
	else if(dis_flag & DIS_DEV_MSG)
	{
	    dis_flag &= ~DIS_DEV_MSG;
	    display_dev_msg();
	    do_smthing = 0;
	}
    else if(dis_flag & DIS_IAP_MSG)
    {
        dis_flag &= ~DIS_IAP_MSG;
        display_iap_msg();
        do_smthing = 0;
    }
    else if(dis_flag & DIS_PROGRAM)
    {
        dis_flag &= ~DIS_PROGRAM;
        printf("IAP program ack!\n");
        do_smthing = 0;
    }
#endif
}
