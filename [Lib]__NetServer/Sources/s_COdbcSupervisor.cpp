#include "pch.h"
#include "s_COdbcSupervisor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COdbcSupervisor::COdbcSupervisor(void) 
	: m_nDBTimeOut( DB_RESPONSE_TIME )
	, m_nPoolSize( DB_POOL_SIZE )
{	
	InitializeCriticalSection( &m_CriticalSection );
	memset( m_szOdbcName, 0, (DB_SVR_NAME_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szUsrID,    0, (USR_ID_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szUsrPass,  0, (USR_PASS_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szDBName,   0, (DB_NAME_LENGTH+1) * sizeof(TCHAR) );
}

COdbcSupervisor::COdbcSupervisor(
	const TCHAR* szOdbcName,
	const TCHAR* szUsrID,
	const TCHAR* szUsrPass,
	const TCHAR* szDBName,
	int nPoolSize,
	int nDBTimeOut ) 
	: m_nDBTimeOut( DB_RESPONSE_TIME )
	, m_nPoolSize( DB_POOL_SIZE )
{
	InitializeCriticalSection( &m_CriticalSection );
	memset( m_szOdbcName, 0, (DB_SVR_NAME_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szUsrID,    0, (USR_ID_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szUsrPass,  0, (USR_PASS_LENGTH+1) * sizeof(TCHAR) );
	memset( m_szDBName,   0, (DB_NAME_LENGTH+1) * sizeof(TCHAR) );
	SetDB( szOdbcName, szUsrID, szUsrPass, szDBName, nPoolSize, nDBTimeOut );
}

COdbcSupervisor::~COdbcSupervisor(void)
{
	DeleteCriticalSection( &m_CriticalSection );
}

void COdbcSupervisor::LockOn()
{
	EnterCriticalSection( &m_CriticalSection );
}

void COdbcSupervisor::LockOff() 
{
	LeaveCriticalSection( &m_CriticalSection );
}

void COdbcSupervisor::SetDB(
	const TCHAR* szOdbcName,
	const TCHAR* szUsrID,
	const TCHAR* szUsrPass,
	const TCHAR* szDBName,
	int nPoolSize,
	int nDBTimeOut )
{
	SetOdbcName( szOdbcName );
	SetUserID( szUsrID );
	SetUserPass( szUsrPass );
	SetDBName( szDBName );
	SetPoolSize( nPoolSize );
	SetResponseTime( nDBTimeOut );
}
	
void COdbcSupervisor::SetOdbcName( const TCHAR* szName )
{
	::StringCchCopy( m_szOdbcName, DB_SVR_NAME_LENGTH+1, szName );
}

void COdbcSupervisor::SetUserID( const TCHAR* szUsrID )
{
	::StringCchCopy( m_szUsrID, USR_ID_LENGTH+1, szUsrID );
}

void COdbcSupervisor::SetUserPass( const TCHAR* szUsrPasswd )
{
	::StringCchCopy( m_szUsrPass, USR_PASS_LENGTH+1, szUsrPasswd );
}

void COdbcSupervisor::SetDBName( const TCHAR* szDBName )
{
	::StringCchCopy( m_szDBName, DB_NAME_LENGTH+1, szDBName );
}

void COdbcSupervisor::SetPoolSize( int nSize )
{
	m_nPoolSize = nSize;
}

void COdbcSupervisor::SetResponseTime( int nTime )
{
	m_nDBTimeOut = nTime;
}

int COdbcSupervisor::OpenDB()
{
	ODBC_STMT* pTemp = CreateNewConnection();
	if (pTemp == NULL)
	{
		return DB_ERROR;
	}
	else
	{
		FreeConnection( pTemp );
		return DB_OK;
	}
}

ODBC_STMT* COdbcSupervisor::CreateNewConnection()
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	SQLHSTMT	hStmt = NULL; // Handle of statement (void*)

	ODBC_STMT* pOdbc = new ODBC_STMT;

	if (pOdbc == NULL) return NULL;

	// ODBC allocate connection memory
	sReturn = ::SQLAllocHandle(
					SQL_HANDLE_DBC,
					COdbcEnv::GetInstance()->GetHandle(),
					(SQLHANDLE *) &pOdbc->hOdbc );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{		
		CConsoleMessage::GetInstance()->WriteDatabase(
			                                   _T("ERROR:SQLAllocHandle SQL_HANDLE_DBC Failed") );
		SAFE_DELETE( pOdbc );
		COdbcEnv::GetInstance()->CreateHandle();
		return NULL;
	}
	
	// Login time out 5 sec
	sReturn = ::SQLSetConnectAttr(
					pOdbc->hOdbc,
					5,
					(void*) SQL_LOGIN_TIMEOUT,
					0 );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			                                   _T("ERROR:SQLSetConnectAttr SQL_LOGIN_TIMEOUT Failed") );
		::SQLFreeHandle( SQL_HANDLE_DBC, pOdbc->hOdbc );
		SAFE_DELETE( pOdbc );
		return NULL;
	}

	sReturn = ::SQLConnect(
					pOdbc->hOdbc, 
					(SQLCHAR*) m_szOdbcName,
					SQL_NTS,
					(SQLCHAR*) m_szUsrID,
					SQL_NTS,
					(SQLCHAR*) m_szUsrPass,
					SQL_NTS );

	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			                                  _T("ERROR:SQLConnect Failed") );
		::SQLFreeHandle( SQL_HANDLE_DBC, pOdbc->hOdbc );
		SAFE_DELETE( pOdbc );
		return NULL;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))

	// Prepare statement handle
	sReturn = ::SQLAllocHandle( SQL_HANDLE_STMT, pOdbc->hOdbc, (SQLHANDLE *) &pOdbc->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn == SQL_SUCCESS_WITH_INFO) )
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			                                   _T("ERROR:SQLAllocHandle SQL_HANDLE_STMT Failed") );
		::SQLFreeHandle( SQL_HANDLE_DBC, pOdbc->hOdbc );
		SAFE_DELETE( pOdbc );
		return NULL;
	}

	return pOdbc;
}

