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
    //获取当前进程调用了哪些模块
    PrintSeparator();
    std::cout << "获取当前进程调用模块：" << std::endl;
    PrintMessage(GetModuleInfo());
    

    //获取当前进程调用模块所有信息
    PrintSeparator();
    std::cout << "获取当前进程调用模块所有信息：" << std::endl;
    PrintMessage(GetModuleInfo(MIT_FULL));

    //获取当前进程调用模块特定信息
    PrintSeparator();
    std::cout << "获取当前进程调用模块特定信息：" << std::endl;
    PrintMessage(GetModuleInfo(MIT_VERSION | MIT_BUILD));

    //获取单个模块信息
    PrintSeparator();
    std::cout << "获取单个模块信息：" << std::endl;
    std::cout << GetModuleInfo(GetModuleName("ntdll"), MIT_FULL) << std::endl;
    //std::cout << GetModuleInfo("ntdll.dll", MIT_FULL) << std::endl;

    std::cin.get();
}