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

	//static char buf[CONTAINER_SIZE];                       //静态缓冲区是必须的，不然会出现栈溢出,这样做可以实现动态数组的功能，但是静态变量作用于整个程序，也就是他会一直占用内存空间，知道程序结束时才释放空间
	//char *buf = new char[CONTAINER_SIZE];                      //动态申请char字符数组，可以在函数结束时释放它的空间
	int count = 0;
    
	while (it != backup_container_list.end()) {
		//cout << *it << endl;
		//cout << "************************************************" << endl;
		//cout<< get_container_num(*it)<<endl;
		ifstream in(*it,ios::binary);
		in.read(restore_buf, CONTAINER_SIZE);
		int len = int(in.gcount());                  //记录读取的数据量
		in.close();

		restore_data_cache* r_d_c = new restore_data_cache();
		r_d_c->container_ID = get_container_num(*it);
		r_d_c->size = len;
		//r_d_c->hit_count = 0;
		memcpy(r_d_c->data,restore_buf,r_d_c->size);

		restore_cache.push_back(r_d_c);
		memset(restore_buf,0, len);

		count++;
		if (count == 20)            //初始时读取10个container
			break;

		it++;
	}
}


void container_replace(vector<string> backup_container_list,int ID) {
	restore_data_cache* p = restore_cache.front();      //每次剔除一个结点都得释放它的空间
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
