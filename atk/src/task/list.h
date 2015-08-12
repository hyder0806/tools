#ifndef __LIST_H__
#define __LIST_H__

#include "ttask.h"

typedef struct _HFC_MSG
{
	//unsigned char priority;
	unsigned char status;
	unsigned char (*pMsgCallback)(struct _TASK *task);
	//unsigned char *buff;
	unsigned int HParam;
	unsigned int LParam;
}HFC_MSG;

typedef struct _HFC_MSG_NODE
{
	struct _HFC_MSG *pMsg;
	struct _HFC_MSG_NODE *pNext;
}HFC_MSG_NODE;

typedef HFC_MSG_NODE * pMsgNode;
typedef HFC_MSG sMsg;


#define MSG_TRUE 		0
#define MSG_QUIT 		0
#define MSG_INIT 		1
#define MSG_SEVICE		2
#define MSG_TIMEOUT		3
#define MSG_DESTROY		4
#define MSG_FAIL		5

#define MSG_LIST_ERR	6
#define MSG_DEL_ERR		7
#define MSG_FIND_ERR	8
#define MSG_INSERT_ERR1	9
#define MSG_INSERT_ERR2	10
#define MSG_INIT_ERR1	11
#define MSG_INIT_ERR2	12


#define __HFC_MSG_SIZE__ sizeof(HFC_MSG)

extern unsigned char msg_list_init(void);
extern void msg_list_display(void);
extern void msg_list_free(void);
extern unsigned char msg_list_find_node(void *pFun);
extern unsigned char msg_list_insert_ahead(unsigned char (*pFun)(struct _TASK *task), sMsg * node);
extern unsigned char msg_list_insert_end(unsigned char (*pFun)(struct _TASK *task), sMsg * node);
extern unsigned char msg_list_delete_node(void *pFun);
extern unsigned char msg_list_delete_first_node(void);
extern sMsg * msg_list_check(void);
extern unsigned char msg_list_process(struct _TASK *task);
extern unsigned char msg_list_quit(void);

#endif

