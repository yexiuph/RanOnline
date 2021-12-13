#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 새로운 클럽을 생성한다.
* \param strClubName 클럽이름
* \param dwChaNum 클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)
*/
int COdbcManager::CreateClub(
	CString strClubName,
	DWORD dwChaNum )
{    
	//std::strstream strTemp;
	//strTemp << "{call sp_create_guild(";
	//strTemp << dwChaNum;
 //   strTemp << ",'" << strClubName.GetString() << "', ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_create_guild(%u,'%s',?)}",
							dwChaNum,
							strClubName.GetString() );
	
	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 클럽을 삭제한다.
* \param dwClub 클럽번호
* \param dwChaNum 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
*/
int COdbcManager::DeleteClub(
	DWORD dwClub,
	DWORD dwChaNum )
{    
	//std::strstream strTemp;
	//strTemp << "{call sp_delete_guild(";
	//strTemp << dwClub   << ",";
 //   strTemp << dwChaNum << ", ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_delete_guild(%u,%u,?)}", dwClub, dwChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	return nReturn;
}

/**
* 클럽랭킹을 세팅한다.
* \param dwClub : 클럽번호
* \param dwRank : 랭킹
*/
int COdbcManager::SetClubRank(
	DWORD dwClub,
	DWORD dwRank )
{
	//std::strstream strTemp;
	//strTemp << "{call sp_update_guild_rank(";
	//strTemp << dwClub << ",";
 //   strTemp << dwRank << ", ?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_update_guild_rank(%u,%u,?)}", dwClub, dwRank );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 클럽 해체시간을 설정한다.
* \param dwClub 클럽번호
* \param tDiss 해체시간
* \return 
*/
int COdbcManager::SetClubDissolutionTime(
	DWORD dwClub,
	__time64_t tDiss )
{    
    CTime cTemp(tDiss);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    //std::strstream strTemp;
    //strTemp << "UPDATE GuildInfo SET ";
    //strTemp << "GuExpireTime='" << strTime.GetString() << "'";    
    //strTemp << " WHERE GuNum=" << dwClub;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuExpireTime='%s' WHERE GuNum=%u",
							strTime.GetString(),
							dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

/**
* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
* \param dwClub 클럽번호
* \param tSec 시간
* \return 
*/
int COdbcManager::SetClubAllianceSec(
	DWORD dwClub,
	__time64_t tSec )
{
	CTime cTemp(tSec);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    //std::strstream strTemp;
    //strTemp << "UPDATE GuildInfo SET ";
    //strTemp << "GuAllianceSec='" << strTime.GetString() << "'";
    //strTemp << " WHERE GuNum=" << dwClub;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuAllianceSec='%s' WHERE GuNum=%u",
							strTime.GetString(),
							dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

/**
* 클럽의 동맹해산 시간을 정한다.
* \param dwClub 클럽번호
* \param tDis 해산시간
* \return 
*/
int COdbcManager::SetClubAllianceDis(
	DWORD dwClub,
	__time64_t tDis )
{
	CTime cTemp(tDis);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    //std::strstream strTemp;
    //strTemp << "UPDATE GuildInfo SET ";
    //strTemp << "GuAllianceDis='" << strTime.GetString() << "'";    
    //strTemp << " WHERE GuNum=" << dwClub;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuAllianceDis='%s' WHERE GuNum=%u",
							strTime.GetString(),
							dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

/**
* 클럽의 마스터위임 시간을 정한다.
* \param dwClub 클럽번호
* \param tDis 마스터 위임시간
* \return 
*/
int COdbcManager::SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority)
{
	CTime cTemp(tAuthority);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuAuthorityTime='%s' WHERE GuNum=%u",
							strTime.GetString(),
							dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);

    return nReturn;
}

/**
* 새로운 캐릭터를 클럽에 가입시킨다
* \param dwClub 클럽번호
* \param dwChaNum 가입시키려는 캐릭터번호
*/
int COdbcManager::AddClubMember(
	DWORD dwClub,
	DWORD dwChaNum )
{
	//std::strstream strTemp;
	//strTemp << "{call sp_add_guild_member(";
	//strTemp << dwClub   << ",";
 //   strTemp << dwChaNum << ", ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_add_guild_member(%u,%u,?)}", dwClub, dwChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
* \param dwChaNum 캐릭터번호
*/
int COdbcManager::DeleteClubMember(DWORD dwChaNum)
{
	//std::strstream strTemp;
	//strTemp << "{call sp_delete_guild_member(";
 //   strTemp << dwChaNum << ", ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_delete_guild_member(%u,?)}", dwChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 해당 클럽의 멤버를 가져온다
* dwGuild : 클럽번호
* vMember : 클럽멤버
*/
int COdbcManager::GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER> &vMember)
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT ChaNum, ChaName, GuPosition FROM viewGuildMember ";
	//strTemp << "WHERE GuNum=" << dwClub;
 //   strTemp << " ORDER BY ChaName";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT ChaNum, ChaName, GuPosition FROM viewGuildMember "
							"WHERE GuNum=%u ORDER BY ChaName", dwClub );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaNum     = 0, cbChaNum     = SQL_NTS;
	SQLINTEGER nGuPosition = 0, cbGuPosition = SQL_NTS;
	
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
			GLCLUBMEMBER sMember;

			// Bind data			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,	  &nChaNum,             0, &cbChaNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_CHAR,    szChaName, CHR_ID_LENGTH+1, &cbChaName);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nGuPosition,             0, &cbGuPosition);

			if (cbChaName != 0 && cbChaName != -1)
				::StringCchCopy(sMember.m_szName, CHR_ID_LENGTH, (const TCHAR*) szChaName);
			sMember.dwID      = (DWORD) nChaNum;
			sMember.m_dwFlags = (DWORD) nGuPosition;			

			vMember.push_back(sMember);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	m_pGameDB->FreeConnection(pConn);
	return DB_OK;
}

/**
* 서버의 모든 클럽정보를 가져온다.
* \param &vClub 클럽정보
* \return 
*/
int COdbcManager::GetClubInfo(std::vector<GLCLUBINFO> &vClub)
{
    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

// 클럽 배틀 시스템 추가로 인해서 GuildInfo Table 컬럼 추가
// GuildInfo Table 컬럼 추가, viewGuildInfo 수정 필요
	SQLINTEGER nGuBattleWin    = 0, cbGuBattleWin=SQL_NTS;
    SQLINTEGER nGuBattleDraw   = 0, cbGuBattleDraw=SQL_NTS;
	SQLINTEGER nGuBattleLose   = 0, cbGuBattleLose=SQL_NTS;

	SQLINTEGER nGuAllianceBattleWin    = 0, cbGuAllianceBattleWin=SQL_NTS;
    SQLINTEGER nGuAllianceBattleDraw   = 0, cbGuAllianceBattleDraw=SQL_NTS;
	SQLINTEGER nGuAllianceBattleLose   = 0, cbGuAllianceBattleLose=SQL_NTS;

	TIMESTAMP_STRUCT sBattleLastTime;    SQLINTEGER cbBattleLastTime=SQL_NTS;

	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "SELECT GuNum, ChaNum, GuDeputy, ChaName, GuName, "
							"GuRank, GuMoney, GuIncomeMoney, GuMarkVer, GuBattleWin, "
							"GuBattleDraw, GuBattleLose, GuBattleLastTime, GuAllianceBattleWin, "
							"GuAllianceBattleDraw, GuAllianceBattleLose, GuMakeTime, "
							"GuExpireTime, GuAuthorityTime, GuAllianceSec, GuAllianceDis, GuNotice FROM viewGuildInfo" );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		
		return DB_ERROR;
	}

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0};    SQLINTEGER cbChaName  = SQL_NTS;
	SQLCHAR    szGuName[CHR_ID_LENGTH+1] = {0};     SQLINTEGER cbGuName   = SQL_NTS;
	SQLCHAR    szGuNotice[EMCLUB_NOTICE_LEN+1]= {0};  SQLINTEGER cbGuNotice = SQL_NTS;

	SQLINTEGER nGuNum    = 0, cbGuNum=SQL_NTS;
    SQLINTEGER nChaNum   = 0, cbChaNum=SQL_NTS;
	SQLINTEGER nGuDeputy = 0, cbGuDeputy=SQL_NTS;
    SQLINTEGER nGuRank   = 0, cbGuRank=SQL_NTS;
    SQLINTEGER nGuMarkVer= 0, cbGuMarkVer=SQL_NTS;

    TIMESTAMP_STRUCT sMakeTime;    SQLINTEGER cbMakeTime    = SQL_NTS;
    TIMESTAMP_STRUCT sExpireTime;  SQLINTEGER cbExpireTime  = SQL_NTS;
    TIMESTAMP_STRUCT sAuthorityTime;  SQLINTEGER cbAuthorityTime  = SQL_NTS;
	TIMESTAMP_STRUCT sAllianceSec; SQLINTEGER cbAllianceSec = SQL_NTS;
	TIMESTAMP_STRUCT sAllianceDis; SQLINTEGER cbAllianceDis = SQL_NTS;

    LONGLONG   llGuildMoney = 0; SQLINTEGER cbGuildMoney = SQL_NTS;
	LONGLONG   llGuildIncomeMoney = 0; SQLINTEGER cbGuildIncomeMoney = SQL_NTS;

    DWORD dwSize = (DWORD) (sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);
    CByteStream ByteStream;
	
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
			GLCLUBINFO sCLUB;

			// Bind data			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,	        &nGuNum,                    0, &cbGuNum);
            ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	        &nChaNum,                   0, &cbChaNum);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	        &nGuDeputy,                 0, &cbGuDeputy);
			::SQLGetData(pConn->hStmt, 4, SQL_C_CHAR,           szChaName,      CHR_ID_LENGTH, &cbChaName);
            ::SQLGetData(pConn->hStmt, 5, SQL_C_CHAR,           szGuName,       CHR_ID_LENGTH, &cbGuName);

            ::SQLGetData(pConn->hStmt, 6, SQL_C_LONG,	        &nGuRank,                   0, &cbGuRank);            
            ::SQLGetData(pConn->hStmt, 7, SQL_C_SBIGINT,        &llGuildMoney,              0, &cbGuildMoney);
			::SQLGetData(pConn->hStmt, 8, SQL_C_SBIGINT,        &llGuildIncomeMoney,        0, &cbGuildIncomeMoney);
            ::SQLGetData(pConn->hStmt, 9, SQL_C_LONG,	        &nGuMarkVer,                0, &cbGuMarkVer);

			// 클럽 배틀 추가
			::SQLGetData(pConn->hStmt,10, SQL_C_LONG,	        &nGuBattleWin,              0, &cbGuBattleWin);
			::SQLGetData(pConn->hStmt,11, SQL_C_LONG,	        &nGuBattleDraw,             0, &cbGuBattleDraw);
			::SQLGetData(pConn->hStmt,12, SQL_C_LONG,	        &nGuBattleLose,             0, &cbGuBattleLose);
			::SQLGetData(pConn->hStmt,13, SQL_C_TYPE_TIMESTAMP, &sBattleLastTime,           0, &cbBattleLastTime);

			::SQLGetData(pConn->hStmt,14, SQL_C_LONG,	        &nGuAllianceBattleWin,      0, &cbGuAllianceBattleWin);
			::SQLGetData(pConn->hStmt,15, SQL_C_LONG,	        &nGuAllianceBattleDraw,     0, &cbGuAllianceBattleDraw);
			::SQLGetData(pConn->hStmt,16, SQL_C_LONG,	        &nGuAllianceBattleLose,     0, &cbGuAllianceBattleLose);

            ::SQLGetData(pConn->hStmt,17, SQL_C_TYPE_TIMESTAMP, &sMakeTime,                 0, &cbMakeTime);
            ::SQLGetData(pConn->hStmt,18, SQL_C_TYPE_TIMESTAMP, &sExpireTime,               0, &cbExpireTime);
            ::SQLGetData(pConn->hStmt,19, SQL_C_TYPE_TIMESTAMP, &sAuthorityTime,			0, &cbAuthorityTime);
			::SQLGetData(pConn->hStmt,20, SQL_C_TYPE_TIMESTAMP, &sAllianceSec,              0, &cbAllianceSec);
			::SQLGetData(pConn->hStmt,21, SQL_C_TYPE_TIMESTAMP, &sAllianceDis,              0, &cbAllianceDis);
			::SQLGetData(pConn->hStmt,22, SQL_C_CHAR,           szGuNotice, EMCLUB_NOTICE_LEN+1, &cbGuNotice);
            
            sCLUB.m_dwID = (DWORD) nGuNum;
            sCLUB.m_dwMasterID = (DWORD) nChaNum; // 선도클럽 인증 대리자 Default(0)
			sCLUB.m_dwCDCertifior = (DWORD) nGuDeputy; // 선도클럽 인증 대리자 Default(0)            
            
            if (cbGuName != 0 && cbGuName != -1)
			{
				CString strGuName(szGuName);
				strGuName.Trim(_T(" ")); // 앞뒤 공백제거
                ::StringCchCopy(
					sCLUB.m_szName,
					CHR_ID_LENGTH,
					(const TCHAR*) strGuName.GetString() );
			}
            
            if (cbChaName != 0 && cbChaName != -1)
			{
				CString strChaName(szChaName);
				strChaName.Trim(_T(" ")); // 앞뒤 공백제거
			    ::StringCchCopy(
					sCLUB.m_szMasterName,
					CHR_ID_LENGTH,
					(const TCHAR*) strChaName.GetString() );
			}
			
            if (cbGuNotice != 0 && cbGuNotice != -1)
			{
				CString strGuNotice(szGuNotice);
				strGuNotice.Trim(_T(" ")); // 앞뒤 공백제거
				strGuNotice.Replace("''", "'"); // '' -> '
				::StringCchCopy(
					sCLUB.m_szNotice,
					EMCLUB_NOTICE_LEN+1,
					(const TCHAR*) strGuNotice.GetString() );
			}
	        
            sCLUB.m_dwRank = (DWORD) nGuRank;
            sCLUB.m_lnStorageMoney = llGuildMoney;
			sCLUB.m_lnIncomeMoney  = llGuildIncomeMoney;
	        sCLUB.m_dwMarkVER = (DWORD) nGuMarkVer;

			// 클럽 배틀 추가
			sCLUB.m_dwBattleWin = (DWORD) nGuBattleWin;
			sCLUB.m_dwBattleDraw = (DWORD) nGuBattleDraw;
			sCLUB.m_dwBattleLose = (DWORD) nGuBattleLose;

			sCLUB.m_dwAllianceBattleWin = (DWORD) nGuAllianceBattleWin;
			sCLUB.m_dwAllianceBattleDraw = (DWORD) nGuAllianceBattleDraw;
			sCLUB.m_dwAllianceBattleLose = (DWORD) nGuAllianceBattleLose;

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sBattleLastTime.year < 1970 || sBattleLastTime.year >= 2999)
			{
				CString strTimeTemp1;
				strTimeTemp1.Format( _T("ERROR:sCLUB.sBattleLastTime year %d"), sBattleLastTime.year );
				Print( strTimeTemp1 );

				sBattleLastTime.year = 1970;
				sBattleLastTime.month = 2;
				sBattleLastTime.day = 1;
				sBattleLastTime.hour = 1;
				sBattleLastTime.minute = 1;
				sBattleLastTime.second = 1;
			}
            CTime cTemp1(sBattleLastTime.year, sBattleLastTime.month,  sBattleLastTime.day, 
                         sBattleLastTime.hour, sBattleLastTime.minute, sBattleLastTime.second);
            sCLUB.m_tLastBattle = cTemp1.GetTime();

             
            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sMakeTime.year < 1970 || sMakeTime.year >= 2999)
			{
				CString strTimeTemp2;
				strTimeTemp2.Format( _T("ERROR:sCLUB.sMakeTime year %d"), sMakeTime.year );
				Print( strTimeTemp2 );

				sMakeTime.year = 1970;
				sMakeTime.month = 2;
				sMakeTime.day = 1;
				sMakeTime.hour = 1;
				sMakeTime.minute = 1;
				sMakeTime.second = 1;
			}
            CTime cTemp2(sMakeTime.year, sMakeTime.month,  sMakeTime.day, 
                         sMakeTime.hour, sMakeTime.minute, sMakeTime.second);
            sCLUB.m_tOrganize = cTemp2.GetTime();
            
			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sExpireTime.year < 1970 || sExpireTime.year >= 2999)
			{
				CString strTimeTemp3;
				strTimeTemp3.Format( _T("ERROR:sCLUB.m_tDissolution year %d"), sExpireTime.year );
				Print( strTimeTemp3 );

				sExpireTime.year = 1970;
				sExpireTime.month = 2;
				sExpireTime.day = 1;
				sExpireTime.hour = 1;
				sExpireTime.minute = 1;
				sExpireTime.second = 1;
			}
            CTime cTemp3(sExpireTime.year, sExpireTime.month,  sExpireTime.day, 
                         sExpireTime.hour, sExpireTime.minute, sExpireTime.second);
            sCLUB.m_tDissolution = cTemp3.GetTime();

            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAllianceSec.year < 1970 || sAllianceSec.year >= 2999)
			{
				CString strTimeTemp4;
				strTimeTemp4.Format( _T("ERROR:sCLUB.m_tAllianceSec year %d"), sAllianceSec.year );
				Print( strTimeTemp4 );

				sAllianceSec.year = 1970;
				sAllianceSec.month = 2;
				sAllianceSec.day = 1;
				sAllianceSec.hour = 1;
				sAllianceSec.minute = 1;
				sAllianceSec.second = 1;
			}
			CTime cTemp4(sAllianceSec.year, sAllianceSec.month,  sAllianceSec.day,
				         sAllianceSec.hour, sAllianceSec.minute, sAllianceSec.second);
			sCLUB.m_tAllianceSec = cTemp4.GetTime();

            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAllianceDis.year < 1970 || sAllianceDis.year >= 2999)
			{
				CString strTimeTemp5;
				strTimeTemp5.Format( _T("ERROR:sCLUB.m_tAllianceDis year %d"), sAllianceDis.year );
				Print( strTimeTemp5 );

				sAllianceDis.year = 1970;
				sAllianceDis.month = 2;
				sAllianceDis.day = 1;
				sAllianceDis.hour = 1;
				sAllianceDis.minute = 1;
				sAllianceDis.second = 1;
			}
			CTime cTemp5(sAllianceDis.year, sAllianceDis.month,  sAllianceDis.day,
				         sAllianceDis.hour, sAllianceDis.minute, sAllianceDis.second);
			sCLUB.m_tAllianceDis = cTemp5.GetTime();

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sAuthorityTime.year < 1970 || sAuthorityTime.year >= 2999)
			{
				CString strTimeTemp6;
				strTimeTemp6.Format( _T("ERROR:sCLUB.m_tAuthority year %d"), sAuthorityTime.year );
				Print( strTimeTemp6 );

				sAuthorityTime.year = 1970;
				sAuthorityTime.month = 2;
				sAuthorityTime.day = 1;
				sAuthorityTime.hour = 1;
				sAuthorityTime.minute = 1;
				sAuthorityTime.second = 1;
			}
            CTime cTemp6(sAuthorityTime.year, sAuthorityTime.month,  sAuthorityTime.day, 
                         sAuthorityTime.hour, sAuthorityTime.minute, sAuthorityTime.second);
            sCLUB.m_tAuthority = cTemp6.GetTime();
            
			ByteStream.ClearBuffer();
	        SQLRETURN sReturn2 = ReadClubMarkImage(sCLUB.m_dwID, ByteStream);
	        if (sReturn2 == DB_ERROR)
            {
            }
            else
            {
				LPBYTE pBuff(NULL);
                ByteStream.GetBuffer ( pBuff, dwSize);				
				
				memcpy ( sCLUB.m_aryMark, pBuff, dwSize );

				ByteStream.ClearBuffer();
			}
            vClub.push_back(sCLUB);
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

