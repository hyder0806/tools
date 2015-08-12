#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

//#define MEM_DEBUG

#ifdef MEM_DEBUG
#define MEM_DBG(x...) printf(x)
#else
#define MEM_DBG(x...)
#endif


pMemNode gMemLink = NULL;

void * mem_list_init(unsigned int size)
{
	unsigned int *pMemFst = NULL;

	pMemFst = (unsigned int *)malloc(size);
	if(!pMemFst)
		return NULL;

	gMemLink = (pMemNode)malloc(sizeof(HFC_MEM_NODE));
	if(!gMemLink)
		return NULL;
	else
	{
		gMemLink->flag = IN_USED;
		gMemLink->pMem = pMemFst;
		gMemLink->size = size;
		gMemLink->pNext = NULL;
	}
	return gMemLink->pMem;
}

void mem_list_display(void)
{
	pMemNode pCur;

	pCur = gMemLink;
	while ( pCur != NULL)
	{
		printf("Flag[%d] - Size[%d] - Mem[%p]\n",
			pCur->flag,pCur->size,pCur->pMem);
		pCur = pCur->pNext;
	}
	printf("\n");
}

void mem_list_free(void)
{
	pMemNode pCur;

	while(gMemLink != NULL)
	{
		pCur = gMemLink;	
		gMemLink = gMemLink->pNext;
		if(pCur->pMem != NULL)
		{
			MEM_DBG("free mem %p, flag %d\n",pCur->pMem,pCur->flag);
			free(pCur->pMem);
		}
		free(pCur);
	}
}

void * mem_insert_node(pMemNode pDst,unsigned int size)
{
	pMemNode NewNode;
	void *MemBuf;

	MemBuf = (unsigned int *)malloc(size);
    if(!MemBuf)
        return NULL;

	NewNode = (pMemNode)malloc(sizeof(HFC_MEM_NODE));
    if(!NewNode)
	{
		free(MemBuf);
        return NULL;
	}

	NewNode->flag = IN_USED;
    NewNode->pMem = MemBuf;
	NewNode->size = size;
    NewNode->pNext = NULL;
         
    NewNode->pNext = pDst->pNext;
    pDst->pNext = NewNode;

	return NewNode->pMem;
}


pMemNode mem_find_node(pMemNode pDst)
{
	pMemNode pCur;
	
	pCur = gMemLink->pNext;
	while( pCur != NULL )
	{
		if(pCur->flag == UN_USED)
		{
			return pCur;
		}
		pCur = pCur->pNext;
	}
	return pCur;
}

void * mem_get_free_node(unsigned int size)
{
	void * pMemFree;
	pMemNode pCur = NULL;

	pCur = mem_find_node(gMemLink);
	if(!pCur)
	{
		pMemFree = mem_insert_node(gMemLink,size);
		if(!pMemFree)
		{
			MEM_DBG("create memory node error!\n");
			return NULL;
		}
		else
		{
			MEM_DBG("get mem success\n");
			return pMemFree;
		}
	}
	else
	{
		if(pCur->size == size)
		{
			MEM_DBG("get mem in pool\n");
			pCur->flag = IN_USED;
			return pCur->pMem;
		}
		else
		{
			MEM_DBG("get free mem node failed!\n");
			return NULL;
		}
	}
}

unsigned char mem_free_node(void *lpMem)
{
    pMemNode previous;

    previous = gMemLink;
    
    do
	{
        previous = previous->pNext;
		if(lpMem == previous->pMem)
		{
			MEM_DBG("free mem: %p\n",previous->pMem);
			previous->flag = UN_USED;
			return 0;
		}
    }while(previous->pNext != NULL);


    return 1;
}

#if 0
void main(void)
{
	gMemLink = mem_list_init(64);
	if(!gMemLink)
	{
		printf("err");
		exit(1);
	}
	mem_list_display(gMemLink);
	mem_list_free(gMemLink);
}
#endif
