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
	unsigned char* hash_buffer = NULL;
	SHA256_CTX ctx;

	if (!check_pointer_ex(pds, "pds", FUNC_NAME, "Null pointer")){
		ret_val = -1;
		goto RETURN;
	}

	hash_buffer = (unsigned char*) malloc (SHA256_BLOCK_SIZE);
	
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

		if(!(*pmhtroot))
			*pmhtroot = node;
		else if((*pmhtroot)->m_level == NODELEVEL_LEAF) {
			;
		}
	}

RETURN:	
	return ret_val;
}

int verify_spfc_dataelem_int(IN PMHTNode pmht){
	int ret_val = 0;

	return ret_val;
}