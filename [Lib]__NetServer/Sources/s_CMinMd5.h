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
		* MD5 를 구해준다
		* \param strSource MD5 값을 구할 원본
		* \param strTarget MD5 값이 리턴되는 버퍼
		* \return true:성공 false:실패
		*/
		bool GetMd5(CString& strSource, CString& strTarget);
	    
		/**
		* Excite Japan 에서 사용하는 암호키를 설정한다.
		* \param szSecretKey 설정할 암호키
		*/
		void ExciteSetSecretKey(const char* szSecretKey);
		
		/**
		* Excite Japan 에서 사용하는 UserID 와 LoginTime 을 받아서 MD5 를 구한다.
		* \param szU UserID
		* \param szT Login Time
		* \param szMd5 Client 에서 넘어온 MD5 값
		* \return true:성공 false:실패
		*/
		bool ExciteGetMd5(const char* szU,
						const char* szT,
						const char* szMd5);
		/**
		* Excite Japan 에서 넘어오는 LoginTime 이 제대로된 데이터인지 확인한다.
		* \param strDate LoginTime 형식:20050303020434 년월일시분초
		* \return true:성공 false:실패
		*/
		bool ExciteCheckDate(CString& strDate);

	protected:
		/**
		* 초기화
		* \return 
		*/
		bool Init();	

	protected:
		HCRYPTPROV m_hCryptProv;	
		DWORD m_dwHashLen;
		CString m_strSecretKey;	
	};
}