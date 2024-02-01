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
std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName)
{
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

    //��ȡģ������
    std::string moduleName = GetModuleName(target);
    if (moduleName.empty())
    {
        return "";
    }

    //��ȡģ�����ơ��汾����Ϣ
    std::string result;
    std::string productName = GetFileSingleInfo(moduleName, "ProductName");
    std::string productVersion = GetFileSingleInfo(moduleName, "ProductVersion");
    result = moduleName + " v" + productVersion;

    //��ȡ����ʱ��{productName: TakProcess C++ API Library((2024-01-25 10:54:11)) ==> 2024-01-25 10:54:11}
    //����ChangeResourceVersion.exe����VS���á�����ǰ�¼�����"$(ProjectDir)\" -NPF1 -PF3,�����ڲ�Ʒ���ƺ���ӱ���ʱ��
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
    //��ȡ��ǰ���̵�������Щģ��
    std::cout << "��ȡ��ǰ���̵���ģ�飺" << std::endl;
    std::unordered_set<std::string> modules = GetModuleNames();
    for (auto module : modules)
    {
        std::cout << module << std::endl;
    }

    //��ȡ�ض�ģ����Ϣ
    std::cout << "��ȡ�ض�ģ����Ϣ��" << std::endl;
    std::string info = GetModuleInfo("GetModuleInfo.exe");
    info += " | " + GetModuleInfo("VERSION.dll");
    std::cout << info << std::endl;

    std::cin.get();
}