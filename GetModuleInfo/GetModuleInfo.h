#pragma once
#include <string>
#include <unordered_set>
namespace ModuleInfo {
	enum ModuleInfoType {
		MIT_EMPTY = 0,		//无其它信息
		MIT_FULL = 1,		//全部信息
		MIT_VERSION = 2,	//版本信息
		MIT_MODIFY = 4,		//修改时间
		MIT_BUILD = 8,		//编译时间.需要该模块VS使用ChangeResourceVersion.exe添加编译时间
							//示例：ProductName ==> GetModul.exe((2024-02-01 11:05:32))
	};

	std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName);
	std::string GetModuleLastWriteTime(const std::string& moduleName);
	std::string GetModuleInfo(const std::string& moduleName, size_t infoType = MIT_EMPTY);
	std::string GetModuleName(const std::string& target);
	std::unordered_set<std::string> GetModuleNames();
	std::unordered_set<std::string> GetModuleInfo(size_t infoType = MIT_EMPTY);
	bool CheckInfoType(size_t infoType, size_t standard);
}