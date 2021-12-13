#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int COdbcManager::AddLogServerState(
	int nSGNum,
	int nSvrNum,
	int nUserNum,
	int nUserMax )
{
/*
	std::strstream strTemp;
	strTemp << "{call log_serverstate(";
	strTemp << nUserNum << ",";
	strTemp << nUserMax << ",";
	strTemp << nSvrNum << ",";	
	strTemp << nSGNum << ")}";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call log_serverstate(%d,%d,%d,%d)}", nUserNum, nUserMax, nSvrNum, nSGNum ); 

	int nReturn = m_pLogDB->ExecuteSp(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	
	return nReturn;
}

int COdbcManager::LogRandomItem(
	const SLOGRANDOMOPTION &_sLOG )
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_LogItemRandom_Insert(";
	strTemp << _sLOG.nNIDMain << ",";
	strTemp << _sLOG.nNIDSub << ",";
	strTemp << _sLOG.nSGNum << ",";	
	strTemp << _sLOG.nSvrNum << ",";
	strTemp << _sLOG.nFldNum << ",";
	strTemp << _sLOG.nMakeType << ",";
	strTemp << _sLOG.lnMakeNum << ",";

	strTemp << (short) _sLOG.cOptTYPE1 << ",";
	strTemp << _sLOG.nOptVALUE1 << ",";

	strTemp << (short) _sLOG.cOptTYPE2 << ",";
	strTemp << _sLOG.nOptVALUE2 << ",";

	strTemp << (short) _sLOG.cOptTYPE3 << ",";
	strTemp << _sLOG.nOptVALUE3 << ",";

	strTemp << (short) _sLOG.cOptTYPE4 << ",";
	strTemp << _sLOG.nOptVALUE4 << ",?)}";

	strTemp << std::ends;
*/

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_LogItemRandom_Insert(%d,%d,%d,%d,%d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", 
									_sLOG.nNIDMain, _sLOG.nNIDSub, _sLOG.nSGNum, _sLOG.nSvrNum,
									_sLOG.nFldNum, _sLOG.nMakeType, _sLOG.lnMakeNum, (short) _sLOG.cOptTYPE1,
									_sLOG.nOptVALUE1, (short) _sLOG.cOptTYPE2, _sLOG.nOptVALUE2,
									(short) _sLOG.cOptTYPE3, _sLOG.nOptVALUE3, (short) _sLOG.cOptTYPE4,
									_sLOG.nOptVALUE4); 

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

int COdbcManager::LogPetAction( const SLOGPETACTION &_sLOG )
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_LogPetAction_Insert(";
	strTemp << _sLOG.nPetNum << ",";
	strTemp << _sLOG.nItemMID << ",";
	strTemp << _sLOG.nItemSID << ",";	
	strTemp << _sLOG.nActionType << ",";
	strTemp << _sLOG.nPetFull << ",?)}";
	strTemp << std::ends;
*/

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_LogPetAction_Insert(%d,%d,%d,%d,%d,?)}", _sLOG.nPetNum, _sLOG.nItemMID, 
													_sLOG.nItemSID, _sLOG.nActionType, _sLOG.nPetFull); 

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

