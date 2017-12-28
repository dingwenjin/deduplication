#include "deduplication.h"
#include <vector>
#include <fstream>
#include <iostream>

list<data_flow*> d_flow;
list<file_info*> file_information;
static char global_buf[READ_BUF];                   //��̬����ռ�ķ�ʽ������ʹ�þ�̬����(��Ϊ��̬����һֱ�������������򣬻�һֱռ���ڴ�ռ䣬֪������������ͷſռ�),������ʹ�þ�̬��ʽ

void read_file(vector<string> v) {         //�ļ�·���б�
	vector<string>::iterator it = v.begin();

	while (it != v.end()) {                //������ͬ���ļ�·��
		cout << *it << endl;
		ifstream in((*it).c_str(), ios::binary);
		int total_singal_file_size = 0;

		while (in.read(global_buf, READ_BUF)) {
			total_singal_file_size += READ_BUF;                    //ͳ���ļ����ֽ���
			char* tmp_read = new char[READ_BUF];               //������ȫ�ֻ����������ݿ�������̬����Ŀռ���
			data_flow* d_f = new data_flow();
			memcpy(tmp_read, global_buf, READ_BUF);
			d_f->size = READ_BUF;
			d_f->data = tmp_read;               //���������ṹ���������ָ��ָ�������̬������ڴ�
			d_flow.push_back(d_f);

			memset(global_buf, 0, sizeof(global_buf));
		}
		if (in.gcount() > 0) {                      //��¼��ȡû��1MB�����
			total_singal_file_size += in.gcount();
			char* tmp_read = new char[in.gcount()];
			data_flow* d_f = new data_flow();
			memcpy(tmp_read, global_buf, in.gcount());
			d_f->size = in.gcount();
			d_f->data = tmp_read;
			d_flow.push_back(d_f);

			memset(global_buf, 0, sizeof(global_buf));
		}
		in.close();

		file_info* f_i = new file_info();
		f_i->file_route = *it;
		f_i->number_of_chunks = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
		file_information.push_back(f_i);
		it++;
	}
}