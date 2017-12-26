#include "deduplication.h"
#include <time.h>


extern list<struct data_flow*> hash_list;
unordered_map<string, char> fp_retrieve;
list<struct data_flow*> dedup_list;

extern unsigned __int64 count_original;
unsigned __int64 count_dedup;

list<string> fp_list;

bool fp_exist(string fp) {
	if (fp_retrieve.find(fp) != fp_retrieve.end())         //当检索到直接返回
		return true;
	else {
		fp_retrieve.insert(unordered_map<string, char>::value_type(fp, '0'));             //检索不到就插入
		return false;
	}
}



void start_data_deduplication() {
	clock_t start, end;
	start = clock();
	uint16_t num = 1;      //用于记录数据块所属的容器ID
	cout << "start deduplication..." << endl;
	list<struct data_flow*>::iterator it = hash_list.begin();
	size_t count = 0,container_id=1;
	while(it!=hash_list.end()) {
		fp_list.push_back((*it)->chunk_fp);

		struct data_flow* d_f = (*it);
		if (!fp_exist((*it)->chunk_fp)) {
			count++;
			//d_f->flag = UNIQUE_CHUNK;
			d_f->container_ID = container_id;

			if (count % 1024 == 0) {
				container_id++;
			}
			dedup_list.push_back(d_f);
		}
		else 
			dedup_list.push_back(d_f);
		delete d_f;
	}
	
	fp_retrieve.clear();
	cout << "original chunk is: " << count_original << "\t" << "dedup chunk is: " << count_dedup << endl;
	end = clock();
	cout << "finish deduplication..." << endl;
	cout << "fingerprint lookup time is: " << double(end - start) << endl;

}


void data_deduplication() {
	start_data_deduplication();
}