int COdbcManager::LogVehicleAction( const SLOGVEHICLEACTION &_sLOG )
{
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_LogVehicleAction_Insert(%d,%d,%d,%d,%d,?)}", _sLOG.nVehicleNum, _sLOG.nItemMID, 
		_sLOG.nItemSID, _sLOG.nActionType, _sLOG.nVehicleFull); 

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////////
// �ɸ��� Ȱ�� �α׸� ����Ѵ�.
int COdbcManager::LogAction(
	const SLOGACTION &_sLOG )
{
	/*
	sp_LogAction_Insert
	_sLOG.m_nCHARID
	_sLOG.m_nTYPE 
	_sLOG.m_nTARID 
	_sLOG.m_nTARTYPE 
	_sLOG.m_nEXP 
	_sLOG.m_nBRIGHT_POINT 
	_sLOG.m_nLIFE_POINT 
	_sLOG.m_nMONEY 
	*/
/*	
	std::strstream strTemp;
	strTemp << "{call sp_LogAction_Insert(";
	strTemp << _sLOG.m_nCHARID << ",";
	strTemp << _sLOG.m_nTYPE << ",";
	strTemp << _sLOG.m_nTARID << ",";	
	strTemp << _sLOG.m_nTARTYPE << ",";
	strTemp << _sLOG.m_nEXP << ",";
	strTemp << _sLOG.m_nBRIGHT_POINT << ",";
	strTemp << _sLOG.m_nLIFE_POINT << ",";
	strTemp << _sLOG.m_nMONEY << ")}";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_LogAction_Insert(%d,%d,%d,%d,%I64d,%d,%d,%d)}", _sLOG.m_nCHARID,
													_sLOG.m_nTYPE, _sLOG.m_nTARID, _sLOG.m_nTARTYPE, 
													_sLOG.m_nEXP, _sLOG.m_nBRIGHT_POINT, _sLOG.m_nLIFE_POINT,
													_sLOG.m_nMONEY );

	int nReturn = m_pLogDB->ExecuteSp(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

/**
 * ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.
 * \param nSGNum �����׷�
 * \param nChaNum �Ű��� ĳ���͹�ȣ
 * \param strMsg �Ű��� (�ִ�ũ��� CHAT_MSG_SIZE)  
 * \return 
 */
int COdbcManager::LogAppeal(
	int nSGNum,
	int nChaNum,
	CString strMsg )
{
    if ((strMsg.GetLength() > CHAT_MSG_SIZE) || (nSGNum < 0) || (nChaNum < 0))
    {
        return DB_ERROR;
    }

	strMsg.Trim(_T(" ")); // �յ� ��������
	strMsg.Replace("'", "''"); // ' -> ''
/*
    std::strstream strTemp;
	strTemp << "INSERT INTO LogAppeal (SGNum, ChaNum, ApNote) VALUES (";
    strTemp << nSGNum << ",";
    strTemp << nChaNum << ",";
    strTemp << "'" << strMsg.GetString() << "')";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "INSERT INTO LogAppeal (SGNum, ChaNum, ApNote) VALUES (%d, %d)",
													 nSGNum, nChaNum);

	int nReturn = m_pLogDB->ExecuteSQL(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

/**
 * �ҹ����α׷� ��볻���� ����Ѵ�.
 * \param nSGNum �����׷�
 * \param nSvrNum ������ȣ
 * \param nUserNum ������ȣ
 * \param nChaNum ĳ���͹�ȣ
 * \param nHackProgramNum ��ŷ���α׷���ȣ
 * \param szComment �޸�Ʈ
 * \return 
 */
int COdbcManager::LogHackProgram(
	int nSGNum, 
	int nSvrNum, 
	int nUserNum, 
	int nChaNum,
	int nHackProgramNum,
	const TCHAR* szComment )
{
	if (szComment == NULL) return DB_ERROR;
	
	CString strComment(szComment);
	strComment.Trim(_T(" ")); // �յ� ��������
	strComment.Replace("'", "''"); // ' -> ''	

/*
	std::strstream strTemp;
	strTemp << "{call InsertLogHackProgram(";
	strTemp << nSGNum << ",";
	strTemp << nSvrNum << ",";
	strTemp << nUserNum << ",";
    strTemp << nChaNum << ",";
	strTemp << nHackProgramNum << ",";
	strTemp << "'" << strComment.GetString() << "',?)}";
	strTemp << std::ends;
*/
	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "{call InsertLogHackProgram(%d,%d,%d,%d,%d,'%s',?)}", 
												nSGNum, nSvrNum, nUserNum, nChaNum, nHackProgramNum,
												strComment.GetString());

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

/**
 * ĳ���� ���� ���
 * \param nSGNum �����׷�
 * \param nSvrNum ������ȣ (ä�ι�ȣ)
 * \param nUserNum ����ڹ�ȣ
 * \param nChaNum ĳ���͹�ȣ
 * \return 
 */
int COdbcManager::LogChaDeleted(
	int nSGNum,
	int nSvrNum,
	int nUserNum,
	int nChaNum )
{
	/*
	std::strstream strTemp;
	strTemp << "Insert Into LogChaDelete (SGNum, SvrNum, nUserNum, nChaNum) Values (";
	strTemp << nSGNum << ",";
	strTemp << nSvrNum << ",";
	strTemp << nUserNum << ",";
	strTemp << nChaNum << ")";
	strTemp << std::ends;

	return m_pLogDB->ExecuteSQL(strTemp);

	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	*/
	return DB_OK;
}

/*
int COdbcManager::LogServerState(int nSGNum, int nSvrNum, int nUserNum, int nUserMax)
{
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pLogDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	std::strstream strTemp;
	strTemp << "{call log_serverstate(";
	strTemp << nUserNum << ",";
	strTemp << nUserMax << ",";
	strTemp << nSvrNum << ",";	
	strTemp << nSGNum << ")}";
	strTemp << std::ends;

	::SQLPrepare(pConn->hStmt,(SQLCHAR*) strTemp.str(), SQL_NTS);

	sReturn = ::SQLExecute(pConn->hStmt);	
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(strTemp.str());		
		Print(GetErrorString(pConn->hStmt));
		m_pLogDB->FreeConnection(pConn);

		strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}
	else
	{
		m_pLogDB->FreeConnection(pConn);

		strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_OK;
	}
}
*/