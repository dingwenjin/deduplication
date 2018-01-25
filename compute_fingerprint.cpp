#include "compute_fingerprint.h"
#include "hash.h"

extern list<data_flow*> chunk_list;
list<data_flow*> hash_list;

extern mutex m_chunk;
mutex m_fp;

extern condition_variable chunk_empty, chunk_full;
condition_variable fp_empty, fp_full;

extern bool is_chunk_read_over;
bool is_fp_over = false;

void start_compute_fp() {
	while (true) {
		if (chunk_list.empty() && is_chunk_read_over)
			break;

		//m_chunk.lock();
		unique_lock<mutex> lk_chunk(m_chunk);
		chunk_full.wait(lk_chunk, []() {return chunk_list.size() != 0; });

		data_flow* d_f = chunk_list.front();
		chunk_list.pop_front();
		
		//m_chunk.unlock();
		chunk_empty.notify_one();

		d_f->chunk_fp = hash_sha1(d_f->data);

		//m_fp.lock();
		unique_lock<mutex> lk_fp(m_fp);
		fp_empty.wait(lk_fp, []() {return hash_list.size() != QUEUE_SIZE; });

		hash_list.push_back(d_f);

		fp_full.notify_one();
		//m_fp.unlock();
	}

	is_fp_over = true;
	//cout << "-----------------------fp over------------------------" << endl;
}


void compute_fp() {
	clock_t start_hash, end_hash;
	//cout << "start computing fingerprint..." << endl;
	start_hash = clock();

	start_compute_fp();

	end_hash = clock();
	//cout << "finish computing fingerprint..." << endl;
	cout << "compute fingerprint time is: " << double(end_hash - start_hash) << endl;
}












