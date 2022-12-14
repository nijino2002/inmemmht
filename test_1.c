#include "defs.h"
#include "mhtdefs.h"
#include "sha256.h"
#include "utils.h"
#include "mht.h"
#include "sqlite3.h"
#include "sha256.h"
#include<sys/time.h>

#define SUCCESS 1
#define FAIL 0

void selectHash(PMHTNode pmhtroot, char *sql);
void updateHash(PMHTNode pmhtroot, char *sql);
void updateTime(PMHTNode pmhtroot, int n);
int diffPgno(unsigned int** listIn, unsigned int* listOut, int lenth);
int getMHash(PMHTNode pmhtroot, unsigned int pgno, char* m_hash);
int getRandomStr(char* random_str, const int random_len);
void updateMTree(PMHTNode pmhtroot, char* hashRet, int pgno);


int main(int argc, char const *argv[])
{
	PDATA_SET pds = NULL;
	PMHTNode pmhtroot = NULL;
	// gen_hashed_ds_file("./testds.ds", 10, 64);
	/*
	gen_ds(100, 64, &pds);
	print_ds(pds);
	create_mht_from_ordered_ds(pds, &pmhtroot);
	println();
	print_mht_preorder(pmhtroot);

	free_mht_postorder(&pmhtroot);
	*/
	printf("Database contains %d leaf table pages.\n", get_page_count_from_db("./aes_cbc_128.db"));
	gen_ds_from_dbfile("./aes_cbc_128.db", &pds);
	create_mht_from_ordered_ds(pds, &pmhtroot);
	// println();
	// print_mht_preorder(pmhtroot);
	char *sql;
	sql = "select * from gps limit 100";
	printf("执行20次SQL: %s\n", sql);
	for(int i=0;i<20;i++) selectHash(pmhtroot, sql);  //执行20次
	// sql = "UPDATE gps SET Lng=1511 WHERE Lng=1501";
	// updateHash(pmhtroot, sql);  // 计算对update语句时间
	for(int i=5;i<101;i+=5){   // 对更新的时间进行计算，更新条数为5、10、15...100，每个执行20次
		printf("更新%d条:\n", i);
		for(int j=0;j<20;j++){
			updateTime(pmhtroot, i);
		}
	}
	free_mht_postorder(&pmhtroot);
}

//计算哈希比较用的时间
void selectHash(PMHTNode pmhtroot, char *sql){
	// printf("SQL: %s\n", sql);
	struct timeval startTime, endTime;
	double diff_t;
	gettimeofday(&startTime,NULL);
	char* dbName = "aes_cbc_128.db";
	char *errmsg;
	sqlite3 *db;

	//打开数据库
	if(SQLITE_OK != sqlite3_open(dbName, &db)){
		printf("sqlite3_open error\n");
        exit(1);
	}

	char **result;
	int nrow, ncolumn;

	//执行select语句
	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0)
	{
	  	printf("error : %s\n", errmsg);
	  	sqlite3_free(errmsg);
	}

	
	unsigned int *selectAll;
	int selectAllNum = getSelectPgno(&selectAll);  //获取select结果所在的所有页
	// for(int i=0;i<selectAllNum;i++) printf("%d ", selectAll[i]);
	// printf("\nselectAllPgno: %d\n", selectAllNum);

	unsigned int selectDiff[selectAllNum];
	int selectDiffNum = diffPgno(&selectAll, selectDiff, selectAllNum);  //对页面结果进行去重

	unsigned int  pgnoNums;
	unsigned char dataHash[SHA256_BLOCK_SIZE];
	char mhash[SHA256_BLOCK_SIZE];
	// printf("\nselectAllPgno: %d\n", selectAllNum);

	//；遍历去重后的页面
	for(int i = 0; i< selectDiffNum; i++){
		int tempPgno = selectDiff[i];
		memset(dataHash, 0, sizeof(unsigned char) * SHA256_BLOCK_SIZE);
		
		//getinfo1函数可以读取页面的数据并计算哈希值
		if(getInfo1(db, dbName, &pgnoNums, dataHash,tempPgno, tempPgno, 1)){
			int ret = getMHash(pmhtroot, tempPgno, mhash);
			if(ret==FAIL){
				printf("未找到指定页码");
			}
			ret = compare_two_hashes(dataHash, mhash);  //比较哈希值
			// printf("pgno: %d %d\n", tempPgno, ret);
		}
	}

	gettimeofday(&endTime,NULL);
	diff_t = 1000000*(endTime.tv_sec-startTime.tv_sec)+endTime.tv_usec-startTime.tv_usec;
	printf("%lf\n", diff_t/1000000);
	sqlite3_free_table(result);
	sqlite3_close(db);
}


// void updateHash(PMHTNode pmhtroot, char *sql){
// 	printf("SQL: %s\n", sql);
// 	struct timeval startTime, endTime;
// 	double diff_t;
// 	gettimeofday(&startTime,NULL);
// 	char* dbName = "aes_cbc_128.db";
// 	char *errmsg;
// 	sqlite3 *db;

// 	if(SQLITE_OK != sqlite3_open(dbName, &db)){
// 		printf("sqlite3_open error\n");
//         exit(1);
// 	}

// 	char **result;
// 	int nrow, ncolumn;

// 	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0)
// 	{
// 	  	printf("error : %s\n", errmsg);
// 	  	sqlite3_free(errmsg);
// 	}
// 	unsigned int *updateDiff;
// 	int updateDiffNum = getDiffUpdatePgno(&updateDiff);
// 	printf("updateDiffNum: %d\n", updateDiffNum);
// 	unsigned int  pgnoNums;
// 	unsigned char dataHash[SHA256_BLOCK_SIZE];
// 	char mhash[SHA256_BLOCK_SIZE];