void COdbcSupervisor::CloseDB()
{
}

ODBC_STMT* COdbcSupervisor::GetConnection()
{	
	/*
	SQLHSTMT StmtTemp = NULL;
	LockOn();
	if (m_UnUse.IsEmpty())
	{
        StmtTemp = CreateNewStmt();
	}
	else
	{
		StmtTemp = m_UnUse.GetHead();
		if (StmtTemp) m_UnUse.Release((MEM_POOLER::NODE<SQLHSTMT>*) StmtTemp);
	}

	if (StmtTemp != NULL)
		m_Use.AddTail((MEM_POOLER::NODE<SQLHSTMT>*) StmtTemp);
	LockOff();
	return StmtTemp;
	*/
	
	ODBC_STMT* pTemp = NULL;
	LockOn();
	pTemp = CreateNewConnection();
	LockOff();
	return pTemp;
}

void COdbcSupervisor::FreeConnection( ODBC_STMT* hConn )
{	
	/*
	LockOn();
	::SQLCloseCursor(hStmt);
	m_Use.Release  ((MEM_POOLER::NODE<SQLHSTMT>*) hStmt);
	m_UnUse.AddTail((MEM_POOLER::NODE<SQLHSTMT>*) hStmt);	
	LockOff();
	*/
	if ( hConn == NULL ) return;

	LockOn();
	::SQLCloseCursor( hConn->hStmt );
	::SQLFreeHandle( SQL_HANDLE_STMT, hConn->hStmt );
	::SQLDisconnect( hConn->hOdbc );
	::SQLFreeHandle( SQL_HANDLE_DBC, hConn->hOdbc );
	SAFE_DELETE( hConn );
	LockOff();
}

int	COdbcSupervisor::ExecuteSQL( std::strstream& strSQL )
{
	ODBC_STMT*	pConn = NULL;
    SQLRETURN	sReturn = 0;
	
	// Prepare statement handle
	pConn = GetConnection();
    if (pConn == NULL) return DB_ERROR;
	
	sReturn = ::SQLExecDirect(
					pConn->hStmt, 
					(SQLCHAR*) strSQL.str(),
					SQL_NTS );
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{		
		FreeConnection(pConn);
		return DB_OK;
	}
	else
	{
		CString strError;
		strError.Format( _T("ERROR %s"), strSQL.str() );
		Print( strError );
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );
		return DB_ERROR;
	}
}

