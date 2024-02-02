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

	/*
	 * @brief 获取模块以下属性信息
	 * 	Comments			附加信息或注释。
	 *  CompanyName			开发公司的名称。
	 *  FileDescription		描述。
	 *  FileVersion			版本号。
	 *  InternalName		内部名称，通常是开发时使用的名称。
	 *  LegalCopyright		版权信息。
	 *  LegalTrademarks		商标信息。
	 *  OriginalFilename	原始名称。
	 *  ProductName			产品名称。
	 *  ProductVersion		产品版本。
	 *  PrivateBuild		标识这个文件是一个私有版本。
	 *  SpecialBuild		对于特殊构建的说明。
	 * 
	 * @param fileName 模块名称
	 * @param attrName 属性名称
	 * @return 属性值
	 */
	std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName);

	/*
	 * @brief 获取文件修改时间
	 *
	 * @param fileName 文件名称
	 * @return 修改时间
	 */
	std::string GetModuleLastWriteTime(const std::string& fileName);

	/*
	 * @brief 获取模块信息
	 *
	 * @param moduleName 模块名称
	 * @param infoType 信息类型，参考ModuleInfoType，默认只输出模块名称
	 * @return 模块信息
	 */
	std::string GetModuleInfo(const std::string& moduleName, size_t infoType = MIT_EMPTY);
	
	/*
	 * @brief 获取模块名称
	 *
	 * @param 模块不完整的名称
	 * @return模块名称
	 */
	std::string GetModuleName(const std::string& target);
	
	/*
	 * @brief 获取当前进程调用的所有模块名称
	 *
	 * @return 所有模块名称
	 */
	std::unordered_set<std::string> GetModuleNames();
	
	/*
	 * @brief 获取当前进程所有模块信息
	 *
	 * @param infoType 信息类型，参考ModuleInfoType，默认只输出模块名称
	 * @return 当前进程所有模块信息
	 */
	std::unordered_set<std::string> GetModuleInfo(size_t infoType = MIT_EMPTY);
	
	/*
	 * @brief 判断信息类型是否符合要求
	 *
	 * @param infoType 传入的信息类型
	 * @param standard 判断依据
	 * @return 是否符合要求
	 */
	bool CheckInfoType(size_t infoType, size_t standard);
}