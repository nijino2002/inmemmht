#include "defs.h"
#include "mhtdefs.h"
#include "sha256.h"
#include "dbqueue.h"

PQNODE makeQHeader() {
	PQNODE node_ptr = NULL;
	node_ptr = (PQNODE) malloc(sizeof(QNODE));
	if(node_ptr == NULL)
		return NULL;
	node_ptr->m_qsize = 0;
	node_ptr->prev = NULL;
	node_ptr->next = NULL;

	return node_ptr;
}

PQNODE makeQNode(PMHTNode pmhtnode){
	PQNODE node_ptr = NULL;
	if(pmhtnode == NULL)
		return NULL;
	node_ptr = (PQNODE) malloc (sizeof(QNODE));
	if(node_ptr == NULL)
		return NULL;
	node_ptr->m_ptr = (void*)pmhtnode;
	node_ptr->prev = NULL;
	node_ptr->next = NULL;

	return node_ptr;
}


void deleteQNode(PQNODE *node_ptr){
	if(*node_ptr){
		(*node_ptr)->m_MHTNode_ptr ? (*node_ptr)->m_MHTNode_ptr = NULL : nop();
		free(*node_ptr);
		*node_ptr = NULL;
	}

	return;
}

PQNODE lookBackward(PQNODE pNode){
	if(!pNode)
		return NULL;

	if(!pNode->prev) {	//header node
		return NULL;
	}

	return pNode->prev;
}

void initQueue(PQNODE *pQHeader, PQNODE *pQ){
	if(*pQHeader != NULL){
		free(*pQHeader);
		*pQHeader = NULL;
	}

	(*pQHeader) = makeQHeader();
	*pQ = *pQHeader;
	return;
}

PQNODE enqueue(PQNODE *pQHeader, PQNODE *pQ, PQNODE pNode){
	if(*pQHeader == NULL && *pQ == NULL && pNode == NULL)
		return NULL;
	(*pQ)->next = pNode;
	pNode->prev = *pQ;
	pNode->next = NULL;
	*pQ = pNode;
	(*pQHeader)->m_qsize++;

	return pNode;
}

PQNode dequeue(PQNode *pQHeader, PQNode *pQ){
	PQNode tmp_ptr = NULL;
	if(*pQ == *pQHeader){	// empty queue
		printf("Empty queue.\n");
		return NULL;
	}
	tmp_ptr = (*pQHeader)->next;
	(*pQHeader)->next = tmp_ptr->next;
	if(tmp_ptr->next)	//otherwise, tmp_ptr == pQ.
		tmp_ptr->next->prev = *pQHeader;
	else
		*pQ = *pQHeader;

	(*pQHeader)->m_length > 0 ? (*pQHeader)->m_length-- : nop();

	return tmp_ptr;
}

PQNode dequeue_sub(PQNode *pQHeader, PQNode *pQ){
	PQNode tmp_ptr = NULL;
	PQNode first_elem_ptr = NULL;
	if(*pQ == *pQHeader){	// empty queue
		printf("Empty queue.\n");
		return NULL;
	}

	first_elem_ptr = (*pQHeader)->next;
	tmp_ptr = first_elem_ptr->next;
	if(!tmp_ptr) {
		check_pointer_ex(tmp_ptr, "tmp_ptr", "dequeue_sub", "null second element pointer");
		return NULL;
	}

	first_elem_ptr->next = tmp_ptr->next;
	if(tmp_ptr->next)	//otherwise, tmp_ptr == pQ.
		tmp_ptr->next->prev = first_elem_ptr;
	else
		*pQ = first_elem_ptr;

	(*pQHeader)->m_length > 0 ? (*pQHeader)->m_length-- : nop();

	return tmp_ptr;
}

PQNode dequeue_sppos(PQNode *pQHeader, PQNode *pQ, PQNode pos) {
	PQNode tmp_ptr = NULL;
	
	if(*pQ == *pQHeader){	// empty queue
		printf("Empty queue.\n");
		return NULL;
	}

	if(pos == (*pQHeader)){   // invalid position
		printf("Invalid dequeue position.\n");
		return NULL;
	}

	tmp_ptr = pos;
	if(pos->next) {
		pos->prev->next = pos->next;
		pos->next->prev = pos->prev;
	}
	else{
		return dequeue(pQHeader, pQ);
	}
	
	(*pQHeader)->m_length > 0 ? (*pQHeader)->m_length-- : nop();

	return tmp_ptr;
}

