#include "deduplication.h"
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include <unordered_map>
#include <iostream>

extern list<file_info*> file_information;
list<file_recipe*> fp_in_container;                       //所有数据块的指纹及它们所属的容器

unordered_map<string, tmp_file_recipe*> fp_metadata;


vector<string> string_split(string s) {
	int start = 0, end = 0;
	vector<string> v;
	while (s.size() != 0) {                   //将字符串按空格分割
		end = s.find("  ");                   //存在空格时返回空格在字符串的下标
		if (end != -1) {
			string s1 = s.substr(start, end);
			v.push_back(s1);
			s = s.substr(end + 1);
		}
		else {                            //最后一个子串直接放到容器
			v.push_back(s);
			break;
		}
	}
	return v;
}


void read_chunk_metadata(string chunk_in_container) {
	//cout << chunk_in_container << endl;
	ifstream in_1(chunk_in_container, ios::binary);
	string s_1,fp;
	vector<string> v_1;
	//ofstream out("G:\\restore\\test.txt", ios::app | ios::binary);
	while (getline(in_1, s_1)) {
		v_1 = string_split(s_1);
		//copy(v_1.begin(),v_1.end(),ostream_iterator<string>(out," "));
		//out << endl;
		tmp_file_recipe* t_f_r = new tmp_file_recipe();
		fp = v_1[0];
		t_f_r->container_ID = atoi(v_1[1].c_str());
		t_f_r->offset = atoi(v_1[2].c_str());
		t_f_r->size = atoi(v_1[3].c_str());
		//out << v_1[2] << " " << v_1[3] << endl;
		fp_metadata.insert(unordered_map<string, tmp_file_recipe*>::value_type(fp, t_f_r));
	}
	//out.close();
	v_1.clear();
	in_1.close();
}

void read_file_metadata(string file_metadata) {
	ifstream in_2(file_metadata, ios::binary);
	vector<string> v_2;
	string s_2;
	while (getline(in_2, s_2)) {
		v_2 = string_split(s_2);
		file_info* f_i = new file_info();
		f_i->file_route = v_2[0];
		f_i->number_of_chunk = atoi(v_2[1].c_str());
		f_i->file_size = atoi(v_2[2].c_str());
		file_information.push_back(f_i);
	}

	v_2.clear();
	in_2.close();
}


void read_chunk_fingerprint(string chunk_fingerprint) {
	ifstream in_3(chunk_fingerprint, ios::binary);
	string fp;
	tmp_file_recipe* t_f_r;
	//ofstream out("G:\\restore\\test.txt", ios::app | ios::binary);
	while (getline(in_3, fp)) {
		if (fp_metadata.find(fp) != fp_metadata.end()) {          //查看数据流的指纹是否在去重的指纹中
			file_recipe* f_r = new file_recipe();
			t_f_r = fp_metadata[fp];
			f_r->chunk_fp = fp;
			f_r->container_ID = t_f_r->container_ID;
			f_r->offset = t_f_r->offset;

			f_r->size = t_f_r->size;
			//out << t_f_r->size<<" "<< t_f_r->offset << endl;
			fp_in_container.push_back(f_r);
		}
	}
	//out.close();
	in_3.close();
}


void clear_tmp_file_recipe() {
	unordered_map<string, tmp_file_recipe*>::iterator it = fp_metadata.begin();
	while (it != fp_metadata.end()) {
		tmp_file_recipe* p = it->second;        //将值(这里用的结构体)的空间释放
		delete p;
		fp_metadata.erase(it++);
	}
}



void read_file_info(vector<string> backup_metadata_info) {
	string chunk_in_container= backup_metadata_info[0],file_metadata = backup_metadata_info[1],chunk_fingerprint= backup_metadata_info[2];
	
	read_chunk_metadata(chunk_in_container);

	read_file_metadata(file_metadata);

	read_chunk_fingerprint(chunk_fingerprint);

	clear_tmp_file_recipe();
}