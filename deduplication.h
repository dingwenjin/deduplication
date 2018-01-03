#pragma once                  //��ֹ�ض���

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
#define PATH_SIZE 1024  //�·������  

#define CONTAINER_ID -1L                   //��ʼ������ID
#define TEMPORARY_FP "0000000000000000000000000000000000000000"


#define UNIQUE_CHUNK 0
#define DEDUP_CHUNK 1


struct data_flow {
	string data;
	size_t size;                //ÿ������ȡ1MB����
	uint16_t container_ID;
	uint8_t flag;
	string chunk_fp;
};


typedef struct file_info {             //��¼�ļ�����Ϣ
	string file_route;             //��¼�ļ���
	uint16_t number_of_chunks;                 //��¼�ļ������ݿ��� 
}file_info;


typedef struct file_recipe {              //�������ݼ���Ԫ����
	string chunk_fp;										  //string chunk_fp;
	int offset;                        //���ݿ��������е�ƫ����(��ʼλ��)
	size_t size;                             //���ݿ�Ĵ�С
	uint16_t container_ID;
}file_recipe;


typedef struct restore_data_cache {
	string data;
	size_t size;                         //������С
	uint16_t container_ID;                    //��Щ�������������ID
	//int hit_count;                  //��¼������������еĴ���
}restore_data_cache;

