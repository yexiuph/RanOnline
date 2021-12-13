#pragma once

#include <Wincrypt.h> // Crypt API, Advapi32.lib

// Auto Link Library Files
#pragma comment (lib, "Advapi32.lib")

namespace SERVER_UTIL
{	
	/**
	 * \ingroup NetServerLib
	 *
	 * \par requirements
	 * win98 or later\n
	 * win2k or later\n
	 * MFC\n
	 *
	 * \version 1.0
	 * first version
	 *
	 * \date 2005-07-04
	 *
	 * \author Jgkim
	 *
	 * \par license
	 * Copyright(c) Mincoms. All rights reserved.
	 * 
	 * \todo 
	 *
	 * \bug 
	 *
	 */
	class CMinMd5
	{
	public:
		/**
		*
		* \param void 
		* \return 
		*/
		CMinMd5(void);

		/**
		*
		* \param void 
		* \return 
		*/
		~CMinMd5(void);

		/**
		* MD5 �� �����ش�
		* \param strSource MD5 ���� ���� ����
		* \param strTarget MD5 ���� ���ϵǴ� ����
		* \return true:���� false:����
		*/
		bool GetMd5(CString& strSource, CString& strTarget);
	    
		/**
		* Excite Japan ���� ����ϴ� ��ȣŰ�� �����Ѵ�.
		* \param szSecretKey ������ ��ȣŰ
		*/
		void ExciteSetSecretKey(const char* szSecretKey);
		
		/**
		* Excite Japan ���� ����ϴ� UserID �� LoginTime �� �޾Ƽ� MD5 �� ���Ѵ�.
		* \param szU UserID
		* \param szT Login Time
		* \param szMd5 Client ���� �Ѿ�� MD5 ��
		* \return true:���� false:����
		*/
		bool ExciteGetMd5(const char* szU,
						const char* szT,
						const char* szMd5);
		/**
		* Excite Japan ���� �Ѿ���� LoginTime �� ����ε� ���������� Ȯ���Ѵ�.
		* \param strDate LoginTime ����:20050303020434 ����Ͻú���
		* \return true:���� false:����
		*/
		bool ExciteCheckDate(CString& strDate);

	protected:
		/**
		* �ʱ�ȭ
		* \return 
		*/
		bool Init();	

	protected:
		HCRYPTPROV m_hCryptProv;	
		DWORD m_dwHashLen;
		CString m_strSecretKey;	
	};
}