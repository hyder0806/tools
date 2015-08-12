#ifndef __TASK_H__
#define __TASK_H__

#include "list.h"
#include "mem.h"
#include "ttask.h"

#define TASK_OK 		0
#define TASK_QUIT 		0
#define TASK_INIT 		1
#define TASK_SEVICE		2
#define TASK_TIMEOUT	3
#define TASK_DESTROY	4
#define TASK_FAIL		5 

#define TASK_Q_UNINIT	6
#define TASK_Q_ALIVE	7
#define TASK_Q_NULL		8
#define TASK_Q_INIT_ERR	9 


#define TASK_PRI_HIGH	0xe0

extern void task_queue_schedule(void);
extern unsigned char task_queue_init(void);
extern unsigned char task_queue_check(void);
extern unsigned char task_queue_del(void * pFun);
extern unsigned char task_queue_is_exist(void * pFun);
extern unsigned char task_queue_process(struct _TASK *task);
extern unsigned char task_queue_fun_quit(void);
extern unsigned char task_queue_display(void);
extern unsigned char task_queue_destroy(void);
extern unsigned char task_queue_add(unsigned char (*pFun)(struct _TASK *task),spTask tsk);
#if 0
extern unsigned char task_queue_add(unsigned char (*pFun)(spTask task),
                                unsigned char Priority,
                                unsigned char *Buff,
                                unsigned char HParam,
                                unsigned char LParam);
#endif
#endif

