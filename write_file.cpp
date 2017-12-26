#include "windows.h"
#include "deduplication.h"




extern list<struct data_flow*> dedup_list;     //����ȫ�ֶ��б���ԭʼ����
extern list<file_info*> file_information;
extern unsigned __int64 count_dedup;

extern list<string> fp_list;


void write_file_info(string container_tmp_path) {
	fstream out;
	string path1 = container_tmp_path + "\\" + "file_information";         //���ݿ�ָ��
	out.open(path1.c_str(),ios::app|ios::binary);
	//cout << path1 << endl;
	list<file_info*>::iterator it = file_information.begin();              //��ָ��д���ļ�����
	while (it != file_information.end()) {

		out << (*it)->file_route << "|" << (*it)->number_of_chunks  << endl;         //����������ո���Ϊ�˷�ֹ�ļ�������֮���пո���·������ȷ
		file_info* p = (*it);
		delete p;

		file_information.erase(it++);
	}
	out.close();
}


string int_to_string(int n) {
	ostringstream stream;
	stream << n;
	return stream.str();
}

void write_all_fingerprint(string container_tmp_path) {
	fstream out_fp;
	string path_all_fp = container_tmp_path + "\\" + "fingerprint_chunk";         //�������ݿ��ָ��
	out_fp.open(path_all_fp.c_str(),ios::app|ios::binary);

	list<string>::iterator it = fp_list.begin();
	while (it != fp_list.end()) {
		out_fp << *it << endl;
		fp_list.erase(it++);
	}
	out_fp.close();
}

void write_file_data(string main_restore_path) {
	cout << "****************************************************" << endl;
	fstream out_data, out_chunk_in_container, in_data, in_metadata;
	string path_data = main_restore_path + "\\" + "backup";                    //������Ϊ��д��������
	in_data.open(path_data.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_data)                                      //���Ŀ¼�������򴴽���
		CreateDirectory(path_data.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_data.close();


	string container_tmp_path = main_restore_path + "\\" + "metadata";                //������Ϊ��д��������Ϣ
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_metadata)                //���Ŀ¼�������򴴽���
		CreateDirectory(container_tmp_path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_metadata.close();


	write_all_fingerprint(container_tmp_path);              //д����ָ��
	write_file_info(container_tmp_path);                  //д�ļ�·�����ļ��Ŀ���

	string container_path = container_tmp_path + "\\" + "chunk_in_container";     //���ݿ����ڵ���������ƫ�ƺʹ�С
	out_chunk_in_container.open(container_path.c_str(),ios::app|ios::binary);
	
	list<struct data_flow*>::iterator it;
	uint16_t i = 1;                         //��¼container��
	string str;
	long number_of_dedup_chunks = count_dedup;
	//cout << number_of_dedup_chunks << endl;

	while (number_of_dedup_chunks) {
		str = int_to_string(i);                 //i��������Ҫ�Ǽ�¼�ڼ���contianer
		string path_url = path_data + "\\" + "container" + str;         //�����ļ�
		out_data.open(path_url.c_str(),ios::app|ios::binary);          //������Ϊ��д�벻ͬ��container
		int offset = 0,count=0;

		it = dedup_list.begin();
		while (it != dedup_list.end()) {
			out_data.write((*it)->data, (*it)->size);
			out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
			offset += (*it)->size;
			number_of_dedup_chunks--;

			struct data_flow* p = (*it);
			char* p_data = (*it)->data;             //�����������������
			delete p_data;                  //�ͷ����������Ŀռ�
			delete p;

			dedup_list.erase(it++);                 //�����������ͽ����Ƴ�����

			count++;
			if (count % 1024 == 0)                 //4MBд����(�����ϣ�Ҳ����С��4MB,��Ϊ�е����ݿ����С��4KB)
				break;
			
		}
		out_data.close();
		i++;
	}
	out_chunk_in_container.close();
}




















	/*
	list<data_file*>::iterator it;
	int i = 1, count = 0;
	string str;
	long number_of_dedup_chunks = count_dedup;
	while (number_of_dedup_chunks) {                         //ȥ�غ�����ݿ���
		str = int_to_string(i);                 //i��������Ҫ�Ǽ�¼�ڼ���contianer
		string path_url = path_data + "\\" + "container" + str;         //�����ļ�
		out_data.open(path_url.c_str(), ios::app | ios::binary);          //������Ϊ��д�벻ͬ��container
		int offset = 0;

		it = data_original.begin();
		while (it != data_original.end()) {
			out_fp << (*it)->chunk_fp << endl;               //�����е�ָ��д���ļ�
			if ((*it)->flag == 0) {              //������¿飬ֱ��д������
				out_data.write((*it)->data, (*it)->size);
				out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
				offset += (*it)->size;
				number_of_dedup_chunks--;

				data_file* p = (*it);
				char* p_data = (*it)->data;             //�����������������
				delete p_data;                  //�ͷ����������Ŀռ�
				delete p;


				data_original.erase(it++);                 //�����������ͽ����Ƴ�����

				if (number_of_dedup_chunks == 0)                //��������Ǹպ�number_of_dedup_chunksΪ0����ʱ��countΪ1024������������ô�˳���������е�ָ�Ʋ�û�б�д��
					count--;                //������Ϊ�˷�ֹ���һ��container��count�ﵽ1024��������������ѭ��
				count++;
				if (count % 1024 == 0)                 //4MBд����(�����ϣ�Ҳ����С��4MB,��Ϊ�е����ݿ����С��4KB)
					break;
			}
			else {
				data_file* p = (*it);
				char* p_data = (*it)->data;             //�����������������
				delete p_data;                  //�ͷ����������Ŀռ�
				delete p;
				data_original.erase(it++);                 //�����������ͽ����Ƴ�����
			}
			
		}
		out_data.close();
		i++;
	}
	out_fp.close();
	out_chunk_in_container.close();
	*/
//}
