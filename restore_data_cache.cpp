#include "deduplication.h"


extern list<file_info*> file_information;
list<restore_data_cache*> restore_cache;

static char restore_buf[CONTAINER_SIZE];

int get_container_num(string container_path) {
	auto index = container_path.find_last_of("container");
	string s = container_path.substr(index+1, container_path.size());              //��ȡcontainer�ĺ���

	return atoi(s.c_str());
}


void cache_container_data(vector<string> backup_container_list) {
	vector<string>::iterator it = backup_container_list.begin();

	int count = 0;

	while (it != backup_container_list.end()) {

		fstream in(*it,ios::in|ios::binary);
		in.read(restore_buf, CONTAINER_SIZE);
		int len = int(in.gcount());                  //��¼��ȡ��������
		in.close();

		restore_data_cache* r_d_c = new restore_data_cache();
		r_d_c->container_ID = get_container_num(*it);
		r_d_c->size = len;
		//restore_buf[CONTAINER_SIZE] = '\0';
		memcpy(r_d_c->data,restore_buf,r_d_c->size);

		restore_cache.push_back(r_d_c);
		memset(restore_buf,0, sizeof(CONTAINER_SIZE));

		count++;
		if (count == 20)            //��ʼʱ��ȡ10��container
			break;

		it++;
	}
}


void container_replace(vector<string> backup_container_list,int ID) {
	restore_data_cache* p = restore_cache.front();      //ÿ���޳�һ����㶼���ͷ����Ŀռ�
	delete p;
	restore_cache.pop_front();               //ɾ�������һ��Ԫ��

	vector<string>::iterator it_container = backup_container_list.begin();
	
	while (it_container != backup_container_list.end()) {
		if (get_container_num(*it_container) == ID) {        //���Ҵ�����Ҫ������ʱ
			fstream in(*it_container, ios::in|ios::binary);
			in.read(restore_buf, CONTAINER_SIZE);
			int len = int(in.gcount());                  //��¼��ȡ��������
			in.close();

			restore_data_cache* r_d_c = new restore_data_cache();
			r_d_c->container_ID = ID;
			r_d_c->size = len;

			//restore_buf[CONTAINER_SIZE] = '\0';
			memcpy(r_d_c->data, restore_buf, r_d_c->size);

			restore_cache.push_back(r_d_c);
			memset(restore_buf, 0, sizeof(CONTAINER_SIZE));
			break;
		}
		else
			it_container++;
	}
}
