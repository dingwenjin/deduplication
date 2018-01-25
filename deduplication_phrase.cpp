#include "deduplication_phrase.h"


extern list<data_flow*> hash_list;
unordered_map<string, char> fp_retrieve;
list<data_flow*> dedup_list;

extern unsigned __int64 count_original;
unsigned __int64 count_dedup;

extern mutex m_fp;
mutex m_dedup;

extern condition_variable fp_empty, fp_full;
condition_variable dedup_empty, dedup_full;

extern bool is_fp_over;
bool is_dedup_over = false;

bool fp_exist(string fp) {
	if (fp_retrieve.find(fp) != fp_retrieve.end())         //当检索到直接返回
		return true;
	else {
		fp_retrieve.insert(unordered_map<string, char>::value_type(fp, '0'));             //检索不到就插入
		return false;
	}
}

void start_data_deduplication() {
	size_t count = 0;
	uint16_t container_id = 1;
	
	while (true) {
		if (hash_list.empty() && is_fp_over)
			break;

		//m_fp.lock();
		unique_lock<mutex> lk_fp(m_fp);
		fp_full.wait(lk_fp, []() {return hash_list.size() != 0; });

		data_flow* d_f = hash_list.front();
		hash_list.pop_front();
		//m_fp.unlock();
		fp_empty.notify_one();


		if (!fp_exist(d_f->chunk_fp)) {
			count++;
			count_dedup++;
			d_f->container_ID = container_id;
			if (count % 1024 == 0)
				container_id++;

			//m_dedup.lock();
			unique_lock<mutex> lk_dedup(m_dedup);
			dedup_empty.wait(lk_dedup, []() {return dedup_list.size() != QUEUE_SIZE; });

			dedup_list.push_back(d_f);
			//m_dedup.unlock();
			dedup_full.notify_one();

		}
		else {
			d_f->flag = DEDUP_CHUNK;

			//m_dedup.lock();
			unique_lock<mutex> lk_dedup(m_dedup);
			dedup_empty.wait(lk_dedup, []() {return dedup_list.size() != QUEUE_SIZE; });

			dedup_list.push_back(d_f);
			//m_dedup.unlock();
			dedup_full.notify_one();
		}
		
	}
	//cout << "--------------------------dedup over---------------------------------" << endl;
	is_dedup_over = true;
	fp_retrieve.clear();
}


void data_deduplication() {
	clock_t start, end;
	//cout << "start deduplication..." << endl;

	start = clock();
	start_data_deduplication();
	end = clock();

	//cout << "finish deduplication..." << endl;
	//cout << "original chunk is: " << count_original << "\t" << "dedup chunk is: " << count_dedup << endl;
	cout << "fingerprint lookup time is: " << double(end - start) << endl;
}