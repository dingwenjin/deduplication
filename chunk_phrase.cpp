#include "deduplication.h"
#include <iterator>
#include <cstring>
#include <iostream>


extern list<data_flow*> d_flow;
list<data_file*> data_original;     //定义全局队列保存原始数据

unsigned __int64 count_original;

extern char global_buf[READ_BUF];
//char tmp_chunk_buf[CHUNK_SIZE];               //用于动态分配数据块内存提供一个临时缓冲区

void fixed_chunking() {
	list<data_flow*>::iterator it = d_flow.begin();
	while (it != d_flow.end()) {
		int index = 0;
		while ((*it)->size >= CHUNK_SIZE) {
			char* new_data = new char[CHUNK_SIZE];
			memcpy(new_data, (*it)->data + index, CHUNK_SIZE);
			data_file *t_d = new data_file();
			t_d->size = CHUNK_SIZE;
			t_d->data = new_data;
			data_original.push_back(t_d);
			(*it)->size -= CHUNK_SIZE;
			index += CHUNK_SIZE;
			count_original++;
		}
		if ((*it)->size > 0) {
			char* new_data = new char[(*it)->size];
			memcpy(new_data, (*it)->data + index, (*it)->size);
	
			cout << (*it)->size << endl;
			data_file *t_d = new data_file();
			t_d->size = (*it)->size;
			t_d->data = new_data;
			data_original.push_back(t_d);
			count_original++;
		}
		data_flow* p = (*it);
		char *p_data = (*it)->data;
		delete p_data;
		delete p;

		d_flow.erase(it++);
	}
}