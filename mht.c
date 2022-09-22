#include "mht.h"

int create_mht_from_ordered_ds_file(const char* filename, int block_num, int block_size){
	int ret_val = 0;

	return ret_val;
}

int create_mht_from_ordered_ds(IN PDATA_SET pds, OUT PMHTNode *pmhtroot){
	const char* FUNC_NAME = "create_mht_from_ordered_ds";
	int ret_val = 0;
	int i = 0;
	PMHTNode node = NULL;
	PMHTNode sub_root = NULL;
	PMHTNode sub_root2 = NULL;
	unsigned char* hash_buffer = NULL;
	SHA256_CTX ctx;
	PQNODE pQHeader = NULL;
	PQNODE pQ = NULL;	// tail

	if (!check_pointer_ex(pds, "pds", FUNC_NAME, "Null pointer")){
		ret_val = -1;
		goto RETURN;
	}

	hash_buffer = (unsigned char*) malloc (SHA256_BLOCK_SIZE);
	initQueue(&pQHeader, &pQ);
	
	for(i = 0; i < pds->m_size; i++){
		if(!pds->m_is_hashed){	/* data within dataset is not hashed */
			/* calculating sha256 hash from string */
			sha256_init(&ctx);
			sha256_update(&ctx, (unsigned char*)(pds->m_pDE[i].m_pdata), pds->m_pDE[i].m_data_len);
			sha256_final(&ctx, hash_buffer);
		}
		else {
			memcpy(hash_buffer, (unsigned char*)(pds->m_pDE[i].m_pdata), SHA256_BLOCK_SIZE); /* pds->m_pDE[i].m_data_len is equal to SHA256_BLOCK_SIZE */
		}
		node = makeMHTNode(pds->m_pDE[i].m_index, NODELEVEL_LEAF, hash_buffer);

		
	} // for

RETURN:	
	return ret_val;
}

int verify_spfc_dataelem_int(IN PMHTNode pmht){
	int ret_val = 0;

	return ret_val;
}

int process_queue(PQNODE *pQHeader, PQNODE *pQ){
	const char* FUNC_NAME = "process_queue";
	int ret_val = 0;
	bool bCombined = FALSE;
	bool bDequeueExec = FALSE;
	PQNODE qnode_ptr = NULL;
	PQNODE bkwd_ptr = NULL;
	PQNODE lchild_ptr = NULL, rchild_ptr = NULL;
	PQNODE cbd_qnode_ptr = NULL;
	PMHTNode cbd_mhtnode_ptr = NULL;

	if(!check_pointer_ex(*pQHeader, "*pQHeader", FUNC_NAME, "null pointer") || 
		!check_pointer_ex(*pQ, "*pQ", FUNC_NAME, "null pointer")){
		return -1;
	}

	qnode_ptr = *pQ;
	while((bkwd_ptr = lookBackward(qnode_ptr)) && bkwd_ptr != *pQHeader){
		if(((PMHTNode)(bkwd_ptr->m_ptr))->m_level > ((PMHTNode)((*pQ)->m_ptr))->m_level)
			break;
		if(((PMHTNode)(bkwd_ptr->m_ptr))->m_level == ((PMHTNode)((*pQ)->m_ptr))->m_level) {
			lchild_ptr = bkwd_ptr;
			rchild_ptr = *pQ;
			cbd_mhtnode_ptr = combineNodes((PMHTNode)(lchild_ptr->m_ptr), (PMHTNode)(rchild_ptr->m_ptr));
			check_pointer_ex(cbd_mhtnode_ptr, "cbd_mhtnode_ptr", FUNC_NAME, "null pointer");
			cbd_qnode_ptr = makeQNode(cbd_mhtnode_ptr);
			bCombined = TRUE;
			check_pointer_ex(cbd_qnode_ptr, "cbd_qnode_ptr", FUNC_NAME, "null pointer");
			enqueue(pQHeader, pQ, cbd_qnode_ptr);
		}

		qnode_ptr = qnode_ptr->prev;
	}

	//dequeue till encountering the new created combined node
	if(bCombined) {
		while ((peeked_qnode_ptr = peekQueue(*pQHeader)) && peeked_qnode_ptr->m_level < cbd_qnode_ptr->m_level) {
			popped_qnode_ptr = dequeue(pQHeader, pQ);
			check_pointer(popped_qnode_ptr, "popped_qnode_ptr");

			// Building MHT blocks based on dequeued nodes, then writing to MHT file.
			mhtblk_buffer = (uchar*) malloc(MHT_BLOCK_SIZE);
			memset(mhtblk_buffer, 0, MHT_BLOCK_SIZE);
			qnode_to_mht_buffer(popped_qnode_ptr, &mhtblk_buffer, MHT_BLOCK_SIZE);

			// record the offset of the first supplementary leaf node
			if(popped_qnode_ptr->m_level == NODELEVEL_LEAF && 
				popped_qnode_ptr->m_MHTNode_ptr->m_pageNo >= UNASSIGNED_INDEX){
				// mark the supplementary leaf node
				popped_qnode_ptr->m_is_supplementary_node = TRUE;
				popped_qnode_ptr->m_is_zero_node = TRUE;
				// record the offset of the first supplementary leaf node
				if(!get_isEncounterFSLO())
				{
					set_mhtFirstSplymtLeafOffset(fo_locate_mht_pos(g_mhtFileFD, 0, SEEK_CUR));
					printf("FROM process_all_pages_fv: FSOS: %d\n", get_mhtFirstSplymtLeafOffset());
					set_isEncounterFSLO(TRUE);
				}
			}

			if(g_mhtFileFD > 0) {
				fo_update_mht_block(g_mhtFileFD, mhtblk_buffer, MHT_BLOCK_SIZE, 0, SEEK_CUR);
			}
			free(mhtblk_buffer); mhtblk_buffer = NULL;

			print_qnode_info(popped_qnode_ptr);

			deleteQNode(&popped_qnode_ptr);
			bDequeueExec = TRUE;
		} //while

		if(bDequeueExec){
			printf("\n\n");
		}

		bDequeueExec = FALSE;
		bCombined = FALSE;
	}// if

	return ret_val;
}