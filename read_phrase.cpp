#include "deduplication.h"
#include <vector>
#include <fstream>
#include <string.h>


using namespace std;

list<data_flow*> d_flow;
list<file_info*> file_information;
static char buf_read[READ_BUF];                   //动态申请空间的方式，避免使用静态变量(因为静态变量一直伴随着整个程序，会一直占用内存空间，知道程序结束后释放空间),但这里使用静态方式

void read_file(vector<string> v) {         //文件路径列表
	
	vector<string>::iterator it = v.begin();
	

	while (it != v.end()) {                //迭代不同的文件路径
		ifstream in((*it).c_str(), ios::binary);
		int read_size = 0;
		unsigned __int64 total_singal_file_size=0;
		while (!in.eof()) {
			in.read(buf_read, READ_BUF);      //读取数据到
			read_size = in.gcount();
			total_singal_file_size += read_size;            //记录文件的大小
			data_flow* d_f = new data_flow();
			if (read_size >= READ_BUF) {           //若文件的大小大于等于4MB时
				d_f->size = read_size;
				memcpy(d_f->data, buf_read, d_f->size);
				d_flow.push_back(d_f);
			}
			else {                         //文件的大小小于1MB时,此刻文件读取结束
				if (read_size > 0) {
					d_f->size = read_size;
					memcpy(d_f->data, buf_read, read_size);
					d_flow.push_back(d_f);
					memset(buf_read, 0, sizeof(buf_read));
					break;
				}
				else
					break;
			}
			memset(buf_read, 0, sizeof(buf_read));
		}
		in.close();
		file_info* f_i = new file_info();
		f_i->file_route = *it;
		f_i->file_size = total_singal_file_size;
		f_i->number_of_chunk = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
		file_information.push_back(f_i);
		it++;
	}
}

/*
void read_file() {         //文件路径列表
	string dir_path = "G:\\test_file\\dwj\\ding";     //这里为整个主目录

	vector<string> v;
	recusive_file(dir_path.c_str(), v);


	vector<string>::iterator it = v.begin();      //各个文件的路径
	vector<long> size_of_singal_file;
	static char buf[READ_BUF];
	string fp;

	data_flow d_f;                   //定义一个数据流节点
	static char tmp_buf[READ_BUF];
	int tmp_read_size = 0, index = 0;
	int read_size = 0;
	bool is_less_than_container = false;
	while (it != v.end()) {
		ifstream in((*it).c_str(), ios::binary);
		long file_size = 0;                //每次读取一个文件都得重新计算大小

		while (!in.eof()) {
			if (is_less_than_container) {
				memcpy(buf + index, tmp_buf, tmp_read_size);
				index += tmp_read_size;                     //这里是为了防止多次读取都少于4MB的情况
				memset(tmp_buf, 0, READ_BUF);
				is_less_than_container = false;
			}

			in.read(buf, READ_BUF - tmp_read_size);         //读取数据到
			read_size = in.gcount();
			if ((read_size + tmp_read_size) == READ_BUF) {           //若文件的大小大于等于4MB时
				d_f.size = read_size;
				memcpy(d_f.data, buf, d_f.size);
				d_flow.push_back(d_f);
				tmp_read_size = 0;                  //每次读取的数据块大大于等于4MB的时候,都将临时的读大小归零
				index = 0;
			}
			else {                         //文件的大小小于4MB时,此刻文件读取结束
				if (read_size != 0) {
					file_size += read_size;
					memcpy(tmp_buf, buf, read_size);
					tmp_read_size = read_size;             //临时保存
					is_less_than_container = true;
					break;
				}
				else
					continue;
			}
			file_size += read_size;

			memset(buf, 0, READ_BUF);
		}
		size_of_singal_file.push_back(file_size);       //记录各个文件的大小
		it++;
		in.close();
	}
	if (is_less_than_container) {                  //处理最后一个文件的数据流小于4MB的情况
		d_f.size = tmp_read_size;
		memcpy(d_f.data, tmp_buf, d_f.size);
		d_flow.push_back(d_f);
	}
}*/