///////////////////////////////////////////////////////////////////////////
// 해당 클럽의 클럽마크 이미지를 읽어온다
int COdbcManager::ReadClubMarkImage(DWORD dwClub, CByteStream &ByteStream)
{
    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	ByteStream.ClearBuffer();
			
	//std::strstream strTemp;
	//strTemp << "SELECT GuildInfo.GuMarkImage FROM GuildInfo where (GuNum=" <<  dwClub << ")";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuildInfo.GuMarkImage FROM GuildInfo where (GuNum=%u)", dwClub );

	// Create a result
	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	// Retrieve and display each row of data.
	BYTE pBuffer[DB_IMAGE_BUF_SIZE] = {0};
	SQLINTEGER  lSize=0, lTotalSize=0;

	while ((sReturn = ::SQLFetch(pConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::ZeroMemory(pBuffer, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(pConn->hStmt, 1, SQL_C_BINARY, pBuffer, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBuffer, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
			Sleep( 0 );
		}
		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);
	return lTotalSize;
}

///////////////////////////////////////////////////////////////////////////
// 클럽의 공지사항을 세팅한다.
int COdbcManager::SetClubNotice(
	DWORD dwClub, 
	const TCHAR* szClubNotice )
{
    if ((szClubNotice == NULL) || 
		(strlen(szClubNotice) > EMCLUB_NOTICE_LEN+1)) 
	{
		Print("szClubNotice NULL or large than EMCLUB_NOTICE_LEN");
		return DB_ERROR;
	}

	//std::strstream strTemp;
	//strTemp << "UPDATE GuildInfo SET GuNotice='";
	//strTemp << szNotice << "  ' ";
	//strTemp << " WHERE GuNum=" << dwClub;
	//strTemp << std::ends;

	TCHAR szTemp[1024] = {0};
	_snprintf_s( szTemp, 1024, "UPDATE GuildInfo SET GuNotice='%s' WHERE GuNum=%u", szClubNotice, dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 해당 클럽의 클럽마크 이미지를 저장한다.
int	COdbcManager::WriteClubMarkImage(
	DWORD dwClub,
	DWORD dwMarkVer,
	BYTE* pData, 
	int nSize )
{    
    if ((pData == NULL) || (nSize < 0) || (nSize > EMCLUB_MARK_SIZE))
        return DB_ERROR;

 //   std::strstream strTempVer;
	//strTempVer << "UPDATE GuildInfo SET GuMarkVer=";
 //   strTempVer << dwMarkVer;
 //   strTempVer << " WHERE GuNum=" << dwClub;
 //   strTempVer << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuMarkVer=%u WHERE GuNum=%u", dwMarkVer, dwClub );

    m_pGameDB->ExecuteSQL(szTemp);

	//strTempVer.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    // Update Image
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "UPDATE GuildInfo SET GuMarkImage = ? WHERE (GuNum=" <<  dwClub << ") ";
	//strTemp << std::ends;

	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuMarkImage=? WHERE (GuNum=%u)", dwClub );

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
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
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(pConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return DB_ERROR;
	}
	
	SQLPOINTER pToken = NULL;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(pConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(pConn->hStmt, pData, (SQLINTEGER) nSize);
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);
	return DB_OK;
}

///////////////////////////////////////////////////////////////////////////
// 캐릭터의 클럽번호를 가져온다.
// dwChaNum : 캐릭터번호
int COdbcManager::GetChaGuildNum(DWORD dwChaNum)
{
    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT GuNum FROM ChaInfo WHERE ChaNum=" << dwChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuNum FROM ChaInfo WHERE ChaNum=%u", dwChaNum );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	SQLINTEGER nGuNum  = 0, cbGuNum=SQL_NTS;
    int nReturn = 0;
    
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
			// Bind data			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nGuNum, 0, &cbGuNum);
            nReturn = nGuNum;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);
	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 수입금액을 세팅한다. (Storage)
// dwClub : 클럽번호
// llMoney : 수입금액 (금액은 >= 0)
int COdbcManager::SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney)
{
	if (llMoney < 0)
    {
		Print("SetClubIncomeMoney llMoney Must be large than zero");
        return DB_ERROR;
    }
    
    //std::strstream strTemp;
    //strTemp << "Update GuildInfo Set GuIncomeMoney=";
    //strTemp << llMoney;
    //strTemp << " WHERE GuNum=" << dwClub;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Update GuildInfo Set GuIncomeMoney=%I64d WHERE GuNum=%u", llMoney, dwClub );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
    
    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 수입금액을 가져온다. (Storage)
// dwClub : 클럽번호
// 리턴값은 >= 0, 0 보다 작으면 ERROR
LONGLONG COdbcManager::GetClubIncomeMoney(DWORD dwClub)
{
	if (dwClub < 1)
    {
		Print(_T("GetClubIncomeMoney dwClub is must large than zero"));
        return DB_ERROR;
    }

    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT GuIncomeMoney From GuildInfo ";
	//strTemp << "WHERE GuNum=" << dwClub;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuIncomeMoney From GuildInfo WHERE GuNum=%u", dwClub );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	LONGLONG   llGuMoney = 0;
    SQLINTEGER cbGuMoney = SQL_NTS;
	
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
			// Bind data
            ::SQLGetData(pConn->hStmt, 1, SQL_C_SBIGINT, &llGuMoney, 0, &cbGuMoney);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);
	return llGuMoney;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 보유금액을 세팅한다.
// dwClub : 클럽번호
// llMoney : 보유금액 (금액은 >= 0)
int COdbcManager::SetClubMoney(DWORD dwClub, LONGLONG llMoney)
{
	if (llMoney < 0)
    {
		Print("SetClubMoney llMoney Must be large than zero");
        return DB_ERROR;
    }
    
    //std::strstream strTemp;
    //strTemp << "Update GuildInfo Set GuMoney=";
    //strTemp << llMoney;
    //strTemp << " WHERE GuNum=" << dwClub;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Update GuildInfo Set GuMoney=%I64d WHERE GuNum=%u", llMoney, dwClub );
    
	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 보유금액을 가져온다.
// dwClub : 클럽번호
// 리턴값은 >= 0, 0 보다 작으면 ERROR
LONGLONG COdbcManager::GetClubMoney(DWORD dwClub)
{
	if (dwClub < 1)
    {
		Print(_T("GetClubMoney dwClub is must large than zero"));
        return DB_ERROR;
    }

    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT GuMoney From GuildInfo ";
	//strTemp << "WHERE GuNum=" << dwClub;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuMoney From GuildInfo WHERE GuNum=%u", dwClub );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	LONGLONG   llGuMoney = 0;
    SQLINTEGER cbGuMoney = SQL_NTS;
	
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
			// Bind data
            ::SQLGetData(pConn->hStmt, 1, SQL_C_SBIGINT, &llGuMoney, 0, &cbGuMoney);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);
	return llGuMoney;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 클럽창고를 읽어온다.
// dwClub : 클럽번호
// ByteStream : 버퍼
int COdbcManager::ReadClubStorage(DWORD dwClub, CByteStream &ByteStream)
{
	if (dwClub < 1)
	{
		Print(_T("ReadClubStorage dwClub must be large than zero"));
		return DB_ERROR;
	}

    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	ByteStream.ClearBuffer();
			
	//std::strstream strTemp;
	//strTemp << "SELECT GuildInfo.GuStorage FROM GuildInfo where (GuNum=" <<  dwClub << ")";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuildInfo.GuStorage FROM GuildInfo where (GuNum=%u)", dwClub );

	// Create a result
	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	// Retrieve and display each row of data.
	BYTE pBuffer[DB_IMAGE_BUF_SIZE] = {0};
	SQLINTEGER  lSize=0, lTotalSize=0;

	while ((sReturn = ::SQLFetch(pConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::ZeroMemory(pBuffer, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(pConn->hStmt, 1, SQL_C_BINARY, pBuffer, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBuffer, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
			Sleep( 0 );
		}
		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);
	return lTotalSize;
}

///////////////////////////////////////////////////////////////////////////
// 해당클럽의 클럽창고를 저장한다.
// dwClub : 클럽번호
// pData : 창고위치
// nSize : 버퍼 전체크기
int COdbcManager::WriteClubStorage(
	DWORD dwClub,
	BYTE* pData,
	int nSize )
{
    if ((pData == NULL) || (nSize < 0) || (dwClub < 1))
	{
		Print(_T("WriteClubStorage (pData, nSize, dwClub) invalid argument"));
        return DB_ERROR;
	}   

    // Update Image
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "UPDATE GuildInfo SET GuStorage = ? WHERE (GuNum=" <<  dwClub << ") ";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE GuildInfo SET GuStorage=? WHERE (GuNum=%u)", dwClub );

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
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
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(pConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return DB_ERROR;
	}
	
	SQLPOINTER pToken = NULL;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(pConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{
			::SQLPutData(pConn->hStmt, pData, (SQLINTEGER) nSize);
		}
		Sleep( 0 );
	}

	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	m_pGameDB->FreeConnection(pConn);
	return DB_OK;
}

///////////////////////////////////////////////////////////////////////////
// 부 클럽장을 세팅한다
// dwClub : 클럽번호
// dwChaNum : 캐릭터번호
// dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
int COdbcManager::SetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags)
{
	if (dwClub == 0 || dwChaNum <= 0)
	{
		return DB_ERROR;
	}

	//std::strstream strTemp;
 //   strTemp << "UPDATE ChaInfo SET GuPosition=";
 //   strTemp << dwSubMasterFlags;
 //   strTemp << " WHERE GuNum=" << dwClub << " AND ChaNum=" << dwChaNum;
 //   strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "UPDATE ChaInfo SET GuPosition=%u"
							" WHERE GuNum=%u AND ChaNum=%u",
							dwSubMasterFlags, dwClub, dwChaNum );
    
	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 클럽의 선도클럽 인증 대리자를 세팅한다.
// dwClub : 클럽번호
// dwDeputy : 선도클럽 인증 대리자 캐릭터번호
int COdbcManager::SetClubDeputy(DWORD dwClub, DWORD dwDeputy)
{
	//std::strstream strTemp;
 //   strTemp << "Update GuildInfo WITH (UPDLOCK) Set GuDeputy=";
 //   strTemp << dwDeputy;
 //   strTemp << " WHERE GuNum=" << dwClub;
 //   strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Update GuildInfo WITH (UPDLOCK) Set GuDeputy=%u WHERE GuNum=%u", dwDeputy, dwClub );
    
	int nReturn = m_pGameDB->ExecuteSQL(szTemp);	
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}


///////////////////////////////////////////////////////////////////////////
// 클럽의 마스터 권한을 위임한다.
// dwClub : 클럽번호
// dwDeputy : 선도클럽 마스터가 될 캐릭터 번호
int COdbcManager::SetClubAuthority (DWORD dwClub, DWORD dwMasterID)
{
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Update GuildInfo WITH (UPDLOCK) Set ChaNum=%u WHERE GuNum=%u", dwMasterID, dwClub );
    
	int nReturn = m_pGameDB->ExecuteSQL(szTemp);	
    return nReturn;
}


///////////////////////////////////////////////////////////////////////////
// 현재 모든지역의 선도 클럽데이터를 가져온다.    
int COdbcManager::GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB)
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::string strTemp;
	//strTemp = "SELECT RegionID, GuNum, RegionTax FROM GuildRegion WITH (NOLOCK) ";	
 //   strTemp += " ORDER BY RegionID";

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT RegionID, GuNum, RegionTax FROM GuildRegion WITH (NOLOCK) ORDER BY RegionID" ); 
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		return DB_ERROR;
	}
		
	SQLINTEGER   nRegionID  = 0; SQLINTEGER cbRegionID  = SQL_NTS;
	SQLINTEGER   nGuNum     = 0; SQLINTEGER cbGuNum     = SQL_NTS;
	SQLFLOAT     fRegionTax = 0; SQLINTEGER cbRegionTax = SQL_NTS;
		
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
			GLGUID_DB sGUID_DB;

			// Bind data			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,	&nRegionID,  0, &cbRegionID);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nGuNum,     0, &cbGuNum);
			::SQLGetData(pConn->hStmt, 3, SQL_C_DOUBLE,	&fRegionTax, 0, &cbRegionTax);			
			
			sGUID_DB.m_dwID           = (DWORD) nRegionID;
			sGUID_DB.m_dwCLUBID       = (DWORD) nGuNum;
			sGUID_DB.m_fCommissionPer = (float) fRegionTax;

			vGUID_DB.push_back(sGUID_DB);
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
    
///////////////////////////////////////////////////////////////////////////
// 지역과 그지역을 소유한 클럽 세율을 정한다.
// dwRegionID : 지역 ID
// dwClub : 클럽 ID
// fTax : 세율
int COdbcManager::SetClubRegion(DWORD dwRegionID, DWORD dwClub, float fTax)
{
	//std::strstream strTemp;
	//strTemp << "{call sp_add_guild_region(";
	//strTemp << dwRegionID   << ",";
 //   strTemp << dwClub << ",";
 //   strTemp << fTax << ", ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_add_guild_region(%u,%u,%f,?)}", dwRegionID, dwClub, fTax );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 해당 지역의 소유 상태를 리셋한다.
// dwRegionID : 지역 ID
// dwClub : 클럽 ID (기본 0 이다)    
int COdbcManager::DelClubRegion(DWORD dwRegionID, DWORD dwClub)
{
	//std::strstream strTemp;
	//strTemp << "{call sp_delete_guild_region(";
	//strTemp << dwRegionID   << ",";
 //   strTemp << dwClub << ", ?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_delete_guild_region(%u,%u,?)}", dwRegionID, dwClub );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽을 결성한다.
// dwClubP : 주 클럽번호 (동맹 클럽의 주)
// dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
// * 주의 *
// db 는 클럽의 무결성을 체크하지 않는다.
// A 가 B 의 보조인 상태에서 
// B 가 C 의 보조로 들어가고
// C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.
int	COdbcManager::SetClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
	if (dwClubP == 0 || dwClubS == 0)
	{
		Print(_T("ERROR:SetClubAlliance:Check Club Number"));
		return DB_ERROR;
	} // if (dwClubP == 0 || dwClubS == 0)

	//std::strstream strTemp;
	//strTemp << "{call InsertGuildAlliance(";
	//strTemp << dwClubP << ",";
	//strTemp << dwClubS << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call InsertGuildAlliance(%u,%u,?)}", dwClubP, dwClubS );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽을 해체한다.
// dwClubP : 주 클럽번호 (동맹 클럽의 주)
// dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
int COdbcManager::DelClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
	if (dwClubP == 0 || dwClubS == 0)
	{
		Print(_T("ERROR:DelClubAlliance:Check Club Number"));
		return DB_ERROR;
	}
	
	//std::strstream strTemp;
	//strTemp << "{call DeleteGuildAlliance(";
	//strTemp << dwClubP << ",";
	//strTemp << dwClubS << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call DeleteGuildAlliance(%u,%u,?)}", dwClubP, dwClubS );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽정보를 가져온다.	
int COdbcManager::GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance )
{
	vecAlliance.clear();

	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::string strTemp;
	//strTemp = "SELECT GuNumP, GuNumS FROM GuildAlliance WITH (NOLOCK) ORDER BY GuNumP";

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT GuNumP, GuNumS FROM GuildAlliance WITH (NOLOCK) ORDER BY GuNumP" );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		return DB_ERROR;
	}
		
	SQLINTEGER   nGuNumP = 0; SQLINTEGER cbGuNumP = SQL_NTS;
	SQLINTEGER   nGuNumS = 0; SQLINTEGER cbGuNumS = SQL_NTS;	
		
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
			// Bind data			
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,	&nGuNumP, 0, &cbGuNumP);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	&nGuNumS, 0, &cbGuNumS);

			vecAlliance.push_back ( std::make_pair(nGuNumP,nGuNumS) );
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
* 클럽 배틀을 신청한다.
* \param dwClubP 주클럽번호( 클럽 배틀의 신청을 받은 주 )
* \param dwClubS 보조 클럽번호( 클럽 배틀의 신청을 한 클럽 )
* \return
*/
int COdbcManager::SetClubBattle( DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, int nAlliance )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ERROR: ODBCManager SetClubBattle"));
		return DB_ERROR;
	}

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 128, "{call sp_RequestGuBattle(%u,%u,%u,%u,?)}", dwClubP, dwClubS, dwEndTime, nAlliance );


	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}


/**
* 클럽 배틀을 종료한다.
* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주 )
* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
* \param nGuFlag 승패여부
* \param nGuKillNum 클럽 배틀 킬수
* \return
*/
int COdbcManager::EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ERROR: ODBCManager EndClubBattle"));
		return DB_ERROR;
	}

	int nReturn;

	if ( bAlliance ) 
	{
		TCHAR szTemp[128] = {0};
		_snprintf_s( szTemp, 128, "{call sp_EndGuAllianceBattle(%u,%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuFlag, nGuKillNum, nGuDeathNum );

		nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	}
	else
	{
		TCHAR szTemp[128] = {0};
		_snprintf_s( szTemp, 128, "{call sp_EndGuBattle(%u,%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuFlag, nGuKillNum, nGuDeathNum );

		nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	}

	return nReturn;
}

/**
* 클럽 배틀정보를 저장한다,( 클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주)
* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
* \param nGuKillNum 클럽 배틀 킬수
* \return
*/
int COdbcManager::SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum )
{
	if (dwClubP <= 0 || dwClubS <= 0)
	{
		Print(_T("ERROR: ODBCManager SaveClubBattle"));
		return DB_ERROR;
	}

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_SaveGuBattle(%u,%u,%u,%u,?)}", dwClubP, dwClubS, nGuKillNum, nGuDeathNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}