// 	for(int i = 0; i< updateDiffNum; i++){
// 		int tempPgno = updateDiff[i];
// 		memset(dataHash, 0, sizeof(unsigned char) * SHA256_BLOCK_SIZE);

// 		if(getInfo1(db, dbName, &pgnoNums, dataHash,tempPgno, tempPgno, 1)){
// 			int ret = getMHash(pmhtroot, tempPgno, mhash);
// 			if(ret==FAIL){
// 				printf("未找到指定页码");
// 			}
// 			ret = compare_two_hashes(dataHash, mhash);
// 			// printf("pgno: %d %d\n", tempPgno, ret);
// 		}
// 	}
// 	gettimeofday(&endTime,NULL);
// 	diff_t = 1000000*(endTime.tv_sec-startTime.tv_sec)+endTime.tv_usec-startTime.tv_usec;
// 	printf("\ntime: %lf\n", diff_t/1000000);

// 	sqlite3_free_table(result);
// 	sqlite3_close(db);
// }

//计算更新的时间
void updateTime(PMHTNode pmhtroot, int n){
	char randText[1024*4];
	getRandomStr(randText, 1024*4);  //获取一个随机的4kb字符串
	unsigned char hashRet[32];
	struct timeval startTime, endTime;
	double diff_t;
	gettimeofday(&startTime,NULL);
	for(int i=0;i<n;i++){
		sha256_2(randText, 1024*4, hashRet);  //计算字符串哈希
		int pgno = rand() %16384;   //随机一个页号
		updateMTree(pmhtroot, hashRet, pgno);  //更新哈希树
	}
	gettimeofday(&endTime,NULL);
	diff_t = 1000000*(endTime.tv_sec-startTime.tv_sec)+endTime.tv_usec-startTime.tv_usec;
	printf("%lf\n", diff_t/1000000);
}

//对页面进行去重
int diffPgno(unsigned int **listIn, unsigned int *listOut, int lenth){
	//这里页面都是有序的，没有加排序
	if(lenth==0)return lenth;
	listOut[0] = (*listIn)[0];
	int index = 1;
	for(int i=1; i<lenth; i++){
		if ((*listIn)[i] != listOut[index-1]){
			listOut[index] = (*listIn)[i];
			index++;
		}
	}
	return index;
}

//从哈希树中找到对应页面的叶子节点，得到哈希值
int getMHash(PMHTNode pmhtroot, unsigned int pgno, char* m_hash){
	PMHTNode temp = pmhtroot;
	int level = temp->m_level;
	//遍历直到到达叶子节点
	while (level!=0)
	{
		if (pgno<=temp->m_pageNo)  //小于等于当前往左
		{
			temp = temp->m_lchild;
		}
		else{
			temp = temp->m_rchild;  //大于往右
		}
		level = temp->m_level;
	}
	if (temp->m_pageNo != pgno)
	{
		return FAIL;
	}
	memcpy(m_hash, temp->m_hash, SHA256_BLOCK_SIZE);
	return SUCCESS;
}


//获得一个随机字符串
int getRandomStr(char* random_str, const int random_len)
{
    int i, random_num, seed_str_len;
    struct timeval tv;
    unsigned int seed_num;
    char seed_str[] = "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; //随机字符串的随机字符集

    seed_str_len = strlen(seed_str);
    
    gettimeofday(&tv, NULL);
    seed_num = (unsigned int)(tv.tv_sec + tv.tv_usec); //超了unsigned int的范围也无所谓，我们要的只是不同的种子数字
    srand(seed_num);

    for(i = 0; i < random_len; i++)
    {
        random_num = rand()%seed_str_len;
        random_str[i] = seed_str[random_num];
    }

    return 0;
}

//更新树
void updateMTree(PMHTNode pmhtroot, char* hashRet, int pgno){
	PMHTNode temp = pmhtroot;
	PMHTNode nodeList[30];
	nodeList[0] = temp;
	int index = 1;
	int level = temp->m_level;
	// printf("pgno %d: %d ", pgno, temp->m_pageNo);
	//找到对应页号的叶子节点，将经过的节点都进栈
	while (level!=0)
	{
		if (pgno<=temp->m_pageNo)
		{
			temp = temp->m_lchild;
		}
		else{
			temp = temp->m_rchild;
		}
		nodeList[index] = temp;
		index++;
		level = temp->m_level;
		// printf("%d ", temp->m_pageNo);
	}
	// printf("\n");
	if (temp->m_pageNo != pgno)
	{
		return FAIL;
	}
	char msg[SHA256_BLOCK_SIZE*2];
	//出栈更新节点
	while (index>0)
	{
		index--;
		temp = nodeList[index];
		// printf("%d ", temp->m_pageNo);
		//如果出栈的是非叶子节点，new data=hash(lchild->data||rchild->data)
		if (temp->m_level != 0)
		{
			memcpy(msg, temp->m_lchild->m_hash, SHA256_BLOCK_SIZE);
			memcpy(msg+SHA256_BLOCK_SIZE, temp->m_rchild->m_hash, SHA256_BLOCK_SIZE);
			sha256_2(msg, SHA256_BLOCK_SIZE*2, hashRet);
		}
		//如果出栈的是叶子节点，new data=hash(页面数据)
		memcpy(temp->m_hash, hashRet, SHA256_BLOCK_SIZE);

	}
	// printf("\n");
	
	return SUCCESS;
}
