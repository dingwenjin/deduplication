#include <list>
#include <string>

using namespace std;

#define CHUNK_SIZE 4096
#define READ_BUF 1048576
#define CONTAINER_SIZE 4194304
#define PATH_SIZE 1024  //�·������  


typedef struct data_flow {
	char data[READ_BUF];
	int size;
}data_flow;

typedef struct data_element {       //���ﶨ��һ���������ڴ�������
	char data[CHUNK_SIZE];                  //�������
	int size;                          //��¼���ݴ�С
	string chunk_fp;								   // string file_fp;                //���ڼ�¼�ļ�����ָ��
	int container_ID;
	int flag;                              //������һ����ǣ������ݿ�Ϊ0���ظ����ݿ�Ϊ1
}data_file;

/*
typedef struct fingerprint_retrieve {
	string chunk_fp;
	int container_ID;
}fingerprint_retrieve;
*/

/*
typedef struct dedup_data_element {       //���ﶨ��һ���������ڴ�������
	char data[CHUNK_SIZE];                  //�������
	int size;                          //��¼���ݴ�С
	string chunk_fp;                  //��¼���ݵ�ָ��
	int container_ID;
}dedup_data_file;
*/


typedef struct tmp_file_recipe {              //ȥ�غ�����ݿ�ָ��
	//string chunk_fp;
	int offset;                        //���ݿ��������е�ƫ����(��ʼλ��)
	int size;                             //���ݿ�Ĵ�С
	int container_ID;            
	//int file_size;
}tmp_file_recipe;

typedef struct file_info {             //��¼�ļ�����Ϣ
	string file_route;             //��¼�ļ���
	int number_of_chunk;                 //��¼�ļ������ݿ���
	unsigned __int64 file_size;                      //�ļ��Ĵ�С  
}file_info;


typedef struct file_recipe {              //�������ݼ���Ԫ����
	string chunk_fp;										  //string chunk_fp;
	int offset;                        //���ݿ��������е�ƫ����(��ʼλ��)
	int size;                             //���ݿ�Ĵ�С
	int container_ID;
	//int file_size;
}file_recipe;


typedef struct restore_data_cache {
	char data[CONTAINER_SIZE];
	int size;                         //4KB��С�����ݽ��ĸ���
	int container_ID;                    //��Щ�������������ID
	//int hit_count;                  //��¼������������еĴ���
}restore_data_cache;

/*
typedef struct file_in_container {
	int container_ID;                     //�ļ����ڵ������ĵ�һ��container
	int offset;                     //�ļ�����ʼ��������������ƫ��
	int file_size;                        //�ļ��Ĵ�С
}file_in_container;
*/