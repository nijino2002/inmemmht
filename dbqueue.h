#ifndef _DBQUEUE
#define _DBQUEUE

#include "defs.h"
#include "mhtdefs.h"

typedef struct _QNODE
{
	union {
		void* m_ptr;
		int m_qsize;
	}
	
	struct _QNODE* next;
	struct _QNODE* prev;
} QNODE, *PQNODE;

PQNODE makeQHeader();

PQNODE makeQNode(PMHTNode pmhtnode);

#endif