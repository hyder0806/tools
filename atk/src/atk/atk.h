#ifndef __ATK_H__
#define __ATK_H__

#include <stdio.h>






#define TASK_TIME_OUT   5

#define ATK_OK			0
#define ATK_ERR			1
#define ATK_MASK		0xff

#define ATK_S   "\x05\x05"
#define ATK_F   "\x04\x04"
#define ATK_W   "\x02\x02"
#define ATK_R   "\x01\x01"

#define ZERO14  "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
#define ZERO12  "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
#define ZERO4   "\x0\x0\x0\x0"
#define ZERO1   "\x0"

#define WR_INT  "\x20"
#define WR_CHAR  "\x08"

#define ATK_R_CMD(addr, type, len)  ATK_R addr type len ZERO4 ZERO1 
#define ATK_W_CMD(addr, type, val)  ATK_W addr type ZERO4 val ZERO1 
#define ATK_F_CMD(addr, val, type)  ATK_F addr ZERO1 val ZERO4 type

#define ATK_RST_IMX     	"\x6\x6\x2\x4" ZERO12
#define ATK_RST_LEN     	(sizeof(ATK_RST_IMX)-1)
#define ATK_RST_CMD			0

#define ATK_GET_STATUS  	"\x5\x5" ZERO14

#define ATK_IS_SECURE   	"\x12\x34\x34\x12"
#define ATK_UN_SECURE   	"\x56\x78\x78\x56"
#define ATK_STATUS		   	"\x88\x88\x88\x88"
#define SECURE				1
#define UNSECURE			2
#define STATUS				3

#define ATK_RES_LEN			4
#define ATK_STATUS_LEN		4

//atk connect status function
#define IMX_INIT_REG  		"\xff\xff\xff\xff"
#define IMX_INIT_VAL  		"\x0\x0\x0\x0"
#define ATK_INIT_IMX  		ATK_W_CMD(IMX_INIT_REG,WR_INT,IMX_INIT_VAL)
#define ATK_INIT_LEN  		(sizeof(ATK_INIT_IMX)-1)

//atk connect status function
#define IMX_CLKCTL_REG  	"\x53\xf8\x0\x8"
#define IMX_CLKCTL_VAL  	"\x30\x3\x40\x0"
#define ATK_CLKCTL_IMX  	ATK_W_CMD(IMX_CLKCTL_REG,WR_INT,IMX_CLKCTL_VAL)
#define ATK_CLKCTL_LEN  	(sizeof(ATK_CLKCTL_IMX)-1)

/* CS5_CSCRU */
#define ATK_CS5CRU_REG 		"\xB8\x0\x20\x50"
#define ATK_CS5CRU_VAL		"\x0\x0\xD8\x43"
#define ATK_CS5CRU_IMX		ATK_W_CMD(ATK_CS5CRU_REG,WR_INT,ATK_CS5CRU_VAL)
#define ATK_CS5CRU_LEN		(sizeof(ATK_CS5CRU_IMX)-1)

/* CS5_CSCRL */
#define ATK_CS5CRL_REG		"\xB8\x0\x20\x54"
#define	ATK_CS5CRL_VAL		"\x22\x25\x25\x21"
#define ATK_CS5CRL_IMX		ATK_W_CMD(ATK_CS5CRL_REG,WR_INT,ATK_CS5CRL_VAL)
#define ATK_CS5CRL_LEN		(sizeof(ATK_CS5CRL_IMX)-1)

/* CS5_CSCRA */	
#define	ATK_CS5CRA_REG		"\xB8\x0\x20\x58"
#define ATK_CS5CRA_VAL		"\x22\x22\x0A\x0"
#define ATK_CS5CRA_IMX		ATK_W_CMD(ATK_CS5CRA_REG,WR_INT,ATK_CS5CRA_VAL)
#define ATK_CS5CRA_LEN		(sizeof(ATK_CS5CRA_IMX)-1)

/* enable mDDR */	
#define ATK_MDDR_REG		"\xB8\x0\x10\x10"
#define ATK_MDDR_VAL		"\x0\x0\x0\x4"
#define ATK_MDDR_IMX		ATK_W_CMD(ATK_MDDR_REG,WR_INT,ATK_MDDR_VAL)
#define ATK_MDDR_LEN		(sizeof(ATK_MDDR_IMX)-1)


