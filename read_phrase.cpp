#include "deduplication.h"
#include <vector>
#include <fstream>
#include <iostream>

list<data_flow*> d_flow;
list<file_info*> file_information;
static char global_buf[READ_BUF];                   //动态申请空间的方式，避免使用静态变量(因为静态变量一直伴随着整个程序，会一直占用内存空间，知道程序结束后释放空间),但这里使用静态方式

void read_file(vector<string> v) {         //文件路径列表
	vector<string>::iterator it = v.begin();

	while (it != v.end()) {                //迭代不同的文件路径
		cout << *it << endl;
		ifstream in((*it).c_str(), ios::binary);
		int total_singal_file_size = 0;

		while (in.read(global_buf, READ_BUF)) {
			total_singal_file_size += READ_BUF;                    //统计文件的字节数
			char* tmp_read = new char[READ_BUF];               //将读到全局缓冲区的数据拷贝到动态申请的空间中
			data_flow* d_f = new data_flow();
			memcpy(tmp_read, global_buf, READ_BUF);
			d_f->size = READ_BUF;
			d_f->data = tmp_read;               //将数据流结构体的数据域指针指向这个动态分配的内存
			d_flow.push_back(d_f);

			memset(global_buf, 0, sizeof(global_buf));
		}
		if (in.gcount() > 0) {                      //记录读取没有1MB的情况
			total_singal_file_size += in.gcount();
			char* tmp_read = new char[in.gcount()];
			data_flow* d_f = new data_flow();
			memcpy(tmp_read, global_buf, in.gcount());
			d_f->size = in.gcount();
			d_f->data = tmp_read;
			d_flow.push_back(d_f);

			memset(global_buf, 0, sizeof(global_buf));
		}
		in.close();

		file_info* f_i = new file_info();
		f_i->file_route = *it;
		f_i->number_of_chunks = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
		file_information.push_back(f_i);
		it++;
	}
}