#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 캐릭터 정보를 가져온다 (바이너리 제거 데이타)
* \param nUserNum 사용자번호
* \param nChaNum 캐릭터번호
* \return 캐릭터정보
*/
/*
SCHARDATA2* COdbcManager::GetCharacter(int nUserNumber, int nChaNum)
{
	// 해킹한 캐릭터의 경우 사용자번호나 캐릭터 번호가 0 이될 가능성이 있다.
	// 실제로 홍콩에서 해킹한 경우 0 이었다.
	if (nUserNumber <= 0 || nChaNum <= 0)
	{
		return NULL;
	}

	int nRowCount = 0;
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return NULL;
	
	// 캐릭터 정보를 가져온다.
	std::strstream strTemp;
	strTemp << "SELECT UserNum, SGNum, ChaName, ChaTribe, ChaClass, ";
	strTemp << "ChaBright, ChaLevel, ChaDex, ChaIntel, ChaPower, ";
	strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaStRemain, ChaAttackP, ";
	strTemp << "ChaDefenseP, ChaFightA, ChaShootA,  ChaSkillPoint, ChaHP, ";
	strTemp << "ChaMP, ChaSP, ChaPK, ChaStartMap, ChaStartGate, ";
	strTemp << "ChaPosX, ChaPosY, ChaPosZ, ChaMoney, ChaExp, ";    
    strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, ";	
	strTemp << "ChaSchool, ChaHair, ChaFace, ChaLiving, ChaInvenLine, ";
    strTemp << "ChaReturnMap, ChaReturnPosX, ChaReturnPosY, ChaReturnPosZ, GuNum, ";	
	strTemp << "ChaGuName, ChaGuSecede, ChaHairColor, ChaSex "; // 2006-01-04 : Add Jgkim
    strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	strTemp << " AND UserNum=" << nUserNumber;

	strTemp << std::ends;

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*) strTemp.str(), 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(strTemp.str());		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return NULL;
	}

	SCHARDATA2* pChaData = new SCHARDATA2;

	if (pChaData == NULL) return NULL;

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLCHAR    szChaGuName[CHAR_SZNAME+1] = {0}; SQLINTEGER cbChaGuName = SQL_NTS;
	SQLCHAR    szChaPhNum[SMS_RECEIVER] = {0}; SQLINTEGER cbChaPhNum = SQL_NTS;

    SQLINTEGER nUserNum = 0, cbUserNum=SQL_NTS;
	SQLINTEGER nChaTribe = 0, cbChaTribe=SQL_NTS;
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nSGNum = 0, cbSGNum=SQL_NTS;

    SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;
	SQLINTEGER nChaPower = 0, cbChaPower=SQL_NTS; 
	SQLINTEGER nChaStrong = 0, cbChaStrong=SQL_NTS;   
	SQLINTEGER nChaStrength = 0, cbChaStrength=SQL_NTS; 
	SQLINTEGER nChaSpirit = 0, cbChaSpirit=SQL_NTS;

	SQLINTEGER nChaDex = 0, cbChaDex=SQL_NTS;
	SQLINTEGER nChaIntel = 0, cbChaIntel=SQL_NTS;
	SQLINTEGER nChaStRemain = 0, cbChaStRemain=SQL_NTS; 
	SQLINTEGER nChaHP = 0, cbChaHP=SQL_NTS;
	SQLINTEGER nChaMP = 0, cbChaMP=SQL_NTS;
	
	SQLINTEGER nChaStartMap = 0, cbChaStartMap=SQL_NTS;
	SQLINTEGER nChaStartGate = 0, cbChaStartGate=SQL_NTS;
	SQLFLOAT fChaPosX = 0; SQLINTEGER cbChaPosX=SQL_NTS;
	SQLFLOAT fChaPosY = 0; SQLINTEGER cbChaPosY=SQL_NTS;
	SQLFLOAT fChaPosZ = 0; SQLINTEGER cbChaPosZ=SQL_NTS;

	SQLINTEGER nGuNum = 0, cbGuNum=SQL_NTS;
	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;
	SQLINTEGER nChaAttackP = 0, cbChaAttackP=SQL_NTS;
	SQLINTEGER nChaDefenseP = 0, cbChaDefenseP=SQL_NTS;
	SQLINTEGER nChaFightA = 0, cbChaFightA=SQL_NTS;

	SQLINTEGER nChaShootA = 0, cbChaShootA=SQL_NTS;
	SQLINTEGER nChaSP = 0, cbChaSP=SQL_NTS;
	SQLINTEGER nChaPK = 0, cbChaPK=SQL_NTS;
	SQLINTEGER nChaSkillPoint = 0, cbChaSkillPoint=SQL_NTS;

	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;
	SQLFLOAT fChaSavePosX = 0; SQLINTEGER cbChaSavePosX=SQL_NTS;
	SQLFLOAT fChaSavePosY = 0; SQLINTEGER cbChaSavePosY=SQL_NTS;
	SQLFLOAT fChaSavePosZ = 0; SQLINTEGER cbChaSavePosZ=SQL_NTS;

	SQLINTEGER nChaSchool = 0, cbChaSchool = SQL_NTS;
	SQLINTEGER nChaHair = 0, cbChaHair = SQL_NTS;
	SQLINTEGER nChaFace = 0, cbChaFace = SQL_NTS;
	SQLINTEGER nChaLiving = 0, cbChaLiving = SQL_NTS;
    
	LONGLONG   llChaExp = 0, llChaMoney = 0;
	SQLINTEGER cbChaExp=SQL_NTS, cbChaMoney=SQL_NTS; 

    SQLINTEGER nChaReturnMap  = 0; SQLINTEGER cbChaReturnMap  = SQL_NTS;
	SQLFLOAT   fChaReturnPosX = 0; SQLINTEGER cbChaReturnPosX = SQL_NTS;
	SQLFLOAT   fChaReturnPosY = 0; SQLINTEGER cbChaReturnPosY = SQL_NTS;
	SQLFLOAT   fChaReturnPosZ = 0; SQLINTEGER cbChaReturnPosZ = SQL_NTS;

    SQLINTEGER nChaInvenLine = 0; SQLINTEGER cbChaInvenLine = SQL_NTS;
    TIMESTAMP_STRUCT sChaGuSecede; SQLINTEGER cbChaGuSecede = SQL_NTS;

	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	SQLSMALLINT nChaSex = 0; SQLINTEGER cbChaSex = SQL_NTS;
	SQLINTEGER nChaHairColor =0, cbChaHairColor = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			SAFE_DELETE(pChaData);
            Print(strTemp.str());		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return NULL;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{ 
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nSGNum,         0, &cbSGNum);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_CHAR, szChaName   ,  CHR_ID_LENGTH+1, &cbChaName); 
            ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nChaClass,      0, &cbChaClass);

			::SQLGetData(pConn->hStmt, 6, SQL_C_LONG, &nChaBright,     0, &cbChaBright);
            ::SQLGetData(pConn->hStmt, 7, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
			::SQLGetData(pConn->hStmt, 8, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
			::SQLGetData(pConn->hStmt, 9, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(pConn->hStmt,10, SQL_C_LONG, &nChaPower,      0, &cbChaPower);

            ::SQLGetData(pConn->hStmt,11, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
			::SQLGetData(pConn->hStmt,12, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);
            ::SQLGetData(pConn->hStmt,13, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);
			::SQLGetData(pConn->hStmt,14, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
			::SQLGetData(pConn->hStmt,15, SQL_C_LONG, &nChaAttackP,    0, &cbChaAttackP);		
                        
            ::SQLGetData(pConn->hStmt,16, SQL_C_LONG, &nChaDefenseP,   0, &cbChaDefenseP);
			::SQLGetData(pConn->hStmt,17, SQL_C_LONG, &nChaFightA,     0, &cbChaFightA);
			::SQLGetData(pConn->hStmt,18, SQL_C_LONG, &nChaShootA,     0, &cbChaShootA);
			::SQLGetData(pConn->hStmt,19, SQL_C_LONG, &nChaSkillPoint, 0, &cbChaSkillPoint);
			::SQLGetData(pConn->hStmt,20, SQL_C_LONG, &nChaHP,		   0, &cbChaHP);

			::SQLGetData(pConn->hStmt,21, SQL_C_LONG, &nChaMP,			0, &cbChaMP);
			::SQLGetData(pConn->hStmt,22, SQL_C_LONG, &nChaSP,			0, &cbChaSP);
			::SQLGetData(pConn->hStmt,23, SQL_C_LONG, &nChaPK,			0, &cbChaPK);
			::SQLGetData(pConn->hStmt,24, SQL_C_LONG, &nChaStartMap,	0, &cbChaStartMap);
			::SQLGetData(pConn->hStmt,25, SQL_C_LONG, &nChaStartGate,   0, &cbChaStartGate);
			
			::SQLGetData(pConn->hStmt,26, SQL_C_DOUBLE, &fChaPosX,	  0, &cbChaPosX);
			::SQLGetData(pConn->hStmt,27, SQL_C_DOUBLE, &fChaPosY,	  0, &cbChaPosY);
			::SQLGetData(pConn->hStmt,28, SQL_C_DOUBLE, &fChaPosZ,	  0, &cbChaPosZ);			
            ::SQLGetData(pConn->hStmt,29, SQL_C_SBIGINT, &llChaMoney, 0, &cbChaMoney);
            ::SQLGetData(pConn->hStmt,30, SQL_C_SBIGINT, &llChaExp,   0, &cbChaExp);
			
			::SQLGetData(pConn->hStmt,31, SQL_C_LONG,   &nChaSaveMap,  0, &cbChaSaveMap);
			::SQLGetData(pConn->hStmt,32, SQL_C_DOUBLE, &fChaSavePosX, 0, &cbChaSavePosX);
			::SQLGetData(pConn->hStmt,33, SQL_C_DOUBLE, &fChaSavePosY, 0, &cbChaSavePosY);
			::SQLGetData(pConn->hStmt,34, SQL_C_DOUBLE, &fChaSavePosZ, 0, &cbChaSavePosZ);

			::SQLGetData(pConn->hStmt,35, SQL_C_LONG, &nChaSchool,	0, &cbChaSchool);
			::SQLGetData(pConn->hStmt,36, SQL_C_LONG, &nChaHair,	0, &cbChaHair);
			::SQLGetData(pConn->hStmt,37, SQL_C_LONG, &nChaFace,	0, &cbChaFace);
			::SQLGetData(pConn->hStmt,38, SQL_C_LONG, &nChaLiving,	0, &cbChaLiving);
            
            ::SQLGetData(pConn->hStmt,39, SQL_C_LONG, &nChaInvenLine, 0, &cbChaInvenLine);

            ::SQLGetData(pConn->hStmt,40, SQL_C_LONG,   &nChaReturnMap,  0, &cbChaReturnMap);
			::SQLGetData(pConn->hStmt,41, SQL_C_DOUBLE, &fChaReturnPosX, 0, &cbChaReturnPosX);
			::SQLGetData(pConn->hStmt,42, SQL_C_DOUBLE, &fChaReturnPosY, 0, &cbChaReturnPosY);
			::SQLGetData(pConn->hStmt,43, SQL_C_DOUBLE, &fChaReturnPosZ, 0, &cbChaReturnPosZ);

            ::SQLGetData(pConn->hStmt,44, SQL_C_LONG, &nGuNum, 0, &cbGuNum);
            ::SQLGetData(pConn->hStmt,45, SQL_C_CHAR, szChaGuName, CHAR_SZNAME+1, &cbChaGuName);
            ::SQLGetData(pConn->hStmt,46, SQL_C_TYPE_TIMESTAMP, &sChaGuSecede, 0, &cbChaGuSecede);

			// 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			::SQLGetData(pConn->hStmt,47, SQL_SMALLINT, &nChaSex,       0, &cbChaSex);
			::SQLGetData(pConn->hStmt,48, SQL_C_LONG,   &nChaHairColor, 0, &cbChaHairColor);

            pChaData->m_dwGuild = (DWORD) nGuNum; // 클럽번호
            pChaData->m_dwCharID = (DWORD) nChaNum;
            pChaData->m_dwUserID = (DWORD) nUserNum;

            if (cbChaName != 0 && cbChaName != -1) // 캐릭터명
				::StringCchCopy(pChaData->m_szName, CHR_ID_LENGTH, (const char*) szChaName);

            if (cbChaGuName != 0 && cbChaGuName != -1) // 클럽에서 닉네임
                ::StringCchCopy(pChaData->m_szNick, CHAR_SZNAME, (const char*) szChaGuName);

            pChaData->m_emTribe = EMTRIBE   (nChaTribe);
            pChaData->m_emClass = EMCHARCLASS(nChaClass);
            // sTemp.SGNum         = nSGNum; // 서버 그룹
            pChaData->m_wLevel = (WORD) nChaLevel;

            // wPow;	//	힘.
	        // wStr;	//	체력.
	        // wSpi;	//	정신.
	        // wDex;	//	민첩.
	        // wInt;	//	지력.
	        // wSta;	//	근력.		--(추가)
            pChaData->m_sStats.wPow = (WORD) nChaPower;
            pChaData->m_sStats.wStr = (WORD) nChaStrong;
            pChaData->m_sStats.wSta = (WORD) nChaStrength;
            pChaData->m_sStats.wSpi = (WORD) nChaSpirit;
            pChaData->m_sStats.wDex = (WORD) nChaDex;
            pChaData->m_sStats.wInt = (WORD) nChaIntel;

            pChaData->m_wStatsPoint = (WORD) nChaStRemain;

            pChaData->m_lnMoney            = llChaMoney;
            pChaData->m_sExperience.lnNow  = llChaExp;

			pChaData->m_sHP.wNow			= (WORD) nChaHP;
			pChaData->m_sMP.wNow			= (WORD) nChaMP;
			pChaData->m_sStartMapID.dwID	= (DWORD) nChaStartMap;
			pChaData->m_dwStartGate		= (DWORD) nChaStartGate;
			pChaData->m_vStartPos.x		= (float) fChaPosX;

			pChaData->m_vStartPos.y		= (float) fChaPosY;
			pChaData->m_vStartPos.z		= (float) fChaPosZ,	
			nGuNum;
			pChaData->m_nBright			= nChaBright;
			pChaData->m_wAP				= (WORD) nChaAttackP;

			pChaData->m_wDP				= (WORD) nChaDefenseP;
			pChaData->m_wPA				= (WORD) nChaFightA;
			pChaData->m_wSA				= (WORD) nChaShootA;
			pChaData->m_sSP.dwData 		= (WORD) nChaSP;
			pChaData->m_wPK				= (WORD) nChaPK;

			pChaData->m_dwSkillPoint    = (DWORD) nChaSkillPoint;
			pChaData->m_sSaveMapID.dwID = (DWORD) nChaSaveMap;
			pChaData->m_vSavePos.x		= (float) fChaSavePosX; 
			pChaData->m_vSavePos.y		= (float) fChaSavePosY; 
			pChaData->m_vSavePos.z		= (float) fChaSavePosZ;

            pChaData->m_sLastCallMapID.dwID = (DWORD) nChaReturnMap;
			pChaData->m_vLastCallPos.x		= (float) fChaReturnPosX; 
			pChaData->m_vLastCallPos.y		= (float) fChaReturnPosY; 
			pChaData->m_vLastCallPos.z		= (float) fChaReturnPosZ;

			pChaData->m_wSchool			= (WORD) nChaSchool;
			pChaData->m_wFace			= (WORD) nChaFace;
			pChaData->m_wHair			= (WORD) nChaHair;
			pChaData->m_wHairColor      = (WORD) nChaHairColor; // 머리색깔
			pChaData->m_wSex            = (WORD) nChaSex; // 성별
			pChaData->m_nLiving			= (int) nChaLiving;

            // 인벤토리 라인
            pChaData->m_wINVENLINE = (WORD) nChaInvenLine;
            // 클럽탈퇴시간
            // 1970-02-01 : Default
			// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
			// 이 코드를 추가로 삽입한다.
			if (sChaGuSecede.year <= 1970 || sChaGuSecede.year >= 2999)
			{
				sChaGuSecede.year = 1970;
				sChaGuSecede.month = 2;
				sChaGuSecede.day = 1;
				sChaGuSecede.hour = 1;
				sChaGuSecede.minute = 1;
				sChaGuSecede.second = 1;
			}
            CTime cTemp(sChaGuSecede.year, sChaGuSecede.month,  sChaGuSecede.day, 
                        sChaGuSecede.hour, sChaGuSecede.minute, sChaGuSecede.second);
            pChaData->m_tSECEDE = cTemp.GetTime();

			nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);

	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nRowCount == 1)
	{
		return pChaData;
	}
	else
	{
		SAFE_DELETE(pChaData);
		return NULL;
	}
}
*/