/* precharge command */
#define ATK_ESDMISC_REG		"\xB8\x00\x10\x00"
#define ATK_ESDMISC_VAL		"\x93\x21\x00\x00"
#define ATK_ESDMISC_IMX		ATK_W_CMD(ATK_ESDMISC_REG,WR_INT,ATK_ESDMISC_VAL)
#define ATK_ESDMISC_LEN		(sizeof(ATK_ESDMISC_IMX)-1)


/* SDRAM Control Register 0 precharge all dummy write */
#define ATK_ESDCTL0_REG		"\x80\x00\x04\x00"
#define ATK_ESDCTL0_VAL		"\x12\x34\x43\x21"
#define ATK_ESDCTL0_IMX		ATK_W_CMD(ATK_ESDCTL0_REG,WR_CHAR,ATK_ESDCTL0_VAL)
#define ATK_ESDCTL0_LEN		(sizeof(ATK_ESDCTL0_IMX)-1)


/* auto-refresh command */
#define ATK_AUTOFRESH_REG	ATK_ESDMISC_REG
#define ATK_AUTOFRESH_VAL	"\xA3\x21\x00\x00"
#define ATK_AUTOFRESH_IMX	ATK_W_CMD(ATK_AUTOFRESH_REG,WR_INT,ATK_AUTOFRESH_VAL)
#define ATK_AUTOFRESH_LEN	(sizeof(ATK_AUTOFRESH_IMX)-1)


/* dummy write for refresh */		
#define ATK_W_RE1_REG		"\x80\x00\x00\x00"
#define ATK_W_RE1_VAL		"\x12\x34\x43\x21"
#define ATK_W_RE1_IMX		ATK_W_CMD(ATK_W_RE1_REG,WR_INT,ATK_W_RE1_VAL)
#define ATK_W_RE1_LEN		(sizeof(ATK_W_RE1_IMX)-1)


/* dummy write for refresh */
#define ATK_W_RE2_REG		"\x80\x00\x00\x00"
#define ATK_W_RE2_VAL		"\x12\x34\x43\x21"
#define ATK_W_RE2_IMX		ATK_W_CMD(ATK_W_RE2_REG,WR_INT,ATK_W_RE2_VAL)
#define ATK_W_RE2_LEN		(sizeof(ATK_W_RE2_IMX)-1)


/* Load Mode Reg command - cas=3 bl=8 */		
#define ATK_LOAD_REG		"\xB8\x00\x10\x00"
#define ATK_LOAD_VAL		"\xB3\x21\x00\x00"
#define ATK_LOAD_IMX		ATK_W_CMD(ATK_LOAD_REG,WR_INT,ATK_LOAD_VAL)
#define ATK_LOAD_LEN		(sizeof(ATK_LOAD_IMX)-1)


/* dummy write -- address has the mode bits */
#define ATK_WR_ADDR_REG		"\x80\x00\x00\x23"
#define ATK_WR_ADDR_VAL		"\x00\x00\x00\xda"
#define ATK_WR_ADDR_IMX		ATK_W_CMD(ATK_WR_ADDR_REG,WR_CHAR,ATK_WR_ADDR_VAL)
#define ATK_WR_ADDR_LEN		(sizeof(ATK_WR_ADDR_IMX)-1)


/* dummy write -- address has the mode bits */
#define ATK_WR_DATA_REG		"\x84\x00\x00\x00"
#define ATK_WR_DATA_VAL		"\x00\x00\x00\xff"
#define ATK_WR_DATA_IMX		ATK_W_CMD(ATK_WR_DATA_REG,WR_CHAR,ATK_WR_DATA_VAL)
#define ATK_WR_DATA_LEN		(sizeof(ATK_WR_DATA_IMX)-1)


/*auto power counter 3<<10 */
#define ATK_COUNTER_REG		"\xB8\x00\x10\x00"
#define ATK_COUNTER_VAL		"\x83\x21\x68\x80"
#define ATK_COUNTER_IMX		ATK_W_CMD(ATK_COUNTER_REG,WR_INT,ATK_COUNTER_VAL)
#define ATK_COUNTER_LEN		(sizeof(ATK_COUNTER_IMX)-1)


