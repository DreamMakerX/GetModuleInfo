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

	/*
	 * @brief ��ȡģ������������Ϣ
	 * 	Comments			������Ϣ��ע�͡�
	 *  CompanyName			������˾�����ơ�
	 *  FileDescription		������
	 *  FileVersion			�汾�š�
	 *  InternalName		�ڲ����ƣ�ͨ���ǿ���ʱʹ�õ����ơ�
	 *  LegalCopyright		��Ȩ��Ϣ��
	 *  LegalTrademarks		�̱���Ϣ��
	 *  OriginalFilename	ԭʼ���ơ�
	 *  ProductName			��Ʒ���ơ�
	 *  ProductVersion		��Ʒ�汾��
	 *  PrivateBuild		��ʶ����ļ���һ��˽�а汾��
	 *  SpecialBuild		�������⹹����˵����
	 * 
	 * @param fileName ģ������
	 * @param attrName ��������
	 * @return ����ֵ
	 */
	std::string GetFileSingleInfo(const std::string& fileName, const std::string& attrName);

	/*
	 * @brief ��ȡ�ļ��޸�ʱ��
	 *
	 * @param fileName �ļ�����
	 * @return �޸�ʱ��
	 */
	std::string GetModuleLastWriteTime(const std::string& fileName);

	/*
	 * @brief ��ȡģ����Ϣ
	 *
	 * @param moduleName ģ������
	 * @param infoType ��Ϣ���ͣ��ο�ModuleInfoType��Ĭ��ֻ���ģ������
	 * @return ģ����Ϣ
	 */
	std::string GetModuleInfo(const std::string& moduleName, size_t infoType = MIT_EMPTY);
	
	/*
	 * @brief ��ȡģ������
	 *
	 * @param ģ�鲻����������
	 * @returnģ������
	 */
	std::string GetModuleName(const std::string& target);
	
	/*
	 * @brief ��ȡ��ǰ���̵��õ�����ģ������
	 *
	 * @return ����ģ������
	 */
	std::unordered_set<std::string> GetModuleNames();
	
	/*
	 * @brief ��ȡ��ǰ��������ģ����Ϣ
	 *
	 * @param infoType ��Ϣ���ͣ��ο�ModuleInfoType��Ĭ��ֻ���ģ������
	 * @return ��ǰ��������ģ����Ϣ
	 */
	std::unordered_set<std::string> GetModuleInfo(size_t infoType = MIT_EMPTY);
	
	/*
	 * @brief �ж���Ϣ�����Ƿ����Ҫ��
	 *
	 * @param infoType �������Ϣ����
	 * @param standard �ж�����
	 * @return �Ƿ����Ҫ��
	 */
	bool CheckInfoType(size_t infoType, size_t standard);
}