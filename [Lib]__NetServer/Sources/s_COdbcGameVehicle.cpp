#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* ���ο� Ż���� �����Ѵ�.
* \param nChaNum  Ż�� ������ ĳ���� ��ȣ
* \param szVehicleName Ż�� �̸�
* \param nVehicleType Ż���� Ÿ��
* \param nVehicleCardMID Ż���� ������ MID
* \param nVehicleCardSID Ż���� ������ SID
* \return DB_ERROR �Ǵ� ������������ Ż���� ������ȣ
*/
int COdbcManager::CreateVehicle(
								int nCharNum,
								const TCHAR* szVehicleName,
								int nVehicleType,
								int nVehicleCardMID,
								int nVehicleCardSID)
{
	if ( nCharNum <=0 || szVehicleName == NULL )
	{
		Print(_T("ERROR:COdbcManager::CreateVehicle"));
		return DB_ERROR;
	}	

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_InsertVehicle('%s',%d,%d,%d,%d,?)}",
		szVehicleName,
		nCharNum,
		nVehicleType,
		nVehicleCardMID,
		nVehicleCardSID );

	// Print( CString( strTemp.str() ) );
	int nReturn = m_pGameDB->ExecuteSpInt( szTemp );
	//strTemp.freeze( false ); // Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.	
	return nReturn;
}

/**
* Ż���� ���͸��� �����Ѵ�. ���͸��� 0-1000 ����
* \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
* \param nVehicleBttery ���͸�
*/
int COdbcManager::SetVehicleBattery(
								   int nVehicleNum,
								   int nCharNum,
								   int nVehicleBttery)
{
	if (nVehicleNum <= 0 ||	nVehicleBttery < 0)
	{
		Print(_T("ERROR:COdbcManager::SetVehicleBttery"));
		return DB_ERROR;
	}

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdateVehicleBattery(%d,%d,%d,?)}", nVehicleNum, nCharNum, nVehicleBttery );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}

int COdbcManager::GetVehicleBattery( int nVehicleNum, int nCharNum )
{
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_GetVehicleBattery(%d,%d,?)}", nVehicleNum, nCharNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}

/**
* Ż���� �����Ѵ�.
* \param nVehicle ������ Ż���� ��ȣ
*/
int COdbcManager:: DeleteVehicle( int nVehicle, int nCharNum)
{
	if (nVehicle <= 0)
	{
		Print(_T("ERROR:COdbcManager::DeleteVehicle"));
		return DB_ERROR;
	}	

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_DeleteVehicle(%d, %d,?)}", nVehicle, nCharNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}

/**
* Ż���� ������ �����´�.
*/

int COdbcManager::GetVehicle(
							GLVEHICLE* pVehicle,
							int nVehicleNum,
							int nCharNum)
{
	if (pVehicle == NULL)
	{
		Print(_T("ERROR:COdbcManager::GetVehicle pVehicle==NULL"));
		return DB_ERROR;
	}

	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	// Ż���� �⺻������ �����´�.
	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "Exec sp_SelectVehicle %d,%d", nVehicleNum, nCharNum );

	SQLRETURN sReturn = 0;
	int		  nRowCount = 0;
	sReturn = ::SQLExecDirect(pConn->hStmt,
		(SQLCHAR*) szTemp, 
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		return DB_ERROR;
	}

	SQLINTEGER nOwnerNum = 0, cbOwnerNum = SQL_NTS;	
	SQLINTEGER nVehicleCardMID = 0, cbVehicleCardMID = SQL_NTS;
	SQLINTEGER nVehicleCardSID = 0, cbVehicleCardSID = SQL_NTS;
	SQLINTEGER nType = 0, cbType = SQL_NTS;
	SQLINTEGER nFull = 0, cbFull = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
			m_pGameDB->FreeConnection(pConn);

			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nOwnerNum, 0, &cbOwnerNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nVehicleCardMID, 0, &cbVehicleCardMID);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nVehicleCardSID, 0, &cbVehicleCardSID);
			::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nType, 0, &cbType);
			::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nFull, 0, &cbFull);

			pVehicle->m_dwOwner        = (DWORD) nOwnerNum;
			pVehicle->m_sVehicleID.wMainID = (WORD) nVehicleCardMID;
			pVehicle->m_sVehicleID.wSubID = (WORD) nVehicleCardSID;
			pVehicle->m_emTYPE         = VEHICLE_TYPE(nType);			
			pVehicle->m_nFull		   = nFull;
			nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	m_pGameDB->FreeConnection(pConn);	

		// Row Count �� 1 �� �ƴϸ� �����̴�.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			_T("ERROR:GetVehicle RowCount = 0, VehicleID = %d, CharID = %d"), nVehicleNum, nCharNum  );
		
		return DB_ROWCNT_ERROR;
	}

	// Ż�� �� �κ��丮�� �����´�.
	if ( GetVehicleInven( pVehicle, nVehicleNum, nCharNum ) == DB_ERROR) return DB_ERROR;	

	return DB_OK;
}


/**
* Ż���� �κ��丮 ������ �����´�
*/

int COdbcManager::GetVehicleInven( 
							    GLVEHICLE* pVehicle,
								int nVehicleNum,
								int nCharNum )
{
	if (pVehicle == NULL)
	{
		Print(_T("ERROR:COdbcManager::GetVehicleInven pVehicle==NULL"));
		return DB_ERROR;
	}

	CByteStream ByteStream;
	int nReturn = 0;

	//  VehicleOnItem
	nReturn = m_pGameDB->ReadImageVehicle("VehicleInfo.VehiclePutOnItems", nVehicleNum, nCharNum,  ByteStream);

	if (nReturn == DB_ERROR)
	{
		return DB_ERROR;
	}
	else 
	{
		SETPUTONITEMS_BYBUF( pVehicle->m_PutOnItems, ByteStream );
		return DB_OK;
	}
}


/**
* Ż�� �κ��丮 ������Ʈ
*/
int COdbcManager::SetVehicleInven(
									int nVehicle,
									int nCharNum,
									CByteStream &ByteStream )
{
		if (nVehicle <= 0)
	{
		Print(_T("ERROR:COdbcManager::SetVehicleInven"));
		return DB_ERROR;
	}

	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	int nResult = 0;
	ByteStream.GetBuffer( pBuffer, dwSize );
	if (pBuffer != NULL)
	{
		std::strstream strnVehicleInven;
		strnVehicleInven << _T("UPDATE VehicleInfo SET VehicleInfo.VehiclePutOnItems=?");
		strnVehicleInven <<" WHERE (VehicleNum=" <<  nVehicle << ") And( VehicleChaNum="<< nCharNum << ")";
		strnVehicleInven << std::ends;
		nResult = m_pGameDB->WriteImage( strnVehicleInven, (DWORD) nVehicle, (BYTE *) pBuffer, dwSize );

		strnVehicleInven.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		pBuffer = NULL;
		return nResult;
	}
	else
	{
		return DB_ERROR;
	}
}