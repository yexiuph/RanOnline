#ifndef S_ADDRESS_CHECKER_H_
#define S_ADDRESS_CHECKER_H_

#include <afxcoll.h> // MFC CMapStringToPtr head file

#ifndef MAX_IP_LENGTH
#define MAX_IP_LENGTH 20 // IP 번호 문자열 최대값
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	if( p ) { delete (p); (p) = NULL; }
#endif

/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-02-01 Jgkim 생성
 *       2006-02-07 Jgkim:Unicode 지원, VS 2005 지원
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
namespace SERVER_UTIL
{
	struct ADDRESS_INFO
	{
		_TCHAR	szAddress[MAX_IP_LENGTH+1]; ///< IP Address
		int     nRefCount;                  ///< IP Address reference count
		DWORD   dwTime;                     ///< Block 일때 Block 시간
		
		ADDRESS_INFO() 
			: nRefCount(0)
			, dwTime(0)
		{
			memset(szAddress, 0, sizeof(_TCHAR) * (MAX_IP_LENGTH+1));
		}
	};
	typedef ADDRESS_INFO* LPADDRESS_INFO;

/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements
 * win98 or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-02-01
 *
 * \author jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 *
 */
	class CAddressChecker
	{
	public:
		static CAddressChecker* GetInstance();
		static void	ReleaseInstance();

    private:
		CAddressChecker(int nBlockCount=10);
		~CAddressChecker(void);

		static CAddressChecker* SelfInstance;

	protected:
		CMapStringToPtr m_AddressMap;   ///< 일반적인 주소 등록 block 과 관계없음
		CMapStringToPtr m_KnownAddress; ///< 알고있는 주소
		CMapStringToPtr m_BlockAddress; ///< Block 하는 주소
		
		int m_nBlockCount; ///< 몇 회 이상의 중복 접속일때 접속을 차단할 것인지 결정한다.

		CRITICAL_SECTION	m_CriticalSection; ///< critical section object

	public:
		
		//! 몇번의 잘못된 접속시도시 접속을 거부할 것인지 정한다.
		//! \param nBlockCount 횟수		
		void setBlockCount(int nBlockCount);
		
		//! 현재 등록된 모든 IP 주소를 csv 파일로 저장한다.
		//! \return 
		int save();

		//! Block 되지 않는 주소를 등록한다 이 IP 주소는 신뢰하는 주소이다.
		//! \param szAddress 등록할 IP 주소
		//! \return
		int addKnownAddress(const _TCHAR* szAddress);

		//! Block 되는 주소를 등록한다.\n
		//! Block Address 는 KnownAddress 보다 우선한다.
		//! \param szAddress Block 될 IP 주소
		//! \param dwBlockTime Block 이 풀리는 시간
		//! \return
		int addBlockAddress(const _TCHAR* szAddress, DWORD dwBlockTime=0);		

		//! 알고 있는 주소인지 검사한다.\n
		//! \param strAddress 검사할 IP 주소
		//! \return true/false		
		bool isKnownAddress (CString strAddress);

		//! Block 해야하는 Address 인지 검사한다.\n
		//! \param szAddress 검사할 IP 주소
		//! \return true/false
		bool isBlock(const _TCHAR* szAddress, DWORD dwCurrentTime=0);

		//! 등록된 주소를 삭제한다.
		//! \param szAddress 삭제할 IP 주소
		//! \return
		int  deleteAddress(const _TCHAR* szAddress);

		//! 등록된 block Address 를 모두 삭제한다.
		//! \return
		int  resetBlockAddress();

		//! 등록된 block 주소를 삭제한다.
		//! \param szAddress 삭제할 주소
		//! \return true, false
		bool deleteBlockAddress(const _TCHAR* szAddress);

	protected:
		LPADDRESS_INFO findAddress(CString strAddress);
		
		//! 알고있는 주소나 Block 주소와 관계없이 IP 를 등록한다.
		//! \param szAddress 등록할 주소
		int insertAddress(const _TCHAR* szAddress);
		 
		int deleteAll();
		
		int deleteKnownAddressAll();
		int deleteBlockAddressAll();		
		
		bool isBlockAddress(CString strAddress, DWORD dwCurrentTime=0);

		int  getRefCount(const _TCHAR* szAddress);
		int  setRefCount(const _TCHAR* szAddress, int nCount=0);
	};
}

#endif // S_ADDRESS_CHECKER_H_