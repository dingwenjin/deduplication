#include "read_phrase.h"
#include "chunk_phrase.h"
#include "compute_fingerprint.h"
#include "deduplication_phrase.h"
#include "write_file.h"

#include "restore_file_info.h"
#include "find_restore_file_directory.h"
#include "restore_file.h"


//*******************************************目录处理************************************************

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
			if (string(FindFileData.cFileName) != "." && string(FindFileData.cFileName) != "..") {
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
	cout << "start backup file: " << endl;
	clock_t start, end;
	string path = main_path + "\\" + "ding";                              //需要备份文件夹
	start = clock();

	thread t1(read_file, path, main_restore_path);
	thread t2(fixed_chunking);
	thread t3(compute_fp);
	thread t4(data_deduplication);
	thread t5(write_file_data, main_restore_path);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	/*
	t2.join();
	t3.join();	
	t4.join();
	t5.join();
	t1.join();
	*/
	//read_file(path, main_restore_path);
	//fixed_chunking();
	//compute_fp();
	//data_deduplication();
	//write_file_data(main_restore_path);

	end = clock();
	double duration = double(end - start);
	
	
	cout << "-----------------------------------------------------------" << endl;
	cout << "total chunks: "<<count_original << "\t" <<"new chunks: "<< count_dedup << endl;
	cout << "deduplication ratio: " << double(count_original - count_dedup) / double(count_original) << endl;
	cout << "-----------------------------------------------------------" << endl<<endl;
	
	cout << "finish backup file..." << endl;
	cout << "-----------------------------------------------------------" << endl;
	cout << "total backup time is: " << duration<<endl<<endl;
	
}


void do_restore(string main_path,string main_restore_path) {
	cout << "start restore file: " << endl;
	clock_t start, end;
	start = clock();
	vector<string> backup_container_list, backup_metadata_list;
	string backup_container_dir = main_restore_path + "\\" + "backup";
	string backup_metadata_dir = main_restore_path + "\\" + "metadata";
	recusive_file(backup_container_dir.c_str(), backup_container_list);           //保存container路径的容器
	recusive_file(backup_metadata_dir.c_str(), backup_metadata_list);          //保存metadata的路径的容器

	
	//thread t1(read_file_info);
	//thread t2(find_restore_directory);
	//thread t3(restore_original_data);

	read_file_info(backup_metadata_list);
	find_restore_directory(main_restore_path, main_path);
	restore_original_data(backup_container_list);


	cout << "**************************************************************" << endl;
	end = clock();
	double duration = double(end - start);
	cout << "total restore time is: " << duration << endl;
}


int main() {
	cout << "-------------------Welcome to canglong's deduplication system---------------------" << endl<<endl;
	string main_path = "G:\\test_file\\dwj";         //需要备份的目录的上一级目录
	string main_restore_path = "G:\\restore";                    //恢复路径

	do_backup(main_path, main_restore_path);

	do_restore(main_path, main_restore_path);

	system("pause");
	return 0;
}