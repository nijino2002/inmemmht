#include "utils.h"
#include "sha256.h"

void gen_ds_file(const char* file_name, int data_block_num, int string_len){
	int fd = -1;
	int open_flags;
	mode_t file_perms;
	int i = 0;
	int index = 0;
	char* gen_str = NULL;
	char* buffer = NULL;
	int buffer_len = sizeof(int) + string_len;

	check_pointer_ex((char*)file_name, "file_name", "gen_ds_file", "null file name");
	data_block_num <= 0 || string_len <= 0 ? debug_print("gen_ds_file", "neither data_block_num nor string_len can be <= 0") : nop();

	open_flags = O_CREAT | O_WRONLY | O_TRUNC;
	file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

	fd = open(file_name, open_flags, file_perms);

	buffer = (char*) malloc (buffer_len);

	for(i = 0; i < data_block_num; i++){
		index = i + 1;
		memset(buffer, 0, buffer_len);
		memcpy(buffer, &index, sizeof(uint32));
		gen_str = generate_random_string(string_len);
		memcpy(buffer + sizeof(uint32), gen_str, string_len);
		free(gen_str); gen_str = NULL;
		write(fd, buffer, buffer_len);
	}

	close(fd);
}

void gen_hashed_ds_file(const char* file_name, int data_block_num, int string_len){
	int fd = -1;
	int open_flags;
	mode_t file_perms;
	int i = 0;
	int index = 0;
	char* gen_str = NULL;
	char* buffer = NULL;
	int buffer_len = sizeof(int) + SHA256_BLOCK_SIZE;
	SHA256_CTX ctx;
	char* sha256_buffer = NULL;

	check_pointer_ex((char*)file_name, "file_name", "gen_ds_file", "null file name");
	data_block_num <= 0 || string_len <= 0 ? debug_print("gen_ds_file", "neither data_block_num nor string_len can be <= 0") : nop();

	open_flags = O_CREAT | O_WRONLY | O_TRUNC;
	file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

	fd = open(file_name, open_flags, file_perms);

	buffer = (char*) malloc (buffer_len);
	sha256_buffer = (char*) malloc (SHA256_BLOCK_SIZE);

	for(i = 0; i < data_block_num; i++){
		index = i + 1;
		memset(sha256_buffer, 0, SHA256_BLOCK_SIZE);
		memset(buffer, 0, buffer_len);
		memcpy(buffer, &index, sizeof(uint32));
		gen_str = generate_random_string(string_len);
		sha256_init(&ctx);
		sha256_update(&ctx, gen_str, string_len);
		sha256_final(&ctx, sha256_buffer);
		memcpy(buffer + sizeof(uint32), sha256_buffer, SHA256_BLOCK_SIZE);
		free(gen_str); gen_str = NULL;
		write(fd, buffer, buffer_len);
	}

	close(fd);
}

void gen_ds(int data_block_num, int string_len, OUT PDATA_SET *pds){
	int i = 0;
	int index = 0;
	char* gen_str = NULL;
	PDATA_SET pds_ptr = NULL;

	pds_ptr = (PDATA_SET) malloc (sizeof(DATA_SET));
	pds_ptr->m_is_hashed = FALSE;

	for(i = 0; i < data_block_num; i++){
		index = i + 1;
		memset(buffer, 0, buffer_len);
		memcpy(buffer, &index, sizeof(uint32));
		gen_str = generate_random_string(string_len);
		memcpy(buffer + sizeof(uint32), gen_str, string_len);
		free(gen_str); gen_str = NULL;
		write(fd, buffer, buffer_len);
	}
}