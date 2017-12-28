#include "deduplication.h"
#include "restore_data_cache.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


extern list<file_info*> file_information;              //文件的相关信息
extern list<file_recipe*> fp_in_container;         //去重后数据的指纹及其所在的container
extern list<restore_data_cache*> restore_cache;

/*
char tmp_buf[CONTAINER_SIZE];

bool find_container(int ID) {
	list<restore_data_cache*>::iterator it_restore_cache = restore_cache.begin();

	while (it_restore_cache != restore_cache.end()) {
		if ((*it_restore_cache)->container_ID == ID) {
			memset(tmp_buf,0,sizeof(CONTAINER_SIZE));
			memcpy(tmp_buf, (*it_restore_cache)->data, (*it_restore_cache)->size);       //将容器得数据拷贝到临时缓冲区
			return true;
		}
		else
			it_restore_cache++;
	}
	return false;
}*/


void restore_original_data(vector<string> backup_container_list) {

	cout << "start cache container..." << endl;
	cache_container_data(backup_container_list);             //缓存数据块
	cout << "finish cache..." << endl;
	/*
	list<file_info*>::iterator it_file_info = file_information.begin();            //此刻这里的路径为恢复的路径
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //所有数据块的元数据
	list<restore_data_cache*>::iterator it;

	int tmp_ID = 0,count=1;
	while (it_file_info != file_information.end()) {        //迭代需要恢复的文件
		
		string file_tmp_route = (*it_file_info)->file_route;                //要恢复的文件路径
		int chunks_of_file = (*it_file_info)->number_of_chunks;

		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //打开需要恢复的文件路径

		while (it_file_recipe != fp_in_container.end()) {
		
			if (chunks_of_file == 0)                          //这里主要是为了防止空文件
				break;

			it = restore_cache.begin();
			while (it != restore_cache.end()) {
				if ((*it)->container_ID == (*it_file_recipe)->container_ID) {
					out_file_route.write((*it)->data+(*it_file_recipe)->offset, (*it_file_recipe)->size);
					chunks_of_file--;
					break;
				}
				it++;
			}
			/*
			if ((*it_file_recipe)->container_ID != tmp_ID) {           //要读取的数据块不在临时缓冲区中,就去更大的缓冲区去找
				tmp_ID = (*it_file_recipe)->container_ID;
				while(!find_container(tmp_ID))            //在内存的缓冲区中找到后直接替换原来的临时缓冲区
					container_replace(backup_container_list, tmp_ID);
			}
			
			out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
			
			*/
			/*
			it_file_recipe++;

			if (chunks_of_file == 0)
				break;
		}
		out_file_route.close();
		it_file_info++;
	}
	list<restore_data_cache*>::iterator it_gc = restore_cache.begin();

	while (it_gc != restore_cache.end()) {                            //文件恢复完成后，回收队列
		restore_data_cache* p_gc = (*it_gc);
		delete p_gc;
		it_gc ++ ;
	}*/
}


/*
void restore_original_data(vector<string> backup_container_list) {
	cout << "start cache container..." << endl;
	cache_container_data(backup_container_list);             //缓存数据块
	cout << "finish cache..." << endl;          

	list<file_info*>::iterator it_file_info = file_information.begin();            //此刻这里的路径为恢复的路径
	list<file_recipe*>::iterator it_file_recipe;          //去重后的数据块指纹序列
	list<string>::iterator it = fp_recipe.begin();
	//vector<string>::iterator it_container = backup_container_list.begin();            
	int tmp_ID = -1;
	bool is_replace_container = false;
	while (it_file_info != file_information.end()) {        //迭代需要恢复的文件
		string file_tmp_route = (*it_file_info)->file_route;                //要恢复的文件路径
		//cout << file_tmp_route << endl;
		//int number_of_chunks = (*it_file_info)->number_of_chunk;
		int size_of_file = (*it_file_info)->file_size;

		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //打开需要恢复的文件路径

		//bool not_found_fp = false;
		while (it != fp_recipe.end()) {                    //检索所有数据块指纹序列
			it_file_recipe = fp_in_container.begin();
			while (it_file_recipe != fp_in_container.end()) {       //检索去重文件谱
				if (*it == (*it_file_recipe)->chunk_fp) {
					if ((*it_file_recipe)->container_ID != tmp_ID) {          //当所需要的容器不在临时缓冲区
						tmp_ID = (*it_file_recipe)->container_ID;
						if (!find_container(tmp_ID)) {          //当在缓存中找到所需要的容器时                   
							container_replace(backup_container_list, tmp_ID);//当在缓存中没有找到所需要的容器时，此刻需要替换缓存中的容器，从磁盘读取所需要的容器；
							is_replace_container = true;
						}
					}
					else {                      //当在同一个容器中遇到连续的数据块时，直接访问上次的容器
						out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
						size_of_file -= (*it_file_recipe)->size;              //每次写入一次数据，该文件所需要写的数据量将减少
						break;
					}

					if (is_replace_container) {                        //刚刚从磁盘中读取所需要的container
						find_container(tmp_ID);                      //将刚刚读取出来的container放入缓冲区
						out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
						size_of_file -= (*it_file_recipe)->size;              //每次写入一次数据，该文件所需要写的数据量将减少
						break;
					}
						
				}
				else {
					//not_found_fp = true;
					it_file_recipe++;
				}
			}
			if (size_of_file == 0) {         //每次一个数据块都得判断文件是否已经写完了
				out_file_route.close();
				it++;
				break;
			}
			//if(not_found_fp)         //这里主要是为了解决文件内部的重复数据块，当文件内部有重复数据块时需要从头检索，否则一直往后检索知道重复数据块指纹序列检索完毕
				//it_file_recipe = fp_in_container.begin();
			//else
			it++;
		}
		it_file_info++;                        //迭代文件
	}
}*/