/**
* 동맹배틀 전적을 초기화 시키긴다.
* \param dwClub 클럽 번호
* \return
*/
int COdbcManager::ReSetAllianceBattle( DWORD dwClub )
{
	if (dwClub <= 0 )
	{
		Print(_T("ERROR: ODBCManager ReSetAllianceBattle"));
		return DB_ERROR;
	}

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_ResetAllianceBattle(%u,?)}", dwClub );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;

}

/**
* 해당 클럽의 배틀중인 클럽 정보를 가져온다.
* \param dwClub 클럽번호
* \param &vMember 클럽배틀 정보
* \return 
*/
int COdbcManager::GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT GuSNum, GuFlag, GuKillNum, GuDeathNum, GuBattleStartDate,"
							"GuBattleEndDate, GuAlliance FROM GuildBattle "
		"WHERE GuPNum=%u and GuFlag=%d ORDER BY GuBattleNum", dwClub, GLCLUBBATTLE::CLUB_BATTLE_NOW );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp,
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		
		return DB_ERROR;
	}
	
	SQLINTEGER nGuSNum		= 0, cbGuSNum		= SQL_NTS;
	SQLINTEGER nGuFlag		= 0, cbGuFlag		= SQL_NTS;
	SQLINTEGER nGuKillNum	= 0, cbGuKillNum	= SQL_NTS;
	SQLINTEGER nGuDeathNum	= 0, cbGuDeathNum	= SQL_NTS;
	SQLINTEGER nGuAlliance	= 0, cbGuAlliance	= SQL_NTS;
	TIMESTAMP_STRUCT sStartDate; SQLINTEGER cbStartDate	= SQL_NTS;
	TIMESTAMP_STRUCT sEndDate; SQLINTEGER cbEndDate	= SQL_NTS;
	
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
			GLCLUBBATTLE sBattle;

			// Bind data
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,	  &nGuSNum,             0, &cbGuSNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	  &nGuFlag,             0, &cbGuFlag);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	  &nGuKillNum,          0, &cbGuKillNum);
			::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,	  &nGuDeathNum,         0, &cbGuDeathNum);
			::SQLGetData(pConn->hStmt, 5, SQL_C_TYPE_TIMESTAMP, &sStartDate,    0, &cbStartDate);
			::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sEndDate,		0, &cbEndDate);
			::SQLGetData(pConn->hStmt, 7, SQL_C_LONG,	  &nGuAlliance,         0, &cbGuAlliance);
			
			sBattle.m_dwCLUBID		= (DWORD) nGuSNum;
			sBattle.m_nBattleFlag	= (GLCLUBBATTLE::CLUB_BATTLE_ENUM)nGuFlag;
			sBattle.m_wKillPoint	= (WORD)nGuKillNum;
			sBattle.m_wDeathPoint	= (WORD)nGuDeathNum;
			sBattle.m_bAlliance		= ( nGuAlliance > 0 ) ? true : false;

			// 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sStartDate.year < 1970 || sStartDate.year >= 2999)
			{
				CString strTimeTemp;
				strTimeTemp.Format( _T("ERROR:vBattleInfo.m_tGuBattleStartTime year %d"), sStartDate.year );
				Print( strTimeTemp );

				sStartDate.year = 1970;
				sStartDate.month = 2;
				sStartDate.day = 1;
				sStartDate.hour = 1;
				sStartDate.minute = 1;
				sStartDate.second = 1;
			}
            CTime cTempStart(sStartDate.year, sStartDate.month,  sStartDate.day, 
                         sStartDate.hour, sStartDate.minute, sStartDate.second);
            sBattle.m_tStartTime = cTempStart.GetTime();

			if (sEndDate.year < 1970 || sEndDate.year >= 2999)
			{
				CString strTimeTemp;
				strTimeTemp.Format( _T("ERROR:vBattleInfo.m_tEndTime year %d"), sEndDate.year );
				Print( strTimeTemp );

				sEndDate.year = 1970;
				sEndDate.month = 2;
				sEndDate.day = 1;
				sEndDate.hour = 1;
				sEndDate.minute = 1;
				sEndDate.second = 1;
			}
            CTime cTempEnd(sEndDate.year, sEndDate.month,  sEndDate.day, 
                         sEndDate.hour, sEndDate.minute, sEndDate.second);
            sBattle.m_tEndTime = cTempEnd.GetTime();

			vBattleInfo.push_back(sBattle);
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