#include "windows.h"
#include "deduplication.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

//extern list<string> fp_recipe;
//extern list<dedup_data_file*> data_dedup;          //定义全局队列保存去重后数据
extern list<data_file*> data_original;     //定义全局队列保存原始数据
extern list<file_info*> file_information;
extern unsigned __int64 count_dedup;

/*
void write_fp_recipe(string main_restore_path) {
	fstream out, in;
	string path = main_restore_path + "\\" + "metadata";
	in.open(path.c_str(), ios::in);          //尝试是否能打开
	if (!in)                //如果目录不存在则创建它
		CreateDirectory(path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in.close();

	string path1 = path + "\\" + "fingerprint_chunk";         //数据块指纹
	out.open(path1.c_str(), ios::app | ios::binary);

	// string path2=path+"\\"+"fingerprint_file";         //文件名指纹
	// out2.open(path2.c_str(),ios::app|ios::binary);

	list<string>::iterator it = fp_recipe.begin();              //将指纹写到文件谱中
	while (it != fp_recipe.end()) {
		out << *it << endl;
		it++;
	}
	out.close();
	fp_recipe.clear();                       //清除所有数据块指纹
}*/


void write_file_info(string main_restore_path) {
	fstream out, in;
	string path = main_restore_path + "\\" + "metadata";
	in.open(path.c_str(), ios::in);          //尝试是否能打开
	if (!in)                //如果目录不存在则创建它
		CreateDirectory(path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in.close();

	string path1 = path + "\\" + "file_information";         //数据块指纹
	out.open(path1.c_str(), ios::app | ios::binary);

	list<file_info*>::iterator it = file_information.begin();              //将指纹写到文件谱中
	while (it != file_information.end()) {
		out << (*it)->file_route << "|" << (*it)->number_of_chunks  << endl;         //这里放两个空格是为了防止文件夹名字之间有空格导致路径不正确

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
	string path_data = main_restore_path + "\\" + "backup";                    //这里是为了写备份数据
	in_data.open(path_data.c_str(), ios::in);          //尝试是否能打开
	if (!in_data)                                      //如果目录不存在则创建它
		CreateDirectory(path_data.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in_data.close();


	string container_tmp_path = main_restore_path + "\\" + "metadata";                //这里是为了写容器的信息
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //尝试是否能打开
	if (!in_metadata)                //如果目录不存在则创建它
		CreateDirectory(container_tmp_path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in_metadata.close();

	string container_path = container_tmp_path + "\\" + "chunk_in_container";     //数据块所在的容器及其偏移和大小
	out_chunk_in_container.open(container_path.c_str(),ios::app|ios::binary);

	string path_all_fp = container_tmp_path + "\\" + "fingerprint_chunk";         //所有数据块的指纹
	out_fp.open(path_all_fp.c_str(), ios::app | ios::binary);

	list<data_file*>::iterator it;
	int i = 1, count = 0;
	string str;
	long number_of_dedup_chunks = count_dedup;


	//write_data();

	while (number_of_dedup_chunks) {                         //去重后的数据块数
		str = int_to_string(i);                 //i在这里主要是记录第几个contianer
		string path_url = path_data + "\\" + "container" + str;         //备份文件
		out_data.open(path_url.c_str(), ios::app | ios::binary);          //这里是为了写入不同的container
		int offset = 0;

		it = data_original.begin();
		while (it != data_original.end()) {
			out_fp << (*it)->chunk_fp << endl;               //将所有的指纹写入文件
			//if ((*it)->flag == 0) {              //如果是新块，直接写入容器
			out_data.write((*it)->data, (*it)->size);
			out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
			offset += (*it)->size;
			number_of_dedup_chunks--;
				
			//data_file* p = (*it);
			//char* p_data = (*it)->data;             //这项这个结点的数据域
			//delete p_data;                  //释放这个数据域的空间
			//delete p;

			//data_original.erase(it++);                 //用完这个结点后就将它移除链表
			it++;
			
			if (number_of_dedup_chunks == 0)                //这种情况是刚好number_of_dedup_chunks为0，此时的count为1024的整数倍，那么退出后可能所有的指纹并没有被写入
				count--;                //这里是为了防止最后一个container的count达到1024的整数倍后跳出循环
			count++;
			if (count % 1024 == 0)                 //4MB写容器(本质上，也可能小于4MB,因为有的数据块可能小于4KB)
				break;
			//}
			/*
			else {
				data_file* p = (*it);
				char* p_data = (*it)->data;             //这项这个结点的数据域
				delete p_data;                  //释放这个数据域的空间
				delete p;
				data_original.erase(it++);                 //用完这个结点后就将它移除链表
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
	string path = main_restore_path + "\\" + "backup";                    //这里是为了写备份数据
	in_data.open(path.c_str(), ios::in);          //尝试是否能打开
	if (!in_data)                                      //如果目录不存在则创建它
		CreateDirectory(path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in_data.close();


	string container_tmp_path = main_restore_path + "\\" + "metadata";                //这里是为了写容器的信息
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //尝试是否能打开
	if (!in_metadata)                //如果目录不存在则创建它
		CreateDirectory(container_tmp_path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in_metadata.close();

	string container_path = container_tmp_path + "\\" + "chunk_in_container";         //数据块指纹

	int i = 1, count = 0;
	string str;
	list<dedup_data_file*>::iterator it;
	int tmp_count_dedup = count_dedup;             //总的去重后的数据块数

	//cout << "----------------------------------------------------------------" << endl;
	out_container.open(container_path.c_str(),ios::app|ios::binary);         //打开存放元数据的容器
	while (tmp_count_dedup) {
		str = int_to_string(i);                 //i在这里主要是记录第几个contianer
		string path_url = path + "\\" + "container" + str;         //备份文件
																  
		out.open(path_url.c_str(), ios::app | ios::binary);          //这里是为了写入不同的container
		int offset = 0;
		it = data_dedup.begin();                //每次写完一个container后就从头开始
		while (it != data_dedup.end()) {
			out.write((*it)->data, (*it)->size);           //这里是为了写数据
			out_container<<(*it)->chunk_fp<<" "<<(*it)->container_ID<<" "<<offset<<" "<<(*it)->size<<endl;    //这里是为了写元数据
			offset += (*it)->size;           //记录每个数据块在容器中的偏移量

			dedup_data_file* p = (*it);
			delete p;
			data_dedup.erase(it++);                 //用完这个结点后就将它移除链表

			count++;
			tmp_count_dedup--;
			if (count % 1024 == 0)                 //4MB写容器(本质上，也可能小于4MB,因为有的数据块可能小于4KB)
				break;
		}
		out.close();
		i++;
	}
	out_container.close();
}*/