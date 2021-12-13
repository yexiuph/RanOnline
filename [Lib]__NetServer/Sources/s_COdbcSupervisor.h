#ifndef _CODBCSUPERVISOR_H_
#define _CODBCSUPERVISOR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// To use the installer DLL functions, you must: 
// Use #include < ODBCINST.H>.
// Link with Odbcinst.lib for 16-bit applications.
// Link with Odbccp32.lib for 32-bit applications. 
// #pragma comment (lib,"Odbccp32.lib")
#pragma comment (lib,"odbc32.lib")
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <Windows.h>
#include <strstream>
#include <stdio.h>

#include "s_NetGlobal.h"
#include "s_DbBase.h"
#include "s_CLock.h"
#include "s_CConsoleMessage.h"
#include "s_CMemPooler.h"
#include "s_COdbcEnv.h"

// #include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlMsg.h"

struct ODBC_STMT
{	
	SQLHSTMT hStmt;
	SQLHDBC hOdbc;

	ODBC_STMT()
	{
		hStmt = NULL;
		hOdbc = NULL;
	};
};
/**
 * \ingroup NetServerLib
 * 
 *
 * \par requirements
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2003-07-12
 *
 * \author jgkim
 *
 * \par license
 * 
 * \todo 
 *
 * \bug 
 *
 */
class COdbcSupervisor
{
public:
	COdbcSupervisor();
	COdbcSupervisor(
		const TCHAR* szOdbcName,
		const TCHAR* szUsrID,
		const TCHAR* szUsrPass,
		const TCHAR* szDBName,
		int nPoolSize = DB_POOL_SIZE,
		int nDBTimeOut = DB_RESPONSE_TIME );
	~COdbcSupervisor();

protected:
	CRITICAL_SECTION	m_CriticalSection;
	void LockOn();
	void LockOff();

	TCHAR m_szOdbcName[DB_SVR_NAME_LENGTH+1];
	TCHAR m_szUsrID[USR_ID_LENGTH+1];	
	TCHAR m_szUsrPass[USR_PASS_LENGTH+1];
	TCHAR m_szDBName[DB_NAME_LENGTH+1];		
	int	m_nDBTimeOut;
	int	m_nPoolSize;

	MEM_POOLER::TCList<ODBC_STMT> m_Use;
	MEM_POOLER::TCList<ODBC_STMT> m_UnUse;

	ODBC_STMT*	CreateNewConnection();

public:	
	void SetDB(
			const TCHAR* szOdbcName,
			const TCHAR* szUsrID,
			const TCHAR* szUsrPass,
			const TCHAR* szDBName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void SetOdbcName( const TCHAR* szName );
	void SetUserID( const TCHAR* szUsrID );
	void SetUserPass( const TCHAR* szUsrPasswd );
	void SetDBName( const TCHAR* szDBName );
	void SetPoolSize( int nSize = DB_POOL_SIZE );
	void SetResponseTime( int nTime = DB_RESPONSE_TIME );
	
	int	OpenDB();
	void CloseDB();

	ODBC_STMT* GetConnection();	
    void FreeConnection( ODBC_STMT* hConn );

	int	ExecuteSQL( CString strSQL );
	int	ExecuteSQL( std::strstream& strSQL );
	int ExecuteSp( std::strstream& strSP );
	int ExecuteSpInt( std::strstream& strSP );

	int	ExecuteSQL( const TCHAR * szSQL );
	int ExecuteSp( const TCHAR * szSP );
	int ExecuteSpInt( const TCHAR * szSP );

	int ReadImage(
			std::strstream& strTemp, 
			int nChaNum, 
			CByteStream &ByteStream );
	int	ReadImage(
			const TCHAR* objName, 
			int nChaNum, 
			CByteStream &ByteStream );
	int	ReadImagePet(
			const TCHAR* objName,
			int nChaNum,
			int nPetNum, 
			CByteStream &ByteStream );
	int	ReadImageVehicle(
		const TCHAR* objName, 
		int nVehicleNum,
		int nCharNum,
		CByteStream &ByteStream );
    int WriteImage(
			std::strstream& strTemp,
			int nChaNum, 
			BYTE* pData, 
			int nSize );
	int WriteImage(
			const TCHAR* strTemp,
			int nChaNum, 
			BYTE* pData, 
			int nSize );
	CString	GetErrorString( SQLHSTMT hStmt );
	void Print( CString strMsg );
	void Print( const TCHAR * szMsg );
};

#endif // _CODBCSUPERVISOR_H_