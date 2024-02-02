#pragma once
#include <string>
#include <unordered_set>
namespace ModuleInfo {
	enum ModuleInfoType {
		MIT_EMPTY = 0,		//��������Ϣ
		MIT_FULL = 1,		//ȫ����Ϣ
		MIT_VERSION = 2,	//�汾��Ϣ
		MIT_MODIFY = 4,		//�޸�ʱ��
		MIT_BUILD = 8,		//����ʱ��.��Ҫ��ģ��VSʹ��ChangeResourceVersion.exe��ӱ���ʱ��
							//ʾ����ProductName ==> GetModul.exe((2024-02-01 11:05:32))
	};

	std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName);
	std::string GetModuleLastWriteTime(const std::string& moduleName);
	std::string GetModuleInfo(const std::string& moduleName, size_t infoType = MIT_EMPTY);
	std::string GetModuleName(const std::string& target);
	std::unordered_set<std::string> GetModuleNames();
	std::unordered_set<std::string> GetModuleInfo(size_t infoType = MIT_EMPTY);
	bool CheckInfoType(size_t infoType, size_t standard);
}