/* dummy write for refresh */
#define ATK_W_RE3_REG		"\x80\x00\x00\x00"
#define ATK_W_RE3_VAL		"\x12\x34\x43\x21"
#define ATK_W_RE3_IMX		ATK_W_CMD(ATK_W_RE3_REG,WR_INT,ATK_W_RE3_VAL)
#define ATK_W_RE3_LEN		(sizeof(ATK_W_RE3_IMX)-1)

/* dummy write for refresh */
#define ATK_W_RE4_REG		"\x80\x00\x00\x00"
#define ATK_W_RE4_VAL		"\x12\x34\x43\x21"
#define ATK_W_RE4_IMX		ATK_W_CMD(ATK_W_RE4_REG,WR_INT,ATK_W_RE4_VAL)
#define ATK_W_RE4_LEN		(sizeof(ATK_W_RE4_IMX)-1)


#define ATK_SET_SDRAM_REG	"\xB8\x00\x10\x04"
#define ATK_SET_SDRAM_VAL	"\x00\x29\x56\x2F"
#define ATK_SET_SDRAM_IMX	ATK_W_CMD(ATK_SET_SDRAM_REG,WR_INT,ATK_SET_SDRAM_VAL)
#define ATK_SET_SDRAM_LEN	(sizeof(ATK_SET_SDRAM_IMX)-1)


/* dummy write for refresh */
#define ATK_W_RE5_REG		"\x80\x00\x00\x00"
#define ATK_W_RE5_VAL		"\x12\x34\x43\x21"
#define ATK_W_RE5_IMX		ATK_W_CMD(ATK_W_RE5_REG,WR_INT,ATK_W_RE5_VAL)
#define ATK_W_RE5_LEN		(sizeof(ATK_W_RE5_IMX)-1)

/* End atk command */
#define ATK_END_REG		"\x80\x00\x00\x00"
#define ATK_END_VAL		"\x00\x00\x00\x00"
#define ATK_END_IMX		ATK_W_CMD(ATK_END_REG,WR_INT,ATK_END_VAL)
#define ATK_END_LEN		(sizeof(ATK_END_IMX)-1)


#define ATK_RECHECK_REG		"\x83\xF0\x00\x00"
#define ATK_RECHECK_VAL		"\x00\x08\xEA\x95" //sec
//#define ATK_RECHECK_VAL		"\x00\x07\xD1\x41" //unsec
#define ATK_RECHECK_TYPE	"\x00"
#define ATK_RECHECK_IMX		ATK_F_CMD(ATK_RECHECK_REG,ATK_RECHECK_VAL,ATK_RECHECK_TYPE)
#define ATK_RECHECK_LEN		(sizeof(ATK_RECHECK_IMX)-1)

#define ATK_RECHECK2_VAL	"\x00\x00\x04\x00"
#define ATK_RECHECK2_TYPE	"\xAA"
#define ATK_RECHECK2_IMX	ATK_F_CMD(ATK_RECHECK_REG,ATK_RECHECK2_VAL,ATK_RECHECK2_TYPE)
#define ATK_RECHECK2_LEN	(sizeof(ATK_RECHECK2_IMX)-1)



enum eTASK{
ATK_INIT_TASK = 1,
ATK_CLKCTL_TASK,
ATK_CS5CRU_TASK,
ATK_CS5CRL_TASK,
ATK_CS5CRA_TASK,
ATK_MDDR_TASK,
ATK_ESDMISC_TASK,
ATK_ESDCTL0_TASK,
ATK_AUTOFRESH_TASK,
ATK_W_RE1_TASK,
ATK_W_RE2_TASK,
ATK_LOAD_TASK,
ATK_WR_ADDR_TASK,
ATK_WR_DATA_TASK,
ATK_COUNTER_TASK,
ATK_W_RE3_TASK,
ATK_W_RE4_TASK,
ATK_SET_SDRAM_TASK,
ATK_W_RE5_TASK,
ATK_END_TASK,
ATK_RECHECK_TASK,
ATK_RECHECK2_TASK,
ATK_STATUS_TASK
};

#define LAST_ONE			ATK_END_TASK

#endif
