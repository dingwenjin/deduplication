#include <list>
#include <string>

using namespace std;

#define CHUNK_SIZE 4096
#define READ_BUF 1048576
#define CONTAINER_SIZE 4194304
#define PATH_SIZE 1024  //最长路径长度  


typedef struct data_flow {
	char data[READ_BUF];
	int size;
}data_flow;

typedef struct data_element {       //这里定义一个队列用于传输数据
	char data[CHUNK_SIZE];                  //存放数据
	int size;                          //记录数据大小
	string chunk_fp;								   // string file_fp;                //用于记录文件名的指纹
	int container_ID;
	int flag;                              //这里做一个标记，新数据块为0，重复数据块为1
}data_file;

/*
typedef struct fingerprint_retrieve {
	string chunk_fp;
	int container_ID;
}fingerprint_retrieve;
*/

/*
typedef struct dedup_data_element {       //这里定义一个队列用于传输数据
	char data[CHUNK_SIZE];                  //存放数据
	int size;                          //记录数据大小
	string chunk_fp;                  //记录数据的指纹
	int container_ID;
}dedup_data_file;
*/


typedef struct tmp_file_recipe {              //去重后的数据块指纹
	//string chunk_fp;
	int offset;                        //数据块在容器中的偏移量(起始位置)
	int size;                             //数据块的大小
	int container_ID;            
	//int file_size;
}tmp_file_recipe;

typedef struct file_info {             //记录文件的信息
	string file_route;             //记录文件名
	int number_of_chunk;                 //记录文件的数据块数
	unsigned __int64 file_size;                      //文件的大小  
}file_info;


typedef struct file_recipe {              //整个数据集的元数据
	string chunk_fp;										  //string chunk_fp;
	int offset;                        //数据块在容器中的偏移量(起始位置)
	int size;                             //数据块的大小
	int container_ID;
	//int file_size;
}file_recipe;


typedef struct restore_data_cache {
	char data[CONTAINER_SIZE];
	int size;                         //4KB大小的数据结点的个数
	int container_ID;                    //这些结点所属的容器ID
	//int hit_count;                  //记录这个容器被命中的次数
}restore_data_cache;

/*
typedef struct file_in_container {
	int container_ID;                     //文件所在的容器的第一个container
	int offset;                     //文件的起始数据所在容器的偏移
	int file_size;                        //文件的大小
}file_in_container;
*/