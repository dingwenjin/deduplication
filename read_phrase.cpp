#include "deduplication.h"


list<struct data_flow*> d_flow;
list<file_info*> file_information;
//static char global_buf[READ_BUF];                   //��̬����ռ�ķ�ʽ������ʹ�þ�̬����(��Ϊ��̬����һֱ�������������򣬻�һֱռ���ڴ�ռ䣬֪������������ͷſռ�),������ʹ�þ�̬��ʽ

void start_read_file(vector<string> v) {         //�ļ�·���б�
	vector<string>::iterator it = v.begin();

	while (it != v.end()) {                //������ͬ���ļ�·��
		fstream in((*it).c_str(), ios::in|ios::binary);
		int total_singal_file_size=0;
		char* tmp_read = new char[READ_BUF];               //������ȫ�ֻ����������ݿ�������̬����Ŀռ���

		while (in.read(tmp_read, READ_BUF)) {
			total_singal_file_size += READ_BUF;                    //ͳ���ļ����ֽ���
			struct data_flow* d_f = new data_flow;
			memcpy(d_f->data, tmp_read, READ_BUF);
			d_f->size = READ_BUF;
			d_f->chunk_fp = TEMPORARY_FP;
			d_f->container_ID = CONTAINER_ID;
			//d_f->flag = 0;                              //����0��ʾ�¿�
			d_flow.push_back(d_f);

			//memset(tmp_read, 0, sizeof(tmp_read));
		}
		if (in.gcount() > 0) {                      //��¼��ȡû��1MB�����
			total_singal_file_size += in.gcount();
			struct data_flow* d_f = new data_flow;
			memcpy(d_f->data, tmp_read, in.gcount());
			d_f->size = in.gcount();
			d_f->chunk_fp = TEMPORARY_FP;
			d_f->container_ID = CONTAINER_ID;
			//d_f->flag = 0;                              //����0��ʾ�¿�
			d_flow.push_back(d_f);
			
			//memset(global_buf, 0, sizeof(global_buf));
		}		
		in.close();
	
		file_info* f_i = new file_info();
		f_i->file_route = *it;
		f_i->number_of_chunks = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
		file_information.push_back(f_i);
		it++;
	}
}

void read_file(vector<string> v) {
	start_read_file(v);
}