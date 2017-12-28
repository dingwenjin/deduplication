#include "deduplication.h"
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>

extern list<file_info*> file_information;
extern list<file_recipe*> fp_in_container;                       //所有数据块的指纹及它们所属的容器

list<restore_data_cache*> restore_cache;

extern list<data_flow*> d_flow;


static char restore_buf[CONTAINER_SIZE];

int get_container_num(string container_path) {
	auto index = container_path.find_last_of("container");
	string s = container_path.substr(index+1, container_path.size());
	return atoi(s.c_str());
}


void cache_container_data(vector<string> backup_container_list) {
	/*
	vector<string>::iterator it = backup_container_list.begin();

	list<file_recipe*>::iterator it_1 = fp_in_container.begin();
	ofstream out("G:\\restore\\ding", ios::app | ios::binary);
	int count = 0,num=0;
	while (it_1 != fp_in_container.end()) {
		if (num != (*it_1)->container_ID) {
			num = (*it_1)->container_ID;
			while (it != backup_container_list.end()) {
				if (num == get_container_num(*it)) {
					ifstream in(*it, ios::binary);
					memset(restore_buf,0, CONTAINER_SIZE);
					in.read(restore_buf, CONTAINER_SIZE);
					break;
				}
				it++;
			}
		}
		out.write(restore_buf+(*it_1)->offset,(*it_1)->size);
		it_1++;
	}
	out.close();
	*/
	/*
	while (it != backup_container_list.end()) {
		ifstream in(*it,ios::binary);
		in.read(restore_buf, CONTAINER_SIZE);
		int len = int(in.gcount());                  //记录读取的数据量
		in.close();

		restore_data_cache* r_d_c = new restore_data_cache();
		char* tmp_buf = new char[len];
		memcpy(tmp_buf,restore_buf,len);
		r_d_c->container_ID = get_container_num(*it);
		r_d_c->size = len;
		r_d_c->data = tmp_buf;

		//memcpy(r_d_c->data,restore_buf,r_d_c->size);
		restore_cache.push_back(r_d_c);
		memset(restore_buf,0, len);

		count++;
		if (count == 200)            //初始时读取10个container
			break;

		it++;
	}*/
}


void container_replace(vector<string> backup_container_list,int ID) {
	restore_data_cache* p = restore_cache.front();      //每次剔除一个结点都得释放它的空间
	char* p_data = p->data;
	delete p_data;
	delete p;
	restore_cache.pop_front();               //删除链表第一个元素

	vector<string>::iterator it_container = backup_container_list.begin();
	
	while (it_container != backup_container_list.end()) {
		if (get_container_num(*it_container) == ID) {        //当找打所需要的容器时
			ifstream in(*it_container, ios::binary);
			in.read(restore_buf, CONTAINER_SIZE);
			int len = int(in.gcount());                  //记录读取的数据量
			in.close();

			restore_data_cache* r_d_c = new restore_data_cache();
			r_d_c->container_ID = ID;
			r_d_c->size = len;
			memcpy(r_d_c->data, restore_buf, r_d_c->size);

			restore_cache.push_back(r_d_c);
			memset(restore_buf, 0, len);
			break;
		}
		else
			it_container++;
	}
}
