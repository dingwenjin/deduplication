#include "deduplication.h"
#include <list>
#include <iostream>
#include <unordered_map>
#include <time.h>
#include <fstream>
using namespace std;


extern list<data_file*> data_original;     //����ȫ�ֶ��б���ԭʼ����
//list<dedup_data_file*> data_dedup;          //����ȫ�ֶ��б���ȥ�غ�����

unordered_map<string, char> fp_retrieve;
extern unsigned __int64 count_original;
unsigned __int64 count_dedup;

/*
void write_data() {
	ofstream out("G:\\restore\\ding1", ios::app | ios::binary);
	list<data_file*>::iterator it = data_original.begin();
	while (it != data_original.end()) {
		out.write((*it)->data, (*it)->size);
		it++;
	}
	out.close();
}*/

bool fp_exist(string fp) {
	//cout << "---------------------------------------" << endl;
	if (fp_retrieve.find(fp) != fp_retrieve.end())         //��������ֱ�ӷ���
		return true;
	else {
		fp_retrieve.insert(unordered_map<string, char>::value_type(fp, '0'));             //���������Ͳ���
		return false;
	}
}
	/*
	list<string>::iterator it = fp_recipe.begin();
	long count = 0;
	while (it != fp_recipe.end()) {          //������һ��������count��������ÿ�δ�0��ʼ���ҵ�index�����Χ��ָ��
		if (count == index)
			break;
		if (*it == fp)
			return true;
		count++;
		it++;
	}
	return false;
	*/
//}

/*
void file_deduplication(char buf[], int len, string fp) {
	count_dedup++;                           //��¼���ظ������ݿ���
	dedup_data_file* d_f = new dedup_data_file();

	d_f->container_ID = num;                          //һ��container�е����ݿ������ID����һ����
	memcpy(d_f->data, buf, len);
	d_f->size = len;
	d_f->chunk_fp = fp;
	data_dedup.push_back(d_f);

	if (count_dedup % 1024 == 0)
		num++;
}*/

void data_deduplication() {
	clock_t start, end;
	start = clock();
	int num = 1;      //���ڼ�¼���ݿ�����������ID
	cout << "start deduplication..." << endl;
	list<data_file*>::iterator it = data_original.begin();
	while (it != data_original.end()) {
		if (!fp_exist((*it)->chunk_fp)) {
			//cout << "----------------------------------------" << endl;
			(*it)->flag = 0;
			(*it)->container_ID = num;                 //����Ҫע�⣬Ҫ�ȸ�ֵ���жϣ���Ȼ�ᵼ�����һ��ָ�Ƶ�container_ID����

			count_dedup++;                             //ͳ�Ʋ��ظ������ݿ���
			if (count_dedup % 1024 == 0)
				num++;

			//file_deduplication((*it)->data, (*it)->size, (*it)->chunk_fp);
		}
		else
			(*it)->flag = 1;                     //��ʾ�ظ����ݿ�
		//data_file* p = data_original.front();
		//delete p;
		//data_original.erase(it++);         //������ԭʼ���ݶ��нڵ��ɾ�������ͷ������ڴ�ռ�
		it++;
	}
	fp_retrieve.clear();
	cout << "original chunk is: " << count_original << "\t" << "dedup chunk is: " << count_dedup << endl;
	end = clock();
	cout << "finish deduplication..." << endl;
	cout << "fingerprint lookup time is: " << double(end - start) << endl;
	//write_data();
}
	/*
	list<string>::iterator it_1 = fp_recipe.begin();
	list<data_file*>::iterator it_2 = data_original.begin();
	long count = 0;
	//cout << "***************************dwj***************************" << endl;
	while (it_1 != fp_recipe.end()) {
		while (it_2 != data_original.end()) {
			if (!fp_exist(*it_1, count))            //�����count�������Ʋ��ҵķ�Χ,���������ָ����Ҫ��ǰ���ָ�ƶԱ��Ƿ����ظ�
				file_deduplication((*it_2)->data, (*it_2)->size, *it_1);      //unique���ݿ�ֱ�ӷŵ�data_dedup������
																		//���ﲻ�����¿黹���ظ���,ԭʼ���ж�������ƶ���ͬʱ�˳���ѭ��
			data_file* p = data_original.front();
			delete p;
			data_original.erase(it_2++);         //������ԭʼ���ݶ��нڵ��ɾ�������ͷ������ڴ�ռ�
			//delete *it_2;                    //�ͷŽ��ռ�
			break;
		}
		count++;
		it_1++;
	}*/
//}