int	COdbcSupervisor::ExecuteSQL( CString strSQL )
{
	ODBC_STMT*	pConn = NULL;
    SQLRETURN	sReturn = 0;
	
	// Prepare statement handle
	pConn = GetConnection();
    if (!pConn) return DB_ERROR;
	
	sReturn = ::SQLExecDirect(
					pConn->hStmt, 
					(SQLCHAR*) strSQL.GetString(), 
					SQL_NTS );
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{
		FreeConnection(pConn);
		return DB_OK;
	}
	else
	{
		Print( strSQL.GetString() );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return DB_ERROR;
	}
}

int COdbcSupervisor::ExecuteSp( std::strstream& strSP )
{
	ODBC_STMT* pConn = NULL;
	SQLRETURN sReturn = 0;
	
	pConn = GetConnection();
	if (!pConn)	return DB_ERROR;
	
	::SQLPrepare( pConn->hStmt,(SQLCHAR*) strSP.str(), SQL_NTS );
	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
        Print( strSP.str() );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return DB_ERROR;
	}
	else
	{
		FreeConnection(pConn);
		return DB_OK;
	}
}

int COdbcSupervisor::ExecuteSpInt( std::strstream& strSP )
{
	ODBC_STMT* pConn = GetConnection();
	if (!pConn)	return DB_ERROR;
		
	SQLRETURN  sReturn=0;
	SQLINTEGER nOutput=0;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare( pConn->hStmt, (SQLCHAR*) strSP.str(), SQL_NTS );
	
	// Bind the parameter.
	sReturn = ::SQLBindParameter(
					pConn->hStmt, 
					1,
					SQL_PARAM_OUTPUT,
					SQL_C_SLONG,
					SQL_INTEGER,
					0,
					0,
					&nOutput,
					0,
					&nLenBuffer );
	
	if ( sReturn == SQL_ERROR || sReturn == SQL_INVALID_HANDLE )
	{		
		Print( strSP.str() );
		Print( _T("ERROR:SQLBindParameter") );
		FreeConnection( pConn );
		return DB_ERROR;
	}

	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print( strSP.str() );
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );
		return DB_ERROR;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	else
	{
		FreeConnection( pConn );
	}
    return nOutput;
}

int	COdbcSupervisor::ExecuteSQL( const TCHAR * szSQL )
{
	ODBC_STMT*	pConn = NULL;
	SQLRETURN	sReturn = 0;

	// Prepare statement handle
	pConn = GetConnection();
	if (!pConn) return DB_ERROR;

	sReturn = ::SQLExecDirect(
		pConn->hStmt, 
		(SQLCHAR*)szSQL, 
		SQL_NTS );
	if ( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO || sReturn == SQL_NO_DATA )
	{
		FreeConnection(pConn);
		return DB_OK;
	}
	else
	{
		Print( szSQL );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return DB_ERROR;
	}
}

int COdbcSupervisor::ExecuteSp( const TCHAR * szSP )
{
	ODBC_STMT* pConn = NULL;
	SQLRETURN sReturn = 0;

	pConn = GetConnection();
	if (!pConn)	return DB_ERROR;

	::SQLPrepare( pConn->hStmt,(SQLCHAR*)szSP, SQL_NTS );
	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print( szSP );
		Print( GetErrorString(pConn->hStmt) );
		FreeConnection( pConn );
		return DB_ERROR;
	}
	else
	{
		FreeConnection(pConn);
		return DB_OK;
	}
}

int COdbcSupervisor::ExecuteSpInt( const TCHAR * szSP )
{
	ODBC_STMT* pConn = GetConnection();
	if (!pConn)	return DB_ERROR;

	SQLRETURN  sReturn=0;
	SQLINTEGER nOutput=0;
	SQLINTEGER nLenBuffer=SQL_NTS;

	::SQLPrepare( pConn->hStmt, (SQLCHAR*)szSP, SQL_NTS );

	// Bind the parameter.
	sReturn = ::SQLBindParameter(
		pConn->hStmt, 
		1,
		SQL_PARAM_OUTPUT,
		SQL_C_SLONG,
		SQL_INTEGER,
		0,
		0,
		&nOutput,
		0,
		&nLenBuffer );

	if ( sReturn == SQL_ERROR || sReturn == SQL_INVALID_HANDLE )
	{		
		Print( szSP );
		Print( _T("ERROR:SQLBindParameter") );
		FreeConnection( pConn );
		return DB_ERROR;
	}

	sReturn = ::SQLExecute( pConn->hStmt );
	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
		Print( szSP );
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );
		return DB_ERROR;
	} // if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	else
	{
		FreeConnection( pConn );
	}
	return nOutput;
}

