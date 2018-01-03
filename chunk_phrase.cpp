#include "chunk_phrase.h"

extern list<data_flow*> d_flow;
list<data_flow*> chunk_list;

unsigned __int64 count_original;


void start_fixed_chunking() {

	while (true) {
		if (d_flow.empty())
			break;
		data_flow* d_f = d_flow.front();
		size_t current_size = d_f->size;
		int index = 0,tmp_size=0;

		while (current_size) {
			data_flow* d_f_chunk = new data_flow;
			if (current_size >= CHUNK_SIZE)
				tmp_size = CHUNK_SIZE;
			else
				tmp_size = current_size;

			d_f_chunk->data.assign(d_f->data, index, tmp_size);
			d_f_chunk->size = tmp_size;
			d_f_chunk->flag = UNIQUE_CHUNK;
			d_f_chunk->chunk_fp = TEMPORARY_FP;
			d_f_chunk->container_ID = CONTAINER_ID;
			chunk_list.push_back(d_f_chunk);

			current_size -= tmp_size;
			index += tmp_size;
			count_original++;
		}

		delete d_f;
		d_flow.pop_front();
	}
}

void fixed_chunking() {
	cout << "start chunking..." << endl;
	start_fixed_chunking();
	cout << "finish chunking..." << endl<<endl;
}