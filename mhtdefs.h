#ifndef _MHTDEFS
#define _MHTDEFS

#include "defs.h"

typedef struct  _MHTNode
{
	int 	m_pageNo;
	char	m_hash[32];
	int     m_level;
	bool 	m_is_supplement_node;
	struct _MHTNode* m_lchild;
	struct _MHTNode* m_rchild;
	/* a minus offset indicates the distance that the file pointer has to move back 
	from current node to the member.
	*/ 
	int 	m_lchildOffset;
	int  	m_rchildOffset;
	int 	m_parentOffset;
	int 	m_lchildPageNo;
	int 	m_rchildPageNo;
	int 	m_parentPageNo;
} MHTNode, *PMHTNode;

/*
Making an MHT node.
Parameters: 
	pageno: page number.
	d: data string.
Return: a pointer to an MHT node.
 */
PMHTNode makeMHTNode(int pageno, int level, const char d[]);


PMHTNode combineNodes(PMHTNode lchild, PMHTNode rchild);

/*
Making a MHT node with hashed zero.
Parameters: 
	pageno: page number.
Return: a pointer to an MHT node.
 */
PMHTNode makeZeroMHTNode(int pageno);

/*
Freeing a given MHT node.
Parameters:
	node_ptr: a 2-d pointer to the given node.
Return:
	NULL.
*/
void deleteMHTNode(PMHTNode *node_ptr);

/*
Generating a hash from a page number with SHA256 algorithm
Parameter:
	page_no [IN]: page number.
	buf [OUT]: buffer holding output hash value.
	buf_len [IN]: the maximal size of given buffer (buf), which must be larger than 32 bytes.
Return:
	NULL.
*/
void generateHashByPageNo_SHA256(int page_no, char *buf, uint32 buf_len);

/**
 * @brief      Generating a hash from a given buffer with SHA256 algorithm
 *
 * @param[in]  in_buf      In buffer containing data
 * @param[in]  in_buf_len  In buffer length
 * @param[out] buf         The output buffer containing hash string
 * @param[in]  buf_len     The buffer length
 */
void generateHashByBuffer_SHA256(char *in_buf, uint32 in_buf_len, char *buf, uint32 buf_len);

/*
Generating a hash by combining two given hashes with SHA256 algorithm
Parameter:
	str1 [IN]: the frist hash.
	str2 [IN]: the second hash.
	buf [OUT]: buffer holding output hash value.
	buf_len [IN]: the maximal size of given buffer (buf), which must be larger than 32 bytes.
Return:
	NULL.
*/
void generateCombinedHash_SHA256(char *hash1, char *hash2, char *buf, uint32 buf_len);

PMHTNode get_the_right_most_child(PMHTNode node);

void print_mhtnode_info(PMHTNode mhtnode_ptr);

#endif