#include "deduplication.h"
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>

extern list<file_info*> file_information;
list<restore_data_cache*> restore_cache;

static char restore_buf[CONTAINER_SIZE];

int get_container_num(string container_path) {
	auto index = container_path.find_last_of("container");
	string s = container_path.substr(index+1, container_path.size());
	//cout << s << endl;
	return atoi(s.c_str());
}


void cache_container_data(vector<string> backup_container_list) {
	vector<string>::iterator it = backup_container_list.begin();
	//copy(backup_container_list.begin(), backup_container_list.end(),ostream_iterator<string>(cout,"\n"));

	//static char buf[CONTAINER_SIZE];                       //��̬�������Ǳ���ģ���Ȼ�����ջ���,����������ʵ�ֶ�̬����Ĺ��ܣ����Ǿ�̬������������������Ҳ��������һֱռ���ڴ�ռ䣬֪���������ʱ���ͷſռ�
	//char *buf = new char[CONTAINER_SIZE];                      //��̬����char�ַ����飬�����ں�������ʱ�ͷ����Ŀռ�
	int count = 0;
    
	while (it != backup_container_list.end()) {
		//cout << *it << endl;
		//cout << "************************************************" << endl;
		//cout<< get_container_num(*it)<<endl;
		ifstream in(*it,ios::binary);
		in.read(restore_buf, CONTAINER_SIZE);
		int len = int(in.gcount());                  //��¼��ȡ��������
		in.close();

		restore_data_cache* r_d_c = new restore_data_cache();
		r_d_c->container_ID = get_container_num(*it);
		r_d_c->size = len;
		//r_d_c->hit_count = 0;
		memcpy(r_d_c->data,restore_buf,r_d_c->size);

		restore_cache.push_back(r_d_c);
		memset(restore_buf,0, len);

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
			ifstream in(*it_container, ios::binary);
			in.read(restore_buf, CONTAINER_SIZE);
			int len = int(in.gcount());                  //��¼��ȡ��������
			in.close();

			restore_data_cache* r_d_c = new restore_data_cache();
			r_d_c->container_ID = ID;
			r_d_c->size = len;
			//r_d_c->hit_count = 0;
			memcpy(r_d_c->data, restore_buf, r_d_c->size);

			restore_cache.push_back(r_d_c);
			memset(restore_buf, 0, len);
			break;
		}
		else
			it_container++;
	}
}