/**	
* 캐릭터 정보를 가져온다. 캐릭터의 모든정보 (바이너리 포함)
* \param nUserNum 사용자번호
* \param nChaNum 캐릭터번호
* \param pChaData2 캐릭터 데이터를 받을 포인트
* \return DB_OK, DB_ERROR
*/
int COdbcManager::GetCharacterInfo(int nUserNumber,
								   int nChaNum, 
								   SCHARDATA2* pChaData2)
{
	if (nUserNumber <= 0 || nChaNum <= 0)
	{
		return DB_ERROR;
	}

	int nRowCount = 0;
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	// 캐릭터 정보를 가져온다.
	//std::strstream strTemp;
	//strTemp << "SELECT UserNum,SGNum,ChaName,ChaTribe,ChaClass,";
	//strTemp << "ChaBright,ChaLevel,ChaDex,ChaIntel,ChaPower,";
	//strTemp << "ChaStrong,ChaSpirit,ChaStrength,ChaStRemain,ChaAttackP,";
	//strTemp << "ChaDefenseP,ChaFightA,ChaShootA,ChaSkillPoint,ChaHP,";
	//strTemp << "ChaMP,ChaSP,ChaPK,ChaStartMap,ChaStartGate,";
	//strTemp << "ChaPosX,ChaPosY,ChaPosZ,ChaMoney,ChaExp,";
 //   strTemp << "ChaSaveMap,ChaSavePosX,ChaSavePosY,ChaSavePosZ,ChaSchool,";
	//strTemp << "ChaHair,ChaFace,ChaLiving,ChaInvenLine,";
 //   strTemp << "ChaReturnMap,ChaReturnPosX,ChaReturnPosY,ChaReturnPosZ,GuNum,";
	//strTemp << "ChaGuName,ChaSex,ChaHairColor";
	//strTemp << " FROM ChaInfo WHERE ChaNum=" << nChaNum;
	//strTemp << " AND UserNum=" << nUserNumber;
	//strTemp << std::ends; // << ends; strTemp.put('\0'); // Same
	// strTemp << ends;

	TCHAR szTemp[1024] = {0};
	_snprintf_s( szTemp, 1024, "SELECT UserNum,SGNum,ChaName,ChaTribe,ChaClass,"
							"ChaBright,ChaLevel,ChaDex,ChaIntel,ChaPower,"
							"ChaStrong,ChaSpirit,ChaStrength,ChaStRemain,ChaAttackP,"							
							"ChaDefenseP,ChaFightA,ChaShootA,ChaSkillPoint,ChaHP,"
							"ChaMP,ChaSP,ChaPK,ChaStartMap,ChaStartGate,"
							"ChaPosX,ChaPosY,ChaPosZ,ChaMoney,ChaExp,"
							"ChaSaveMap,ChaSavePosX,ChaSavePosY,ChaSavePosZ,ChaSchool,"
							"ChaHair,ChaFace,ChaLiving,ChaInvenLine,"
							"ChaReturnMap,ChaReturnPosX,ChaReturnPosY,ChaReturnPosZ,GuNum,"
							"ChaGuName,ChaSex,ChaHairColor, ChaReExp, ChaSpMID, ChaSpSID "
							"FROM ChaInfo WHERE ChaNum=%d AND UserNum=%d", nChaNum, nUserNumber );


	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*) szTemp,
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

