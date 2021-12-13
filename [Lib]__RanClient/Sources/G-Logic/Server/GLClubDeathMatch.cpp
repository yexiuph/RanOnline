#include "pch.h"

#include <algorithm>
//#include <strstream>
#include "GLDefine.h"
#include "gltexfile.h"
#include "IniLoader.h"
//#include "GLOGIC.h"
#include "GLClubDeathMatch.h"

//#include "dbactionlogic.h"
#include "GLLandMan.h"
#include "GLGaeaServer.h"
#include "GLAgentServer.h"
//#include "GLChar.h"
//#include "GLCrow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLClubDeathMatch::GLClubDeathMatch () :
	m_dwID(UINT_MAX),
	
	m_dwClubMap(0),
	m_dwCLubMapGate(0),
	m_dwClubHallMap(0),
	m_dwClubHallGate(0),

//	m_fItemDropRate(0),
//	m_fDefaultCommissionPer(5),

	m_dwWinClub(CLUB_NULL),
//	m_fCommissionPercent(0.0f),

	m_dwBattleTime(3600),

//	m_bNewCommission(false),
//	m_fCommissionTimer(0.0f),
//	m_fNewCommissionPercent(0.0f),

	m_bNotifyOneHour(false),
	m_bNotifyHalfHour(false),
	m_bNotify10MinHour(false),
	
	m_emBattleState(BATTLE_NOSTART),
	m_dwBattleOrder(0),
	m_dwLastBattleDay(0),
	m_fBattleTimer(0.0f),

//	m_dwCERTIFY_CHARID(0),
//	m_dwCERTIFY_NPCID(0),
//	m_fCERTIFY_TIMER(0),
//	m_vCERTIFY_POS(0,0,0),

	m_fCHECK_TIMER(0.0f),
	m_fCHECK_TIME(0.0f),
	m_fRankingUpdate(0.0f)
{
}

GLClubDeathMatch& GLClubDeathMatch::operator= ( const GLClubDeathMatch& value )
{
	m_dwID = value.m_dwID;
	m_strName = value.m_strName;
//	m_vecMaps = value.m_vecMaps;

	m_dwClubMap = value.m_dwClubMap;
	m_dwCLubMapGate = value.m_dwCLubMapGate;
	m_dwClubHallMap = value.m_dwClubHallMap;
	m_dwClubHallGate = value.m_dwClubHallGate;

	m_dwBattleTime = value.m_dwBattleTime;

	for ( int i=0; i<MAX_TIME; ++i )
		m_sCdmTIME[i] = value.m_sCdmTIME[i];


	m_sCdmAwardItem = value.m_sCdmAwardItem;

//	m_fItemDropRate = value.m_fItemDropRate;
//	m_fDefaultCommissionPer = value.m_fDefaultCommissionPer;

	m_dwWinClub = value.m_dwWinClub;
//	m_fCommissionPercent = value.m_fCommissionPercent;

//	m_bNewCommission = value.m_bNewCommission;
//	m_fCommissionTimer = value.m_fCommissionTimer;
//	m_fNewCommissionPercent = value.m_fNewCommissionPercent;

	m_bNotifyOneHour = value.m_bNotifyOneHour;
	m_bNotifyHalfHour = value.m_bNotifyHalfHour;
	m_bNotify10MinHour = value.m_bNotify10MinHour;

	m_emBattleState = value.m_emBattleState;
	m_dwLastBattleDay = value.m_dwLastBattleDay;
	m_fBattleTimer = value.m_fBattleTimer;

//	m_dwCERTIFY_CHARID = value.m_dwCERTIFY_CHARID;
//	m_dwCERTIFY_NPCID = value.m_dwCERTIFY_NPCID;
//	m_fCERTIFY_TIMER = value.m_fCERTIFY_TIMER;
//	m_vCERTIFY_POS = value.m_vCERTIFY_POS;

//	m_bBattleEndCheck = value.m_bBattleEndCheck;
	m_fCHECK_TIMER = value.m_fCHECK_TIMER;
	m_fCHECK_TIME = value.m_fCHECK_TIME;

	m_vecCdmAwardChar = value.m_vecCdmAwardChar;

	m_fRankingUpdate = value.m_fRankingUpdate;

	return *this;
}

