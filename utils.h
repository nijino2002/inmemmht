#ifndef UTILS_H
#define UTILS_H

#include "defs.h"
#include "mht.h"

void gen_ds_file(const char* file_name, int data_block_num, int string_len);

void gen_hashed_ds_file(const char* file_name, int data_block_num, int string_len);

void gen_ds(int data_block_num, int string_len, OUT PDATA_SET *pds);

void print_ds(IN PDATA_SET pds);

#endif