//		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
        return DB_ERROR;
	}

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS; // 캐릭터명
	SQLCHAR    szChaGuName[CHAR_SZNAME+1] = {0}; SQLINTEGER cbChaGuName = SQL_NTS;// 클럽에서의 닉네임
	
    SQLINTEGER nUserNum = 0, cbUserNum=SQL_NTS;
	SQLINTEGER nChaTribe = 0, cbChaTribe=SQL_NTS;
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nSGNum = 0, cbSGNum=SQL_NTS;

    SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;
	SQLINTEGER nChaPower = 0, cbChaPower=SQL_NTS; 
	SQLINTEGER nChaStrong = 0, cbChaStrong=SQL_NTS;   
	SQLINTEGER nChaStrength = 0, cbChaStrength=SQL_NTS; 
	SQLINTEGER nChaSpirit = 0, cbChaSpirit=SQL_NTS;

	SQLINTEGER nChaDex = 0, cbChaDex=SQL_NTS;
	SQLINTEGER nChaIntel = 0, cbChaIntel=SQL_NTS;
	SQLINTEGER nChaStRemain = 0, cbChaStRemain=SQL_NTS; 
	SQLINTEGER nChaHP = 0, cbChaHP=SQL_NTS;
	SQLINTEGER nChaMP = 0, cbChaMP=SQL_NTS;
	
	SQLINTEGER nChaStartMap = 0, cbChaStartMap=SQL_NTS;
	SQLINTEGER nChaStartGate = 0, cbChaStartGate=SQL_NTS;
	SQLFLOAT fChaPosX = 0; SQLINTEGER cbChaPosX=SQL_NTS;
	SQLFLOAT fChaPosY = 0; SQLINTEGER cbChaPosY=SQL_NTS;
	SQLFLOAT fChaPosZ = 0; SQLINTEGER cbChaPosZ=SQL_NTS;

	// SQLINTEGER nGuNum, cbGuNum=SQL_NTS;
	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;
	SQLINTEGER nChaAttackP = 0, cbChaAttackP=SQL_NTS;
	SQLINTEGER nChaDefenseP = 0, cbChaDefenseP=SQL_NTS;
	SQLINTEGER nChaFightA = 0, cbChaFightA=SQL_NTS;

	SQLINTEGER nChaShootA = 0, cbChaShootA=SQL_NTS;
	SQLINTEGER nChaSP = 0, cbChaSP=SQL_NTS;
	SQLINTEGER nChaPK = 0, cbChaPK=SQL_NTS;
	SQLINTEGER nChaSkillPoint = 0, cbChaSkillPoint=SQL_NTS;
	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;

	SQLFLOAT fChaSavePosX = 0; SQLINTEGER cbChaSavePosX=SQL_NTS;
	SQLFLOAT fChaSavePosY = 0; SQLINTEGER cbChaSavePosY=SQL_NTS;
	SQLFLOAT fChaSavePosZ = 0; SQLINTEGER cbChaSavePosZ=SQL_NTS;

	SQLINTEGER nChaSchool = 0, cbChaSchool = SQL_NTS;
	SQLINTEGER nChaHair = 0, cbChaHair = SQL_NTS;
	SQLINTEGER nChaFace = 0, cbChaFace = SQL_NTS;
	SQLINTEGER nChaLiving = 0, cbChaLiving = SQL_NTS;
	
	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	SQLSMALLINT nChaSex = 0; SQLINTEGER cbChaSex = SQL_NTS;
	SQLINTEGER nChaHairColor =0, cbChaHairColor = SQL_NTS;
    
	LONGLONG   llChaExp = 0, llChaMoney = 0;
	SQLINTEGER cbChaExp=SQL_NTS, cbChaMoney=SQL_NTS; 
    
    SQLINTEGER nChaInvenLine = 0; SQLINTEGER cbChaInvenLine = SQL_NTS;
    SQLINTEGER nGuNum = 0; SQLINTEGER cbGuNum = SQL_NTS;

    SQLINTEGER nChaReturnMap  = 0; SQLINTEGER cbChaReturnMap  = SQL_NTS;
	SQLFLOAT   fChaReturnPosX = 0; SQLINTEGER cbChaReturnPosX = SQL_NTS;
	SQLFLOAT   fChaReturnPosY = 0; SQLINTEGER cbChaReturnPosY = SQL_NTS;
	SQLFLOAT   fChaReturnPosZ = 0; SQLINTEGER cbChaReturnPosZ = SQL_NTS;

	// Add to ChaReExp
	LONGLONG   llChaReExp = 0;
	SQLINTEGER cbChaReExp=SQL_NTS;

	// Add to ChaSpMID, ChaSpSID
	SQLINTEGER nChaSpMID = 0, cbChaSpMID=SQL_NTS;
	SQLINTEGER nChaSpSID = 0, cbChaSpSID=SQL_NTS;





	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {	
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

//			strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{		
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nUserNum,       0, &cbUserNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nSGNum,         0, &cbSGNum);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_CHAR, szChaName   ,  CHR_ID_LENGTH+1, &cbChaName); 
            ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nChaTribe,      0, &cbChaTribe);
            ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nChaClass,      0, &cbChaClass);

			::SQLGetData(pConn->hStmt, 6, SQL_C_LONG, &nChaBright,     0, &cbChaBright);
            ::SQLGetData(pConn->hStmt, 7, SQL_C_LONG, &nChaLevel,      0, &cbChaLevel);
			::SQLGetData(pConn->hStmt, 8, SQL_C_LONG, &nChaDex,        0, &cbChaDex);
			::SQLGetData(pConn->hStmt, 9, SQL_C_LONG, &nChaIntel,      0, &cbChaIntel);
            ::SQLGetData(pConn->hStmt,10, SQL_C_LONG, &nChaPower,      0, &cbChaPower);

            ::SQLGetData(pConn->hStmt,11, SQL_C_LONG, &nChaStrong,     0, &cbChaStrong);
			::SQLGetData(pConn->hStmt,12, SQL_C_LONG, &nChaSpirit,     0, &cbChaSpirit);
            ::SQLGetData(pConn->hStmt,13, SQL_C_LONG, &nChaStrength,   0, &cbChaStrength);
			::SQLGetData(pConn->hStmt,14, SQL_C_LONG, &nChaStRemain,   0, &cbChaStRemain);
			::SQLGetData(pConn->hStmt,15, SQL_C_LONG, &nChaAttackP,    0, &cbChaAttackP);		
                        
            ::SQLGetData(pConn->hStmt,16, SQL_C_LONG, &nChaDefenseP,   0, &cbChaDefenseP);
			::SQLGetData(pConn->hStmt,17, SQL_C_LONG, &nChaFightA,     0, &cbChaFightA);
			::SQLGetData(pConn->hStmt,18, SQL_C_LONG, &nChaShootA,     0, &cbChaShootA);
			::SQLGetData(pConn->hStmt,19, SQL_C_LONG, &nChaSkillPoint, 0, &cbChaSkillPoint);
			::SQLGetData(pConn->hStmt,20, SQL_C_LONG, &nChaHP,			0, &cbChaHP);

			::SQLGetData(pConn->hStmt,21, SQL_C_LONG, &nChaMP,			0, &cbChaMP);
			::SQLGetData(pConn->hStmt,22, SQL_C_LONG, &nChaSP,			0, &cbChaSP);
			::SQLGetData(pConn->hStmt,23, SQL_C_LONG, &nChaPK,			0, &cbChaPK);
			::SQLGetData(pConn->hStmt,24, SQL_C_LONG, &nChaStartMap,	0, &cbChaStartMap);
			::SQLGetData(pConn->hStmt,25, SQL_C_LONG, &nChaStartGate,  0, &cbChaStartGate);
			
			::SQLGetData(pConn->hStmt,26, SQL_C_DOUBLE, &fChaPosX,		0, &cbChaPosX);
			::SQLGetData(pConn->hStmt,27, SQL_C_DOUBLE, &fChaPosY,		0, &cbChaPosY);
			::SQLGetData(pConn->hStmt,28, SQL_C_DOUBLE, &fChaPosZ,		0, &cbChaPosZ);			
            ::SQLGetData(pConn->hStmt,29, SQL_C_SBIGINT, &llChaMoney,  0, &cbChaMoney);
            ::SQLGetData(pConn->hStmt,30, SQL_C_SBIGINT, &llChaExp,    0, &cbChaExp);
			
			::SQLGetData(pConn->hStmt,31, SQL_C_LONG, &nChaSaveMap,    0, &cbChaSaveMap);
			::SQLGetData(pConn->hStmt,32, SQL_C_DOUBLE, &fChaSavePosX,   0, &cbChaSavePosX);
			::SQLGetData(pConn->hStmt,33, SQL_C_DOUBLE, &fChaSavePosY,   0, &cbChaSavePosY);
			::SQLGetData(pConn->hStmt,34, SQL_C_DOUBLE, &fChaSavePosZ,   0, &cbChaSavePosZ);

			::SQLGetData(pConn->hStmt,35, SQL_C_LONG, &nChaSchool,		0, &cbChaSchool);
			::SQLGetData(pConn->hStmt,36, SQL_C_LONG, &nChaHair,		0, &cbChaHair);
			::SQLGetData(pConn->hStmt,37, SQL_C_LONG, &nChaFace,		0, &cbChaFace);
			::SQLGetData(pConn->hStmt,38, SQL_C_LONG, &nChaLiving,		0, &cbChaLiving);

            ::SQLGetData(pConn->hStmt,39, SQL_C_LONG, &nChaInvenLine, 0, &cbChaInvenLine);
            
            ::SQLGetData(pConn->hStmt,40, SQL_C_LONG,   &nChaReturnMap,  0, &cbChaReturnMap);
			::SQLGetData(pConn->hStmt,41, SQL_C_DOUBLE, &fChaReturnPosX, 0, &cbChaReturnPosX);
			::SQLGetData(pConn->hStmt,42, SQL_C_DOUBLE, &fChaReturnPosY, 0, &cbChaReturnPosY);
			::SQLGetData(pConn->hStmt,43, SQL_C_DOUBLE, &fChaReturnPosZ, 0, &cbChaReturnPosZ);

            ::SQLGetData(pConn->hStmt,44, SQL_C_LONG, &nGuNum, 0, &cbGuNum);
            ::SQLGetData(pConn->hStmt,45, SQL_C_CHAR, szChaGuName, CHAR_SZNAME, &cbChaGuName); 

			// 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			::SQLGetData(pConn->hStmt,46, SQL_SMALLINT, &nChaSex,       0, &cbChaSex);
			::SQLGetData(pConn->hStmt,47, SQL_C_LONG,   &nChaHairColor, 0, &cbChaHairColor);

			// Add to ChaReExp
			::SQLGetData(pConn->hStmt,48, SQL_C_SBIGINT, &llChaReExp, 0, &cbChaReExp);

			// Add to SpMID, SpSID
			::SQLGetData(pConn->hStmt,49, SQL_C_LONG, &nChaSpMID, 0, &cbChaSpMID);
			::SQLGetData(pConn->hStmt,50, SQL_C_LONG, &nChaSpSID, 0, &cbChaSpSID);			

   
            pChaData2->m_dwCharID			= (DWORD) nChaNum; // 캐릭터번호
			pChaData2->SetUserID( (DWORD) nUserNum ); // 사용자번호
            pChaData2->m_dwGuild            = (DWORD) nGuNum; // 클럽번호

            if (cbChaName != 0 && cbChaName != -1) // 캐릭터명
				::StringCchCopy(pChaData2->m_szName, CHR_ID_LENGTH, (const char*) szChaName);

            if (cbChaGuName != 0 && cbChaGuName != -1) // 클럽에서 닉네임
                ::StringCchCopy(pChaData2->m_szNick, CHAR_SZNAME, (const char*) szChaGuName);

            pChaData2->m_emTribe			= EMTRIBE(nChaTribe);
            pChaData2->m_emClass			= EMCHARCLASS(nChaClass);
            
            pChaData2->m_wLevel				= (WORD) nChaLevel;           
            pChaData2->m_sStats.wPow		= (WORD) nChaPower;
            pChaData2->m_sStats.wStr		= (WORD) nChaStrong;
            pChaData2->m_sStats.wSta		= (WORD) nChaStrength;
            pChaData2->m_sStats.wSpi		= (WORD) nChaSpirit;

            pChaData2->m_sStats.wDex		= (WORD) nChaDex;
            pChaData2->m_sStats.wInt		= (WORD) nChaIntel;
            pChaData2->m_wStatsPoint		= (WORD) nChaStRemain;
            pChaData2->m_lnMoney            = llChaMoney;
            pChaData2->m_sExperience.lnNow  = llChaExp;

			pChaData2->m_sHP.wNow			= (WORD) nChaHP;
			pChaData2->m_sMP.wNow			= (WORD) nChaMP;
			pChaData2->m_sStartMapID.dwID	= (DWORD) nChaStartMap;
			pChaData2->m_dwStartGate		= (DWORD) nChaStartGate;
			pChaData2->m_vStartPos.x		= (float) fChaPosX;

			pChaData2->m_vStartPos.y		= (float) fChaPosY;
			pChaData2->m_vStartPos.z		= (float) fChaPosZ,	
			pChaData2->m_nBright			= nChaBright;
			pChaData2->m_wAP				= (WORD) nChaAttackP;
			pChaData2->m_wDP				= (WORD) nChaDefenseP;

			pChaData2->m_wPA				= (WORD) nChaFightA;
			pChaData2->m_wSA				= (WORD) nChaShootA;
			pChaData2->m_dwSkillPoint		= (DWORD) nChaSkillPoint;
			pChaData2->m_sSP.dwData 		= (WORD) nChaSP;
			pChaData2->m_wPK				= (WORD) nChaPK;
			
			pChaData2->m_sSaveMapID.dwID	= (DWORD) nChaSaveMap;
			pChaData2->m_vSavePos.x			= (float) fChaSavePosX; 
			pChaData2->m_vSavePos.y			= (float) fChaSavePosY; 
			pChaData2->m_vSavePos.z			= (float) fChaSavePosZ;

            pChaData2->m_sLastCallMapID.dwID= (DWORD) nChaReturnMap;
			pChaData2->m_vLastCallPos.x		= (float) fChaReturnPosX; 
			pChaData2->m_vLastCallPos.y		= (float) fChaReturnPosY; 
			pChaData2->m_vLastCallPos.z		= (float) fChaReturnPosZ;

			pChaData2->m_wSchool			= (WORD) nChaSchool;

			pChaData2->m_wFace				= (WORD) nChaFace;
			pChaData2->m_wHair				= (WORD) nChaHair;
			pChaData2->m_nLiving			= (int) nChaLiving;
			pChaData2->m_wHairColor         = (WORD) nChaHairColor;
			pChaData2->m_wSex               = (WORD) nChaSex;


			if( nChaSpMID != 0 && nChaSpSID != 0 )
			{
				// Add to SpMID, SpSID
				pChaData2->m_sSummonPosionID.wMainID = (WORD) nChaSpMID;
				pChaData2->m_sSummonPosionID.wSubID = (WORD) nChaSpSID;
			}

            // 인벤토리 라인
            pChaData2->m_wINVENLINE = (WORD) nChaInvenLine; 

			pChaData2->m_lnReExp = llChaReExp;

			nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);

