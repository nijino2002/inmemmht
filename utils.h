#ifndef UTILS_H
#define UTILS_H

#include "defs.h"

void gen_ds_file(const char* file_name, int data_block_num, int string_len);

void gen_hashed_ds_file(const char* file_name, int data_block_num, int string_len);
#endif