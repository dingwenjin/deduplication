#include "compute_fingerprint.h"
#include "hash.h"

extern list<data_flow*> chunk_list;
list<data_flow*> hash_list;

void start_compute_fp() {
	while (true) {
		if (chunk_list.empty())
			break;

		data_flow* d_f = chunk_list.front();
		d_f->chunk_fp = hash_sha1(d_f->data);

		hash_list.push_back(d_f);
		chunk_list.pop_front();
	}
}


void compute_fp() {
	clock_t start_hash, end_hash;
	cout << "start computing fingerprint..." << endl;
	start_hash = clock();

	start_compute_fp();

	end_hash = clock();
	cout << "finish computing fingerprint..." << endl;
	cout << "compute fingerprint time is: " << double(end_hash - start_hash) << endl<<endl;
}












