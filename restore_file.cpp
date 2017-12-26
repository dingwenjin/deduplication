#include "deduplication.h"
#include "restore_data_cache.h"



extern list<file_info*> file_information;              //�ļ��������Ϣ
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
	cache_container_data(backup_container_list);             //�������ݿ�
	cout << "finish cache..." << endl;

	list<file_info*>::iterator it_file_info = file_information.begin();            //�˿������·��Ϊ�ָ���·��
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //�������ݿ��Ԫ����
	
	int tmp_ID = 0,count=1;
	while (it_file_info != file_information.end()) {        //������Ҫ�ָ����ļ�

		string file_tmp_route = (*it_file_info)->file_route;                //Ҫ�ָ����ļ�·��
		int chunks_of_file = (*it_file_info)->number_of_chunks;

		fstream out_file_route(file_tmp_route, ios::app | ios::binary);      //����Ҫ�ָ����ļ�·��

		while (it_file_recipe != fp_in_container.end()) {
	
			if (chunks_of_file == 0)                          //������Ҫ��Ϊ�˷�ֹ���ļ�
				break;

			if ((*it_file_recipe)->container_ID != tmp_ID) {           //Ҫ��ȡ�����ݿ鲻����ʱ��������,��ȥ����Ļ�����ȥ��
				tmp_ID = (*it_file_recipe)->container_ID;
				while(!find_container(tmp_ID))            //���ڴ�Ļ��������ҵ���ֱ���滻ԭ������ʱ������
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

