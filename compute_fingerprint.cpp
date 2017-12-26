#include "deduplication.h"
#include "hash.h"

extern list<struct data_flow*> chunk_list;
list<struct data_flow*> hash_list;

void compute_fp() {
	cout << "start computing fingerprint..." << endl;
	list<struct data_flow*>::iterator it = chunk_list.begin();
	string s = "";
	while (it != chunk_list.end()) {
		struct data_flow* d_f = (*it);
		s.assign((*it)->data);
		d_f->chunk_fp = hash_md5(s);

		hash_list.push_back(d_f);

		delete d_f;

		chunk_list.erase(it++);
	}
	cout << "finish computing fingerprint..." << endl;
}




















/*
string int_to_string_1(int n) {
ostringstream stream;
stream << n;
return stream.str();
}

void test_content() {

list<tmp_data_chunk*>::iterator it = tmp_chunk_data.begin();
int k = 1;
int count = 0;
while (it != tmp_chunk_data.end()) {
count++;
if (count == 1 || count == 34) {
string path = "G:\\restore\\test_write\\chunk" + int_to_string_1(k);
fstream out(path, ios::app);
char* a = (*it)->data;
for (int i = 0; i < (*it)->size; i++) {
bitset<8> b(__toascii(*(a + i)));
//char* b = *(a + i);

if (i % 32 == 0)
out << endl;
out << b << endl;
}

out.close();
k++;
}


it++;
}
}*/

