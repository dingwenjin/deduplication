#include "chunk_phrase.h"

extern list<data_flow*> d_flow;
list<data_flow*> chunk_list;

unsigned __int64 count_original;

extern mutex m_read;
mutex m_chunk;

extern condition_variable read_empty, read_full;
condition_variable chunk_empty, chunk_full;

extern bool is_file_read_over;

bool is_chunk_read_over = false;

void start_fixed_chunking() {
	while (true) {
		if (d_flow.empty() && is_file_read_over)             //当流队列为空，并且文件读取完毕则退出循环
			break;

		//m_read.lock();
		unique_lock<mutex> lk_read(m_read);
		read_full.wait(lk_read, []() {return d_flow.size() != 0; });              //当队列为空时，阻塞等待

		data_flow* d_f = d_flow.front();
		d_flow.pop_front();
		//m_read.unlock();
		read_empty.notify_one();

		size_t current_size = d_f->size;
		int index = 0,tmp_size=0;

		//m_chunk.lock();                //一次处理一个data_flow
		unique_lock<mutex> lk_chunk(m_chunk);
		chunk_empty.wait(lk_chunk, []() {return chunk_list.size() != QUEUE_SIZE; });                    //当队列为满时，阻塞等待

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
		//cout << "chunk_list: " << chunk_list.size() << endl;

		//m_chunk.unlock();
		chunk_full.notify_one();

		delete d_f;
	}
	//cout << "------------------------chunk over-----------------------------" << endl;
	is_chunk_read_over = true;
}

void fixed_chunking() {
	//cout << "start chunking..." << endl;
	start_fixed_chunking();
	//cout << "finish chunking..." << endl<<endl;
}