int	COdbcSupervisor::ReadImagePet(
		const TCHAR* objName, 
		int nChaNum,
		int nPetNum, 
		CByteStream &ByteStream )
{
	if (objName == NULL) return DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	SQLCHAR		pBinary[DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	

	pConn = GetConnection();
	if (!pConn) return DB_ERROR;	

	//std::strstream strTemp;
	//strTemp << _T("SELECT " << objName << " FROM PetInfo WHERE (PetNum=" <<  nPetNum << ")");
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT %s FROM PetInfo where (PetNum=%d And PetChaNum=%d)", objName, nPetNum, nChaNum );

	// Create a result
	sReturn = ::SQLExecDirect(
		pConn->hStmt,
		(SQLCHAR*)szTemp,
		SQL_NTS );
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ( sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO )
	{
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );

		return DB_ERROR;
	}

	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(pConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
				pConn->hStmt,
				1,
				SQL_C_BINARY,
				pBinary,
				DB_IMAGE_BUF_SIZE,
				&lSize );
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	FreeConnection(pConn);
	return DB_OK;
}

int	COdbcSupervisor::ReadImageVehicle(
								  const TCHAR* objName, 
								  int nVehicleNum,
								  int nCharNum,
								  CByteStream &ByteStream )
{
	if (objName == NULL) return DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	SQLCHAR		pBinary[DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	

	pConn = GetConnection();
	if (!pConn) return DB_ERROR;	

	//std::strstream strTemp;
	//strTemp << _T("SELECT " << objName << " FROM PetInfo WHERE (PetNum=" <<  nPetNum << ")");
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT %s FROM VehicleInfo where (VehicleNum=%d) And (VehicleChaNum=%d)", objName, nVehicleNum, nCharNum );

	// Create a result
	sReturn = ::SQLExecDirect(
		pConn->hStmt,
		(SQLCHAR*)szTemp,
		SQL_NTS );
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ( sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO )
	{
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );

		return DB_ERROR;
	}

	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(pConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
				pConn->hStmt,
				1,
				SQL_C_BINARY,
				pBinary,
				DB_IMAGE_BUF_SIZE,
				&lSize );
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	FreeConnection(pConn);
	return DB_OK;
}