bool GLClubDeathMatch::Load ( std::string strFile )
{
	if( strFile.empty() )	return FALSE;

	std::string strPath;
	strPath = GLOGIC::GetServerPath ();
	strPath += strFile;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	if( !cFILE.open ( strPath, true ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLClubDeathMatch::Load(), File Open %s", strFile.c_str() );
		return false;
	}
	
	cFILE.getflag( "CLUB_DM", "ID", 0, 1, m_dwID );
	cFILE.getflag( "CLUB_DM", "NAME", 0, 1, m_strName );

	SNATIVEID nidMAP;
	cFILE.getflag( "CLUB_DM", "CLUB_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "CLUB_DM", "CLUB_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubMap = nidMAP.dwID;

	cFILE.getflag( "CLUB_DM", "CLUB_GATE", 0, 1, m_dwCLubMapGate );

	cFILE.getflag( "CLUB_DM", "CLUB_HALL_MAP", 0, 2, nidMAP.wMainID );
	cFILE.getflag( "CLUB_DM", "CLUB_HALL_MAP", 1, 2, nidMAP.wSubID );
	m_dwClubHallMap = nidMAP.dwID;

	cFILE.getflag( "CLUB_DM", "CLUB_HALL_GATE", 0, 1, m_dwClubHallGate );

	DWORD dwNUM = cFILE.GetKeySize( "CLUB_DM", "BATTLE_TIME" );
	if( dwNUM > 4 )
	{
		CDebugSet::ToLogFile( "ERROR : GLClubDeathMatch::Load(), %s, It was wrong size of BATTLE_TIME", strFile.c_str() );
		return false;
	}

	for( DWORD i=0; i<dwNUM; ++i )
	{
		cFILE.getflag( i, "CLUB_DM", "BATTLE_TIME", 0, 2, m_sCdmTIME[i].dwWeekDay );
		cFILE.getflag( i, "CLUB_DM", "BATTLE_TIME", 1, 2, m_sCdmTIME[i].dwStartTime );
	}

	cFILE.getflag( "CLUB_DM", "BATTLE_THE_TIME", 0, 1, m_dwBattleTime );

	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_1ST", 0, 2, m_sCdmAwardItem.nAwardItem[0].wMainID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_1ST", 1, 2, m_sCdmAwardItem.nAwardItem[0].wSubID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_2ND", 0, 2, m_sCdmAwardItem.nAwardItem[1].wMainID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_2ND", 1, 2, m_sCdmAwardItem.nAwardItem[1].wSubID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_3RD", 0, 2, m_sCdmAwardItem.nAwardItem[2].wMainID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_3RD", 1, 2, m_sCdmAwardItem.nAwardItem[2].wSubID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_ETC", 0, 2, m_sCdmAwardItem.nAwardItem[3].wMainID );
	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_ETC", 1, 2, m_sCdmAwardItem.nAwardItem[3].wSubID );

	cFILE.getflag( "CLUB_DM", "AWARD_ITEM_LIMIT", 0, 1, m_sCdmAwardItem.dwAwardLimit );
	return true;
}

DWORD GLClubDeathMatch::IsBattleWeekDay ( int nDayOfWeek, int nDay )
{
	if ( IsBattle() )	return UINT_MAX;

	//	Note : 결정전이 있는 날인지 검사. 결정전이 오늘 있었는지 검사.
	for ( DWORD i=0; i<MAX_TIME; ++i )
	{
		if ( (nDayOfWeek==m_sCdmTIME[i].dwWeekDay) && (m_dwLastBattleDay!=nDay) )
		{
			return i;
		}
	}
	return UINT_MAX;
}

// 선도클럽 시작시간인가?
bool GLClubDeathMatch::IsBattleHour ( DWORD dwORDER, int nHour )
{
	// 선도클럽 결정전은 날짜가 다른 요일 총 4번이 가능하다.
	if ( dwORDER>=MAX_TIME )
	{
		return false;
	}
	
	if (m_sCdmTIME[dwORDER].dwStartTime == nHour)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

// 최종적으로 선도클럽결정전이 일어난 날을 세팅한다.
// 하루에 두번 일어나지 않기 위해서이다.
void GLClubDeathMatch::UpdateBattleDay ( int nDay )
{
	m_dwLastBattleDay = nDay;
}

// 선도클럽 결정전 남은 시간을 클라이언트에게 알린다.
void GLClubDeathMatch::UpdateNotifyBattle ( DWORD dwORDER, int nHour, int nMinute )
{
	if ( dwORDER>=MAX_TIME )	return;

	DWORD dwOTime = m_sCdmTIME[dwORDER].dwStartTime;
	if ( dwOTime <= 0 )			return;
	
	dwOTime -= 1;
	if ( nHour == dwOTime )
	{
		//	Note : 
		if ( !m_bNotifyOneHour )
		{
			m_bNotifyOneHour = true;

			//	Note : 모든 클라이언트에 알림.
			GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
			NetMsgBrd.nTIME = 60-nMinute;
			StringCchCopy ( NetMsgBrd.szName, GLMSG::SNET_CLUB_DEATHMATCH_START_BRD::TEXT_LEN, m_strName.c_str() );
			GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );
		}

		if ( nMinute == 30 )
		{
			if ( !m_bNotifyHalfHour )
			{
				m_bNotifyHalfHour = true;

				//	Note : 모든 클라이언트에 알림.
				GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 30;
				StringCchCopy ( NetMsgBrd.szName, GLMSG::SNET_CLUB_DEATHMATCH_START_BRD::TEXT_LEN, m_strName.c_str() );
				GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );
			}
		}

		if ( nMinute == 50 )
		{
			if ( !m_bNotify10MinHour )
			{
				m_bNotify10MinHour = true;
				m_emBattleState = BATTLE_READY;

				//	Note : 모든 클라이언트에 알림.
				GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
				NetMsgBrd.nTIME = 10;
				StringCchCopy ( NetMsgBrd.szName, GLMSG::SNET_CLUB_DEATHMATCH_START_BRD::TEXT_LEN, m_strName.c_str() );
				GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );

				GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD NetMsgFld;
				NetMsgFld.dwID = m_dwID;
				GLAgentServer::GetInstance().SENDTOCHANNEL ( &NetMsgFld, 0 );
			}
		}
	}

}

