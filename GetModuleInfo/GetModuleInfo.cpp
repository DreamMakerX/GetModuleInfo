#include "GetModuleInfo.h"
#include <windows.h>
#include <Psapi.h>
#include <cmath>

#pragma comment(lib, "Version.lib")
namespace ModuleInfo {
    std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName)
    {
        /*fileName��ѡֵ��
        Comments: �ļ��ĸ�����Ϣ��ע�͡�
        CompanyName: ������˾�����ơ�
        FileDescription: �ļ���������
        FileVersion: �ļ��汾�š�
        InternalName: �ڲ����ƣ�ͨ���ǿ���ʱʹ�õ����ơ�
        LegalCopyright: ��Ȩ��Ϣ��
        LegalTrademarks: �̱���Ϣ��
        OriginalFilename: �ļ���ԭʼ���ơ�
        ProductName: ��Ʒ�����ơ�
        ProductVersion: ��Ʒ�İ汾�š�
        PrivateBuild: ��ʶ����ļ���һ��˽�а汾��
        SpecialBuild: �������⹹����˵����
        */
        // ��ȡ�ļ��汾��Ϣ��С
        DWORD dwHandle;
        DWORD dwSize = GetFileVersionInfoSizeA(fileName.c_str(), &dwHandle);
        if (dwSize == 0) {
            return "";
        }

        // ����ռ�洢�ļ��汾��Ϣ
        std::vector<BYTE> buffer(dwSize);

        // ��ȡ�ļ��汾��Ϣ
        if (!GetFileVersionInfoA(fileName.c_str(), dwHandle, dwSize, buffer.data())) {
            return "";
        }

        // ��ѯ���õ����Ժʹ���ҳ
        UINT uiLen;
        LPVOID lpData;
        if (VerQueryValueA(buffer.data(), "\\VarFileInfo\\Translation", &lpData, &uiLen)) {
            DWORD* pLang = reinterpret_cast<DWORD*>(lpData);
            int numTranslations = uiLen / sizeof(DWORD);

            for (int i = 0; i < numTranslations; ++i) {
                // ������ѯ·��
                char queryPath[256];
                sprintf_s(queryPath, ("\\StringFileInfo\\%04x%04x\\" + attrName).c_str(), LOWORD(pLang[i]), HIWORD(pLang[i]));

                // ��ѯ��Ʒ����
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

        //��ȡģ�����ơ��汾����Ϣ
        std::string productName = GetFileSingleInfo(moduleName, "ProductName");
        if (CheckInfoType(infoType, MIT_VERSION))
        {
            std::string productVersion = GetFileSingleInfo(moduleName, "ProductVersion");
            result += " | v" + productVersion;
        }

        //��ȡ�޸�ʱ��
        if (CheckInfoType(infoType, MIT_MODIFY))
        {
            std::string modifyTime = GetModuleLastWriteTime(moduleName);
            if (!modifyTime.empty())
            {
                result += " | Modified At " + modifyTime; //�޸�ʱ��
            }
        }

        //��ȡ����ʱ��{productName: TakProcess C++ API Library((2024-01-25 10:54:11)) ==> 2024-01-25 10:54:11}
        //����ChangeResourceVersion.exe����VS���á�����ǰ�¼�����"$(ProjectDir)\" -NPF1 -PF3,�����ڲ�Ʒ���ƺ���ӱ���ʱ��
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
                result += " | Complied At " + buildTime;                          //����ʱ��
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

        // ��ȡ��ǰ���̵�ģ�����б�
        if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded))
        {
            // ����ģ������
            int moduleCount = cbNeeded / sizeof(HMODULE);

            // ����ÿ��ģ�鲢����ģ������
            for (int i = 0; i < moduleCount; ++i)
            {
                char moduleName[MAX_PATH];

                // ��ȡģ��Ļ�������
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

        //��ȡģ������
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