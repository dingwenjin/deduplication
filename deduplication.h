#pragma once                  //防止重定义

#include <list>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <time.h>
#include <iterator>
#include "windows.h"


using std::string;
using std::vector;
using std::list;
using std::vector;
using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::ostringstream;
using std::unordered_map;

#define CHUNK_SIZE 4096
#define READ_BUF 1048576
#define CONTAINER_SIZE 4194304
#define PATH_SIZE 1024  //最长路径长度  

#define CONTAINER_ID -1L                   //初始的容器ID
#define TEMPORARY_FP "0000000000000000000000000000000000000000"


#define UNIQUE_CHUNK 0
#define DEDUP_CHUNK 1


struct data_flow {
	string data;
	size_t size;                //每次最大读取1MB数据
	uint16_t container_ID;
	uint8_t flag;
	string chunk_fp;
};


typedef struct file_info {             //记录文件的信息
	string file_route;             //记录文件名
	uint16_t number_of_chunks;                 //记录文件的数据块数 
}file_info;


typedef struct file_recipe {              //整个数据集的元数据
	string chunk_fp;										  //string chunk_fp;
	int offset;                        //数据块在容器中的偏移量(起始位置)
	size_t size;                             //数据块的大小
	uint16_t container_ID;
}file_recipe;


typedef struct restore_data_cache {
	string data;
	size_t size;                         //容器大小
	uint16_t container_ID;                    //这些结点所属的容器ID
	//int hit_count;                  //记录这个容器被命中的次数
}restore_data_cache;