// 선도클럽 결정전을 시작한다.
void GLClubDeathMatch::DoBattleStart ( DWORD dwORDER, int nDay )
{
	//	Note : 결정전을 시작시킴.
	m_emBattleState = BATTLE_START;
	m_dwBattleOrder = dwORDER;
	m_fRankingUpdate = 0.0f;
	m_vecCdmAwardChar.clear();
	m_mapCdmScore.clear();

	UpdateBattleDay ( nDay );

/*
	//	Note : 선도 클럽 변경을 필드서버에 알림.
	//
	GLMSG::SNET_CLUB_CERTIFIED_FLD NetMsgGuidFld;
	NetMsgGuidFld.dwID = m_dwID;
	NetMsgGuidFld.dwCLUBID = m_dwGuidanceClub;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgGuidFld );

	//	Note : 수수료 변경 필드에 알림.
	GLMSG::SNET_CLUB_GUID_COMMISSION_FLD NetMsgCommissionFld;
	NetMsgCommissionFld.dwID = m_dwID;
	NetMsgCommissionFld.fCommission = m_fCommissionPercent;
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgCommissionFld );

	//	Note : 클라이언트들에게 알림.
	GLMSG::SNET_CLUB_GUID_COMMISSION_BRD NetMsgCmBrd;
	NetMsgCmBrd.fCommission = m_fCommissionPercent;

	std::vector<DWORD>::size_type nSize = m_vecMaps.size();
	for ( std::vector<DWORD>::size_type i=0; i<nSize; ++i )
	{
		SNATIVEID nidMAP = (m_vecMaps[i]);
		GLAGLandMan *pLand = GLAgentServer::GetInstance().GetByMapID ( nidMAP );
		if ( !pLand )	continue;

		pLand->SENDTOALLCLIENT ( &NetMsgCmBrd );
	}
*/
	//	Note : 전투 시작을 알림.
	GLMSG::SNET_CLUB_DEATHMATCH_START_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	GLAgentServer::GetInstance().SENDTOCHANNEL ( &NetMsgFld, 0 );

	//	Note : 모든 클라이언트에 알림.
	GLMSG::SNET_CLUB_DEATHMATCH_START_BRD NetMsgBrd;
	NetMsgBrd.nTIME = 0;
	StringCchCopy ( NetMsgBrd.szName, GLMSG::SNET_CLUB_BATTLE_START_BRD::TEXT_LEN, m_strName.c_str() );
	GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );

}

