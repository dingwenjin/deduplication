#include "deduplication.h"
#include "restore_data_cache.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


extern list<file_info*> file_information;              //�ļ��������Ϣ
extern list<file_recipe*> fp_in_container;         //ȥ�غ����ݵ�ָ�Ƽ������ڵ�container
extern list<restore_data_cache*> restore_cache;

/*
char tmp_buf[CONTAINER_SIZE];

bool find_container(int ID) {
	list<restore_data_cache*>::iterator it_restore_cache = restore_cache.begin();

	while (it_restore_cache != restore_cache.end()) {
		if ((*it_restore_cache)->container_ID == ID) {
			memset(tmp_buf,0,sizeof(CONTAINER_SIZE));
			memcpy(tmp_buf, (*it_restore_cache)->data, (*it_restore_cache)->size);       //�����������ݿ�������ʱ������
			return true;
		}
		else
			it_restore_cache++;
	}
	return false;
}*/


void restore_original_data(vector<string> backup_container_list) {

	cout << "start cache container..." << endl;
	cache_container_data(backup_container_list);             //�������ݿ�
	cout << "finish cache..." << endl;
	/*
	list<file_info*>::iterator it_file_info = file_information.begin();            //�˿������·��Ϊ�ָ���·��
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //�������ݿ��Ԫ����
	list<restore_data_cache*>::iterator it;

	int tmp_ID = 0,count=1;
	while (it_file_info != file_information.end()) {        //������Ҫ�ָ����ļ�
		
		string file_tmp_route = (*it_file_info)->file_route;                //Ҫ�ָ����ļ�·��
		int chunks_of_file = (*it_file_info)->number_of_chunks;

		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //����Ҫ�ָ����ļ�·��

		while (it_file_recipe != fp_in_container.end()) {
		
			if (chunks_of_file == 0)                          //������Ҫ��Ϊ�˷�ֹ���ļ�
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
			if ((*it_file_recipe)->container_ID != tmp_ID) {           //Ҫ��ȡ�����ݿ鲻����ʱ��������,��ȥ����Ļ�����ȥ��
				tmp_ID = (*it_file_recipe)->container_ID;
				while(!find_container(tmp_ID))            //���ڴ�Ļ��������ҵ���ֱ���滻ԭ������ʱ������
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

	while (it_gc != restore_cache.end()) {                            //�ļ��ָ���ɺ󣬻��ն���
		restore_data_cache* p_gc = (*it_gc);
		delete p_gc;
		it_gc ++ ;
	}*/
}


/*
void restore_original_data(vector<string> backup_container_list) {
	cout << "start cache container..." << endl;
	cache_container_data(backup_container_list);             //�������ݿ�
	cout << "finish cache..." << endl;          

	list<file_info*>::iterator it_file_info = file_information.begin();            //�˿������·��Ϊ�ָ���·��
	list<file_recipe*>::iterator it_file_recipe;          //ȥ�غ�����ݿ�ָ������
	list<string>::iterator it = fp_recipe.begin();
	//vector<string>::iterator it_container = backup_container_list.begin();            
	int tmp_ID = -1;
	bool is_replace_container = false;
	while (it_file_info != file_information.end()) {        //������Ҫ�ָ����ļ�
		string file_tmp_route = (*it_file_info)->file_route;                //Ҫ�ָ����ļ�·��
		//cout << file_tmp_route << endl;
		//int number_of_chunks = (*it_file_info)->number_of_chunk;
		int size_of_file = (*it_file_info)->file_size;

		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //����Ҫ�ָ����ļ�·��

		//bool not_found_fp = false;
		while (it != fp_recipe.end()) {                    //�����������ݿ�ָ������
			it_file_recipe = fp_in_container.begin();
			while (it_file_recipe != fp_in_container.end()) {       //����ȥ���ļ���
				if (*it == (*it_file_recipe)->chunk_fp) {
					if ((*it_file_recipe)->container_ID != tmp_ID) {          //������Ҫ������������ʱ������
						tmp_ID = (*it_file_recipe)->container_ID;
						if (!find_container(tmp_ID)) {          //���ڻ������ҵ�����Ҫ������ʱ                   
							container_replace(backup_container_list, tmp_ID);//���ڻ�����û���ҵ�����Ҫ������ʱ���˿���Ҫ�滻�����е��������Ӵ��̶�ȡ����Ҫ��������
							is_replace_container = true;
						}
					}
					else {                      //����ͬһ���������������������ݿ�ʱ��ֱ�ӷ����ϴε�����
						out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
						size_of_file -= (*it_file_recipe)->size;              //ÿ��д��һ�����ݣ����ļ�����Ҫд��������������
						break;
					}

					if (is_replace_container) {                        //�ոմӴ����ж�ȡ����Ҫ��container
						find_container(tmp_ID);                      //���ոն�ȡ������container���뻺����
						out_file_route.write(tmp_buf + (*it_file_recipe)->offset, (*it_file_recipe)->size);
						size_of_file -= (*it_file_recipe)->size;              //ÿ��д��һ�����ݣ����ļ�����Ҫд��������������
						break;
					}
						
				}
				else {
					//not_found_fp = true;
					it_file_recipe++;
				}
			}
			if (size_of_file == 0) {         //ÿ��һ�����ݿ鶼���ж��ļ��Ƿ��Ѿ�д����
				out_file_route.close();
				it++;
				break;
			}
			//if(not_found_fp)         //������Ҫ��Ϊ�˽���ļ��ڲ����ظ����ݿ飬���ļ��ڲ����ظ����ݿ�ʱ��Ҫ��ͷ����������һֱ�������֪���ظ����ݿ�ָ�����м������
				//it_file_recipe = fp_in_container.begin();
			//else
			it++;
		}
		it_file_info++;                        //�����ļ�
	}
}*/