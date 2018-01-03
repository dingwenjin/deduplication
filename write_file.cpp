#include "write_file.h"

extern list<data_flow*> dedup_list;     //定义全局队列保存原始数据


string int_to_string(int n) {
	ostringstream stream;
	stream << n;
	return stream.str();
}

void start_write_file_data(string main_restore_path) {

	fstream out_data, out_chunk_in_container, in_data, in_metadata,full_fp;
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

	string fp_path = container_tmp_path + "\\" + "full_fingerprint";     //记录所有数据块的指纹
	full_fp.open(fp_path.c_str(), ios::app | ios::binary);

	uint16_t i = 1;                         //记录container号
	string str;

	bool is_dedup_over = false;                           //这里主要是为了控制外层循环

	while (true) {
		str = int_to_string(i);                 //i在这里主要是记录第几个contianer
		string path_url = path_data + "\\" + "container" + str;         //备份文件
		out_data.open(path_url.c_str(),ios::app|ios::binary);          //这里是为了写入不同的container
		int offset = 0,count=0;

		while (true) {
			if (dedup_list.empty()) {
				is_dedup_over = true;
				break;
			}
			data_flow* d_f = dedup_list.front();
			full_fp << d_f->chunk_fp << endl;                  //写所有数据块的指纹
			if (d_f->flag == UNIQUE_CHUNK) {                           //写不重复数据块的数据
				out_data.write(d_f->data.c_str(), d_f->size);
				out_chunk_in_container << d_f->chunk_fp << "|" << d_f->container_ID << "|" << offset << "|" << d_f->size << endl;
				offset += d_f->size;

				count++;                               //主要用于记录写入的container中的数据块数目
			}
			
			delete d_f;
			dedup_list.pop_front();

			if (count % 1024 == 0)                 //4MB写容器(本质上，也可能小于4MB,因为有的数据块可能小于4KB)
				break;
			
		}
		out_data.close();
		i++;

		if (is_dedup_over)                //判断列表中是否遍历完了
			break;
	}
	full_fp.close();
	out_chunk_in_container.close();
}


void write_file_data(string main_restore_path) {
	cout << "start writing container..." << endl;
	start_write_file_data(main_restore_path);
	cout << "finish writing container..." << endl << endl;
}