void GLClubDeathMatch::DoBattleEnd()
{
	//	Note : 결정전이 종료.
	m_emBattleState = BATTLE_END;
	m_dwBattleOrder = UINT_MAX;

	m_bNotifyOneHour = false;
	m_bNotifyHalfHour = false;
	m_bNotify10MinHour = false;
	m_fBattleTimer = 0.0f;
	m_fRankingUpdate = 0.0f;


	//	Note : 결정전 종료를 알림.
	//
	GLCLUB *pCLUB = GLAgentServer::GetInstance().GetClubMan().GetClub(m_dwWinClub);

	std::string strClubName;
	if ( pCLUB )	strClubName = pCLUB->m_szName;

	//	Note : 전투 종료를 알림.
	GLMSG::SNET_CLUB_DEATHMATCH_END_FLD NetMsgFld;
	NetMsgFld.dwID = m_dwID;
	GLAgentServer::GetInstance().SENDTOCHANNEL ( &NetMsgFld, 0 );

	//	Note : 전투 결과를 알려줌 ( 해당 맵에 있는 유저들에게만 ) 

	GLMSG::SNET_CLUB_DEATHMATCH_END_BRD NetMsgBrd;
	StringCchCopy ( NetMsgBrd.szName, GLMSG::SNET_CLUB_DEATHMATCH_END_BRD::TEXT_LEN, m_strName.c_str() );
	StringCchCopy ( NetMsgBrd.szClubName, CHAR_SZNAME, strClubName.c_str() );
	GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );
}

/*
void GLClubDeathMatch::CheckExtraGuild ( float fElaps )
{
	m_fCHECK_TIMER += fElaps;
	if ( m_fCHECK_TIMER > m_fCHECK_TIME )
	{
		if ( m_fCHECK_TIME >= 30.0f )
		{
			m_bBattleEndCheck = false;
			m_fCHECK_TIMER = 0.0f;
		}

		//	보상 지급
		{

		}

		GLLandMan *pLandMan = GLGaeaServer::GetInstance().GetByMapID ( m_dwClubMap );
		if ( pLandMan )
		{
//			pLandMan->DoGateOutPCAll ( m_dwCLubMapGate );
		}
		m_fCHECK_TIME += 10.0f;
	}
}
*/

void GLClubDeathMatch::CheckAwardClub( float fElaps )
{
	m_fCHECK_TIMER += fElaps;
	if ( m_fCHECK_TIMER > m_fCHECK_TIME )
	{
		//	30초 동안은 보상지급
		if ( m_fCHECK_TIME < 30.0f )
		{
			//보상 지급
			m_fCHECK_TIME += 10.0f;
			return;
		}

		m_fCHECK_TIME = 999.0f;	
	}

	if ( m_fCHECK_TIMER > 300.0f )
	{
		SetBattle( EM_BATTLE_STATE::BATTLE_END );
		m_fCHECK_TIMER = 0.0f;
		m_fCHECK_TIME = 0.0f;
		return;
	}
}

bool GLClubDeathMatch::IsEnterMap( DWORD dwClubID )
{
	if ( dwClubID == CLUB_NULL ) return false;
	if ( IsBattle() ) return true;
	if ( IsBattleReady() ) return true;
//	if ( IsBattleEnd() && dwClubID == m_dwWinClub )	return true;

	return false;
}

SCDM_RANK_INFO* GLClubDeathMatch::GetCDMRankInof( DWORD dwClubID )
{
	CDM_RANK_INFO_MAP_ITER pos = m_mapCdmScore.find( dwClubID );
	if( pos == m_mapCdmScore.end() )
	{
		return NULL;
	}
	
	return &(*pos).second;;
}

