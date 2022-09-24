#include "defs.h"
#include "mhtdefs.h"
#include "sha256.h"
#include "utils.h"
#include "mht.h"

int main(int argc, char const *argv[])
{
	PDATA_SET pds = NULL;
	PMHTNode pmhtroot = NULL;
	// gen_hashed_ds_file("./testds.ds", 10, 64);
	gen_ds(100, 64, &pds);
	print_ds(pds);
	create_mht_from_ordered_ds(pds, &pmhtroot);
	println();
	print_mht_preorder(pmhtroot);

	free_mht_postorder(&pmhtroot);

	printf("Database contains %d pages.\n", get_page_count_from_db("./aes_cbc_128.db"));

	return 0;
}