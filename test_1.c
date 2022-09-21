#include "defs.h"
#include "mhtdefs.h"
#include "sha256.h"
#include "utils.h"

int main(int argc, char const *argv[])
{
	/* code */
	gen_hashed_ds_file("./testds.ds", 10, 64);
	return 0;
}