//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (nRowCount != 1)
	{
		return DB_ERROR;
	}
#if defined(KRT_PARAM)
	// 캐릭터의 전화번호를 가져온다.
	GetChaPhoneNumber( nChaNum, pChaData2->m_szPhoneNumber );
#endif

#if defined(VN_PARAM) //vietnamtest%%%
	// 베트남의 필요한 추가 정보를 가져온다.
	GetVTCharInfo( nChaNum,	pChaData2 );
#endif

#if defined(TW_PARAM) || defined(_RELEASED) || defined(HK_PARAM) 
	GetLastCharInfo( nUserNumber, nChaNum, pChaData2 );
#endif


	CByteStream ByteStream;
	
	// Skill	
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaSkills", nChaNum, ByteStream);
	if (sReturn == DB_ERROR) 
		return DB_ERROR;
	else 
		pChaData2->SETEXPSKILLS_BYBUF(ByteStream);

	// Skill Quick Slot		
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaSkillSlot", nChaNum, ByteStream);
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
        pChaData2->SETSKILL_QUICKSLOT(ByteStream);
	
	// 2003-11-27 : Add
	// Action Quick Slot	
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaActionSlot", nChaNum, ByteStream);
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETACTION_QUICKSLOT(ByteStream);

	// Quest
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaQuest", nChaNum, ByteStream);
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETQUESTPLAY(ByteStream);


	// 착용아이템
	//std::strstream strPutOnItems;
	//strPutOnItems << "SELECT ChaInfo.ChaPutOnItems FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	//strPutOnItems << std::ends;
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
	//strPutOnItems.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		SETPUTONITEMS_BYBUF(pChaData2->m_PutOnItems,ByteStream);

	// Character Quest information

	// 캐릭터 인벤토리
	//std::strstream strChaInven;
	//strChaInven << "SELECT ChaInfo.ChaInven FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	//strChaInven << std::ends;
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaInven", nChaNum, ByteStream);
	//strChaInven.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETINVENTORY_BYBUF(ByteStream);

