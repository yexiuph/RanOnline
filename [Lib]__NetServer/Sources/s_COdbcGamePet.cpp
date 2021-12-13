#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 새로운 팻을 생성한다.
* \param nChaNum  팻 소유자 캐릭터 번호
* \param szPetName 팻이름
* \param nPetType 팻의 타입
* \param nPetMID 팻 MID
* \param nPetSid 팻 SID
* \param nPetStyle 팻의 스타일
* \param nPetColor 팻의 컬러
* \return DB_ERROR 또는 성공했을때는 Pet 의 고유번호
*/
int COdbcManager::CreatePet(
	int nChaNum,
	const TCHAR* szPetName,
	int nPetType,
	int nPetMID,
	int nPetSID,
	int nPetStyle,
	int nPetColor,
	int nPetCardMID,
	int nPetCardSID
	)
{
	if ( nChaNum <=0 ||
		 szPetName == NULL )
	{
		Print(_T("ERROR:COdbcManager::CreatePet"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_InsertPet(";
	//strTemp << "'" << szPetName << "',";
	//strTemp << nChaNum << ",";
	//strTemp << nPetType << ",";
	//strTemp << nPetMID << ",";
	//strTemp << nPetSID << ",";
	//strTemp << nPetCardMID << ",";
	//strTemp << nPetCardSID << ",";
	//strTemp << nPetStyle << ",";
	//strTemp << nPetColor << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_InsertPet('%s',%d,%d,%d,%d,%d,%d,%d,%d,?)}",
		szPetName,
		nChaNum,
		nPetType,
		nPetMID,
		nPetSID,
		nPetCardMID,
		nPetCardSID,
		nPetStyle,
		nPetColor );

	// Print( CString( strTemp.str() ) );
	int nReturn = m_pGameDB->ExecuteSpInt( szTemp );
	//strTemp.freeze( false ); // Note : std::strstream의 freeze. 안 하면 Leak 발생.	
	return nReturn;
}

/**
* 팻의 이름을 변경한다.
* \param nPetNum 팻의 고유번호
* \param szPetName 팻이름
* \return DB_ERROR, DB_OK
*/
int COdbcManager::RenamePet(
	int nChaNum,
	int nPetNum,
	const TCHAR* szPetName )
{
	if (szPetName == NULL)
	{
		Print(_T("ERROR:COdbcManager::RenamePet szPetName==NULL"));
		return DB_ERROR;
	}

	TCHAR szPetNameTemp[PETNAMESIZE+1] = {0};
	::StringCchCopy( szPetNameTemp, PETNAMESIZE+1, szPetName );

	//std::strstream strTemp;
	//strTemp << "{call sp_RenamePet(";
	//strTemp << nPetNum << ",";
	//strTemp << "'" << szPetNameTemp << "',?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_RenamePet(%d,%d,'%s',?)}", nChaNum, nPetNum, szPetNameTemp ); 

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 팻의 소유권을 이전한다.
* \param nChaNum 팻의 새로운 소유자
* \param nPetNum 소유권을 이전할 팻 번호
*/
int COdbcManager::ExchangePet(
	int nChaNum,
	int nPetNum )
{
	if ( nChaNum <= 0 ||
		 nPetNum <=0)
	{
		Print(_T("ERROR:COdbcManager::ExchangePet"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_UpdatePetChaNum(";
	//strTemp << nPetNum << ",";
	//strTemp << nChaNum << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetChaNum(%d,%d,?)}", nChaNum, nPetNum  );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;	
}

/**
* 팻의 컬러를 변경한다.
* \param nPetNum 컬러를 변경하려는 팻의 번호
* \param nPetColor 팻의 컬러
*/
int COdbcManager::SetPetColor(
    int nChaNum,
	int nPetNum,
	int nPetColor)
{
	if (nPetNum <= 0)
	{
		Print(_T("ERROR:COdbcManager::SetPetColor"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_UpdatePetColor(";
	//strTemp << nPetNum << ",";
	//strTemp << nPetColor << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetColor(%d,%d,%d,?)}", nChaNum, nPetNum, nPetColor );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 팻의 스타일을 변경한다.
* \param nPetNum 스타일을 변경하려는 팻의 번호
* \param nPetStyle 팻의 스타일
*
*/
int COdbcManager::SetPetStyle(
	int nChaNum,
	int nPetNum,
	int nPetStyle)
{
	if (nPetNum <= 0)
	{
		Print(_T("ERROR:COdbcManager::SetPetStyle"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_UpdatePetStyle(";
	//strTemp << nPetNum << ",";
	//strTemp << nPetStyle << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetStyle(%d,%d,%d,?)}", nChaNum, nPetNum, nPetStyle );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 팻의 포만도를 설정한다. 포만도는 0-100 사이
* \param nPetNum 포만도를 설정할 팻의 번호
* \param fPetFull 포만도
*/
int COdbcManager::SetPetFull(
	int nChaNum,
	int nPetNum,
	int nPetFull)
{
	if (nPetNum <= 0 ||
		nPetFull < 0)
	{
		Print(_T("ERROR:COdbcManager::SetPetFull"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_UpdatePetFull(";
	//strTemp << nPetNum << ",";
	//strTemp << nPetFull << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetFull(%d,%d,%d,?)}", nChaNum, nPetNum, nPetFull );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 팻을 삭제한다.
* \param nPetNum 삭제할 팻의 번호
*/
int COdbcManager::DeletePet(
	int nChaNum, 
	int nPetNum)
{
	if (nPetNum <= 0)
	{
		Print(_T("ERROR:COdbcManager::DeletePet"));
		return DB_ERROR;
	}

	// sp_DeletePet
	//std::strstream strTemp;
	//strTemp << "{call sp_DeletePet(";
	//strTemp << nPetNum << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_DeletePet(%d,%d,?)}", nChaNum, nPetNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 팻의 인벤토리 업데이트
*/
int COdbcManager::SetPetInven(
	int nChaNum,
	int nPetNum,
	CByteStream &ByteStream )
{
	if (nPetNum <= 0)
	{
		Print(_T("ERROR:COdbcManager::SetPetInven"));
		return DB_ERROR;
	}

	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;
	int nResult = 0;
	ByteStream.GetBuffer( pBuffer, dwSize );
	if (pBuffer != NULL)
	{
		std::strstream strPetInven;
		strPetInven << _T("UPDATE PetInfo SET PetInfo.PetPutOnItems=? WHERE (PetNum=" <<  nPetNum << "And PetChaNum=" << nChaNum << ")");
		strPetInven << std::ends;
		nResult = m_pGameDB->WriteImage( strPetInven, (DWORD) nPetNum, (BYTE *) pBuffer, dwSize );

		strPetInven.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		pBuffer = NULL;
		return nResult;
	}
	else
	{
		return DB_ERROR;
	}
}

/**
* 팻의 스킬 업데이트
*  
*/
int COdbcManager::SetPetSkill(
   	 int nChaNum,
	int nPetNum,
	int nPetInvenType,
	int nPetInvenMID,
	int nPetInvenSID,
	int nPetInvenCMID,
	int nPetInvenCSID,
	int nPetInvenAvailable)
{
	if (nPetNum <= 0)
	{
		Print(_T("ERROR:COdbcManager::SetPetSkill"));
		return DB_ERROR;
	}

	// sp_UpdatePetInven	
	//std::strstream strTemp;
	//strTemp << "{call sp_UpdatePetInven(";
	//strTemp << nPetNum << ",";
	//strTemp << nPetInvenType << ",";
	//strTemp << nPetInvenMID << ",";
	//strTemp << nPetInvenSID << ",";
	//strTemp << nPetInvenCMID << ",";
	//strTemp << nPetInvenCSID << ",";
	//strTemp << nPetInvenAvailable << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetInven(%d,%d,%d,%d,%d,%d,%d,%d,?)}", 
		nChaNum,
		nPetNum,
		nPetInvenType,
		nPetInvenMID,
		nPetInvenSID,
		nPetInvenCMID,
		nPetInvenCSID,
		nPetInvenAvailable );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;	
}

/**
* 팻의 정보를 가져온다.
*/
int COdbcManager::GetPet(
	GLPET* pPet,
	DWORD dwPetNum,
	DWORD dwClientID)
{
	if (pPet == NULL)
	{
		Print(_T("ERROR:COdbcManager::GetPet pPet==NULL"));
		return DB_ERROR;
	}
	
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	// Pet 의 기본정보를 가져온다.
	//std::strstream strTemp;
	//strTemp << "SELECT PetName, PetChaNum, PetType, PetMID, PetSID, PetCardMID, PetCardSID, PetStyle, PetColor, PetFull ";
	//strTemp << "FROM PetInfo WHERE PetNum=" << dwPetNum;
	//strTemp << " And PetChaNum=" << dwClientID;
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT PetName, PetChaNum, PetType, PetMID, PetSID, PetCardMID, PetCardSID, PetStyle, PetColor, PetFull, PetSkinMID, PetSkinSID, PetSkinScale, PetSkinTime, PetSkinStartDate "
							"FROM PetInfo WHERE PetNum=%u And PetChaNum=%u",
							dwPetNum, dwClientID );

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

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return DB_ERROR;
	}

	SQLCHAR szPetName[PETNAMESIZE+1] = {0}; SQLINTEGER cbPetName = SQL_NTS;
	SQLINTEGER nOwnerNum = 0, cbOwnerNum = SQL_NTS;
	SQLINTEGER nType = 0, cbType = SQL_NTS;
	SQLINTEGER nMainID = 0, cbMainID = SQL_NTS;
	SQLINTEGER nSubID = 0, cbSubID = SQL_NTS;
	SQLINTEGER nPetCardMID = 0, cbPetCardMID = SQL_NTS;
	SQLINTEGER nPetCardSID = 0, cbPetCardSID = SQL_NTS;
	SQLINTEGER nStyle = 0, cbStyle = SQL_NTS;
	SQLINTEGER nColor = 0, cbColor = SQL_NTS;
	SQLINTEGER nFull = 0, cbFull = SQL_NTS;
	// Pet 스킨팩 추가
	SQLINTEGER nPetSkinMID = 0, cbPetSkinMID = SQL_NTS;
	SQLINTEGER nPetSkinSID = 0, cbPetSkinSID = SQL_NTS;
	SQLINTEGER nPetSkinScale = 0, cbPetSkinScale = SQL_NTS;
	SQLINTEGER nPetSkinTime = 0, cbPetSkinTime = SQL_NTS;
	TIMESTAMP_STRUCT sPetSkinStartDate; SQLINTEGER cbPetSkinStartDate   = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szPetName, PETNAMESIZE+1, &cbPetName);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nOwnerNum, 0, &cbOwnerNum);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nType, 0, &cbType);	
			::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nMainID, 0, &cbMainID);
			::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nSubID, 0, &cbSubID);
			::SQLGetData(pConn->hStmt, 6, SQL_C_LONG, &nPetCardMID, 0, &cbPetCardMID);
			::SQLGetData(pConn->hStmt, 7, SQL_C_LONG, &nPetCardSID, 0, &cbPetCardSID);
			::SQLGetData(pConn->hStmt, 8, SQL_C_LONG, &nStyle, 0, &cbStyle);
			::SQLGetData(pConn->hStmt, 9, SQL_C_LONG, &nColor, 0, &cbColor);
			::SQLGetData(pConn->hStmt, 10, SQL_C_LONG, &nFull, 0, &cbFull);
			::SQLGetData(pConn->hStmt, 11, SQL_C_LONG, &nPetSkinMID, 0, &cbPetSkinMID);
			::SQLGetData(pConn->hStmt, 12, SQL_C_LONG, &nPetSkinSID, 0, &cbPetSkinSID);
			::SQLGetData(pConn->hStmt, 13, SQL_C_LONG, &nPetSkinScale, 0, &cbPetSkinScale);
			::SQLGetData(pConn->hStmt, 14, SQL_C_LONG, &nPetSkinTime, 0, &cbPetSkinTime);
			::SQLGetData(pConn->hStmt, 15, SQL_C_TYPE_TIMESTAMP, &sPetSkinStartDate, 0, &cbPetSkinStartDate);

			if (cbPetName != 0 && cbPetName != -1)
				::StringCchCopy(pPet->m_szName, PETNAMESIZE+1, (const char*) szPetName);
			pPet->m_dwOwner								= (DWORD) nOwnerNum;
			pPet->m_emTYPE								= PETTYPE(nType);
			pPet->m_sPetID.wMainID						= (WORD) nMainID;
			pPet->m_sPetID.wSubID						= (WORD) nSubID;
			pPet->m_sPetCardID.wMainID					= (WORD) nPetCardMID;
			pPet->m_sPetCardID.wSubID					= (WORD) nPetCardSID;
			pPet->m_wStyle								= (WORD) nStyle;
			pPet->m_wColor								= (WORD) nColor;
			pPet->m_nFull								= nFull;
			pPet->m_sPetSkinPackData.dwPetSkinTime		= (DWORD) nPetSkinTime;
			pPet->m_sPetSkinPackData.sMobID.wMainID		= (WORD) nPetSkinMID;
			pPet->m_sPetSkinPackData.sMobID.wSubID		= (WORD) nPetSkinSID;
			pPet->m_sPetSkinPackData.fScale				= (float)nPetSkinScale / 100.0f;			
						
			// 프리미엄 만료기간
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sPetSkinStartDate.year <= 1970 || sPetSkinStartDate.year >= 2999)
			{
				sPetSkinStartDate.year = 1970;
				sPetSkinStartDate.month = 2;
				sPetSkinStartDate.day = 1;
				sPetSkinStartDate.hour = 1;
				sPetSkinStartDate.minute = 1;
				sPetSkinStartDate.second = 1;
			}
            CTime cTemp(sPetSkinStartDate.year, sPetSkinStartDate.month,  sPetSkinStartDate.day, 
                        sPetSkinStartDate.hour, sPetSkinStartDate.minute, sPetSkinStartDate.second);
            pPet->m_sPetSkinPackData.startTime   = cTemp.GetTime();

			if( pPet->m_sPetSkinPackData.dwPetSkinTime == 0 )
			{
				pPet->m_sPetSkinPackData.Init();
			}else {
				CTime currentTime = CTime::GetCurrentTime();
				CTimeSpan timeSpan = currentTime - pPet->m_sPetSkinPackData.startTime;
				if( timeSpan.GetTotalSeconds() >= pPet->m_sPetSkinPackData.dwPetSkinTime )
				{
					pPet->m_sPetSkinPackData.Init();
				}else{
					pPet->m_sPetSkinPackData.bUsePetSkinPack = TRUE;
				}
			}

			nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	m_pGameDB->FreeConnection(pConn);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			_T("ERROR:GetPet RowCount = 0, PetID = %d, CharID = %d"), dwPetNum, dwClientID );
		
		return DB_ROWCNT_ERROR;
	}

	// Pet 의 인벤토리를 가져온다.
	if ( GetPetInven( dwClientID, pPet, dwPetNum ) == DB_ERROR) return DB_ERROR;

	// Pet 의 스킬을 가져온다.
	if ( GetPetSkill( dwClientID, pPet, dwPetNum ) == DB_ERROR) return DB_ERROR;		

	return DB_OK;
}

/**
* 팻의 인벤토리 정보를 가져온다
*/
int COdbcManager::GetPetInven( 
    int nChaNum,
	GLPET* pPet,
	DWORD dwPetNum )
{
	if (pPet == NULL)
	{
		Print(_T("ERROR:COdbcManager::GetPetInven pPet==NULL"));
		return DB_ERROR;
	}

	CByteStream ByteStream;
	int nReturn = 0;

	// Pet PutOnItem
	//std::strstream strPutOnItems;
	//strPutOnItems << "SELECT PetInfo.PetPutOnItems FROM PetInfo WHERE (PetNum=" <<  dwPetNum << ")";
	//strPutOnItems << std::ends;
	nReturn = m_pGameDB->ReadImagePet("PetInfo.PetPutOnItems", nChaNum, dwPetNum, ByteStream);
	//strPutOnItems.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nReturn == DB_ERROR)
	{
		return DB_ERROR;
	}
	else 
	{
		SETPUTONITEMS_BYBUF( pPet->m_PutOnItems, ByteStream );
		return DB_OK;
	}
}

/**
* 팻의 스킬을 가져온다.
* \param pPet 펫의 포인터	
*/
int COdbcManager::GetPetSkill(
	int nChaNum,
	GLPET* pPet,
	DWORD dwPetNum)
{
	if (pPet == NULL) 
	{
		Print(_T("ERROR:COdbcManager::GetPetSkill pPet==NULL"));
		return DB_ERROR;
	}

	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT PetInvenMID, PetInvenSID, PetInvenAvailable ";
	//strTemp << " FROM PetInven WHERE PetNum=" << dwPetNum;
	//strTemp << " AND PetInvenType=" << DB_PET_INVEN_TYPE::SKILL; // PetInvenType=3 : 스킬
	//strTemp << " ORDER BY PetInvenNum";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT PetInvenMID, PetInvenSID, PetInvenAvailable "
							" FROM PetInven WHERE PetNum=%u"
							" AND PetInvenType=%d"
							" AND PetChaNum=%d"
							" ORDER BY PetInvenNum",
							dwPetNum,
							DB_PET_INVEN_TYPE::SKILL,
							nChaNum);

    SQLRETURN sReturn = 0;
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*) szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return DB_ERROR;
	}

	SQLINTEGER nAvailable = 0, cbAvailable = SQL_NTS;
	SQLINTEGER nMainID = 0, cbMainID = SQL_NTS;
	SQLINTEGER nSubID = 0, cbSubID = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(pConn->hStmt, 1, SQL_C_SHORT, &nMainID, 0, &cbMainID);
			::SQLGetData(pConn->hStmt, 2, SQL_C_SHORT, &nSubID, 0, &cbSubID);
			::SQLGetData(pConn->hStmt, 3, SQL_C_TINYINT, &nAvailable, 0, &cbAvailable);
            
			PETSKILL PetSkill;
			PetSkill.sNativeID.wMainID = (WORD) nMainID;
			PetSkill.sNativeID.wSubID  = (WORD) nSubID;
			
			
			// pPet->m_ExpSkills.insert ( PETSKILL_PAIR ( PetSkill.sNativeID.dwID, PetSkill ) );
			// pPet->m_ExpSkills[PetSkill.sNativeID.dwID] = PetSkill;
			pPet->m_ExpSkills.insert( std::make_pair( PetSkill.sNativeID.dwID, PetSkill ) );

			// 활성화된 스킬
			if (nAvailable == 1)
			{
				pPet->m_sActiveSkillID = PetSkill.sNativeID;
			}
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return DB_OK;
}

/**
* 팻의 포만감을 가져온다
* \param dwPetNum 팻 번호
* \return 팻의 포만감 or DB_ERROR
*/
int COdbcManager::GetPetFull( 
		int nChaNum, 
		DWORD dwPetNum )
{
	// sp_UpdatePetInven	
	//std::strstream strTemp;
	//strTemp << "{call sp_GetPetFull(";
	//strTemp << dwPetNum << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_GetPetFull(%d,%u,?)}", nChaNum, dwPetNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze(false); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;
}

/**
* 소멸된 팻의 리스트를 가져온다.
* \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
* \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
*/
int COdbcManager::GetRestorePetList(
	int nChaNum,
	std::vector<PET_RELIST> &vecPetReList )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	// 부활할 PET의 리스트를 가져온다.
	// PetNum, PetName, PetType의 정보를 DB Table에서 가져온다.
	//std::stringstream strTemp;
	//strTemp << "SELECT PetNum, PetName, PetType, PetCardMID, PetCardSID ";
	//strTemp << "From PetInfo Where PetChaNum=" << nChaNum;
	//strTemp << " And PetDeleted=1";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT PetNum, PetName, PetType, PetCardMID, PetCardSID "
							"From PetInfo Where PetChaNum=%d"
							" And PetDeleted=1",
							nChaNum );

	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
	
		return DB_ERROR;
	}

	SQLCHAR    szPetName[PETNAMESIZE+1] = {0}; SQLINTEGER cbPetName = SQL_NTS;
	SQLINTEGER nPetNum = 0, cbPetNum=SQL_NTS;
	SQLINTEGER nPetType = 0,	cbPetType=SQL_NTS;
	SQLINTEGER nPetCardMID = 0, cbPetCardMID=SQL_NTS;
	SQLINTEGER nPetCardSID = 0, cbPetCardSID=SQL_NTS;

	
	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(CString(szTemp));		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

            return DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			PET_RELIST sPetReList;

			// Bind data			
			::SQLGetData(pConn->hStmt,  1, SQL_C_LONG,	&nPetNum,                0, &cbPetNum);
			::SQLGetData(pConn->hStmt,  2, SQL_C_CHAR,  szPetName, PETNAMESIZE+1, &cbPetName);
			::SQLGetData(pConn->hStmt,  3, SQL_C_LONG,	&nPetType,				 0, &cbPetType);
			::SQLGetData(pConn->hStmt,  4, SQL_C_LONG,	&nPetCardMID,            0, &cbPetCardMID);
			::SQLGetData(pConn->hStmt,  5, SQL_C_LONG,	&nPetCardSID,            0, &cbPetCardSID);

			 if (cbPetName != 0 && cbPetName != -1)
				::StringCchCopy(sPetReList.szPetName, PETNAMESIZE+1, (const TCHAR*) szPetName);

			 sPetReList.nPetNum = nPetNum;
			 sPetReList.emType = (PETTYPE) nPetType;
			 sPetReList.sPetCardID.wMainID = (WORD) nPetCardMID;
			 sPetReList.sPetCardID.wSubID = (WORD) nPetCardSID;

			 vecPetReList.push_back( sPetReList );
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}	  
	m_pGameDB->FreeConnection(pConn);

	return DB_OK;
}

/**
* 팻을 부활시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
*/
int COdbcManager::RestorePet( DWORD dwPetNum, int nChaNum )
{	
	if (dwPetNum<=0 )
	{
		Print(_T("ERROR:COdbcManager::RestorePet"));
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "{call sp_RestorePet(";
	//strTemp << dwPetNum << ",";
	//strTemp << nChaNum << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_RestorePet(%u,%d,?)}", dwPetNum, nChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt( szTemp );
	//strTemp.freeze( false ); // Note : std::strstream의 freeze. 안 하면 Leak 발생.
	return nReturn;	
}

/**
* 팻의 스킨팩을 업데이트 한다.
* \param nChaNum		팻소유 캐릭터 번호
* \param dwPetNum		펫 번호
* \param nPetSkinMID	펫 스킨 MID
* \param nPetSkinSID	펫 스킨 SID
* \param nPetSkinScale	펫 스킨 Scale
* \param nPetSkinTime	펫 스킨 적용시간
*/
int COdbcManager::UpdatePetSkin( int nChaNum,
								 DWORD dwPetNum,
								 int nPetSkinMID,
								 int nPetSkinSID,
								 int nPetSkinScale,
								 int nPetSkinTime )
{
	if( dwPetNum <= 0 || nChaNum <= 0 )
	{
		Print(_T("ERROR:COdbcManager::UpdatePetSkin") );
		return DB_ERROR;
	}

	TCHAR szTemp[128];
	_snprintf_s( szTemp, 128, "{call sp_UpdatePetSkin(%d,%u,%d,%d,%d,%d,?)}",
		nChaNum,
		dwPetNum,
		nPetSkinMID,
		nPetSkinSID,
		nPetSkinScale,
		nPetSkinTime );

	int nReturn = m_pGameDB->ExecuteSpInt( szTemp );

	return nReturn;																			
}