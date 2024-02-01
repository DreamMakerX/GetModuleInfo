#include <string>
#include <iostream>
#include <windows.h>
#include <Psapi.h>
#include <unordered_set>

#pragma comment(lib, "Version.lib")

std::unordered_set<std::string> GetModuleNames()
{
    std::unordered_set<std::string> moduleNames;

    HMODULE hModules[1024];
    DWORD cbNeeded;

    // 获取当前进程的模块句柄列表
    if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded))
    {
        // 计算模块数量
        int moduleCount = cbNeeded / sizeof(HMODULE);

        // 遍历每个模块并保存模块名称
        for (int i = 0; i < moduleCount; ++i)
        {
            char moduleName[MAX_PATH];

            // 获取模块的基本名称
            if (GetModuleBaseNameA(GetCurrentProcess(), hModules[i], moduleName, sizeof(moduleName)))
            {
                moduleNames.insert(moduleName);
            }
        }
    }

    return moduleNames;
}
std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName)
{
    // 获取文件版本信息大小
    DWORD dwHandle;
    DWORD dwSize = GetFileVersionInfoSizeA(fileName.c_str(), &dwHandle);
    if (dwSize == 0) {
        return "";
    }

    // 申请空间存储文件版本信息
    std::vector<BYTE> buffer(dwSize);

    // 读取文件版本信息
    if (!GetFileVersionInfoA(fileName.c_str(), dwHandle, dwSize, buffer.data())) {
        return "";
    }

    // 查询可用的语言和代码页
    UINT uiLen;
    LPVOID lpData;
    if (VerQueryValueA(buffer.data(), "\\VarFileInfo\\Translation", &lpData, &uiLen)) {
        DWORD* pLang = reinterpret_cast<DWORD*>(lpData);
        int numTranslations = uiLen / sizeof(DWORD);

        for (int i = 0; i < numTranslations; ++i) {
            // 构建查询路径
            char queryPath[256];
            sprintf_s(queryPath, ("\\StringFileInfo\\%04x%04x\\" + attrName).c_str(), LOWORD(pLang[i]), HIWORD(pLang[i]));

            // 查询产品名称
            LPVOID lpProductName;
            UINT uiProductNameLen;
            if (VerQueryValueA(buffer.data(), queryPath, &lpProductName, &uiProductNameLen)) {
                return static_cast<char*>(lpProductName);
            }
        }
    }

    return "";
}
std::string GetModuleName(const std::string& target)
{
    //TakProcess_CPlus ==> TakProcess_CPlus_X86.dll
    if (target.empty())
    {
        return "";
    }
    std::unordered_set<std::string> moduleNames = GetModuleNames();
    for (auto it = moduleNames.begin(); it != moduleNames.end(); ++it)
    {
        if ((*it).find(target) != std::string::npos)
        {
            return *it;
        }
    }
    return "";
}
std::string GetModuleInfo(const std::string& target)
{
    if (target.empty())
    {
        return "";
    }

    //获取模块名称
    std::string moduleName = GetModuleName(target);
    if (moduleName.empty())
    {
        return "";
    }

    //获取模块名称、版本号信息
    std::string result;
    std::string productName = GetFileSingleInfo(moduleName, "ProductName");
    std::string productVersion = GetFileSingleInfo(moduleName, "ProductVersion");
    result = moduleName + " v" + productVersion;

    //获取编译时间{productName: TakProcess C++ API Library((2024-01-25 10:54:11)) ==> 2024-01-25 10:54:11}
    //利用ChangeResourceVersion.exe，在VS配置“生成前事件”："$(ProjectDir)\" -NPF1 -PF3,即可在产品名称后添加编译时间
    std::string buildTime;
    auto start = productName.find("((");
    auto end = productName.find("))");
    if (start != std::string::npos && end != std::string::npos && start > 1 && end > start + 2)
    {
        buildTime = productName.substr(start + 2, end - start - 2);
    }
    if (!buildTime.empty())
    {
        result += " Complied At " + buildTime;
    }

    return result;
}

void main()
{
    //获取当前进程调用了哪些模块
    std::cout << "获取当前进程调用模块：" << std::endl;
    std::unordered_set<std::string> modules = GetModuleNames();
    for (auto module : modules)
    {
        std::cout << module << std::endl;
    }

    //获取特定模块信息
    std::cout << "获取特定模块信息：" << std::endl;
    std::string info = GetModuleInfo("GetModuleInfo.exe");
    info += " | " + GetModuleInfo("VERSION.dll");
    std::cout << info << std::endl;

    std::cin.get();
}