PQNode peekQueue(PQNode pQHeader){
	if(pQHeader && pQHeader->next)
		return (PQNode)(pQHeader->next);
	return NULL;
}

void freeQueue(PQNode *pQHeader, PQNode *pQ) {
	PQNode tmp_ptr = NULL;
	if(!(*pQHeader))
		return;
	tmp_ptr = (*pQHeader)->next;
	if(!tmp_ptr){
		free(*pQHeader);
		*pQHeader = NULL;
		return;
	}
	while(tmp_ptr = dequeue(pQHeader, pQ)){
		tmp_ptr->m_MHTNode_ptr != NULL ? free(tmp_ptr->m_MHTNode_ptr) : nop();
		free(tmp_ptr);
		tmp_ptr = NULL;
	}
	free(*pQHeader);
	*pQHeader = NULL;
	return;
}

void freeQueue2(PQNode *pQHeader){
	PQNode tmp_ptr = NULL;
	if(!(*pQHeader))
		return;
	tmp_ptr = (*pQHeader)->next;
	if(!tmp_ptr){
		free(*pQHeader);
		*pQHeader = NULL;
		return;
	}
	while(tmp_ptr = ((*pQHeader)->next)){
		(*pQHeader)->next = tmp_ptr->next;
		if(tmp_ptr->next) {
			tmp_ptr->next->prev = *pQHeader;
			tmp_ptr->m_MHTNode_ptr != NULL ? free(tmp_ptr->m_MHTNode_ptr) : nop();
			free(tmp_ptr);
		}
	}
	free(*pQHeader);
	*pQHeader = NULL;
	return;
}

void freeQueue3(PQNode *pQ) {
	PQNode tmp_ptr = NULL;
	if(!(*pQ))
		return;
	tmp_ptr = *pQ;
	do{
		tmp_ptr = tmp_ptr->prev;
		if(tmp_ptr->prev == NULL)	//tmp_ptr == pQHeader
			break;
	}while(tmp_ptr);

	return freeQueue2(&tmp_ptr);
}

/********* Test & Debug ********/

void printQueue(PQNode pQHeader) {
	PQNode tmp_ptr = NULL;
	uint32 i = 1;

	if(!pQHeader){
		check_pointer(pQHeader, "printQueue: pQHeader");
		return;
	}

	tmp_ptr = pQHeader->next;
	while(tmp_ptr){
		printf("%d: PageNo-Level: %d-%d\n", i, tmp_ptr->m_MHTNode_ptr->m_pageNo, tmp_ptr->m_level);
		tmp_ptr = tmp_ptr->next;
		i++;
	}

	return;
}


void print_qnode_info(PQNode qnode_ptr){
	if(!qnode_ptr){
		check_pointer(qnode_ptr, "qnode_ptr");
		debug_print("print_qnode_info", "Null parameters");
		return;
	}

	printf("PageNo|Level|LCPN|LCOS|RCPN|RCOS|PPN|POS: %d|%d|%d|%d|%d|%d|%d|%d\t", 
			qnode_ptr->m_MHTNode_ptr->m_pageNo, 
			qnode_ptr->m_level,
			qnode_ptr->m_MHTNode_ptr->m_lchildPageNo,
			qnode_ptr->m_MHTNode_ptr->m_lchildOffset,
			qnode_ptr->m_MHTNode_ptr->m_rchildPageNo,
			qnode_ptr->m_MHTNode_ptr->m_rchildOffset,
			qnode_ptr->m_MHTNode_ptr->m_parentPageNo,
			qnode_ptr->m_MHTNode_ptr->m_parentOffset);

	return;
}

void print_qnode_info_ex(PQNode qnode_ptr, uint32 flags){
	const char* THIS_FUNC_NAME = "printQNode";

	if(!qnode_ptr){
		check_pointer_ex(qnode_ptr, "qnode_ptr", THIS_FUNC_NAME, "null qnode_ptr");
		return;
	}

	printf("[");
	if(flags && PRINT_QNODE_FLAG_INDEX){
		printf("index: %d, ", qnode_ptr->m_MHTNode_ptr->m_pageNo);
	}
	if(flags && PRINT_QNODE_FLAG_HASH){
		print_hash_value(qnode_ptr->m_MHTNode_ptr->m_hash);
	}
}