#if defined(VN_PARAM) //vietnamtest%%%
	// 베트남의 필요한 추가 정보를 가져온다.
	sReturn = m_pGameDB->ReadImage("ChaInfo.VTAddInven", nChaNum, ByteStream);

	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETVTADDINVENTORY_BYBUF(ByteStream);
#endif

	
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaCoolTime", nChaNum, ByteStream);
	//strChaInven.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		pChaData2->SETITEMCOOLTIME_BYBUF(ByteStream);

	return DB_OK;
}

int	COdbcManager::GetChaBInfo(int nUserNum,
							  int nChaNum, 
							  SCHARINFO_LOBBY* sci)
{
	if (nUserNum <= 0 || nChaNum <= 0)
	{
		return DB_ERROR;
	}
    
	SQLRETURN sReturn = 0;
	int nRowCount = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	// 2003-11-27 Jgkim:Add
	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	//std::strstream strTemp;
	//strTemp << "SELECT ChaName, ChaClass, ChaSchool, ChaDex, ChaIntel, ChaPower,";
	//strTemp << "ChaStrong, ChaSpirit, ChaStrength, ChaLevel, ChaHair,";	
	//strTemp << "ChaFace, ChaBright, ChaSex, ChaHairColor, ChaExp,";
	//strTemp << "ChaSaveMap, ChaHP FROM ChaInfo ";
	//strTemp << "WHERE ChaNum=" << nChaNum;
	//strTemp << " AND UserNum=" << nUserNum;
	//strTemp << std::ends;

	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "SELECT ChaName, ChaClass, ChaSchool, ChaDex, ChaIntel, ChaPower,"
							"ChaStrong, ChaSpirit, ChaStrength, ChaLevel, ChaHair,"
							"ChaFace, ChaBright, ChaSex, ChaHairColor, ChaExp,"
							"ChaSaveMap, ChaHP FROM ChaInfo "
							"WHERE ChaNum=%d AND UserNum=%d",
							nChaNum, nUserNum );
	
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
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nChaSchool = 0, cbChaSchool=SQL_NTS;
	SQLINTEGER nChaDex = 0, cbChaDex=SQL_NTS;
	SQLINTEGER nChaIntel = 0, cbChaIntel=SQL_NTS;
	SQLINTEGER nChaPower = 0, cbChaPower=SQL_NTS; 
	
	SQLINTEGER nChaStrong = 0, cbChaStrong=SQL_NTS;   
	SQLINTEGER nChaSpirit = 0, cbChaSpirit=SQL_NTS;
	SQLINTEGER nChaStrength = 0, cbChaStrength=SQL_NTS; 
	SQLINTEGER nChaLevel = 0, cbChaLevel=SQL_NTS;
	SQLINTEGER nChaHair = 0, cbChaHair = SQL_NTS;
	
	SQLINTEGER nChaFace = 0, cbChaFace = SQL_NTS;	
	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;
	
	// 2005-12-29 Jgkim:Add ChaSex, ChaHairColor 
	SQLSMALLINT nChaSex = 0; SQLINTEGER cbChaSex = SQL_NTS;
	SQLINTEGER nChaHairColor =0, cbChaHairColor = SQL_NTS;

	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;
	SQLINTEGER nChaHP = 0, cbChaHP=SQL_NTS;

	LONGLONG   llChaExp = 0; SQLINTEGER cbChaExp=SQL_NTS;

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
			sci->m_dwCharID = nChaNum;
			::SQLGetData(pConn->hStmt,  1, SQL_C_CHAR, szChaName, CHR_ID_LENGTH+1, &cbChaName);
			::SQLGetData(pConn->hStmt,  2, SQL_C_LONG, &nChaClass, 0, &cbChaClass);
			::SQLGetData(pConn->hStmt,  3, SQL_C_LONG, &nChaSchool, 0, &cbChaSchool);
			::SQLGetData(pConn->hStmt,  4, SQL_C_LONG, &nChaDex,   0, &cbChaDex);
			::SQLGetData(pConn->hStmt,  5, SQL_C_LONG, &nChaIntel, 0, &cbChaIntel);
			::SQLGetData(pConn->hStmt,  6, SQL_C_LONG, &nChaPower, 0, &cbChaPower);

			::SQLGetData(pConn->hStmt,  7, SQL_C_LONG, &nChaStrong, 0, &cbChaStrong);
			::SQLGetData(pConn->hStmt,  8, SQL_C_LONG, &nChaSpirit, 0, &cbChaSpirit);
			::SQLGetData(pConn->hStmt,  9, SQL_C_LONG, &nChaStrength, 0, &cbChaStrength);
			::SQLGetData(pConn->hStmt, 10, SQL_C_LONG, &nChaLevel,    0, &cbChaLevel);
			::SQLGetData(pConn->hStmt, 11, SQL_C_LONG, &nChaHair,	  0, &cbChaHair);

			::SQLGetData(pConn->hStmt, 12, SQL_C_LONG,	&nChaFace,     0, &cbChaFace);
			::SQLGetData(pConn->hStmt, 13, SQL_C_LONG,  &nChaBright,   0, &cbChaBright);

			// 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			::SQLGetData(pConn->hStmt, 14, SQL_SMALLINT, &nChaSex,       0, &cbChaSex);
			::SQLGetData(pConn->hStmt, 15, SQL_C_LONG,	 &nChaHairColor, 0, &cbChaHairColor);			

			::SQLGetData(pConn->hStmt, 16, SQL_C_SBIGINT,&llChaExp,    0, &cbChaExp);

			::SQLGetData(pConn->hStmt, 17, SQL_C_LONG, &nChaSaveMap,    0, &cbChaSaveMap);
			::SQLGetData(pConn->hStmt, 18, SQL_C_LONG, &nChaHP,			0, &cbChaHP);

			 if (cbChaName != 0 && cbChaName != -1) 				 
				::StringCchCopy(sci->m_szName, CHR_ID_LENGTH, (const char*) szChaName);

			 sci->m_emClass = EMCHARCLASS(nChaClass);
			 sci->m_wSchool = (WORD) nChaSchool;
			 sci->m_sStats.wDex = (WORD) nChaDex;
			 sci->m_sStats.wInt = (WORD) nChaIntel;
			 sci->m_sStats.wPow = (WORD) nChaPower;

			 sci->m_sStats.wStr = (WORD) nChaStrong;
			 sci->m_sStats.wSpi = (WORD) nChaSpirit;
			 sci->m_sStats.wSta = (WORD) nChaStrength;
			 sci->m_wLevel      = (WORD) nChaLevel;
			 sci->m_wHair		= (WORD) nChaHair;

			 sci->m_wFace       = (WORD) nChaFace;
			 sci->m_nBright     = nChaBright;

			 // 2005-12-29 Jgkim:Add nChaSex, nChaHairColor
			 sci->m_wSex        = (WORD) nChaSex;
			 sci->m_wHairColor  = (WORD) nChaHairColor;

			 sci->m_sHP.wNow	= (WORD) nChaHP;			 
			 sci->m_sSaveMapID  = (DWORD) nChaSaveMap;

			 sci->m_sExperience.lnNow  = llChaExp;

			 nRowCount++;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);

	// 반드시 nRowCount 가 1 이어야 한다.
	if (nRowCount != 1)
	{
		return DB_ERROR;
	}

	// 착용아이템 정보
	CByteStream ByteStream;	
	//std::strstream strPutOnItems;
	//strPutOnItems << "SELECT ChaInfo.ChaPutOnItems FROM ChaInfo where (ChaNum=" <<  nChaNum << ")";
	//strPutOnItems << std::ends;
	sReturn = m_pGameDB->ReadImage("ChaInfo.ChaPutOnItems", nChaNum, ByteStream);
	//strPutOnItems.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if (sReturn == DB_ERROR)
		return DB_ERROR;
	else
		SETPUTONITEMS_BYBUF(sci->m_PutOnItems,ByteStream);

	return DB_OK;
}

