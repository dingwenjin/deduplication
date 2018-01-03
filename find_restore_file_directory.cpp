#include "find_restore_file_directory.h"
#include <direct.h> //_mkdir������ͷ�ļ�
#include <io.h>     //_access������ͷ�ļ�

extern list<file_info*> file_information;


string CreateDir(const char *dir) {       //�ݹ鴴��Ŀ¼
	int m = 0, n;
	string str1, str2, str;

	str1 = dir;
	str2 = str1.substr(0, 2);
	str1 = str1.substr(3, str1.size());

	while (m >= 0) {
		m = str1.find('\\');

		str2 += '\\' + str1.substr(0, m);
		n = _access(str2.c_str(), 0); //�жϸ�Ŀ¼�Ƿ����
		if (n == -1)
			_mkdir(str2.c_str());     //����Ŀ¼
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
		main_backup_route=s.substr(main_path.size(),s.size());         //��ȡҪ���ݵ��ļ�����һ����·��
		full_restore_route = main_restore_path + main_backup_route;       //����·���������ļ���
		(*it)->file_route = full_restore_route;               //�����µĻָ�·����ֵ���ļ���Ϣ�ṹ���е��ļ�·��
		full_restore_dir=cut_file_name(full_restore_route);     //ȥ���ļ�����Ҳ����Ϊ�˴����ļ���֮ǰ��������¼
	
		CreateDir(full_restore_dir.c_str());          //�ݹ鴴���ָ����ļ�·��Ŀ¼
		it++;
	}
}