void GLClubDeathMatch::AddCDMScore( DWORD dwKillClub, DWORD dwDeathClub )
{
	SCDM_RANK_INFO* pKillCdmRankInfo = GetCDMRankInof( dwKillClub );
	if ( pKillCdmRankInfo )
	{
		pKillCdmRankInfo->wKillNum++;
	}
	else
	{

		SCDM_RANK_INFO sCdmRankInfo;		
		sCdmRankInfo.dwClubID = dwKillClub;
		sCdmRankInfo.wKillNum++;

		GLCLUB* pClub = GLGaeaServer::GetInstance().GetClubMan().GetClub( dwKillClub );
		if ( pClub )
		{
			StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->m_szName );
			sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
		}	
	
		m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
	}

	SCDM_RANK_INFO* pDeathCdmRankInfo = GetCDMRankInof( dwDeathClub );
	if ( pDeathCdmRankInfo )
	{
		pDeathCdmRankInfo->wDeathNum++;
	}
	else
	{
		
		SCDM_RANK_INFO sCdmRankInfo;		
		sCdmRankInfo.dwClubID = dwDeathClub;
		sCdmRankInfo.wDeathNum++;

		GLCLUB* pClub = GLGaeaServer::GetInstance().GetClubMan().GetClub( dwDeathClub );
		if ( pClub )
		{
			StringCchCopy ( sCdmRankInfo.szClubName, CLUB_NAME+1, pClub->m_szName );
			sCdmRankInfo.szClubName[CLUB_NAME] = '\0';
		}
		
	
		m_mapCdmScore[sCdmRankInfo.dwClubID] = sCdmRankInfo;
	}
}


bool GLClubDeathMatchMan::Load ( std::vector<std::string> &vecFiles )
{
	std::vector<std::string>::size_type i = 0, j = vecFiles.size();
	for( ; i < j; ++i )
	{
		GLClubDeathMatch sClubDM;
		bool bOK = sClubDM.Load( vecFiles[i] );
		if( !bOK )
		{
			CDebugSet::ToLogFile ( "sClubDeathMatch.Load() fail, %s", vecFiles[i].c_str() );
		}

		m_vecClubDM.push_back ( sClubDM );
	}

	std::sort( m_vecClubDM.begin(), m_vecClubDM.end() );

	return true;
}

GLClubDeathMatch* GLClubDeathMatchMan::Find ( DWORD dwID )
{
	GLClubDeathMatch cObj;
	cObj.m_dwID = dwID;

	CDM_VEC_ITER pos = std::lower_bound ( m_vecClubDM.begin(), m_vecClubDM.end(), cObj );
	if ( pos==m_vecClubDM.end() )
	{
		return NULL;
	}
	else
	{
		return &(*pos);
	}
}

bool GLClubDeathMatchMan::IsBattle ()
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];
	
		if ( sClubDM.IsBattle() )		return true;
	}

	return false;
}

/*
bool GLClubDeathMatchMan::IsCheckExtraGuild ()
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];
	
		if ( sClubDM.m_bBattleEndCheck )		return true;
	}

	return false;
}
*/

GLClubDeathMatchAgentMan& GLClubDeathMatchAgentMan::GetInstance()
{
	static GLClubDeathMatchAgentMan cInstance;
	return cInstance;
}
/*
GLClubDeathMatch* GLClubDeathMatchAgentMan::FindByClubID ( DWORD dwClubID )
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];

		if ( sGuidance.m_dwGuidanceClub == dwClubID )	return &sGuidance;
	}

	return NULL;
}
*/
/*
DWORD GLClubDeathMatchAgentMan::GetGuidID ( DWORD dwClubID )
{
	for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];

		if ( sGuidance.m_dwGuidanceClub == dwClubID )		return sGuidance.m_dwID;
	}

	return UINT_MAX;
}
*/

bool GLClubDeathMatchAgentMan::SetMapState ()
{
	
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sCDM= m_vecClubDM[i];

		//	Note : 선도 클럽 전투 입구존.
		SNATIVEID nidHallMAP ( sCDM.m_dwClubHallMap );
		GLAGLandMan *pLandMan = GLAgentServer::GetInstance().GetByMapID ( nidHallMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
		}

		//	Note : 선도 클럽 전투존.
		SNATIVEID nidMAP ( sCDM.m_dwClubMap );
		pLandMan = GLAgentServer::GetInstance().GetByMapID ( nidMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
			pLandMan->m_bClubDeathMatchMap = true;
		}
	}

	return true;
}

