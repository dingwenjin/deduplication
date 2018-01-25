#include "read_phrase.h"

list<data_flow*> d_flow;
list<file_info*> file_information;
//deque<string> file_list;

mutex m_read;
condition_variable read_empty, read_full;
bool is_file_read_over = false;                     //用于检测文件夹的文件是否已经读取完毕

void find_all_file(string path) {                          //文件搜索

    if (path[path.size() - 1] != '\\') {
        path = path + "\\";
    }

    WIN32_FIND_DATA fileinfo;
    string search_path = path + "*.*";
    HANDLE findend = FindFirstFile(search_path.c_str(), &fileinfo);

    if (findend != INVALID_HANDLE_VALUE) {
        do {
            if (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (string(fileinfo.cFileName) != "." && 
                    string(fileinfo.cFileName) != "..") {
                    find_all_file(path + string(fileinfo.cFileName));
                }
            }
            else {
				start_read_file(path + string(fileinfo.cFileName));
            }

        } while (FindNextFile(findend, &fileinfo));
    }
   // else {
	//	is_file_read_over = true;
        //return -1;
   // }
    FindClose(findend);
	//cout << "-----------------------------read file over----------------------------------" << endl;
	//is_file_read_over = true;           //不能在这里设置标志位，因为递归返回时，后面分块开始首先会结束，导致有的文件没有分块就退出了
    //return 1;
}


void start_read_file(string path) {                    //文件路径列表
	ifstream in(path.c_str(),ios::binary);
	int total_singal_file_size=0;
	char* global_buf = new char[READ_BUF];

	//m_read.lock();                       //每次读取一个文件
	unique_lock<mutex> lk_read(m_read);
	read_empty.wait(lk_read, []() {return d_flow.size() != QUEUE_SIZE; });

	while (in.read(global_buf, READ_BUF)) {
			
		total_singal_file_size += READ_BUF;                    //统计文件的字节数
		data_flow* d_f = new data_flow;                   //申明结构体变量
	
		d_f->data.assign(global_buf, READ_BUF);
		d_f->size = READ_BUF;
		d_f->chunk_fp = TEMPORARY_FP;
		d_f->container_ID = CONTAINER_ID;
		d_f->flag = UNIQUE_CHUNK;                              //这里0表示新块

		d_flow.push_back(d_f);

	}
	if (in.gcount() > 0) {                      //记录读取没有1MB的情况

		total_singal_file_size += in.gcount();
		data_flow* d_f = new data_flow;

		d_f->data.assign(global_buf, in.gcount());
		d_f->size = in.gcount();
		d_f->chunk_fp = TEMPORARY_FP;
		d_f->container_ID = CONTAINER_ID;
		d_f->flag = UNIQUE_CHUNK;                              //这里0表示新块
		d_flow.push_back(d_f);
			
	}

	//cout << "d_flow: " << d_flow.size() << endl;

	//m_read.unlock();
	read_full.notify_one();

	in.close();
	delete global_buf;

	file_info* f_i = new file_info;
	f_i->file_route = path;
	f_i->number_of_chunks = (total_singal_file_size%CHUNK_SIZE == 0) ? total_singal_file_size / CHUNK_SIZE : total_singal_file_size / CHUNK_SIZE + 1;
	file_information.push_back(f_i);

}


void write_file_info(string main_restore_path) {
	fstream out, in_metadata;

	string container_tmp_path = main_restore_path + "\\" + "metadata";                //这里是为了写容器的信息
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //尝试是否能打开
	if (!in_metadata)                //如果目录不存在则创建它
		CreateDirectory(container_tmp_path.c_str(), NULL);        //创建目录
	else                              //存在则关闭
		in_metadata.close();

	string path1 = container_tmp_path + "\\" + "file_information";         //数据块指纹
	out.open(path1.c_str(), ios::app | ios::binary);

	while (true) {
		if (file_information.empty())
			break;
		file_info* f_i = file_information.front();
		out << f_i->file_route << "|" << f_i->number_of_chunks << endl;         //这里放两个空格是为了防止文件夹名字之间有空格导致路径不正确

		delete f_i;
		file_information.pop_front();

	}
	file_information.clear();
	out.close();
}


void read_file(string path,string main_restore_path) {
	//cout << "start reading data flow..." << endl;
	find_all_file(path);                    
	is_file_read_over = true;                        //所有文件递归检索完成，则设置文件结束标志
	//cout << "---------------------start writing file route-------------------------" << endl;
	//cout << "finish reading data flow..." << endl<<endl;
	write_file_info(main_restore_path);
}