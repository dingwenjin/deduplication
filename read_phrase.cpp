#include "read_phrase.h"

list<data_flow*> d_flow;
list<file_info*> file_information;
//deque<string> file_list;

mutex m_read;
condition_variable read_empty, read_full;
bool is_file_read_over = false;                     //���ڼ���ļ��е��ļ��Ƿ��Ѿ���ȡ���

void find_all_file(string path) {                          //�ļ�����

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
	//is_file_read_over = true;           //�������������ñ�־λ����Ϊ�ݹ鷵��ʱ������ֿ鿪ʼ���Ȼ�����������е��ļ�û�зֿ���˳���
    //return 1;
}


void start_read_file(string path) {                    //�ļ�·���б�
	ifstream in(path.c_str(),ios::binary);
	int total_singal_file_size=0;
	char* global_buf = new char[READ_BUF];

	//m_read.lock();                       //ÿ�ζ�ȡһ���ļ�
	unique_lock<mutex> lk_read(m_read);
	read_empty.wait(lk_read, []() {return d_flow.size() != QUEUE_SIZE; });

	while (in.read(global_buf, READ_BUF)) {
			
		total_singal_file_size += READ_BUF;                    //ͳ���ļ����ֽ���
		data_flow* d_f = new data_flow;                   //�����ṹ�����
	
		d_f->data.assign(global_buf, READ_BUF);
		d_f->size = READ_BUF;
		d_f->chunk_fp = TEMPORARY_FP;
		d_f->container_ID = CONTAINER_ID;
		d_f->flag = UNIQUE_CHUNK;                              //����0��ʾ�¿�

		d_flow.push_back(d_f);

	}
	if (in.gcount() > 0) {                      //��¼��ȡû��1MB�����

		total_singal_file_size += in.gcount();
		data_flow* d_f = new data_flow;

		d_f->data.assign(global_buf, in.gcount());
		d_f->size = in.gcount();
		d_f->chunk_fp = TEMPORARY_FP;
		d_f->container_ID = CONTAINER_ID;
		d_f->flag = UNIQUE_CHUNK;                              //����0��ʾ�¿�
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

	string container_tmp_path = main_restore_path + "\\" + "metadata";                //������Ϊ��д��������Ϣ
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_metadata)                //���Ŀ¼�������򴴽���
		CreateDirectory(container_tmp_path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_metadata.close();

	string path1 = container_tmp_path + "\\" + "file_information";         //���ݿ�ָ��
	out.open(path1.c_str(), ios::app | ios::binary);

	while (true) {
		if (file_information.empty())
			break;
		file_info* f_i = file_information.front();
		out << f_i->file_route << "|" << f_i->number_of_chunks << endl;         //����������ո���Ϊ�˷�ֹ�ļ�������֮���пո���·������ȷ

		delete f_i;
		file_information.pop_front();

	}
	file_information.clear();
	out.close();
}


void read_file(string path,string main_restore_path) {
	//cout << "start reading data flow..." << endl;
	find_all_file(path);                    
	is_file_read_over = true;                        //�����ļ��ݹ������ɣ��������ļ�������־
	//cout << "---------------------start writing file route-------------------------" << endl;
	//cout << "finish reading data flow..." << endl<<endl;
	write_file_info(main_restore_path);
}