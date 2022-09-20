#ifndef MHT_H
#define MHT_H

#include "mhtdefs.h"
#include "dataelem.h"

typedef struct _DATA_SET {
	PDATA_ELEM m_pDE;
	int m_size;
} DATA_SET, *PDATA_SET;

/**
 * @brief      Creates an MHT from a dataset with ordered indices.
 *
 * @param[in]  pds       The pointer to the dataset array
 * @param      pmhtnode  The pointer to the root node of the created MHT
 *
 * @return     { 0 if successful }
 */
int create_mht_from_ordered_ds(IN PDATA_SET pds, OUT PMHTNode *pmhtnode);

/**
 * @brief      Verifies the integrity of the specific data element (a leaf node of the MHT)
 *
 * @param[in]  pmht  The pointer to the root of the MHT
 *
 * @return     { 0 if successful }
 */
int verify_spfc_dataelem_int(IN PMHTNode pmht);

#endif