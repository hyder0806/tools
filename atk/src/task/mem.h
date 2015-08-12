
#ifndef __MEM_H__
#define __MEM_H__


#define IN_USED		1
#define UN_USED		0

typedef struct _HFC_MEM_NODE
{
	unsigned char flag;
	unsigned int size;
	void *pMem;
	struct _HFC_MEM_NODE *pNext;
}HFC_MEM_NODE;

typedef HFC_MEM_NODE * pMemNode;

extern void * mem_list_init(unsigned int size);
extern void mem_list_display(void);
extern void mem_list_free(void);
extern void * mem_insert_node(pMemNode pDst,unsigned int size);
extern pMemNode mem_find_node(pMemNode pDst);
extern unsigned char mem_free_node(void *lpMem);
#endif
