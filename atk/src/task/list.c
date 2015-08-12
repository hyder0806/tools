#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "mem.h"
#include "list.h"

//#define LIST_DEBUG

#ifdef LIST_DEBUG
#define LIST_DBG(x...) printf(x)
#else
#define LIST_DBG(x...)
#endif

pMsgNode gMsgStart = NULL;
pMsgNode gMsgEnd = NULL;

unsigned char msg_list_init(void)
{
	gMsgStart = (pMsgNode)malloc(sizeof(HFC_MSG_NODE));
	if(!gMsgStart)
	{
		return MSG_INIT_ERR1;
	}

	gMsgStart->pMsg = (HFC_MSG *)mem_list_init(__HFC_MSG_SIZE__);
	if(!gMsgStart->pMsg)
		return MSG_INIT_ERR2;
	else
	{
		memset((unsigned char *)gMsgStart->pMsg,0,__HFC_MSG_SIZE__);
		gMsgStart->pNext = NULL;
	}

	gMsgEnd = gMsgStart;

	return MSG_TRUE;
}

void msg_list_display(void)
{
	pMsgNode pCur;
	
	if(gMsgEnd->pNext != NULL)
		return;

	pCur = gMsgStart;
	while(pCur != NULL)
	{
		if(pCur->pMsg != NULL)
		{
			printf("Node[%p]\n",pCur);
			printf("\tMsg[%p]\t",pCur->pMsg);
			printf("Status[%d]\t",pCur->pMsg->status);
			printf("CallBk[%p]\t",pCur->pMsg->pMsgCallback);
			//printf("Buff[%p]\t",pCur->pMsg->buff);
			printf("High[%d]\t",pCur->pMsg->HParam);
			printf("Low[%d]\n",pCur->pMsg->LParam);
		}
		pCur = pCur->pNext;
	}
	printf("\n");
}

void msg_list_free(void)
{
	pMsgNode pCur;

	while(gMsgStart != NULL)
	{
		pCur = gMsgStart;	
		gMsgStart = gMsgStart->pNext;
		pCur->pMsg = NULL;
		LIST_DBG("free task:%p\n",pCur);
		free(pCur);
	}
	gMsgEnd = NULL;
	
	mem_list_free();
}

unsigned char msg_list_find_node(void *pFun)
{
	pMsgNode pCur;
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	pCur = gMsgStart->pNext;
	while( pCur != NULL )
	{
		if(pCur->pMsg->pMsgCallback == pFun)
		{
			LIST_DBG("find func %p\n",pFun);
			return MSG_TRUE;
		}
		pCur = pCur->pNext;
	}
			
	return MSG_FIND_ERR;
}

unsigned char msg_list_insert_ahead(unsigned char (*pFun)(struct _TASK *task), sMsg * node)
{
	pMsgNode pTmp;
	
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;
			
	pTmp = (pMsgNode)malloc(sizeof(pMsgNode));
	if(!pTmp)
		return MSG_INSERT_ERR1;
	
	pTmp->pMsg = (HFC_MSG *)mem_get_free_node(__HFC_MSG_SIZE__);
	if(!pTmp->pMsg)
	{
		free(pTmp);
		return MSG_INSERT_ERR2;
	}

	memset((unsigned char *)pTmp->pMsg,0,__HFC_MSG_SIZE__);

	pTmp->pMsg->pMsgCallback = pFun;
	pTmp->pMsg->status = node->status;
	//pTmp->pMsg->buff = node->buff;
	pTmp->pMsg->HParam = node->HParam;
	pTmp->pMsg->LParam = node->LParam;
	
	if(gMsgStart->pNext == NULL)
	{
		gMsgEnd = pTmp;
		pTmp->pNext = gMsgStart->pNext;
		gMsgStart->pNext = pTmp;
	}
	else
	{
		if(gMsgEnd == gMsgStart->pNext)
		{
			gMsgEnd = pTmp;
		}
		pTmp->pNext = gMsgStart->pNext->pNext;
		gMsgStart->pNext->pNext = pTmp;
	}

	return MSG_TRUE;
}

unsigned char msg_list_insert_end(unsigned char (*pFun)(struct _TASK *task), sMsg * node)
{
	pMsgNode pTmp;
			
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	pTmp = (pMsgNode)malloc(sizeof(pMsgNode));
	if(!pTmp)
		return MSG_INSERT_ERR1;
	
	pTmp->pMsg = (HFC_MSG *)mem_get_free_node(__HFC_MSG_SIZE__);
	if(!pTmp->pMsg)
	{
		free(pTmp);
		return MSG_INSERT_ERR2;
	}
		
	memset((unsigned char *)pTmp->pMsg,0,__HFC_MSG_SIZE__);

	pTmp->pNext = NULL;
	pTmp->pMsg->pMsgCallback = pFun;
	pTmp->pMsg->status = node->status;
	//pTmp->pMsg->buff = node->buff;
	pTmp->pMsg->HParam = node->HParam;
	pTmp->pMsg->LParam = node->LParam;

	gMsgEnd->pNext = pTmp;
	gMsgEnd = pTmp;

	return MSG_TRUE;
}

unsigned char msg_list_delete_node(void *pFun)
{
	pMsgNode previous,pCur;
	
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	previous = gMsgStart;
	pCur = gMsgStart->pNext;
	
	while(pCur != NULL)
	{
		if(pCur->pMsg->pMsgCallback == pFun)
		{
			if(pCur == gMsgEnd)
				gMsgEnd = previous;
				
			previous->pNext = pCur->pNext;
			mem_free_node(pCur->pMsg);
			free(pCur);
			return MSG_TRUE;
		}
		pCur = pCur->pNext;
		previous = previous->pNext;
	}

	return MSG_DEL_ERR;
}

unsigned char msg_list_delete_first_node(void)
{
	pMsgNode pCur;
	
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	pCur = gMsgStart->pNext;
	
	if(pCur != NULL)
	{
		if(pCur == gMsgEnd)
			gMsgEnd = gMsgStart;
			
		gMsgStart->pNext = pCur->pNext;
		mem_free_node(pCur->pMsg);
		free(pCur);
		return MSG_TRUE;
	}

	return MSG_DEL_ERR;
}

sMsg * msg_list_check(void)
{
	pMsgNode pCur;
	
	if(gMsgEnd->pNext != NULL)
		return NULL;

	pCur = gMsgStart->pNext;
	if(pCur == NULL)
		return NULL;

	if(pCur->pMsg->pMsgCallback != NULL)
	{
		LIST_DBG("check ok\n");
		return pCur->pMsg;
	}
	else
	{
		return NULL;
		LIST_DBG("check err\n");
	}
}

unsigned char msg_list_process(struct _TASK *task)
{
	pMsgNode pCur;
	
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	pCur = gMsgStart->pNext;
	if(pCur->pMsg->pMsgCallback == NULL)
		return MSG_FAIL;

	LIST_DBG("msg process\n");
	return pCur->pMsg->pMsgCallback(task);
}

#if 0
unsigned char msg_list_quit(void)
{
	pMsgNode pCur;
	
	if(gMsgEnd->pNext != NULL)
		return MSG_LIST_ERR;

	if(gMsgStart->pNext == NULL)
		return MSG_FAIL;

	pCur = gMsgStart->pNext;
	gMsgStart->pNext = gMsgStart->pNext->pNext;
	
	if((pCur == gMsgEnd) && (pCur->pNext == NULL))
		gMsgEnd = gMsgStart;
	
	mem_free_node(pCur->pMsg);
	free(pCur);
	
	return MSG_TRUE;
}
#endif

