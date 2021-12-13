#pragma once

#include <string>
#include <vector>
#include "strsafe.h"

struct SBLOCK_INFO
{
	std::string	strName;			// 블럭 프로그램 설명
	std::string	strWindowText;		// 윈도우 텍스트
	std::string	strProductName;		// ProductName
	std::string	strFileDescription;	// FileDescription
	std::string	strLoadDll;			// FileDescription
	bool bBLOCK;					// 발견시 바로 블럭됨.
	bool bREPORT;
	bool bVALID;

	std::string strFileStream;
	std::vector<BYTE> vecFileStream;

	SBLOCK_INFO () :
		bBLOCK(false),
		bREPORT(false),
		bVALID(false)
	{
	}

	SBLOCK_INFO ( const SBLOCK_INFO &sINFO )
	{
		strName = sINFO.strName;
		strWindowText = sINFO.strWindowText;
		strProductName = sINFO.strProductName;
		strFileDescription = sINFO.strFileDescription;
		strLoadDll = sINFO.strLoadDll;
		strFileStream = sINFO.strFileStream;
		vecFileStream = sINFO.vecFileStream;

		bBLOCK = sINFO.bBLOCK;
		bREPORT = sINFO.bREPORT;
		bVALID = sINFO.bVALID;
	}

	SBLOCK_INFO& operator =  ( const SBLOCK_INFO &sINFO )
	{
		strName = sINFO.strName;
		strWindowText = sINFO.strWindowText;
		strProductName = sINFO.strProductName;
		strFileDescription = sINFO.strFileDescription;
		strLoadDll = sINFO.strLoadDll;
		strFileStream = sINFO.strFileStream;
		vecFileStream = sINFO.vecFileStream;

		bBLOCK = sINFO.bBLOCK;
		bREPORT = sINFO.bREPORT;
		bVALID = sINFO.bVALID;

		return *this;
	}

	bool valid ()
	{
		return !( strName=="null" && strWindowText=="null" && strProductName=="null" && strFileDescription=="null" && strLoadDll=="null" && strFileStream=="null" );
	}
};

typedef std::vector<SBLOCK_INFO>	VECBLOCK;
typedef VECBLOCK::iterator			VECBLOCK_ITER;
class CHPro;
class CCriticalSection;

class CBLOCK_PROG
{
public:
	enum { MAX_INFO = 512 };

	struct SDETECT
	{
		DWORD	dwID;
		bool	bREPORT;
		char	szINFO[MAX_INFO];

		SDETECT() 
			: dwID(0)
			, bREPORT(false)
		{
            memset(szINFO, 0, sizeof(char) * MAX_INFO);
		}

		SDETECT ( DWORD _dwID, bool _bREPORT, const char* _szINFO ) 
			: dwID(_dwID)
			, bREPORT(_bREPORT)
		{
			StringCchCopy(szINFO, MAX_INFO, _szINFO);
		}
	};

	typedef std::vector<SDETECT> VECDETECT;

public:
	std::string			m_strLIST_FILE;
	VECBLOCK			m_vecBLOCK_LIST;
	CHPro*				m_pHPro;

	bool				m_bDETECT;
	VECDETECT			m_vecDETECT;

public:
	bool LOAD ( std::string strROOT, std::string strFile );
	VECBLOCK& GetVecBlock()
	{
		return m_vecBLOCK_LIST;
	}

public:
	bool DoDETECT ();
	bool IsDETECT ()	{ return m_bDETECT; }

public:
	SDETECT NextDETECT ();

public:
	CBLOCK_PROG ();
	~CBLOCK_PROG ();

protected:
	struct STHREAD
	{
		BOOL		m_bFORCE_TERMINATE;
		BOOL		m_bEND;
		void*		m_pPROCESS;

		STHREAD () :
			m_bFORCE_TERMINATE(FALSE),
			m_bEND(TRUE),

			m_pPROCESS(NULL)
		{
		}
	};

public:
	static STHREAD				m_sTHREAD;
	static CCriticalSection		m_criticalSECT;

protected:
	DWORD	m_dwThreadID;			//	스레드 ID
	HANDLE	m_hThread;				//	스레드 핸들

public:
	BOOL	DoSTARTTHREAD ();
	void	DoENDTHREAD ();

public:
	static unsigned int WINAPI ThreadMain ( LPVOID pData );
};
