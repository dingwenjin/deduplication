#include "deduplication.h"



extern list<struct data_flow*> d_flow;
list<struct data_flow*> chunk_list;

unsigned __int64 count_original;


void start_fixed_chunking() {
	list<struct data_flow*>::iterator it = d_flow.begin();

	while (it != d_flow.end()) {
		int index = 0;
		while ((*it)->size >= CHUNK_SIZE) {
			struct data_flow* d_f = new data_flow;

			memcpy(d_f->data, (*it)->data + index, CHUNK_SIZE);
			d_f->size = CHUNK_SIZE;
			d_f->chunk_fp = TEMPORARY_FP;
			d_f->container_ID = CONTAINER_ID;
			//d_f->flag = 0;                              //这里0表示新块

			chunk_list.push_back(d_f);
			(*it)->size -= CHUNK_SIZE;
			index += CHUNK_SIZE;
			count_original++;
		}
		if ((*it)->size > 0) {
			struct data_flow* d_f = new data_flow;

			memcpy(d_f->data, (*it)->data + index, (*it)->size);
			d_f->size = (*it)->size;
			d_f->chunk_fp = TEMPORARY_FP;
			d_f->container_ID = CONTAINER_ID;
			//d_f->flag = 0;                              //这里0表示新块
			
			chunk_list.push_back(d_f);
			count_original++;
			
		}
		data_flow* p=(*it);
		delete p;

		d_flow.erase(it++);
	}
}

void fixed_chunking() {
	start_fixed_chunking();
}