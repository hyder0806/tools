/**************************************Copyright( c ) *****************************************
**
**
**---------File Info-------------------------------------------------------------------------------------------------------------------
** Created By: hyder
** Created Date: 2015-03-21
** Version : 
** Description: task interface
**
*******************************************************************************************/

#include "main.h"
#include "task.h"

//#define TASK_DEBUG

#ifdef TASK_DEBUG
#define TASK_DBG(x...) g_print(x)
#else
#define TASK_DBG(x...)
#endif

static volatile unsigned char task_init = TASK_Q_NULL;

sMsg sMsgTmp = {0};
static TASK sTask;

static void task_struct_init(void)
{
	memset(&sTask,0,sizeof(sTask));
}

unsigned char task_queue_init(void)
{
	unsigned char ret;

	if(task_init != TASK_Q_NULL)
		return TASK_Q_ALIVE;

	ret = msg_list_init();
	if(ret != TASK_OK)
		return TASK_Q_INIT_ERR;
		
	TASK_DBG("Task init\n");
			
	task_struct_init();
	task_init = TASK_Q_ALIVE;
	
	return TASK_OK;
}

unsigned char task_queue_check(void)
{
	sMsg *msg = NULL;

	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	msg = msg_list_check();
	if(msg == NULL)
	{
		return 2;
	}
	
	TASK_DBG("L:%d,S:%d\n",sTask.Loop,msg->status);

	if(msg->status == 1)
	{
		TASK_DBG("get data\n");
		sTask.Loop = msg->status;
		sTask.ParamH = msg->HParam;
		sTask.ParamL = msg->LParam;
		msg->status = 2;
	}
	else
	{
		msg->status = sTask.Loop;
		TASK_DBG("status:%d\n",msg->status);
	}
	
	return TASK_OK;
}

unsigned char task_queue_add(unsigned char (*pFun)(struct _TASK *task),spTask tsk)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	memset(&sMsgTmp,0,sizeof(sMsg));
	sMsgTmp.status = (tsk->Loop & 0xf);
	//sMsgTmp.buff = tsk->Buffer;
	sMsgTmp.HParam = tsk->ParamH;
	sMsgTmp.LParam = tsk->ParamL;
	
	if((tsk->Loop & 0xf0) == TASK_PRI_HIGH)
	{
		TASK_DBG("add ahead, satus:%d\n",sMsgTmp.status);
		msg_list_insert_ahead(pFun,&sMsgTmp);
	}
	else
	{
		TASK_DBG("add end, satus:%d\n",sMsgTmp.status);
		msg_list_insert_end(pFun,&sMsgTmp);
	}
	
	return TASK_OK;
}

unsigned char task_queue_del(void * pFun)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	msg_list_delete_node(pFun);
	
	return TASK_OK;
}

unsigned char task_queue_is_exist(void * pFun)
{
	unsigned char ret;

	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	ret = msg_list_find_node(pFun);
	if(ret != 0)
	{
		return TASK_FAIL;
	}
	else
	{
		return TASK_OK;
	}
}

unsigned char task_queue_process(struct _TASK *task)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	return msg_list_process(task);	
}

unsigned char task_queue_fun_quit(void)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	TASK_DBG("Task quit\n");
	msg_list_delete_first_node();

	return TASK_OK;
}

unsigned char  task_queue_display(void)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	msg_list_display();
	return TASK_OK;
}

unsigned char task_queue_destroy(void)
{
	if(task_init == TASK_Q_NULL)
		return TASK_Q_UNINIT;

	msg_list_free();

	return TASK_OK;
}

void task_queue_schedule(void)
{
    if(!task_queue_check())
    {
        if(task_queue_process(&sTask) == TASK_QUIT)
        {
            task_queue_fun_quit();
			task_struct_init();
        }
    }
}
