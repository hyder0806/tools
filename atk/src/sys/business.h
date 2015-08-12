#ifndef __BUSINESS_H__
#define __BUSINESS_H__

extern unsigned char g_sys_flag;


typedef struct {
	unsigned char cmd_type;
	unsigned char hareware[2];
	unsigned char sofeware[2];
	unsigned char f_capacity[2];
	unsigned char f_pagesz;
	unsigned char reserved;
}urine_dev_msg;

typedef struct {
    unsigned char cmd_type;
    unsigned char data;
    unsigned char reserved;
}urine_test_msg;


void handle(void);

/***********test_function***********************/
void send_update_cmd(void);
void send_test_cmd(void);
void display_iap_msg(void);
void send_test_iap(unsigned char data);
/***********************************************/


#endif
