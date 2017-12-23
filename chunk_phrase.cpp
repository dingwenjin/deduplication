#include "deduplication.h"
#include <iterator>
#include <cstring>

extern list<data_flow*> d_flow;
list<data_file*> data_original;     //����ȫ�ֶ��б���ԭʼ����

long count_original;

void fixed_chunking() {
	
	//cout << "*******************************" << endl;
	list<data_flow*>::iterator it = d_flow.begin();
	//cout << d_flow.size() << endl;
	
	int tmp_size = 0;           //����һ��container��ѭ���ֿ�

	while (it != d_flow.end()) {
		//cout << "------------------------------------------------" << endl;
		char *buf_chunk = new char[READ_BUF];
		memcpy(buf_chunk, (*it)->data, (*it)->size);             //����������㿽����һ����ʱ�Ļ�����
		//cout << buf << endl;
		tmp_size = (*it)->size;

		data_flow* p = (*it);
		delete p;

		d_flow.erase(it++);     //ÿ������һ�������ͷ����Ŀռ�

		//delete *it;                      //�ͷŽ��Ŀռ�

		int index = 0;
		while (tmp_size) {
			data_file* d_f = new data_file();
			if (tmp_size >= CHUNK_SIZE) {
				memcpy(d_f->data, buf_chunk + index, CHUNK_SIZE);         //������ÿ�λ�������ǰ����4KB��λ��
				d_f->size = CHUNK_SIZE;
				data_original.push_back(d_f);
				tmp_size -= CHUNK_SIZE;
				index += CHUNK_SIZE;
			}
			else {
				memcpy(d_f->data, buf_chunk + index, tmp_size);
				d_f->size = tmp_size;
				data_original.push_back(d_f);
				tmp_size = 0;
			}
			count_original++;
		}
		delete[] buf_chunk;
	}
}