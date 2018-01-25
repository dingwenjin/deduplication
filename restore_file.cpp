#include "restore_file.h"
#include "restore_data_cache.h"



extern list<file_info*> file_information;              //�ļ��������Ϣ
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
	
	list<file_info*>::iterator it_file_info = file_information.begin();            //�˿������·��Ϊ�ָ���·��
	list<file_recipe*>::iterator it_file_recipe = fp_in_container.begin();           //�������ݿ��Ԫ����
	
	int tmp_ID = 0,count=1;

	//m_file_info.lock();

	while (it_file_info != file_information.end()) {        //������Ҫ�ָ����ļ�

		string file_tmp_route = (*it_file_info)->file_route;                //Ҫ�ָ����ļ�·��
		int chunks_of_file = (*it_file_info)->number_of_chunks;
		ofstream out_file_route(file_tmp_route, ios::app | ios::binary);      //����Ҫ�ָ����ļ�·��


		//m_fp_in_container.lock();
		while (it_file_recipe != fp_in_container.end()) {
	
			if (chunks_of_file == 0)                          //������Ҫ��Ϊ�˷�ֹ���ļ�
				break;

			if ((*it_file_recipe)->container_ID != tmp_ID) {           //Ҫ��ȡ�����ݿ鲻����ʱ��������,��ȥ����Ļ�����ȥ��
				tmp_ID = (*it_file_recipe)->container_ID;

				//m_restore_cache.lock();             //�ڼ�����ݿ����ڵĻ�����ʱ����
				while(!find_container(tmp_ID))            //���ڴ�Ļ��������ҵ���ֱ���滻ԭ������ʱ������
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
	cache_container_data(backup_container_list);             //�������ݿ�
	//m_restore_cache.unlock();

	cout << "finish cache..." << endl<<endl;

	cout << "start restore ..." << endl;
	start_restore_original_data(backup_container_list);
	cout << "finish restore ..." << endl<<endl;
}