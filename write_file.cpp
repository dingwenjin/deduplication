#include "windows.h"
#include "deduplication.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

//extern list<string> fp_recipe;
//extern list<dedup_data_file*> data_dedup;          //����ȫ�ֶ��б���ȥ�غ�����
extern list<data_file*> data_original;     //����ȫ�ֶ��б���ԭʼ����
extern list<file_info*> file_information;
extern unsigned __int64 count_dedup;

/*
void write_fp_recipe(string main_restore_path) {
	fstream out, in;
	string path = main_restore_path + "\\" + "metadata";
	in.open(path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in)                //���Ŀ¼�������򴴽���
		CreateDirectory(path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in.close();

	string path1 = path + "\\" + "fingerprint_chunk";         //���ݿ�ָ��
	out.open(path1.c_str(), ios::app | ios::binary);

	// string path2=path+"\\"+"fingerprint_file";         //�ļ���ָ��
	// out2.open(path2.c_str(),ios::app|ios::binary);

	list<string>::iterator it = fp_recipe.begin();              //��ָ��д���ļ�����
	while (it != fp_recipe.end()) {
		out << *it << endl;
		it++;
	}
	out.close();
	fp_recipe.clear();                       //����������ݿ�ָ��
}*/


void write_file_info(string main_restore_path) {
	fstream out, in;
	string path = main_restore_path + "\\" + "metadata";
	in.open(path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in)                //���Ŀ¼�������򴴽���
		CreateDirectory(path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in.close();

	string path1 = path + "\\" + "file_information";         //���ݿ�ָ��
	out.open(path1.c_str(), ios::app | ios::binary);

	list<file_info*>::iterator it = file_information.begin();              //��ָ��д���ļ�����
	while (it != file_information.end()) {
		out << (*it)->file_route << "|" << (*it)->number_of_chunks  << endl;         //����������ո���Ϊ�˷�ֹ�ļ�������֮���пո���·������ȷ

		file_info* p = (*it);
		delete p;

		file_information.erase(it++);
		//delete *it;
	}
	out.close();
}


string int_to_string(int n) {
	ostringstream stream;
	stream << n;
	return stream.str();
}



void write_data() {
	ofstream out("G:\\restore\\ding", ios::app | ios::binary);
	list<data_file*>::iterator it = data_original.begin();
	while (it != data_original.end()) {
		out.write((*it)->data, (*it)->size);
		it++;
	}
	out.close();
}

void write_file_data(string main_restore_path) {
	cout << "****************************************************" << endl;
	fstream out_data,out_chunk_in_container,out_fp ,in_data, in_metadata;
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

	string container_path = container_tmp_path + "\\" + "chunk_in_container";     //���ݿ����ڵ���������ƫ�ƺʹ�С
	out_chunk_in_container.open(container_path.c_str(),ios::app|ios::binary);

	string path_all_fp = container_tmp_path + "\\" + "fingerprint_chunk";         //�������ݿ��ָ��
	out_fp.open(path_all_fp.c_str(), ios::app | ios::binary);

	list<data_file*>::iterator it;
	int i = 1, count = 0;
	string str;
	long number_of_dedup_chunks = count_dedup;


	//write_data();

	while (number_of_dedup_chunks) {                         //ȥ�غ�����ݿ���
		str = int_to_string(i);                 //i��������Ҫ�Ǽ�¼�ڼ���contianer
		string path_url = path_data + "\\" + "container" + str;         //�����ļ�
		out_data.open(path_url.c_str(), ios::app | ios::binary);          //������Ϊ��д�벻ͬ��container
		int offset = 0;

		it = data_original.begin();
		while (it != data_original.end()) {
			out_fp << (*it)->chunk_fp << endl;               //�����е�ָ��д���ļ�
			//if ((*it)->flag == 0) {              //������¿飬ֱ��д������
			out_data.write((*it)->data, (*it)->size);
			out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
			offset += (*it)->size;
			number_of_dedup_chunks--;
				
			//data_file* p = (*it);
			//char* p_data = (*it)->data;             //�����������������
			//delete p_data;                  //�ͷ����������Ŀռ�
			//delete p;

			//data_original.erase(it++);                 //�����������ͽ����Ƴ�����
			it++;
			
			if (number_of_dedup_chunks == 0)                //��������Ǹպ�number_of_dedup_chunksΪ0����ʱ��countΪ1024������������ô�˳���������е�ָ�Ʋ�û�б�д��
				count--;                //������Ϊ�˷�ֹ���һ��container��count�ﵽ1024��������������ѭ��
			count++;
			if (count % 1024 == 0)                 //4MBд����(�����ϣ�Ҳ����С��4MB,��Ϊ�е����ݿ����С��4KB)
				break;
			//}
			/*
			else {
				data_file* p = (*it);
				char* p_data = (*it)->data;             //�����������������
				delete p_data;                  //�ͷ����������Ŀռ�
				delete p;
				data_original.erase(it++);                 //�����������ͽ����Ƴ�����
			}*/
		}
		out_data.close();
		i++;
		if (i == 9)
			break;
	}
	out_fp.close();
	out_chunk_in_container.close();
	write_data();
}



/*
void write_file_data(string main_restore_path) {
	fstream out,in_data,in_metadata,out_container;
	string path = main_restore_path + "\\" + "backup";                    //������Ϊ��д��������
	in_data.open(path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_data)                                      //���Ŀ¼�������򴴽���
		CreateDirectory(path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_data.close();


	string container_tmp_path = main_restore_path + "\\" + "metadata";                //������Ϊ��д��������Ϣ
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_metadata)                //���Ŀ¼�������򴴽���
		CreateDirectory(container_tmp_path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_metadata.close();

	string container_path = container_tmp_path + "\\" + "chunk_in_container";         //���ݿ�ָ��

	int i = 1, count = 0;
	string str;
	list<dedup_data_file*>::iterator it;
	int tmp_count_dedup = count_dedup;             //�ܵ�ȥ�غ�����ݿ���

	//cout << "----------------------------------------------------------------" << endl;
	out_container.open(container_path.c_str(),ios::app|ios::binary);         //�򿪴��Ԫ���ݵ�����
	while (tmp_count_dedup) {
		str = int_to_string(i);                 //i��������Ҫ�Ǽ�¼�ڼ���contianer
		string path_url = path + "\\" + "container" + str;         //�����ļ�
																  
		out.open(path_url.c_str(), ios::app | ios::binary);          //������Ϊ��д�벻ͬ��container
		int offset = 0;
		it = data_dedup.begin();                //ÿ��д��һ��container��ʹ�ͷ��ʼ
		while (it != data_dedup.end()) {
			out.write((*it)->data, (*it)->size);           //������Ϊ��д����
			out_container<<(*it)->chunk_fp<<" "<<(*it)->container_ID<<" "<<offset<<" "<<(*it)->size<<endl;    //������Ϊ��дԪ����
			offset += (*it)->size;           //��¼ÿ�����ݿ��������е�ƫ����

			dedup_data_file* p = (*it);
			delete p;
			data_dedup.erase(it++);                 //�����������ͽ����Ƴ�����

			count++;
			tmp_count_dedup--;
			if (count % 1024 == 0)                 //4MBд����(�����ϣ�Ҳ����С��4MB,��Ϊ�е����ݿ����С��4KB)
				break;
		}
		out.close();
		i++;
	}
	out_container.close();
}*/