#ifndef __TTASK_H__
#define __TTASK_H__

typedef struct _TASK
{
	unsigned char Loop;
	unsigned char *Buffer;
	unsigned int ParamH;
	unsigned int ParamL;
}TASK;

typedef TASK * spTask; 

#endif

