#include <iostream>
#include "GetModuleInfo.h"

using namespace ModuleInfo;

void PrintSeparator()
{
    std::cout << "===================================================================================================" << std::endl;

}
void PrintMessage(const std::unordered_set<std::string>& infos)
{
    for (auto info : infos)
    {
        std::cout << info << std::endl;
    }
}
void main()
{
    //��ȡ��ǰ���̵�������Щģ��
    PrintSeparator();
    std::cout << "��ȡ��ǰ���̵���ģ�飺" << std::endl;
    PrintMessage(GetModuleInfo());
    

    //��ȡ��ǰ���̵���ģ��������Ϣ
    PrintSeparator();
    std::cout << "��ȡ��ǰ���̵���ģ��������Ϣ��" << std::endl;
    PrintMessage(GetModuleInfo(MIT_FULL));

    //��ȡ��ǰ���̵���ģ���ض���Ϣ
    PrintSeparator();
    std::cout << "��ȡ��ǰ���̵���ģ���ض���Ϣ��" << std::endl;
    PrintMessage(GetModuleInfo(MIT_VERSION | MIT_BUILD));

    //��ȡ����ģ����Ϣ
    PrintSeparator();
    std::cout << "��ȡ����ģ����Ϣ��" << std::endl;
    std::cout << GetModuleInfo(GetModuleName("ntdll"), MIT_FULL) << std::endl;
    //std::cout << GetModuleInfo("ntdll.dll", MIT_FULL) << std::endl;

    std::cin.get();
}