/*
bool GLGuidanceAgentMan::ChangeGuidClub ( DWORD dwID, DWORD dwClubID )
{
	GLGuidance *pGuid = Find ( dwID );
	if ( !pGuid )				return false;

	pGuid->ChangeGuidClub ( dwClubID );

	return true;
}

bool GLGuidanceAgentMan::ChangeCommission ( DWORD dwID, float fRate )
{
	GLGuidance *pGuid = Find ( dwID );
	if ( !pGuid )				return false;

	pGuid->ChangeCommission ( fRate );

	return true;
}
*/

bool GLClubDeathMatchAgentMan::FrameMove ( float fElapsedAppTime )
{
	// 선도전을 시작하지 않는다.
	if ( GLCONST_CHAR::bPKLESS ) return false;

	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];

		CTime cCurTime = CTime::GetCurrentTime();
		int nDayOfWeek = cCurTime.GetDayOfWeek ();
		int nDay = cCurTime.GetDay ();
		int nHour = cCurTime.GetHour();
		int nMinute = cCurTime.GetMinute ();

		DWORD dwORDER = sClubDM.IsBattleWeekDay(nDayOfWeek,nDay);
		if ( dwORDER!=UINT_MAX )
		{
			//	Note : 결정전 시작 점검.
			if ( sClubDM.IsBattleHour ( dwORDER, nHour ) )
			{
				//	Note : 클럽 결정전 시작.
				sClubDM.DoBattleStart ( dwORDER, nDay );
				m_fRemainTimer = (float)sClubDM.m_dwBattleTime;
				m_fTimer = 0.0f;
			}
			else
			{
				//	Note : 결정전 시작 공지.
				sClubDM.UpdateNotifyBattle ( dwORDER, nHour, nMinute );
			}
		}

		if ( sClubDM.IsBattle() )
		{
			sClubDM.m_fBattleTimer += fElapsedAppTime;
			if ( sClubDM.m_fBattleTimer > float(sClubDM.m_dwBattleTime) )
			{
				sClubDM.DoBattleEnd();
			}
		}
	}

	// 클라이언트에게 경과 시간을 알린다.
	if ( !m_vecClubDM.empty() )
	{
		if ( m_vecClubDM[0].IsBattle() )
		{
			GLMSG::SNET_CLUB_DEATHMATCH_REMAIN_BRD NetMsgBrd;

			// 시작하면 통지
			if ( m_fRemainTimer == m_vecClubDM[0].m_dwBattleTime )
			{
				NetMsgBrd.dwTime = (DWORD)m_fRemainTimer;
				GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );
			}

			m_fTimer += fElapsedAppTime;
			m_fRemainTimer -= fElapsedAppTime;

			// 이후 10분만다 통지
			if ( m_fTimer > 600.0f )
			{
				NetMsgBrd.dwTime = (DWORD)m_fRemainTimer;
				GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsgBrd );
				m_fTimer = 0.0f;
			}
		}
	}

	return true;
}

GLClubDeathMatchFieldMan& GLClubDeathMatchFieldMan::GetInstance()
{
	static GLClubDeathMatchFieldMan cInstance;
	return cInstance;
}

bool GLClubDeathMatchFieldMan::SetMapState ()
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sCDM = m_vecClubDM[i];

		//	Note : 선도 클럽 전투 입구존.
		SNATIVEID nidHallMAP ( sCDM.m_dwClubHallMap );
		GLLandMan *pLandMan = GLGaeaServer::GetInstance().GetByMapID ( nidHallMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
//			pLandMan->m_pGuidance = GLGuidanceFieldMan::GetInstance().Find ( sGuidance.m_dwID );

			pLandMan->m_bClubDeathMatchMapHall = true;
		}

		//	Note : 선도 클럽 전투존.
		SNATIVEID nidMAP ( sCDM.m_dwClubMap );
		SMAPNODE *pMAPNODE = GLGaeaServer::GetInstance().FindMapNode ( nidMAP );
		if ( pMAPNODE )
		{
			pMAPNODE->bCDMZone = TRUE;
		}

		pLandMan = GLGaeaServer::GetInstance().GetByMapID ( nidMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sCDM.m_dwID;
//			pLandMan->m_pGuidance = GLGuidanceFieldMan::GetInstance().Find ( sGuidance.m_dwID );

			pLandMan->m_bClubDeathMatchMap = true;	
		}
	}

	return true;
}
bool GLClubDeathMatchFieldMan::ReadyBattle( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_READY );

	//	Note : 멥 설정 변경.

	return true;
}

