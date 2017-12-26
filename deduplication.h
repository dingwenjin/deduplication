#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

using std::string;
using std::vector;
using std::list;
using std::vector;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::ostringstream;
using std::unordered_map;

#define CHUNK_SIZE 4096
#define READ_BUF 1048576
#define CONTAINER_SIZE 4194304
#define PATH_SIZE 1024  //�·������  

#define CONTAINER_ID -1L                   //��ʼ������ID
#define TEMPORARY_FP "0000000000000000000000000000000000000000"


//#define UNIQUE_CHUNK 1
//#define DEDUP_CHUNK 0


struct data_flow {
	char *data=nullptr;
	size_t size;                //ÿ������ȡ1MB����
	uint16_t container_ID;
	//uint8_t flag;                         //�ж����¿黹���ظ���
	string chunk_fp;
};


typedef struct file_info {             //��¼�ļ�����Ϣ
	string file_route;             //��¼�ļ���
	uint16_t number_of_chunks;                 //��¼�ļ������ݿ��� 
}file_info;


struct file_recipe {              //�������ݼ���Ԫ����
	string chunk_fp;										  //string chunk_fp;
	int offset;                        //���ݿ��������е�ƫ����(��ʼλ��)
	size_t size;                             //���ݿ�Ĵ�С
	uint16_t container_ID;
};


typedef struct restore_data_cache {
	char data[CONTAINER_SIZE];
	size_t size;                         //������С
	uint16_t container_ID;                    //��Щ�������������ID
	//int hit_count;                  //��¼������������еĴ���
}restore_data_cache;

