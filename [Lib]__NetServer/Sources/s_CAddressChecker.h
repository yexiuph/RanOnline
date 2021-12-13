#ifndef S_ADDRESS_CHECKER_H_
#define S_ADDRESS_CHECKER_H_

#include <afxcoll.h> // MFC CMapStringToPtr head file

#ifndef MAX_IP_LENGTH
#define MAX_IP_LENGTH 20 // IP ��ȣ ���ڿ� �ִ밪
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
 * \date 2005-02-01 Jgkim ����
 *       2006-02-07 Jgkim:Unicode ����, VS 2005 ����
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
		DWORD   dwTime;                     ///< Block �϶� Block �ð�
		
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
		CMapStringToPtr m_AddressMap;   ///< �Ϲ����� �ּ� ��� block �� �������
		CMapStringToPtr m_KnownAddress; ///< �˰��ִ� �ּ�
		CMapStringToPtr m_BlockAddress; ///< Block �ϴ� �ּ�
		
		int m_nBlockCount; ///< �� ȸ �̻��� �ߺ� �����϶� ������ ������ ������ �����Ѵ�.

		CRITICAL_SECTION	m_CriticalSection; ///< critical section object

	public:
		
		//! ����� �߸��� ���ӽõ��� ������ �ź��� ������ ���Ѵ�.
		//! \param nBlockCount Ƚ��		
		void setBlockCount(int nBlockCount);
		
		//! ���� ��ϵ� ��� IP �ּҸ� csv ���Ϸ� �����Ѵ�.
		//! \return 
		int save();

		//! Block ���� �ʴ� �ּҸ� ����Ѵ� �� IP �ּҴ� �ŷ��ϴ� �ּ��̴�.
		//! \param szAddress ����� IP �ּ�
		//! \return
		int addKnownAddress(const _TCHAR* szAddress);

		//! Block �Ǵ� �ּҸ� ����Ѵ�.\n
		//! Block Address �� KnownAddress ���� �켱�Ѵ�.
		//! \param szAddress Block �� IP �ּ�
		//! \param dwBlockTime Block �� Ǯ���� �ð�
		//! \return
		int addBlockAddress(const _TCHAR* szAddress, DWORD dwBlockTime=0);		

		//! �˰� �ִ� �ּ����� �˻��Ѵ�.\n
		//! \param strAddress �˻��� IP �ּ�
		//! \return true/false		
		bool isKnownAddress (CString strAddress);

		//! Block �ؾ��ϴ� Address ���� �˻��Ѵ�.\n
		//! \param szAddress �˻��� IP �ּ�
		//! \return true/false
		bool isBlock(const _TCHAR* szAddress, DWORD dwCurrentTime=0);

		//! ��ϵ� �ּҸ� �����Ѵ�.
		//! \param szAddress ������ IP �ּ�
		//! \return
		int  deleteAddress(const _TCHAR* szAddress);

		//! ��ϵ� block Address �� ��� �����Ѵ�.
		//! \return
		int  resetBlockAddress();

		//! ��ϵ� block �ּҸ� �����Ѵ�.
		//! \param szAddress ������ �ּ�
		//! \return true, false
		bool deleteBlockAddress(const _TCHAR* szAddress);

	protected:
		LPADDRESS_INFO findAddress(CString strAddress);
		
		//! �˰��ִ� �ּҳ� Block �ּҿ� ������� IP �� ����Ѵ�.
		//! \param szAddress ����� �ּ�
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