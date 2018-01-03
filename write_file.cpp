#include "write_file.h"

extern list<data_flow*> dedup_list;     //����ȫ�ֶ��б���ԭʼ����


string int_to_string(int n) {
	ostringstream stream;
	stream << n;
	return stream.str();
}

void start_write_file_data(string main_restore_path) {

	fstream out_data, out_chunk_in_container, in_data, in_metadata,full_fp;
	string path_data = main_restore_path + "\\" + "backup";                    //������Ϊ��д��������
	in_data.open(path_data.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_data)                                      //���Ŀ¼�������򴴽���
		CreateDirectory(path_data.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_data.close();

	string container_tmp_path = main_restore_path + "\\" + "metadata";                //������Ϊ��д��������Ϣ
	in_metadata.open(container_tmp_path.c_str(), ios::in);          //�����Ƿ��ܴ�
	if (!in_metadata)                //���Ŀ¼�������򴴽���
		CreateDirectory(container_tmp_path.c_str(), NULL);        //����Ŀ¼
	else                              //������ر�
		in_metadata.close();

	string container_path = container_tmp_path + "\\" + "chunk_in_container";     //���ݿ����ڵ���������ƫ�ƺʹ�С
	out_chunk_in_container.open(container_path.c_str(),ios::app|ios::binary);

	string fp_path = container_tmp_path + "\\" + "full_fingerprint";     //��¼�������ݿ��ָ��
	full_fp.open(fp_path.c_str(), ios::app | ios::binary);

	uint16_t i = 1;                         //��¼container��
	string str;

	bool is_dedup_over = false;                           //������Ҫ��Ϊ�˿������ѭ��

	while (true) {
		str = int_to_string(i);                 //i��������Ҫ�Ǽ�¼�ڼ���contianer
		string path_url = path_data + "\\" + "container" + str;         //�����ļ�
		out_data.open(path_url.c_str(),ios::app|ios::binary);          //������Ϊ��д�벻ͬ��container
		int offset = 0,count=0;

		while (true) {
			if (dedup_list.empty()) {
				is_dedup_over = true;
				break;
			}
			data_flow* d_f = dedup_list.front();
			full_fp << d_f->chunk_fp << endl;                  //д�������ݿ��ָ��
			if (d_f->flag == UNIQUE_CHUNK) {                           //д���ظ����ݿ������
				out_data.write(d_f->data.c_str(), d_f->size);
				out_chunk_in_container << d_f->chunk_fp << "|" << d_f->container_ID << "|" << offset << "|" << d_f->size << endl;
				offset += d_f->size;

				count++;                               //��Ҫ���ڼ�¼д���container�е����ݿ���Ŀ
			}
			
			delete d_f;
			dedup_list.pop_front();

			if (count % 1024 == 0)                 //4MBд����(�����ϣ�Ҳ����С��4MB,��Ϊ�е����ݿ����С��4KB)
				break;
			
		}
		out_data.close();
		i++;

		if (is_dedup_over)                //�ж��б����Ƿ��������
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