int	COdbcSupervisor::ReadImage(
		const TCHAR* objName, 
		int nChaNum, 
		CByteStream &ByteStream )
{
	if (objName == NULL) return DB_ERROR;

	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	SQLCHAR		pBinary[DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;	
	
	pConn = GetConnection();
    if (!pConn) return DB_ERROR;	
	
	//std::strstream strTemp;
	//strTemp << _T("SELECT " << objName << " FROM ChaInfo where (ChaNum=" <<  nChaNum << ")");
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT %s FROM ChaInfo where (ChaNum=%d)", objName, nChaNum );

	// Create a result
	sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*)szTemp,
					SQL_NTS );
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if ( sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO )
	{
		Print( GetErrorString( pConn->hStmt ) );
		FreeConnection( pConn );

		return DB_ERROR;
	}
	
	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ( (sReturn = ::SQLFetch(pConn->hStmt) ) != SQL_NO_DATA )
	{			
		while (1) 
		{
			::memset( pBinary, 0, DB_IMAGE_BUF_SIZE );
			lSize = 0;
			sReturn = ::SQLGetData(
							pConn->hStmt,
							1,
							SQL_C_BINARY,
							pBinary,
							DB_IMAGE_BUF_SIZE,
							&lSize );
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer( (LPBYTE) pBinary, lSize );
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	FreeConnection(pConn);
	return DB_OK;
}


int	COdbcSupervisor::ReadImage(
		std::strstream& strTemp, 
		int nChaNum, 
		CByteStream &ByteStream )
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	SQLCHAR		pBinary[DB_IMAGE_BUF_SIZE] = {0};	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;
	
	pConn = GetConnection();
    if (!pConn) return DB_ERROR;
	
	// Create a result
	sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*) strTemp.str(),
					SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(pConn->hStmt));
		FreeConnection(pConn);
		return DB_ERROR;
	}
	
	ByteStream.ClearBuffer();

	// Retrieve and display each row of data.
	while ((sReturn = ::SQLFetch(pConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::memset(pBinary, 0, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(pConn->hStmt, 1, SQL_C_BINARY, pBinary, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBinary, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
		}

		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
	}

	FreeConnection(pConn);
	return DB_OK;
}

int COdbcSupervisor::WriteImage(
			   const TCHAR* strTemp,
			   int nChaNum, 
			   BYTE* pData, 
			   int nSize )
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	// SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	pConn = GetConnection();
	if (!pConn) return DB_ERROR;

	// Prepare parameter
	sReturn = ::SQLPrepare(pConn->hStmt, (SQLCHAR*)strTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(GetErrorString(pConn->hStmt));
		FreeConnection(pConn);
		return DB_ERROR;
	}

	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(pConn->hStmt, 
		1, 
		SQL_PARAM_INPUT,
		SQL_C_BINARY, 
		SQL_LONGVARBINARY,
		nSize, 
		0, 
		(SQLPOINTER) 2, 
		0, 
		&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeConnection(pConn);
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(pConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(GetErrorString(pConn->hStmt));
		FreeConnection(pConn);
		return DB_ERROR;
	}

	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(pConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(pConn->hStmt, pData, (SQLINTEGER) nSize);
		}
	}
	// Close the cursor.
	// ::SQLCloseCursor(hStmt);
	FreeConnection(pConn);
	return DB_OK;
}

int COdbcSupervisor::WriteImage(
		std::strstream& strTemp, 
		int nChaNum, 
		BYTE* pData, 
		int nSize )
{
	SQLRETURN	sReturn = 0; // SQL return value (short)	
	ODBC_STMT*	pConn = NULL; // Handle of statement (void*)
	// SQLCHAR		pBinary[1024];	// Declare a binary buffer to retrieve 1024 bytes of data at a time.
	SQLINTEGER  lSize=0, lTotalSize=0;

	pConn = GetConnection();
    if (!pConn) return DB_ERROR;
	
	// Prepare parameter
	sReturn = ::SQLPrepare(pConn->hStmt, (SQLCHAR*) strTemp.str(), SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(GetErrorString(pConn->hStmt));
		FreeConnection(pConn);
		return DB_ERROR;
	}

	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(pConn->hStmt, 
								1, 
								SQL_PARAM_INPUT,
								SQL_C_BINARY, 
								SQL_LONGVARBINARY,
								nSize, 
								0, 
								(SQLPOINTER) 2, 
								0, 
								&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		FreeConnection(pConn);
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(pConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
    {
		Print(GetErrorString(pConn->hStmt));
        FreeConnection(pConn);
        return DB_ERROR;
	}
	
	SQLPOINTER pToken;
	// SQLINTEGER cbData;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(pConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(pConn->hStmt, pData, (SQLINTEGER) nSize);
		}
	}
    // Close the cursor.
	// ::SQLCloseCursor(hStmt);
	FreeConnection(pConn);
	return DB_OK;
}

void COdbcSupervisor::Print( CString strMsg )
{
	// for Console 
	// printf(strMsg.GetString());
	// for GUI
	CConsoleMessage::GetInstance()->WriteDatabase( _T("DB:%s"), strMsg.GetString() );
}

void COdbcSupervisor::Print( const TCHAR * szMsg )
{
	CConsoleMessage::GetInstance()->WriteDatabase( _T("DB:%s"), szMsg );
}

CString	COdbcSupervisor::GetErrorString(SQLHSTMT hStmt)
{
	SQLRETURN sReturn = 0;
	SQLCHAR SqlState[6] = {0};
	SQLCHAR Msg[ODBC_ERROR_MESSAGE_LENGTH+1] = {0};
	SQLSMALLINT MsgLen = 0;
	SQLINTEGER nError = 0;
	int nDiag = 0;

	CString strTemp;

	::SQLGetDiagField(SQL_HANDLE_STMT, hStmt, 0, SQL_DIAG_NUMBER, &nDiag, 0, &MsgLen);

	for (nDiag=1;;nDiag++)
	{
		sReturn = ::SQLGetDiagRec(SQL_HANDLE_STMT, 
								  hStmt, 
								  nDiag,
								  SqlState,
								  &nError,
								  Msg,
								  ODBC_ERROR_MESSAGE_LENGTH, 
								  &MsgLen);
		if (sReturn == SQL_NO_DATA)
			break;
		else
			strTemp.Format(_T("%s, NativeError:%d, %s"),
						   (LPCTSTR) SqlState,
						   nError,
						   (LPCTSTR) Msg);

	}
	return strTemp;
}