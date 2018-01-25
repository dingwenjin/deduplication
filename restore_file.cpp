#include "restore_file.h"
#include "restore_data_cache.h"



extern list<file_info*> file_information;              //文件的相关信息
extern list<file_recipe*> fp_in_container;         
extern list<restore_data_cache*> restore_cache;

//mutex m_restore_cache;
//extern mutex m_file_info;
//extern mutex m_fp_in_container;

static char tmp_buf[CONTAINER_SIZE];

bool find_container(int ID) {
	list<restore_data_cache*>::iterator it_restore_cache = restore_cache.begin();
	while (it_restore_cache != restore_cache.end()) {
		if ((*it_restore_cache)->container_ID == ID) {
			memset(tmp_buf,0,sizeof(CONTAINER_SIZE));
			memcpy(tmp_buf, (*it_restore_cache)->data.c_str(), (*it_restore_cache)->size);
			return true;
		}
		else
			it_restore_cache++;
	}
	return false;
}


void start_restore_original_data(vector<string> backup_container_list) {
	
	list<file_info*>::iterator it_file_info = file_information.begin();            //此刻这里的路径为恢复的路径
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //所有数据块的元数据
	
	int tmp_ID = 0,count=1;

	//m_file_info.lock();

	while (it_file_info != file_information.end()) {        //迭代需要恢复的文件

		string file_tmp_route = (*it_file_info)->file_route;                //要恢复的文件路径
		int chunks_of_file = (*it_file_info)->number_of_chunks;
		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //打开需要恢复的文件路径


		//m_fp_in_container.lock();
		while (it_file_recipe != fp_in_container.end()) {
	
			if (chunks_of_file == 0)                          //这里主要是为了防止空文件
				break;

			if ((*it_file_recipe)->container_ID != tmp_ID) {           //要读取的数据块不在临时缓冲区中,就去更大的缓冲区去找
				tmp_ID = (*it_file_recipe)->container_ID;

				//m_restore_cache.lock();             //在检测数据块所在的缓冲区时加锁
				while(!find_container(tmp_ID))            //在内存的缓冲区中找到后直接替换原来的临时缓冲区
					container_replace(backup_container_list, tmp_ID);
				//m_restore_cache.unlock();

			}
			out_file_route.write(tmp_buf +(*it_file_recipe)->offset, (*it_file_recipe)->size);
			chunks_of_file--;
			
			it_file_recipe++;
			
			if (chunks_of_file == 0)
				break;
		}
		out_file_route.close();

		//m_fp_in_container.unlock();

		it_file_info++;
	}

	//m_file_info.unlock();
}


void restore_original_data(vector<string> backup_container_list) {
	cout << "start cache container..." << endl;

	//m_restore_cache.lock();
	cache_container_data(backup_container_list);             //缓存数据块
	//m_restore_cache.unlock();

	cout << "finish cache..." << endl<<endl;

	cout << "start restore ..." << endl;
	start_restore_original_data(backup_container_list);
	cout << "finish restore ..." << endl<<endl;
}