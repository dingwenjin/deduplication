#include "find_restore_file_directory.h"
#include <direct.h> //_mkdir函数的头文件
#include <io.h>     //_access函数的头文件

extern list<file_info*> file_information;


string CreateDir(const char *dir) {       //递归创建目录
	int m = 0, n;
	string str1, str2, str;

	str1 = dir;
	str2 = str1.substr(0, 2);
	str1 = str1.substr(3, str1.size());

	while (m >= 0) {
		m = str1.find('\\');

		str2 += '\\' + str1.substr(0, m);
		n = _access(str2.c_str(), 0); //判断该目录是否存在
		if (n == -1)
			_mkdir(str2.c_str());     //创建目录
		str1 = str1.substr(m + 1, str1.size());
	}
	str = dir;
	return str;
}


string cut_file_name(string s) {
	int end = s.find_last_of("\\");
	return s.substr(0, end);
}


void find_restore_directory(string main_restore_path,string main_path) {

	list<file_info*>::iterator it = file_information.begin();
	string s,main_backup_route,full_restore_route,full_restore_dir;
	while (it != file_information.end()) {
		s = (*it)->file_route;
		main_backup_route=s.substr(main_path.size(),s.size());         //截取要备份的文件的那一部分路径
		full_restore_route = main_restore_path + main_backup_route;       //这条路径包括了文件名
		(*it)->file_route = full_restore_route;               //将最新的恢复路径赋值给文件信息结构体中的文件路径
		full_restore_dir=cut_file_name(full_restore_route);     //去掉文件名，也就是为了创建文件名之前的所有名录
	
		CreateDir(full_restore_dir.c_str());          //递归创建恢复的文件路径目录
		it++;
	}
}
