#include "deduplication.h"
#include <vector>
#include <fstream>
#include <string.h>


using namespace std;

list<data_flow*> d_flow;
list<file_info*> file_information;
static char buf_read[READ_BUF];                   //��̬����ռ�ķ�ʽ������ʹ�þ�̬����(��Ϊ��̬����һֱ�������������򣬻�һֱռ���ڴ�ռ䣬֪������������ͷſռ�),������ʹ�þ�̬��ʽ

void read_file(vector<string> v) {         //�ļ�·���б�
	
	vector<string>::iterator it = v.begin();
	

	while (it != v.end()) {                //������ͬ���ļ�·��
		ifstream in((*it).c_str(), ios::binary);
		int read_size = 0;
		unsigned __int64 total_singal_file_size=0;
		while (!in.eof()) {
			in.read(buf_read, READ_BUF);      //��ȡ���ݵ�
			read_size = in.gcount();
			total_singal_file_size += read_size;            //��¼�ļ��Ĵ�С
			data_flow* d_f = new data_flow();
			if (read_size >= READ_BUF) {           //���ļ��Ĵ�С���ڵ���4MBʱ
				d_f->size = read_size;
				memcpy(d_f->data, buf_read, d_f->size);
				d_flow.push_back(d_f);
			}
			else {                         //�ļ��Ĵ�СС��1MBʱ,�˿��ļ���ȡ����
				if (read_size > 0) {
					d_f->size = read_size;
					memcpy(d_f->data, buf_read, read_size);
					d_flow.push_back(d_f);
					memset(buf_read, 0, sizeof(buf_read));
					break;
				}
				else
					break;
			}
			memset(buf_read, 0, sizeof(buf_read));
		}
		in.close();
		file_info* f_i = new file_info();
		f_i->file_route = *it;
		f_i->file_size = total_singal_file_size;
		f_i->number_of_chunk = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
		file_information.push_back(f_i);
		it++;
	}
}

/*
void read_file() {         //�ļ�·���б�
	string dir_path = "G:\\test_file\\dwj\\ding";     //����Ϊ������Ŀ¼

	vector<string> v;
	recusive_file(dir_path.c_str(), v);


	vector<string>::iterator it = v.begin();      //�����ļ���·��
	vector<long> size_of_singal_file;
	static char buf[READ_BUF];
	string fp;

	data_flow d_f;                   //����һ���������ڵ�
	static char tmp_buf[READ_BUF];
	int tmp_read_size = 0, index = 0;
	int read_size = 0;
	bool is_less_than_container = false;
	while (it != v.end()) {
		ifstream in((*it).c_str(), ios::binary);
		long file_size = 0;                //ÿ�ζ�ȡһ���ļ��������¼����С

		while (!in.eof()) {
			if (is_less_than_container) {
				memcpy(buf + index, tmp_buf, tmp_read_size);
				index += tmp_read_size;                     //������Ϊ�˷�ֹ��ζ�ȡ������4MB�����
				memset(tmp_buf, 0, READ_BUF);
				is_less_than_container = false;
			}

			in.read(buf, READ_BUF - tmp_read_size);         //��ȡ���ݵ�
			read_size = in.gcount();
			if ((read_size + tmp_read_size) == READ_BUF) {           //���ļ��Ĵ�С���ڵ���4MBʱ
				d_f.size = read_size;
				memcpy(d_f.data, buf, d_f.size);
				d_flow.push_back(d_f);
				tmp_read_size = 0;                  //ÿ�ζ�ȡ�����ݿ����ڵ���4MB��ʱ��,������ʱ�Ķ���С����
				index = 0;
			}
			else {                         //�ļ��Ĵ�СС��4MBʱ,�˿��ļ���ȡ����
				if (read_size != 0) {
					file_size += read_size;
					memcpy(tmp_buf, buf, read_size);
					tmp_read_size = read_size;             //��ʱ����
					is_less_than_container = true;
					break;
				}
				else
					continue;
			}
			file_size += read_size;

			memset(buf, 0, READ_BUF);
		}
		size_of_singal_file.push_back(file_size);       //��¼�����ļ��Ĵ�С
		it++;
		in.close();
	}
	if (is_less_than_container) {                  //�������һ���ļ���������С��4MB�����
		d_f.size = tmp_read_size;
		memcpy(d_f.data, tmp_buf, d_f.size);
		d_flow.push_back(d_f);
	}
}*/