bool GLClubDeathMatchFieldMan::BeginBattle ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_START );

	//	Note : 멥 설정 변경.
	return true;
}

bool GLClubDeathMatchFieldMan::EndBattle ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return false;

	pCDM->SetBattle( GLClubDeathMatch::BATTLE_END_AWARD );

	// 체크시작
//	pCDM->m_bBattleEndCheck = true;
	pCDM->m_fCHECK_TIME = 10.0f;


	UpdateCDMRanking( dwID );
	CDMAwardItem( dwID );

//	pLandMan->DoGateOutPCAll ( pCDM->m_dwCLubMapGate );

	return true;
}


const std::string GLClubDeathMatchFieldMan::GetName ( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )				return "";

	return pCDM->m_strName;
}

DWORD GLClubDeathMatchFieldMan::GetWinClubID ( DWORD dwClubID )
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];

		if ( sClubDM.m_dwWinClub == dwClubID )		return sClubDM.m_dwID;
	}

	return UINT_MAX;
}


bool GLClubDeathMatchFieldMan::FrameMove ( float fElaps )
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];

		if( sClubDM.IsBattle() )
		{
			sClubDM.m_fRankingUpdate += fElaps;

			//	5분마다 랭킹 갱신
			if ( sClubDM.m_fRankingUpdate > 60.0f )
			{
				UpdateCDMRanking( sClubDM.m_dwID );
				sClubDM.m_fRankingUpdate = 0.0f;
			}
		}

		if ( sClubDM.IsBattleEndAward() )
		{
			sClubDM.CheckAwardClub( fElaps );
		}

		/*if ( sClubDM.IsBattleEnd() )
		{
			sClubDM.CheckExtraClub( fElaps );
		}*/
	}

	return true;
}

void GLClubDeathMatchFieldMan::UpdateCDMRanking( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return;
	
	GLClubDeathMatch &sClubDM = *pCDM;
	
	CDM_RANK_INFO_VEC	m_vecCdmRankNew;
	m_vecCdmRankNew.reserve( sClubDM.m_mapCdmScore.size() );

	CDM_RANK_INFO_MAP_ITER	pos = sClubDM.m_mapCdmScore.begin();
	CDM_RANK_INFO_MAP_ITER	end = sClubDM.m_mapCdmScore.end();

	for ( ; pos != end; pos++ )
	{
		const SCDM_RANK_INFO&	sRankInfo = pos->second;					
		m_vecCdmRankNew.push_back( sRankInfo );
	}
	
	std::sort( m_vecCdmRankNew.begin(), m_vecCdmRankNew.end() );	

	// 랭킹 계산
	int nSize = m_vecCdmRankNew.size();

	for ( int i = 0; i < nSize; ++i )
	{
		m_vecCdmRankNew[i].wClubRanking = i+1;
		m_vecCdmRankNew[i].nIndex = i;
	}

	//	동점상황 랭킹 계산 아래 랭킹에 맞춤
	// int nRanking;

	for ( int i = nSize-1; i > 0; --i )
	{
		if ( m_vecCdmRankNew[i] == m_vecCdmRankNew[i-1] )
		{
			m_vecCdmRankNew[i-1].wClubRanking = m_vecCdmRankNew[i].wClubRanking;			
		}
	}	
	
	
	GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE	NetMsg;

	for ( int i = 0; i < (int)m_vecCdmRankNew.size(); ++i )
	{
		CDM_RANK_INFO_MAP_ITER iter = sClubDM.m_mapCdmScore.find( m_vecCdmRankNew[i].dwClubID );
		if ( iter == sClubDM.m_mapCdmScore.end() )	continue;	//	이럴경우는 없어야 함

		SCDM_RANK_INFO&	sRankInfo = iter->second;
		
		if ( m_vecCdmRankNew[i].nIndex != sRankInfo.nIndex || 
			 m_vecCdmRankNew[i].wClubRanking != sRankInfo.wClubRanking )
		{
			sRankInfo.wClubRanking = m_vecCdmRankNew[i].wClubRanking;
			sRankInfo.nIndex = m_vecCdmRankNew[i].nIndex;
			
			//	9개 클럽만 보낸다.
			if ( m_vecCdmRankNew[i].nIndex < RANKING_NUM ) 
			{
				SCDM_RANK sCdmRank = sRankInfo;
				NetMsg.ADDCLUB( sCdmRank );
			}
		}		
	}

	if ( NetMsg.wRankNum > 0 )	
		GLGaeaServer::GetInstance().SENDTOCLIENT_ONMAP( sClubDM.m_dwClubMap, &NetMsg );


	GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE	NetMsgMy;

	pos = sClubDM.m_mapCdmScore.begin();
	end = sClubDM.m_mapCdmScore.end();

	for ( ; pos != end; pos++ )
	{
		const SCDM_RANK_INFO&	sRankInfo = pos->second;
		NetMsgMy.sMyCdmRank.wClubRanking = sRankInfo.wClubRanking;
		NetMsgMy.sMyCdmRank.wKillNum = sRankInfo.wKillNum;
		NetMsgMy.sMyCdmRank.wDeathNum = sRankInfo.wDeathNum;
		
		GLGaeaServer::GetInstance().SENDTOCLUBCLIENT_ONMAP( sClubDM.m_dwClubMap, sRankInfo.dwClubID, &NetMsgMy );
	}

}

