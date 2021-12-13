#include "pch.h"
#include "s_CMinMd5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SERVER_UTIL
{
	CMinMd5::CMinMd5(void) 
		: m_hCryptProv(NULL)
		, m_dwHashLen(16)
	{
		Init();
	}

	CMinMd5::~CMinMd5(void)
	{	
		if (m_hCryptProv)
			CryptReleaseContext(m_hCryptProv, 0);
	}

	bool CMinMd5::Init()
	{
		// PROV_RSA_FULL
		// 
		// Algorithms Supported
		// Key Exchange : RSA 
		// Signature : RSA 
		// Encryption RC2, RC4 
		// Hashing : MD5,SHA
		if (!CryptAcquireContext(&m_hCryptProv,
								NULL,
								NULL,
								PROV_RSA_FULL,
								0))
		{
			int nRetCode = GetLastError();
			// 이미 다른곳에서 키셋을 사용중
			// 새로운 키셋을 요청한다.
			if (nRetCode == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&m_hCryptProv,
										 NULL,
										 NULL,
										 PROV_RSA_FULL,
										 CRYPT_NEWKEYSET))
				{
					nRetCode = GetLastError();
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	bool CMinMd5::GetMd5(CString& strSource, CString& strTarget)
	{
		HCRYPTHASH hHash = NULL;
		BYTE bHash[0x7f];
		DWORD dwContent = strSource.GetLength();
		const BYTE* pbContent = (const BYTE*) strSource.GetString();

		strTarget.Empty();
		// Acquire a hash object handle.
		if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
		{
			int nError = GetLastError();
			return false;
		}

		if (!CryptHashData(hHash, pbContent, dwContent, 0))
		{
			if (hHash) CryptDestroyHash(hHash);
			return false;
		}
		
		if (!CryptGetHashParam(hHash, HP_HASHVAL, bHash, &m_dwHashLen, 0))
		{
			if (hHash) CryptDestroyHash(hHash);
			return false;
		}

		CString strTemp;
		for (int i=0; i<16; i++)
		{
			strTemp.Format("%02x", bHash[i]);
			strTarget += strTemp;
		}
		
		if (hHash) CryptDestroyHash(hHash);
		return true;
	}

	void CMinMd5::ExciteSetSecretKey(const char* szSecretKey)
	{
		m_strSecretKey = szSecretKey;
	}

	bool CMinMd5::ExciteGetMd5(const char* szU,
							   const char* szT,
							   const char* szMd5)
	{
		if (szU == NULL || szT == NULL || szMd5 == NULL)
			return false;

		CString strU = szU;	
		CString strT = szT;
		CString strTemp = strU + strT + m_strSecretKey;
		CString strMd5Source = szMd5;
		CString strMd5Target;		

		if (GetMd5(strTemp, strMd5Target) == false)
		{
			return false;
		}
		else
		{
			if (strMd5Source.CompareNoCase(strMd5Target) == 0)
			{
				return true;
			}
			else
			{
				return false;
			}			
		}
	}

	bool CMinMd5::ExciteCheckDate(CString& strDate)
	{
		if (strDate.GetLength() != 14)	
			return false;

		CTime tNow = CTime::GetCurrentTime(); // 현재시간

		int nYear, nMonth, nDay, nHour, nMin, nSec;

		// ex) 2005^06^30^16^34^53

		nYear  = atoi(strDate.Mid( 0, 4).GetString());
		nMonth = atoi(strDate.Mid( 4, 2).GetString());
		nDay   = atoi(strDate.Mid( 6, 2).GetString());
		nHour  = atoi(strDate.Mid( 8, 2).GetString());
		nMin   = atoi(strDate.Mid(10, 2).GetString());
		nSec   = atoi(strDate.Mid(12, 2).GetString());

		// The upper date limit is January 18, 2038. For a wider range of dates, see COleDateTime.
		// CTime object and January 1, 1970.
		if (nYear  < 1971 || nYear  > 2037) return false;
		if (nMonth < 1    || nMonth > 12)   return false;
		if (nDay   < 1    || nDay   > 31)   return false;
		if (nHour  < 0    || nHour  > 23)   return false;
		if (nMin   < 0    || nMin   > 59)   return false;
		if (nSec   < 0    || nSec   > 59)   return false;

		CTime tFireTime(nYear, nMonth, nDay, nHour, nMin, nSec);

		__time64_t diffTime = tFireTime.GetTime() - tNow.GetTime();

		if (diffTime > 0)
			return true;
		else
			return false;
	}
};