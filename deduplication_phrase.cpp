#include "deduplication_phrase.h"


extern list<data_flow*> hash_list;
unordered_map<string, char> fp_retrieve;
list<data_flow*> dedup_list;

extern unsigned __int64 count_original;
unsigned __int64 count_dedup;


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
		if (hash_list.empty())
			break;
		data_flow* d_f = hash_list.front();
		if (!fp_exist(d_f->chunk_fp)) {
			count++;
			count_dedup++;
			d_f->container_ID = container_id;
			if (count % 1024 == 0)
				container_id++;
			dedup_list.push_back(d_f);
		}
		else {
			d_f->flag = DEDUP_CHUNK;
			dedup_list.push_back(d_f);
		}
		hash_list.pop_front();
	}
	fp_retrieve.clear();

}


void data_deduplication() {
	clock_t start, end;
	cout << "start deduplication..." << endl;

	start = clock();
	start_data_deduplication();
	end = clock();

	cout << "finish deduplication..." << endl;
	cout << "original chunk is: " << count_original << "\t" << "dedup chunk is: " << count_dedup << endl;
	cout << "fingerprint lookup time is: " << double(end - start) << endl<<endl;
}