void GLClubDeathMatchFieldMan::CDMAwardItem( DWORD dwID )
{
	GLClubDeathMatch *pCDM = Find ( dwID );
	if ( !pCDM )	return;

	if ( !pCDM->IsBattleEnd() ) return;	

	GLLandMan *pLandMan = GLGaeaServer::GetInstance().GetByMapID ( pCDM->m_dwClubMap );
	if ( !pLandMan ) return;

	//	해당 맵에 있는 유저들 보상 지급
	{
		GLClubMan& cClubMan = GLGaeaServer::GetInstance().GetClubMan();

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar( pCharNode->Data->m_dwGaeaID );
			if ( !pChar ) continue;

			GLCLUB* pClub = cClubMan.GetClub( pChar->m_dwGuild );
			if ( !pClub ) continue;

			CDM_RANK_INFO_MAP_ITER iter = pCDM->m_mapCdmScore.find( pClub->m_dwID );
			if ( iter != pCDM->m_mapCdmScore.end() ) 
			{
				SCDM_RANK_INFO& sCdmRankInfo = iter->second;

				// ( 보상 받은 캐릭터인지 점검 ) 
				CDM_AWARD_CHAR_ITER iter = pCDM->m_vecCdmAwardChar.find( pChar->m_dwCharID );
				if ( iter != pCDM->m_vecCdmAwardChar.end() )	continue;

				int nAwardIndex;
				if ( sCdmRankInfo.wClubRanking < 4 ) nAwardIndex = sCdmRankInfo.wClubRanking - 1;
				else if ( sCdmRankInfo.wClubRanking <= pCDM->m_sCdmAwardItem.dwAwardLimit ) nAwardIndex = 3;
				else continue;
				
				SNATIVEID sNativeID = pCDM->m_sCdmAwardItem.nAwardItem[nAwardIndex];
				
				SITEM* pItem = GLItemMan::GetInstance().GetItem( sNativeID );
				if ( !pItem ) continue;

				SITEMCUSTOM sITEM_NEW;
				sITEM_NEW.sNativeID = sNativeID;
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
				sITEM_NEW.cGenType = EMGEN_SYSTEM;
				sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
				sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
				sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				if ( pChar->IsInsertToInvenEx ( &cDropItem ) )
				{
					pChar->InsertToInvenEx ( &cDropItem );
					
					GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_CLUB, pChar->m_dwGuild, ID_CHAR, pChar->m_dwCharID, 
						EMITEM_ROUTE_SYSTEM, sITEM_NEW.wTurnNum );
				}
				else
				{
					pLandMan->DropItem ( pChar->m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, pChar->m_dwGaeaID );
				}

				pCDM->m_vecCdmAwardChar.insert( pChar->m_dwCharID );
			}
		}
	}

}

/*
void GLClubDeathMatchFieldMan::CheckExtraGuild ( float fElaps )
{
	for ( CDM_VEC::size_type i=0; i<m_vecClubDM.size(); ++i )
	{
		GLClubDeathMatch &sClubDM = m_vecClubDM[i];
		if ( sClubDM.m_bBattleEndCheck )
		{
			sClubDM.CheckExtraGuild( fElaps );
		}
	}
}
*/