int COdbcManager::GetChaPhoneNumber(
	int nChaNum,
	TCHAR* szPhoneNumber )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	// 캐릭터 정보를 가져온다.
	//std::strstream strTemp;
	//strTemp << "SELECT ChaPhone ";
	//strTemp << " FROM ChaSmsInfo WHERE ChaNum=" << nChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT ChaPhone FROM ChaSmsInfo WHERE ChaNum=%d", nChaNum );

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

	SQLCHAR szChaPhNum[SMS_RECEIVER] = {0}; SQLINTEGER cbChaPhNum = SQL_NTS;

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
			::SQLGetData(pConn->hStmt,  1, SQL_C_CHAR, szChaPhNum, SMS_RECEIVER, &cbChaPhNum);			

			if (cbChaPhNum != 0 && cbChaPhNum != -1) 				 
				::StringCchCopy( szPhoneNumber, SMS_RECEIVER, (const char*) szChaPhNum );			
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


// 베트남 탐직방지 시스템의 추가 정보 불러오는 부분 추가
int COdbcManager::GetVTCharInfo( int nChaNum,
								SCHARDATA2* pChaData2 )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT saveMoney, saveExp, itemCount FROM ChaInfo WHERE ChaNum=%d", nChaNum );

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

	LONGLONG   llChaSaveExp = 0, llChaSaveMoney = 0;	
	SQLINTEGER cbChaSaveExp = SQL_NTS, cbChaSaveMoney = SQL_NTS; 
	SQLINTEGER nChaitemCount = 0, cbChaitemCount = SQL_NTS;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			// Bind data			
			::SQLGetData(pConn->hStmt,1, SQL_C_SBIGINT, &llChaSaveMoney,  0, &cbChaSaveMoney);
			::SQLGetData(pConn->hStmt,2, SQL_C_SBIGINT, &llChaSaveExp,    0, &cbChaSaveExp);
			::SQLGetData(pConn->hStmt,3, SQL_C_LONG, &nChaitemCount,    0, &cbChaitemCount);

			pChaData2->m_lVNGainSysMoney        = llChaSaveMoney;
			pChaData2->m_lVNGainSysExp			= llChaSaveExp;
			pChaData2->m_dwVietnamInvenCount	= nChaitemCount;
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

