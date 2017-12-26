#include "deduplication.h"
#include "restore_data_cache.h"



extern list<file_info*> file_information;              //文件的相关信息
extern list<file_recipe*> fp_in_container;         
extern list<restore_data_cache*> restore_cache;


char tmp_buf[CONTAINER_SIZE];

bool find_container(int ID) {
	list<restore_data_cache*>::iterator it_restore_cache = restore_cache.begin();
	while (it_restore_cache != restore_cache.end()) {
		if ((*it_restore_cache)->container_ID == ID) {
			memset(tmp_buf,0,sizeof(CONTAINER_SIZE));
			memcpy(tmp_buf, (*it_restore_cache)->data, (*it_restore_cache)->size);
			return true;
		}
		else
			it_restore_cache++;
	}
	return false;
}


void restore_original_data(vector<string> backup_container_list) {

	cout << "start cache container..." << endl;
	cache_container_data(backup_container_list);             //缓存数据块
	cout << "finish cache..." << endl;

	list<file_info*>::iterator it_file_info = file_information.begin();            //此刻这里的路径为恢复的路径
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //所有数据块的元数据
	
	int tmp_ID = 0,count=1;
	while (it_file_info != file_information.end()) {        //迭代需要恢复的文件

		string file_tmp_route = (*it_file_info)->file_route;                //要恢复的文件路径
		int chunks_of_file = (*it_file_info)->number_of_chunks;

		fstream out_file_route(file_tmp_route, ios::app | ios::binary);      //打开需要恢复的文件路径

		while (it_file_recipe != fp_in_container.end()) {
	
			if (chunks_of_file == 0)                          //这里主要是为了防止空文件
				break;

			if ((*it_file_recipe)->container_ID != tmp_ID) {           //要读取的数据块不在临时缓冲区中,就去更大的缓冲区去找
				tmp_ID = (*it_file_recipe)->container_ID;
				while(!find_container(tmp_ID))            //在内存的缓冲区中找到后直接替换原来的临时缓冲区
					container_replace(backup_container_list, tmp_ID);
			}
			out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
			chunks_of_file--;
			
			it_file_recipe++;
			
			if (chunks_of_file == 0)
				break;
		}
		out_file_route.close();
		it_file_info++;
	}
}

