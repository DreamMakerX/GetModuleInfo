#include "GetModuleInfo.h"
#include <windows.h>
#include <Psapi.h>
#include <cmath>

#pragma comment(lib, "Version.lib")
namespace ModuleInfo {
    std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName)
    {
        /*fileName可选值：
        Comments: 文件的附加信息或注释。
        CompanyName: 开发公司的名称。
        FileDescription: 文件的描述。
        FileVersion: 文件版本号。
        InternalName: 内部名称，通常是开发时使用的名称。
        LegalCopyright: 版权信息。
        LegalTrademarks: 商标信息。
        OriginalFilename: 文件的原始名称。
        ProductName: 产品的名称。
        ProductVersion: 产品的版本号。
        PrivateBuild: 标识这个文件是一个私有版本。
        SpecialBuild: 对于特殊构建的说明。
        */
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
    std::string GetModuleLastWriteTime(const std::string& moduleName) {
        HMODULE hModule = GetModuleHandleA(moduleName.c_str());
        if (hModule == NULL) {
            return "Module handle not found.";
        }

        char modulePath[MAX_PATH];
        if (GetModuleFileNameA(hModule, modulePath, MAX_PATH) == 0) {
            return "Failed to get module file name.";
        }

        HANDLE hFile = CreateFileA(modulePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return "Failed to open module file.";
        }

        FILETIME ftCreate, ftAccess, ftWrite;
        if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
            CloseHandle(hFile);
            return "Failed to get file time.";
        }

        CloseHandle(hFile);

        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        char buffer[64] = { 0 };
        sprintf_s(buffer, 64, "%04d-%02d-%02d %02d:%02d:%02d",
            stLocal.wYear, stLocal.wMonth, stLocal.wDay,
            stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

        return std::string(buffer);
    }
    std::string GetModuleInfo(const std::string& moduleName, size_t infoType)
    {
        std::string result = moduleName;
        if (moduleName.empty() || CheckInfoType(infoType, MIT_EMPTY))
        {
            return result;
        }

        //获取模块名称、版本号信息
        std::string productName = GetFileSingleInfo(moduleName, "ProductName");
        if (CheckInfoType(infoType, MIT_VERSION))
        {
            std::string productVersion = GetFileSingleInfo(moduleName, "ProductVersion");
            result += " | v" + productVersion;
        }

        //获取修改时间
        if (CheckInfoType(infoType, MIT_MODIFY))
        {
            std::string modifyTime = GetModuleLastWriteTime(moduleName);
            if (!modifyTime.empty())
            {
                result += " | Modified At " + modifyTime; //修改时间
            }
        }

        //获取编译时间{productName: TakProcess C++ API Library((2024-01-25 10:54:11)) ==> 2024-01-25 10:54:11}
        //利用ChangeResourceVersion.exe，在VS配置“生成前事件”："$(ProjectDir)\" -NPF1 -PF3,即可在产品名称后添加编译时间
        if (CheckInfoType(infoType, MIT_BUILD))
        {
            std::string buildTime;
            auto start = productName.find("((");
            auto end = productName.find("))");
            if (start != std::string::npos && end != std::string::npos && start > 1 && end > start + 2)
            {
                buildTime = productName.substr(start + 2, end - start - 2);
            }

            if (!buildTime.empty())
            {
                result += " | Complied At " + buildTime;                          //编译时间
            }
        }

        return result;
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
    std::unordered_set<std::string> GetModuleInfo(size_t infoType)
    {
        std::unordered_set<std::string> result;

        //获取模块名称
        std::unordered_set<std::string>  moduleNames = GetModuleNames();
        if (moduleNames.empty())
        {
            return result;
        }
        for (auto moduleName : moduleNames)
        {
            result.insert(GetModuleInfo(moduleName, infoType));
        }

        return result;
    }
    bool CheckInfoType(size_t infoType, size_t standard)
    {
        if (standard == MIT_EMPTY)
        {
            return infoType == MIT_EMPTY;
        }
        if ((infoType & (1 << static_cast<size_t>(log2(standard)))) == standard || infoType == MIT_FULL)
        {
            return true;
        }
        return false;
    }
}