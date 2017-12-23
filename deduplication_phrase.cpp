#include "deduplication.h"
#include <list>
#include <iostream>
#include <unordered_map>
#include <time.h>
using namespace std;


extern list<data_file*> data_original;     //定义全局队列保存原始数据
//list<dedup_data_file*> data_dedup;          //定义全局队列保存去重后数据

unordered_map<string, int> fp_retrieve;
extern long count_original;
long count_dedup;

int num = 1;      //用于记录数据块所属的容器ID

bool fp_exist(string fp) {
	//cout << "---------------------------------------" << endl;
	if (fp_retrieve.find(fp) != fp_retrieve.end())         //当检索到直接返回
		return true;
	else {
		fp_retrieve.insert(unordered_map<string, int>::value_type(fp, 0));             //检索不到就插入
		return false;
	}
}
	/*
	list<string>::iterator it = fp_recipe.begin();
	long count = 0;
	while (it != fp_recipe.end()) {          //这里用一个计数器count来计数，每次从0开始查找到index这个范围的指纹
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
	count_dedup++;                           //记录不重复的数据块数
	dedup_data_file* d_f = new dedup_data_file();

	d_f->container_ID = num;                          //一个container中的数据块的容器ID都是一样的
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
	cout << "start deduplication..." << endl;
	list<data_file*>::iterator it = data_original.begin();
	while (it != data_original.end()) {
		if (!fp_exist((*it)->chunk_fp)) {
			//cout << "----------------------------------------" << endl;
			(*it)->flag = 0;
			(*it)->container_ID = num;                 //这里要注意，要先赋值在判断，不然会导致最后一个指纹的container_ID不对

			count_dedup++;                             //统计不重复的数据块数
			if (count_dedup % 1024 == 0)
				num++;

			//file_deduplication((*it)->data, (*it)->size, (*it)->chunk_fp);
		}
		else
			(*it)->flag = 1;                     //表示重复数据块
		//data_file* p = data_original.front();
		//delete p;
		//data_original.erase(it++);         //检索完原始数据队列节点后删除它，释放它的内存空间
		it++;
	}
	cout <<"original chunk is: "<<count_original<<"\t"<< "dedup chunk is: " << count_dedup << endl;
	end = clock();
	cout << "finish deduplication..." << endl;
	cout << "fingerprint lookup time is: " << double(end - start) << endl;
	
	/*
	list<string>::iterator it_1 = fp_recipe.begin();
	list<data_file*>::iterator it_2 = data_original.begin();
	long count = 0;
	//cout << "***************************dwj***************************" << endl;
	while (it_1 != fp_recipe.end()) {
		while (it_2 != data_original.end()) {
			if (!fp_exist(*it_1, count))            //这里的count用来控制查找的范围,检索后面的指纹需要与前面的指纹对比是否有重复
				file_deduplication((*it_2)->data, (*it_2)->size, *it_1);      //unique数据块直接放到data_dedup队列中
																		//这里不管是新块还是重复块,原始队列都得向后移动，同时退出内循环
			data_file* p = data_original.front();
			delete p;
			data_original.erase(it_2++);         //检索完原始数据队列节点后删除它，释放它的内存空间
			//delete *it_2;                    //释放结点空间
			break;
		}
		count++;
		it_1++;
	}*/
}