// 대만 해킹 문제 관련해서 최근 로그아웃 했을때의 정보를 DB에서 읽어옴.
int COdbcManager::GetLastCharInfo( 
								  int nUserNum,
								  int nChaNum,
								  SCHARDATA2* pChaData2 )
{
	SQLRETURN sReturn = 0;
	{		
		ODBC_STMT* pConn = m_pGameDB->GetConnection();
		if (!pConn) return DB_ERROR;
	
		TCHAR szTemp[128] = {0};
		_snprintf_s( szTemp, 128, "SELECT ChaMoney,ChaLevel FROM ChaLastInfo WHERE ChaNum=%d", nChaNum );
	
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
	
		// ChaLastInfo Table의 금액 컬럼의 암호화를 위해서 ChaMoney 컬럼의 DataType을 int -> varchar(100)으로 수정한다.
		//	LONGLONG   llChaSaveMoney = 0;	
		//	SQLINTEGER cbChaSaveMoney = SQL_NTS; 
	
		// 캐릭터 로그아웃 할때의 ChaMoney
		SQLCHAR    szTempMoney[CHAR_TEMPMONEY] = {0}; SQLINTEGER cbTempMoney = SQL_NTS;
		SQLINTEGER nChaLevel = 0, cbChaLevel = SQL_NTS;
	
		while (true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
	
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				// Bind data			
				//			::SQLGetData(pConn->hStmt,1, SQL_C_SBIGINT, &llChaSaveMoney,  0, &cbChaSaveMoney);
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szTempMoney, CHAR_TEMPMONEY, &cbTempMoney);
				::SQLGetData(pConn->hStmt,2, SQL_C_LONG, &nChaLevel,    0, &cbChaLevel);
	
				//			pChaData2->m_lnTempMoney    = llChaSaveMoney;			
				if (cbTempMoney != 0 && cbTempMoney != -1) // TempMoney 값 복사
				{
					char szTempMoney1[CHAR_TEMPMONEY], szTemp[2];
					LONGLONG lnTempMoeny = 0;
	
					::StringCchCopy(szTempMoney1, CHAR_TEMPMONEY, (const char*) szTempMoney);
	
					size_t i;
					for( i = 0; i < strlen(szTempMoney1); i++ )
					{
						szTempMoney1[i] -= 20;
						sprintf_s( szTemp, "%c", szTempMoney1[i] );
						lnTempMoeny = ( lnTempMoeny * 10 ) + atoi(szTemp);
					}
					/*LONGLONG lnTempValue = (nChaNum * pChaData2->m_wLevel);

					lnTempMoeny -= lnTempValue;*/
					pChaData2->m_lnTempMoney = lnTempMoeny >> 0x04;
				}
				pChaData2->m_wTempLevel	= (WORD)nChaLevel;
			}
			else
			{
				break;
			}
			Sleep( 0 );
		}
	
		m_pGameDB->FreeConnection(pConn);
	}

	{	
		sReturn = 0;
		ODBC_STMT* pConn = m_pGameDB->GetConnection();
		if (!pConn) return DB_ERROR;

		TCHAR szTemp[128] = {0};
		_snprintf_s( szTemp, 128, "SELECT UserMoney FROM UserLastInfo WHERE UserNum=%d", nUserNum );

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

		// UserLastInfo Table의 금액 컬럼의 암호화를 위해서 ChaMoney 컬럼의 DataType을 int -> varchar(100)으로 수정한다.
		//	LONGLONG   llUserSaveMoney = 0;
		//	SQLINTEGER cbUserSaveMoney = SQL_NTS;	
		SQLCHAR    szTempStorageMoney[CHAR_TEMPMONEY] = {0}; 
		SQLINTEGER cbTempStorageMoney = SQL_NTS;

		while (true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);

			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				// Bind data
				//			::SQLGetData(pConn->hStmt,1, SQL_C_SBIGINT, &llUserSaveMoney,  0, &cbUserSaveMoney);
				::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szTempStorageMoney, CHAR_TEMPMONEY, &cbTempStorageMoney);

				//			pChaData2->m_lnTempStorageMoney  = llUserSaveMoney;
				if (cbTempStorageMoney != 0 && cbTempStorageMoney != -1) // TempStorageMoney 값 복사
				{
					char szTempStorageMoney1[CHAR_TEMPMONEY], szTemp[2];
					LONGLONG lnTempMoeny = 0;

					::StringCchCopy(szTempStorageMoney1, CHAR_TEMPMONEY, (const char*) szTempStorageMoney);

					size_t i;
					for( i = 0; i < strlen(szTempStorageMoney1); i++ )
					{
						szTempStorageMoney1[i] -= 20;
						sprintf_s( szTemp, "%c", szTempStorageMoney1[i] );
						lnTempMoeny = ( lnTempMoeny * 10 ) + atoi(szTemp);
					}
					/*LONGLONG lnTempValue = (nUserNum);

					lnTempMoeny -= lnTempValue;*/
					pChaData2->m_lnTempStorageMoney = lnTempMoeny >> 0x04;
				}
			}
			else
			{
				break;
			}
			Sleep( 0 );
		}

		m_pGameDB->FreeConnection(pConn);
	}

	return DB_OK;
}


