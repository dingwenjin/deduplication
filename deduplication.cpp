#include "windows.h"
#include "deduplication.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iterator>
#include <time.h>

#include "read_phrase.h"
#include "chunk_phrase.h"
#include "compute_fingerprint.h"
#include "deduplication_phrase.h"
#include "write_file.h"


#include "restore_file_info.h"
#include "find_restore_file_directory.h"
#include "restore_file.h"



using namespace std;

//*******************************************Ŀ¼����************************************************

extern unsigned __int64 count_original;
extern unsigned __int64 count_dedup;


void recusive_file(const char* lpPath, vector<string> &fileList) {
	char szFind[PATH_SIZE];
	WIN32_FIND_DATA FindFileData;
	string path;

	strcpy_s(szFind, lpPath);
	strcat_s(szFind, "\\*.*");

	HANDLE hFind = FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (true) {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (FindFileData.cFileName[0] != '.') {
				char szFile[PATH_SIZE];
				strcpy_s(szFile, lpPath);
				strcat_s(szFile, "\\");
				strcat_s(szFile, (char*)(FindFileData.cFileName));
				recusive_file(szFile, fileList);
			}
		}
		else {
			string path = string(lpPath) + "\\" + string(FindFileData.cFileName);
			fileList.push_back(path);
		}
		if (!FindNextFile(hFind, &FindFileData))
			break;
	}
	FindClose(hFind);
}


void do_backup(string main_path,string main_restore_path) {
	cout << "backup file: " << endl;
	clock_t start, end,start_hash,end_hash;
	start = clock();
	string dir_path = main_path + "\\" + "test";                    //��Ҫ���ݵ��ļ���
	vector<string> v;
	recusive_file(dir_path.c_str(), v);            //����Ҫ���ݵ�Ŀ¼�ļ�

	cout << "start reading data flow..." << endl;
	read_file(v);
	v.clear();                                //�ͷ���������Ŀռ�
	cout << "start chunking..." << endl;
	
	fixed_chunking();
	
	start_hash = clock();
	compute_fp();
	end_hash = clock();
	cout << "compute fingerprint time is: " << double(end_hash - start_hash) << endl;
	
	data_deduplication();
	
	cout << "start backup..." << endl;
	//string main_restore_path = "G:\\restore";                    //�ָ�·��
	//write_fp_recipe(main_restore_path);
	write_file_info(main_restore_path);
	write_file_data(main_restore_path);
	cout << "finish backup..." << endl<<endl;
	cout << "-----------------------------------------------------------" << endl;
	end = clock();
	double duration = double(end - start);
	cout << "deduplication time is: " << duration << endl;
	cout << "total chunks: "<<count_original << "\t" <<"new chunks: "<< count_dedup << endl;
	cout << "deduplication ratio: " << double(count_original - count_dedup) / double(count_original) << endl;
	cout << "-----------------------------------------------------------" << endl<<endl;
	
	
}


void do_restore(string main_path,string main_restore_path) {
	cout << "restore file: " << endl;
	clock_t start, end,start_tmp,end_tmp;
	start = clock();
	vector<string> backup_container_list, backup_metadata_list;
	string backup_container_dir = main_restore_path + "\\" + "backup";
	string backup_metadata_dir = main_restore_path + "\\" + "metadata";
	recusive_file(backup_container_dir.c_str(), backup_container_list);           //����container·��������
	//copy(backup_container_list.begin(), backup_container_list.end(), ostream_iterator<string>(cout, "\n"));

	recusive_file(backup_metadata_dir.c_str(), backup_metadata_list);          //����metadata��·��������

	cout << "start reading file recipe..." << endl;
	read_file_info(backup_metadata_list);

	//string main_restore_path = "G:\\restore";                    //�ָ�·��
	find_restore_directory(main_restore_path, main_path);
	cout << "finish reading..." << endl;


	cout << "start restore..." << endl;
	start_tmp = clock();
	restore_original_data(backup_container_list);
	end_tmp = clock();
	cout << "restore file time is: " << double(end_tmp - start_tmp) << endl;
	cout << "finish restore..." << endl << endl;;
	//restore_original_data(backup_container_list);
	cout << "**************************************************************" << endl;
	end = clock();
	double duration = double(end - start);
	cout << "restore time is: " << duration << endl;
}


int main() {
	cout << "-------------------Welcome to canglong's deduplication system---------------------" << endl<<endl;
	string main_path = "G:\\test_file\\dwj";         //��Ҫ���ݵ�Ŀ¼����һ��Ŀ¼
	string main_restore_path = "G:\\restore";                    //�ָ�·��
	do_backup(main_path, main_restore_path);
	do_restore(main_path, main_restore_path);

	system("pause");
	return 0;
}