#include "windows.h"
#include "deduplication.h"




extern list<struct data_flow*> dedup_list;     //定义全局队列保存原始数据
extern list<file_info*> file_information;
extern unsigned __int64 count_dedup;

extern list<string> fp_list;


void write_file_info(string container_tmp_path) {
	fstream out;
	string path1 = container_tmp_path + "\\" + "file_information";         //数据块指纹
	out.open(path1.c_str(),ios::app|ios::binary);
	//cout << path1 << endl;
	list<file_info*>::iterator it = file_information.begin();              //将指纹写到文件谱中
	while (it != file_information.end()) {

		out << (*it)->file_route << "|" << (*it)->number_of_chunks  << endl;         //这里放两个空格是为了防止文件夹名字之间有空格导致路径不正确
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
	string path_all_fp = container_tmp_path + "\\" + "fingerprint_chunk";         //所有数据块的指纹
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


	write_all_fingerprint(container_tmp_path);              //写所有指纹
	write_file_info(container_tmp_path);                  //写文件路径及文件的块数

	string container_path = container_tmp_path + "\\" + "chunk_in_container";     //数据块所在的容器及其偏移和大小
	out_chunk_in_container.open(container_path.c_str(),ios::app|ios::binary);
	
	list<struct data_flow*>::iterator it;
	uint16_t i = 1;                         //记录container号
	string str;
	long number_of_dedup_chunks = count_dedup;
	//cout << number_of_dedup_chunks << endl;

	while (number_of_dedup_chunks) {
		str = int_to_string(i);                 //i在这里主要是记录第几个contianer
		string path_url = path_data + "\\" + "container" + str;         //备份文件
		out_data.open(path_url.c_str(),ios::app|ios::binary);          //这里是为了写入不同的container
		int offset = 0,count=0;

		it = dedup_list.begin();
		while (it != dedup_list.end()) {
			out_data.write((*it)->data, (*it)->size);
			out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
			offset += (*it)->size;
			number_of_dedup_chunks--;

			struct data_flow* p = (*it);
			char* p_data = (*it)->data;             //这项这个结点的数据域
			delete p_data;                  //释放这个数据域的空间
			delete p;

			dedup_list.erase(it++);                 //用完这个结点后就将它移除链表

			count++;
			if (count % 1024 == 0)                 //4MB写容器(本质上，也可能小于4MB,因为有的数据块可能小于4KB)
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
	while (number_of_dedup_chunks) {                         //去重后的数据块数
		str = int_to_string(i);                 //i在这里主要是记录第几个contianer
		string path_url = path_data + "\\" + "container" + str;         //备份文件
		out_data.open(path_url.c_str(), ios::app | ios::binary);          //这里是为了写入不同的container
		int offset = 0;

		it = data_original.begin();
		while (it != data_original.end()) {
			out_fp << (*it)->chunk_fp << endl;               //将所有的指纹写入文件
			if ((*it)->flag == 0) {              //如果是新块，直接写入容器
				out_data.write((*it)->data, (*it)->size);
				out_chunk_in_container << (*it)->chunk_fp << "|" << (*it)->container_ID << "|" << offset << "|" << (*it)->size << endl;
				offset += (*it)->size;
				number_of_dedup_chunks--;

				data_file* p = (*it);
				char* p_data = (*it)->data;             //这项这个结点的数据域
				delete p_data;                  //释放这个数据域的空间
				delete p;


				data_original.erase(it++);                 //用完这个结点后就将它移除链表

				if (number_of_dedup_chunks == 0)                //这种情况是刚好number_of_dedup_chunks为0，此时的count为1024的整数倍，那么退出后可能所有的指纹并没有被写入
					count--;                //这里是为了防止最后一个container的count达到1024的整数倍后跳出循环
				count++;
				if (count % 1024 == 0)                 //4MB写容器(本质上，也可能小于4MB,因为有的数据块可能小于4KB)
					break;
			}
			else {
				data_file* p = (*it);
				char* p_data = (*it)->data;             //这项这个结点的数据域
				delete p_data;                  //释放这个数据域的空间
				delete p;
				data_original.erase(it++);                 //用完这个结点后就将它移除链表
			}
			
		}
		out_data.close();
		i++;
	}
	out_fp.close();
	out_chunk_in_container.close();
	*/
//}
