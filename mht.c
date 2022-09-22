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
	PQNode pQHeader = NULL;
	PQNode pQ = NULL;	// tail

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

		if(!(*pmhtroot))	// MHT root is null
			*pmhtroot = node;
		else if((*pmhtroot)->m_level == NODELEVEL_LEAF) { // MHT root is a leaf node
			sub_root = combineNodes(*pmhtroot, node);
			*pmhtroot = sub_root;
			sub_root = NULL;
		}
		else{ // MHT root level != node's level ( usually '>' )
			enqueue(&pQHeader, &pQ, makeQNode(node, NODELEVEL_LEAF));

		}
	} // for

RETURN:	
	return ret_val;
}

int verify_spfc_dataelem_int(IN PMHTNode pmht){
	int ret_val = 0;

	return ret_val;
}


void combine_nodes_with_same_levels(PQNode *pQHeader, 
                                    PQNode *pQ){
	const char* FUNC_NAME = "combine_nodes_with_same_levels";
	PQNode lchild_ptr = NULL;
	PQNode rchild_ptr = NULL;
	PQNode cbd_qnode_ptr = NULL;
	PQNode tmp_node_ptr = NULL;
	PQNode popped_qnode_ptr = NULL;
	bool bCombined = FALSE;

	if(!(*pQHeader) || !(*pQ) || (*pQHeader) == (*pQ)){
		check_pointer_ex(*pQHeader, "*pQHeader", THIS_FUNC_NAME, "null *pQHeader");
		check_pointer_ex(*pQ, "*pQ", THIS_FUNC_NAME, "null *pQ");
		debug_print(THIS_FUNC_NAME, "queue is null");
		return;
	}

	while(*pQ && (*pQ)->prev && (*pQ)->prev != (*pQHeader) && (*pQ)->m_level == (*pQ)->prev->m_level){
		lchild_ptr = (*pQ)->prev;
		rchild_ptr = (*pQ);
		cbd_qnode_ptr = makeCombinedQNode(lchild_ptr, rchild_ptr);
		bCombined = TRUE;
		check_pointer_ex(cbd_qnode_ptr, "cbd_qnode_ptr", FUNC_NAME, "creating cbd_qnode_ptr failed");
		enqueue(pQHeader, pQ, cbd_qnode_ptr);

		

		tmp_node_ptr = (*pQ)->prev->prev;
		popped_qnode_ptr = dequeue_sppos(pQHeader, pQ, tmp_node_ptr);
		if(!popped_qnode_ptr->m_is_written){
			print_qnode_info(popped_qnode_ptr); println();
			memset(mht_block_buffer, 0, mht_block_buffer_len);
			qnode_to_mht_buffer(popped_qnode_ptr, &mht_block_buffer, mht_block_buffer_len);
			/*
			// record the offset of the first supplementary leaf node
			if(!get_isEncounterFSLO() && 
				popped_qnode_ptr->m_level == NODELEVEL_LEAF && 
				popped_qnode_ptr->m_MHTNode_ptr->m_pageNo >= UNASSIGNED_INDEX){
				set_mhtFirstSplymtLeafOffset(fo_locate_mht_pos(of_fd, 0, SEEK_CUR));
				set_isEncounterFSLO(TRUE);
			}*/
			if(popped_qnode_ptr->m_level == NODELEVEL_LEAF && 
				popped_qnode_ptr->m_MHTNode_ptr->m_pageNo >= UNASSIGNED_INDEX){
				// mark the supplementary leaf node
				popped_qnode_ptr->m_is_supplementary_node = TRUE;
				popped_qnode_ptr->m_is_zero_node = TRUE;
				// record the offset of the first supplementary leaf node
				if(!get_isEncounterFSLO())
				{
					set_mhtFirstSplymtLeafOffset(fo_locate_mht_pos(of_fd, 0, SEEK_CUR));
					set_isEncounterFSLO(TRUE);
				}
			}

			fo_update_mht_block2(of_fd, 
							 mht_block_buffer,
							 mht_block_buffer_len,
							 0,
							 SEEK_CUR);
			fsync(of_fd);
			popped_qnode_ptr->m_is_written = TRUE;
			deleteQNode(&popped_qnode_ptr);
		}
		else{	// popped_qnode_ptr->m_is_written is TRUE
			// update index info. of the corresponding block in the MHT file
			if(popped_qnode_ptr->m_level > NODELEVEL_LEAF && popped_qnode_ptr->m_is_written){
				update_mht_block_index_info(of_fd, popped_qnode_ptr);
#ifdef PRINT_INFO_ENABLED
				printf("UPDATED pQ->prev->prev INDEX\n");
#endif
			}
		}

		tmp_node_ptr = (*pQ)->prev;
		popped_qnode_ptr = dequeue_sppos(pQHeader, pQ, tmp_node_ptr);
		if(!popped_qnode_ptr->m_is_written){
#ifdef PRINT_INFO_ENABLED
			print_qnode_info(popped_qnode_ptr); println();
#endif
			memset(mht_block_buffer, 0, mht_block_buffer_len);
			qnode_to_mht_buffer(popped_qnode_ptr, &mht_block_buffer, mht_block_buffer_len);
			/*
			// record the offset of the first supplementary leaf node
			if(!get_isEncounterFSLO() && 
				popped_qnode_ptr->m_level == NODELEVEL_LEAF && 
				popped_qnode_ptr->m_MHTNode_ptr->m_pageNo >= UNASSIGNED_INDEX){
				set_mhtFirstSplymtLeafOffset(fo_locate_mht_pos(of_fd, 0, SEEK_CUR));
				set_isEncounterFSLO(TRUE);
			}
			*/
			if(popped_qnode_ptr->m_level == NODELEVEL_LEAF && 
				popped_qnode_ptr->m_MHTNode_ptr->m_pageNo >= UNASSIGNED_INDEX){
				// mark the supplementary leaf node
				popped_qnode_ptr->m_is_supplementary_node = TRUE;
				popped_qnode_ptr->m_is_zero_node = TRUE;
				// record the offset of the first supplementary leaf node
				if(!get_isEncounterFSLO())
				{
					set_mhtFirstSplymtLeafOffset(fo_locate_mht_pos(of_fd, 0, SEEK_CUR));
					set_isEncounterFSLO(TRUE);
				}
			}
			
			fo_update_mht_block2(of_fd, 
							 mht_block_buffer,
							 mht_block_buffer_len,
							 0,
							 SEEK_CUR);
			fsync(of_fd);
			popped_qnode_ptr->m_is_written = TRUE;
			deleteQNode(&popped_qnode_ptr);
		}
		else{	// popped_qnode_ptr->m_is_written is TRUE
			// update index info. of the corresponding block in the MHT file
			if(popped_qnode_ptr->m_level > NODELEVEL_LEAF && popped_qnode_ptr->m_is_written){
				update_mht_block_index_info(of_fd, popped_qnode_ptr);
#ifdef PRINT_INFO_ENABLED
				printf("UPDATED pQ->prev INDEX\n");
#endif
			}
		}

		if(!(*pQ)->m_is_written){
#ifdef PRINT_INFO_ENABLED
			print_qnode_info(*pQ); println();
#endif
			(*pQ)->m_is_written = TRUE;
			memset(mht_block_buffer, 0, mht_block_buffer_len);
			qnode_to_mht_buffer(*pQ, &mht_block_buffer, mht_block_buffer_len);
			fo_update_mht_block2(of_fd, 
							 mht_block_buffer,
							 mht_block_buffer_len,
							 0,
							 SEEK_CUR);
			fsync(of_fd);
		}
#ifdef PRINT_INFO_ENABLED
		printQueue(*pQHeader);
#endif
		bCombined = FALSE;
	} // while

	free(mht_block_buffer);

	return;
}