#include "pch.h"
#include "./GLClubMan.h"
#include "./GLGaeaServer.h"
#include "./DbActionLogic.h"
#include "./GLAgentServer.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__NetServer/Sources/s_CCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLCLUBMEMBER& GLCLUBMEMBER::operator = ( const GLCLUBMEMBER& value )
{
	if( this == &value )
		return *this;

	dwID = value.dwID;
	bONLINE = value.bONLINE;
	nCHANNEL = value.nCHANNEL;
	nidMAP = value.nidMAP;
	vecPOS = value.vecPOS;
	StringCchCopy ( m_szName, CHAR_SZNAME, value.m_szName );
	m_dwFlags = value.m_dwFlags;

	return *this;
}

GLCLUBINFO& GLCLUBINFO::operator = ( const GLCLUBINFO& value )
{
	if( this == &value )
		return *this;

	m_dwID = value.m_dwID;
	StringCchCopy ( m_szName, CHAR_SZNAME, value.m_szName );

	m_dwMasterID = value.m_dwMasterID;
	StringCchCopy ( m_szMasterName, CHAR_SZNAME, value.m_szMasterName );
	m_dwCDCertifior = value.m_dwCDCertifior;

	m_dwRank = value.m_dwRank;

	m_dwMarkVER = value.m_dwMarkVER;
	memcpy ( m_aryMark, value.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

	m_tOrganize = value.m_tOrganize;
	m_tDissolution = value.m_tDissolution;
	m_tAuthority = value.m_tAuthority;

	m_tAllianceSec = value.m_tAllianceSec;
	m_tAllianceDis = value.m_tAllianceDis;

	m_lnIncomeMoney = value.m_lnIncomeMoney;
	m_lnStorageMoney = value.m_lnStorageMoney;

	m_dwBattleWin = value.m_dwBattleWin;
	m_dwBattleLose = value.m_dwBattleLose;
	m_dwBattleDraw = value.m_dwBattleDraw;
	m_tLastBattle = value.m_tLastBattle;

	m_dwAllianceBattleWin = value.m_dwAllianceBattleWin;
	m_dwAllianceBattleLose = value.m_dwAllianceBattleLose;
	m_dwAllianceBattleDraw = value.m_dwAllianceBattleDraw;

	StringCchCopy ( m_szNotice, EMCLUB_NOTICE_LEN+1, value.m_szNotice );

	return *this;
}

GLCLUBBATTLE& GLCLUBBATTLE::operator = ( const GLCLUBBATTLE& value )
{

	if( this == &value )
		return *this;

	m_dwCLUBID = value.m_dwCLUBID;
	m_tStartTime = value.m_tStartTime;
	m_tEndTime = value.m_tEndTime;
	m_wKillPointDB = value.m_wKillPointDB;
	m_wDeathPointDB = value.m_wDeathPointDB;
	m_wKillPoint = value.m_wKillPoint;
	m_wDeathPoint = value.m_wDeathPoint;
	m_wKillPointTemp = value.m_wKillPointTemp;
	m_wDeathPointTemp = value.m_wDeathPointTemp;
	m_bKillUpdate = value.m_bKillUpdate;
	m_nBattleFlag = value.m_nBattleFlag;
	m_bAlliance = value.m_bAlliance;
	
	StringCchCopy ( m_szClubName, CHAR_SZNAME, value.m_szClubName );

	return *this;
}

GLCLUBBATTLE& GLCLUBBATTLE::operator = ( const GLCLUBBATTLE_LOBY& value )
{
	m_dwCLUBID = value.m_dwCLUBID;
	m_tStartTime = value.m_tStartTime;
	m_tEndTime = value.m_tEndTime;
	m_wKillPoint = value.m_wKillPoint;
	m_wDeathPoint = value.m_wDeathPoint;
	m_bAlliance = value.m_bAlliance;
	
	StringCchCopy ( m_szClubName, CHAR_SZNAME, value.m_szClubName );

	return *this;
}

GLCLUBBATTLE_LOBY& GLCLUBBATTLE_LOBY::operator = ( const GLCLUBBATTLE_LOBY& value )
{
	m_dwCLUBID = value.m_dwCLUBID;
	m_tStartTime = value.m_tStartTime;
	m_tEndTime = value.m_tEndTime;
	m_wKillPoint = value.m_wKillPoint;
	m_wDeathPoint = value.m_wDeathPoint;
	m_bAlliance = value.m_bAlliance;
		
	StringCchCopy( m_szClubName, CHAR_SZNAME, value.m_szClubName );

	return *this;
}

GLCLUBBATTLE_LOBY& GLCLUBBATTLE_LOBY::operator = ( const GLCLUBBATTLE& value )
{
	m_dwCLUBID = value.m_dwCLUBID;
	m_tStartTime = value.m_tStartTime;
	m_tEndTime = value.m_tEndTime;
	m_wKillPoint = value.m_wKillPoint + value.m_wKillPointTemp;
	m_wDeathPoint = value.m_wDeathPoint + value.m_wDeathPointTemp;
	m_bAlliance = value.m_bAlliance;
		
	StringCchCopy( m_szClubName, CHAR_SZNAME, value.m_szClubName );

	return *this;
}

GLCLUB::GLCLUB () 
	: m_dwID(0)
	, m_dwMasterID(0)
	, m_dwRank(0)
	, m_dwCDCertifior(0)
	, m_dwMarkVER(0)
	, m_dwconftCLUB(CLUB_NULL)
	, m_fconftTIMER(0)
	, m_fTIMER_MONEY(0)
	, m_fTIMER_POS(0)
	, m_fTIMER_BATTLE(0)
	, m_bVALID_STORAGE(FALSE)
	, m_lnIncomeMoney(0)
	, m_lnStorageMoney(0)
	, m_dwAlliance(CLUB_NULL)
	, m_dwReqClubID(CLUB_NULL)
	, m_dwBattleArmisticeReq(0)
	, m_dwBattleWin(0)
	, m_dwBattleLose(0)
	, m_dwBattleDraw(0)
	, m_dwAllianceBattleWin(0)
	, m_dwAllianceBattleLose(0)
	, m_dwAllianceBattleDraw(0)
{
	SecureZeroMemory(m_szName, sizeof(m_szName));
	SecureZeroMemory(m_szMasterName, sizeof(m_szMasterName));
	SecureZeroMemory(m_szNotice, sizeof(m_szNotice));
	SecureZeroMemory(m_aryMark, sizeof(m_aryMark));

	CTime tDisTime(1970,2,1,1,1,1);

	m_tOrganize = tDisTime.GetTime();
	m_tDissolution = tDisTime.GetTime();
	m_tAuthority = tDisTime.GetTime();	
	m_tAllianceSec = tDisTime.GetTime();
	m_tAllianceDis = tDisTime.GetTime();	
	m_tLastBattle = tDisTime.GetTime();

	for ( int i=0; i<MAX_CLUBSTORAGE; ++i )
	{
		m_cStorage[i].SetState ( STORAGE_INVEN_X, STORAGE_INVEN_Y );
	}
}

GLCLUB::~GLCLUB ()
{
}

GLCLUB& GLCLUB::operator = ( const GLCLUB& value )
{
	if( this == &value )
		return *this;

	m_dwID = value.m_dwID;
	StringCchCopy ( m_szName, CHAR_SZNAME, value.m_szName );

	m_dwMasterID = value.m_dwMasterID;
	StringCchCopy ( m_szMasterName, CHAR_SZNAME, value.m_szMasterName );

	m_dwCDCertifior = value.m_dwCDCertifior;

	m_dwRank = value.m_dwRank;

	m_dwMarkVER = value.m_dwMarkVER;
	memcpy ( m_aryMark, value.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

	m_tOrganize = value.m_tOrganize;
	m_tDissolution = value.m_tDissolution;
	m_tAuthority = value.m_tAuthority;

	m_tAllianceSec = value.m_tAllianceSec;
	m_tAllianceDis = value.m_tAllianceDis;

	StringCchCopy ( m_szNotice, EMCLUB_NOTICE_LEN+1, value.m_szNotice );

	m_mapMembers = value.m_mapMembers;
	m_listCDMMembers = value.m_listCDMMembers;

	m_dwAlliance = value.m_dwAlliance;
	m_setAlliance = value.m_setAlliance;

	m_dwconftCLUB = value.m_dwconftCLUB;
	m_fconftTIMER = value.m_fconftTIMER;
	m_setConftSTART = value.m_setConftSTART;
	m_setConftCURRENT = value.m_setConftCURRENT;
	m_conftOPT = value.m_conftOPT;

	m_fTIMER_MONEY = value.m_fTIMER_MONEY;
	m_fTIMER_POS = value.m_fTIMER_POS;
	m_fTIMER_BATTLE = value.m_fTIMER_POS;

	m_bVALID_STORAGE = value.m_bVALID_STORAGE;
	m_lnIncomeMoney = value.m_lnIncomeMoney;
	m_lnStorageMoney = value.m_lnStorageMoney;
	
	for ( int i=0; i<MAX_CLUBSTORAGE; ++i )
	{
		m_cStorage[i].Assign ( value.m_cStorage[i] );
	}

	m_mapBattle = value.m_mapBattle;
	m_mapBattle = value.m_mapBattle;								//	배틀 정보
	m_dwReqClubID = value.m_dwReqClubID;
	m_dwBattleWin = value.m_dwBattleWin;
	m_dwBattleLose = value.m_dwBattleLose;
	m_dwBattleDraw = value.m_dwBattleDraw;
	m_tLastBattle = value.m_tLastBattle;

	m_dwAllianceBattleWin = value.m_dwAllianceBattleWin;
	m_dwAllianceBattleLose = value.m_dwAllianceBattleLose;
	m_dwAllianceBattleDraw = value.m_dwAllianceBattleDraw;

	return *this;
}

void GLCLUB::SetInfo ( GLCLUBINFO& value )
{
	m_dwID = value.m_dwID;
	StringCchCopy ( m_szName, CHAR_SZNAME, value.m_szName );

	m_dwMasterID = value.m_dwMasterID;
	StringCchCopy ( m_szMasterName, CHAR_SZNAME, value.m_szMasterName );
	m_dwCDCertifior = value.m_dwCDCertifior;

	StringCchCopy ( m_szName, CHAR_SZNAME, value.m_szName );
	m_dwRank = value.m_dwRank;

	m_dwMarkVER = value.m_dwMarkVER;
	memcpy ( m_aryMark, value.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

	m_tOrganize = value.m_tOrganize;
	m_tDissolution = value.m_tDissolution;
	m_tAuthority = value.m_tAuthority;

	m_tAllianceSec = value.m_tAllianceSec;
	m_tAllianceDis = value.m_tAllianceDis;

	StringCchCopy ( m_szNotice, EMCLUB_NOTICE_LEN+1, value.m_szNotice );

	m_lnIncomeMoney = value.m_lnIncomeMoney;
	m_lnStorageMoney = value.m_lnStorageMoney;

	m_dwBattleWin = value.m_dwBattleWin;
	m_dwBattleLose = value.m_dwBattleLose;
	m_dwBattleDraw = value.m_dwBattleDraw;

	m_dwAllianceBattleWin = value.m_dwAllianceBattleWin;
	m_dwAllianceBattleLose = value.m_dwAllianceBattleLose;
	m_dwAllianceBattleDraw = value.m_dwAllianceBattleDraw;
}

BOOL GLCLUB::SETSTORAGE_BYBUF ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )		return TRUE;

	DWORD dwChannel = 0;
	ByteStream >> dwChannel;
	if ( dwChannel > MAX_CLUBSTORAGE )	dwChannel = MAX_CLUBSTORAGE;
	
	for ( DWORD i=0; i<dwChannel; ++i )
	{
		BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER ( ByteStream );
		if ( !bOk )		return TRUE;
	}

	return TRUE;
}

BOOL GLCLUB::GETSTORAGE_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << DWORD(MAX_CLUBSTORAGE);
	for ( DWORD i=0; i<MAX_CLUBSTORAGE; ++i )	m_cStorage[i].GETITEM_BYBUFFER_FORSTORAGE ( ByteStream );

	return TRUE;
}

bool GLCLUB::DoDissolution ( __time64_t tDISSOLUTION )
{
	m_tDissolution = tDISSOLUTION;
	return true;
}

bool GLCLUB::DoDissolutionCancel ()
{
	CTime tDisTime(1970,2,1,1,1,1);
	m_tDissolution = tDisTime.GetTime();
	return true;
}

bool GLCLUB::IsRegDissolution ()
{
	CTime tDisTime ( m_tDissolution );
	return tDisTime.GetYear()!=1970;
}

bool GLCLUB::CheckDissolution ()
{
	if ( !IsRegDissolution() )		return false;

	CTime tCurTime = CTime::GetCurrentTime();
	return tCurTime>=m_tDissolution;
}

void GLCLUB::RESET ()
{
	m_dwID = 0;
	m_dwMasterID = 0;
	m_dwRank = 0;
	m_dwCDCertifior = 0;

	m_dwMarkVER = 0;
	m_tOrganize = 0;
	m_tDissolution = 0;

	memset (m_szName, 0, sizeof(char) * CHAR_SZNAME);
	memset (m_szMasterName, 0, sizeof(char) * CHAR_SZNAME);
	memset (m_szNotice, 0, sizeof(char) * EMCLUB_NOTICE_LEN+1);

	CTime tDisTime(1970,2,1,1,1,1);
	m_tDissolution = tDisTime.GetTime();

	m_mapMembers.clear();
	m_listCDMMembers.clear();

	m_dwAlliance = CLUB_NULL;
	m_setAlliance.clear();	

	m_setConftSTART.clear();
	m_setConftCURRENT.clear();
	m_conftOPT.RESET ();

	m_fTIMER_POS = 0;
	m_fTIMER_MONEY = 0;
	m_fTIMER_BATTLE = 0;

	m_bVALID_STORAGE = FALSE;
	m_lnIncomeMoney = 0;
	m_lnStorageMoney = 0;
	for ( int i=0; i<MAX_CLUBSTORAGE; ++i )
	{
		m_cStorage[i].DeleteItemAll();
	}

	m_dwReqClubID = CLUB_NULL;
	m_dwBattleArmisticeReq = 0;
	m_mapBattle.clear();
	m_mapBattleDel.clear();

	m_dwBattleWin = 0;
	m_dwBattleLose = 0;
	m_dwBattleDraw = 0;
	m_tLastBattle = tDisTime.GetTime();

	m_dwAllianceBattleWin = 0;
	m_dwAllianceBattleLose = 0;
	m_dwAllianceBattleDraw = 0;
}

void GLCLUB::RESET_STORAGE ()
{
	m_bVALID_STORAGE = FALSE;
	m_lnIncomeMoney = 0;
	m_lnStorageMoney = 0;
	for ( int i=0; i<MAX_CLUBSTORAGE; ++i )
		m_cStorage[i].DeleteItemAll();
}

bool GLCLUB::AddMember ( DWORD dwCharID, const char *szCharName, DWORD dwFlags )
{
	GLCLUBMEMBER cMEMBER;
	cMEMBER.dwID = dwCharID;
	StringCchCopy ( cMEMBER.m_szName, CHAR_SZNAME, szCharName );
	cMEMBER.m_dwFlags = dwFlags;

	m_mapMembers[dwCharID] = cMEMBER;

	return true;
}

bool GLCLUB::DelMember ( DWORD dwCharID )
{
	CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
	if ( pos==m_mapMembers.end() )		return false;

	if ( m_dwCDCertifior==dwCharID )	m_dwCDCertifior = 0;

	m_mapMembers.erase(pos);

	return true;
}

bool GLCLUB::IsMember ( DWORD dwCharID )
{
	CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
	return pos!=m_mapMembers.end();
}

inline GLCLUBMEMBER* GLCLUB::GetMember ( DWORD dwCharID )
{
	CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
	if ( pos==m_mapMembers.end() )		return NULL;

	return &(*pos).second;
}

void GLCLUB::SetMemberState ( DWORD dwCharID, bool bONLINE, int nCHANNEL, SNATIVEID nidMAP )
{
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return;

	pMember->bONLINE = bONLINE;
	pMember->nCHANNEL = nCHANNEL;
	pMember->nidMAP = nidMAP;
}

void GLCLUB::SetMemberPos ( DWORD dwCharID, D3DXVECTOR3 vecPOS )
{
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return;

	pMember->vecPOS = vecPOS;
}

bool GLCLUB::SetMemberFlag ( DWORD dwCharID, DWORD dwFlags )
{
	//	Note : 실제로 지원되는 flag만 설정.
	dwFlags = (dwFlags&EMCLUB_SUBMATER_ALL);

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	//	CDM 셋팅
	if ( !SetMemberCDMFlag( dwCharID, (dwFlags & EMCLUB_SUBMATER_CDM) ) ) return false;

	//	Note : 선도클럽 인증 권한 유무.
	if ( dwFlags&EMCLUB_SUBMATER_CD )
	{
		// 이미 다른 부마스터가 인증권한을 갖고 있을경우
		if ( m_dwCDCertifior != 0 && m_dwCDCertifior != dwCharID )
		{
			// 인증권한 플래그를 제거한다.
			GLCLUBMEMBER* pOldMember = GetMember ( m_dwCDCertifior );
			if ( pOldMember )
			{
				pOldMember->m_dwFlags &= ~EMCLUB_SUBMATER_CD;
			}
		}

		//	선도클럽 인증 권한 부여.
		m_dwCDCertifior = dwCharID;
	}
	else if ( m_dwCDCertifior == dwCharID )
	{
		//	선도클럽 인증 권한 제거.
		m_dwCDCertifior = 0;
		pMember->m_dwFlags &= ~EMCLUB_SUBMATER_CD;
	}

	pMember->m_dwFlags = dwFlags;

	return true;
}

DWORD GLCLUB::GetMemberFlag ( DWORD dwCharID )
{
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return NULL;

	return pMember->m_dwFlags;
}

bool GLCLUB::SetMemberCDMFlag ( DWORD dwCharID, BOOL bSet )
{
	if ( bSet )
	{
		if ( !EnableCDMFlag( dwCharID )	) return false;

		if ( !IsMemberFlgCDM( dwCharID ) )
		{
			m_listCDMMembers.push_back( dwCharID );
		}
	}
	else
	{
		//	CDM 권한 있을경우 제거
		if ( IsMemberFlgCDM( dwCharID ) )
		{
			CLUB_CDM_MEM_ITER pos = std::find(m_listCDMMembers.begin(),m_listCDMMembers.end(),dwCharID );
			if ( pos != m_listCDMMembers.end() )	m_listCDMMembers.erase( pos );
		}
	}

	return true;
}

bool GLCLUB::AddAlliance ( DWORD dwAlliance, const char *szClubName )
{
	std::pair <CLUB_ALLIANCE_ITER, bool> status = m_setAlliance.insert ( GLCLUBALLIANCE(dwAlliance,szClubName) );
	if (status.second)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

bool GLCLUB::DelAlliance ( DWORD dwAlliance )
{
	CLUB_ALLIANCE_ITER pos = m_setAlliance.find ( GLCLUBALLIANCE(dwAlliance,"") );
	if ( pos!=m_setAlliance.end() )	m_setAlliance.erase ( pos );

	return true;
}

bool GLCLUB::ClearAlliance ()
{
	m_dwAlliance = CLUB_NULL;
	m_setAlliance.clear();

	ReSetAllianceBattle();

	return true;
}

void GLCLUB::SetAllianceDis ()
{
	//	해체시 제한시간 설정.
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_ALLIANCE_DIS, 0, 0 );
	tCurTime += tSpan;

	m_tAllianceDis = tCurTime.GetTime();
}

void GLCLUB::SetAllianceSec ()
{
	//	제명시 제한시간 설정.
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_ALLIANCE_SEC, 0, 0 );
	tCurTime += tSpan;

	m_tAllianceSec = tCurTime.GetTime();
}

void GLCLUB::SetAuthority ()
{
	//	제명시 제한시간 설정.
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_AUTHORITY, 0, 0 );
	tCurTime += tSpan;

	m_tAuthority = tCurTime.GetTime();
}


bool GLCLUB::IsMaster ( DWORD dwCharID )
{
	return (m_dwMasterID==dwCharID);
}

bool GLCLUB::IsSubMaster ( DWORD dwCharID )
{
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return ( pMember->m_dwFlags&EMCLUB_SUBMATER_ALL ) != NULL;
}

bool GLCLUB::IsAlliance()
{
	return (m_dwAlliance!=CLUB_NULL);
}

bool GLCLUB::IsAllianceGuild ( DWORD dwGuildID )
{
	if ( m_dwID == 0 || dwGuildID == 0 ) return false;
    
	if ( m_dwID==dwGuildID )	return true;

	CLUB_ALLIANCE_ITER pos = m_setAlliance.find ( GLCLUBALLIANCE(dwGuildID,"") );
	return pos!=m_setAlliance.end();
}

bool GLCLUB::IsChief ()
{
	return (m_dwAlliance==m_dwID);
}

// *****************************************************
// Desc: 동맹 결성 제약시간 유무 체크
// *****************************************************
bool GLCLUB::IsAllianceDis ()
{
	CTime tDis(m_tAllianceDis);
	CTime tCur = CTime::GetCurrentTime();

	// CTime 의 멤버를 호출할때는 한번에 값을 받아와야 한다.
	// 내부적으로 static 으로 정의되어 있기 때문에
	// 다음번 동작이 유효하다고 보장되지 않는다.	
	// This function calls GetLocalTm, which uses an internal statically allocated buffer.
	// The data in this buffer is overwritten because of calls to other CTime member functions.
	int nYear = tDis.GetYear();
	if ( nYear <= 1970 || nYear > 2038 ) 
		return false;

	// 시간이 지나면 초기화
	if ( tDis <= tCur )
	{
		CTime tTime(1970,2,1,1,1,1);
		m_tAllianceDis = tTime.GetTime();
		return false;
	}

	return true;
}

// *****************************************************
// Desc: 동맹 가입 제약시간 유무 체크
// *****************************************************
bool GLCLUB::IsAllianceSec ()
{
	CTime tSec(m_tAllianceSec);
	CTime tCur = CTime::GetCurrentTime();

	int nYear = tSec.GetYear();
	if ( nYear <= 1970 || nYear > 2038 ) 
		return false;

	// 시간이 지나면 초기화
	if ( tSec <= tCur )
	{
		CTime tTime(1970,2,1,1,1,1);
		m_tAllianceSec = tTime.GetTime();
		return false;
	}

	return true;
}

bool GLCLUB::IsAuthority ()
{
	CTime tSec(m_tAuthority);
	CTime tCur = CTime::GetCurrentTime();

	int nYear = tSec.GetYear();
	if ( nYear <= 1970 || nYear > 2038 ) 
		return false;

	// 시간이 지나면 초기화
	if ( tSec <= tCur )
	{
		CTime tTime(1970,2,1,1,1,1);
		m_tAuthority = tTime.GetTime();
		return false;
	}

	return true;
}

bool GLCLUB::SetAuthorityMaster( DWORD dwCharID )
{
	
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;	

	m_dwMasterID = dwCharID;
	StringCchCopy( m_szMasterName, CHAR_SZNAME, pMember->m_szName );

	//	CDM 권한 있을경우 제거
	SetMemberCDMFlag( dwCharID, FALSE );

	pMember->m_dwFlags = 0;

	GLDBMan* pDBMan = GLAgentServer::GetInstance().GetDBMan();
	if ( !pDBMan )	return false;
	
	//	Note : Flag 저장.
	CSetClubMasterFlags *pDbAction = new CSetClubMasterFlags(m_dwID,dwCharID,pMember->m_dwFlags);
	pDBMan->AddJob ( pDbAction );

	if ( m_dwCDCertifior == dwCharID )
	{
		m_dwCDCertifior = 0; 
		
		CDbAction *pNewDbAction = new CSetClubDeputy ( m_dwID, m_dwCDCertifior );
		pDBMan->AddJob ( pNewDbAction );

		//	Note : 클럽 인증 CD 권한 설정에 대한 로그
		GLITEMLMT::GetInstance().ReqAction
		(
			m_dwMasterID,		//	당사자.
			EMLOGACT_CLUB_CLUBDEPUTY_MEMBER,	//	행위.
			ID_CHAR, m_dwCDCertifior,	//	상대방.
			0,							//	exp
			0,							//	bright
			0,							//	life
			0							//	money
		);
	}

	return true;

}

bool GLCLUB::IsMemberFlgJoin ( DWORD dwCharID )
{
	if ( IsMaster(dwCharID) )	return true;

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return (pMember->m_dwFlags&EMCLUB_SUBMATER_JOIN)!=NULL;
}

bool GLCLUB::IsMemberFlgKick ( DWORD dwCharID )
{
	if ( IsMaster(dwCharID) )	return true;

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return (pMember->m_dwFlags&EMCLUB_SUBMATER_KICK)!=NULL;
}

bool GLCLUB::IsMemberFlgNotice ( DWORD dwCharID )
{
	if ( IsMaster(dwCharID) )	return true;

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return (pMember->m_dwFlags&EMCLUB_SUBMATER_NOTICE)!=NULL;
}

bool GLCLUB::IsMemberFlgCDCertify ( DWORD dwCharID )
{
	// 클럽의 맴버가 아니면
	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )										return false;

	//	동맹에 가입되어 있을 경우
	if ( IsAlliance() )
	{
		// 동맹의 마스터 클럽이면
		if ( IsChief() )
		{
			if ( m_dwCDCertifior==0 && IsMaster(dwCharID) )
			{
				return true;
			}
			else
			{
				return m_dwCDCertifior==dwCharID;
			}
		}
	}
	else
	{
		//	인증 권한 대행자가 없을경우는 마스터에게 권한이 있음.
		if ( m_dwCDCertifior==0 && IsMaster(dwCharID) )	
		{
			return true;
		}
		else
		{
			//	인증 권한 대행자가 있을 경우 점검.
			return m_dwCDCertifior==dwCharID;
		}
	}

	return false;
}

bool GLCLUB::IsMemberFlgMarkChange ( DWORD dwCharID )
{
	if ( IsMaster(dwCharID) )	return true;

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return (pMember->m_dwFlags&EMCLUB_SUBMATER_MARK)!=NULL;
}

bool GLCLUB::IsMemberFlgCDM ( DWORD dwCharID )
{
	if ( IsMaster(dwCharID) )	return true;

	GLCLUBMEMBER* pMember = GetMember ( dwCharID );
	if ( !pMember )		return false;

	return (pMember->m_dwFlags&EMCLUB_SUBMATER_CDM)!=NULL;
}

bool GLCLUB::EnableCDMFlag( DWORD dwCharID )
{
	if ( IsMemberFlgCDM ( dwCharID ) ) return true;
	
	if ( m_listCDMMembers.size() < GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM ) return true;

	return false;
}

void GLCLUB::CLEARCONFT ()
{
	m_dwconftCLUB = CLUB_NULL;
	m_fconftTIMER = 0;
	m_conftOPT.RESET();

	m_setConftSTART.clear ();
	m_setConftCURRENT.clear ();
}

void GLCLUB::ADDCONFT_MEMBER ( DWORD dwID )
{
	m_setConftCURRENT.insert ( dwID );
}

void GLCLUB::DELCONFT_MEMBER ( DWORD dwID )
{
	CLUBCONFT_ITER pos = m_setConftCURRENT.find ( dwID );
	if ( pos!=m_setConftCURRENT.end() )		m_setConftCURRENT.erase(pos);
}

DWORD GLCLUB::GETCONFT_NUM ()
{
	return (DWORD) m_setConftCURRENT.size();
}

void GLCLUB::SETCONFT_MEMBER ()
{
	m_setConftSTART = m_setConftCURRENT;
}

bool GLCLUB::ISCONFT_MEMBER ( DWORD dwID )
{
	CLUBCONFT_ITER pos = m_setConftCURRENT.find ( dwID );
	return ( pos!=m_setConftCURRENT.end() );
}

bool GLCLUB::ISCONFTING ()
{
	return m_dwconftCLUB!=CLUB_NULL;
}

bool GLCLUB::ISCONFT_LOSS ()
{
	return m_setConftCURRENT.empty()==true;
}

void GLCLUB::SAVESTORAGE2DB ()
{
	GLDBMan *pDBMan = GLGaeaServer::GetInstance().GetDBMan();
	if ( !pDBMan )				return;

	if ( !m_bVALID_STORAGE )	return;

	//	Note : 창고 인벤 저장.
	CByteStream cStream;
	BOOL bOK = GETSTORAGE_BYBUF ( cStream );
	if ( bOK )
	{
		LPBYTE pBuff(NULL);
		DWORD dwSize(NULL);
		cStream.GetBuffer( pBuff, dwSize );

		CWriteClubStorage *pSaveClubStorageDB = new CWriteClubStorage ( m_dwID, (BYTE*) pBuff, dwSize );
		pDBMan->AddJob ( pSaveClubStorageDB );
	}

	//	Note : Money Db에 저장.
	CSetClubMoney *pDbAction = new CSetClubMoney ( m_dwID, m_lnStorageMoney );
	pDBMan->AddJob ( pDbAction );
}

void GLCLUB::ADDBATTLECLUB( const GLCLUBBATTLE& sBattleClub )
{
	m_mapBattle[sBattleClub.m_dwCLUBID] = sBattleClub;
}

void GLCLUB::DELBATTLECLUB( DWORD dwClubID )
{
	CLUB_BATTLE_ITER pos = m_mapBattle.find(dwClubID);
	if ( pos==m_mapBattle.end() )		return;
	m_mapBattle.erase(pos);
}

void GLCLUB::ADDBATTLE_READY_CLUB( const GLCLUBBATTLE& sBattleClub )
{
	m_mapBattleReady[sBattleClub.m_dwCLUBID] = sBattleClub;
}

void GLCLUB::DELBATTLE_READY_CLUB( DWORD dwClubID )
{
	CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
	if ( pos==m_mapBattleReady.end() )		return;
	m_mapBattleReady.erase(pos);
}

GLCLUBBATTLE* GLCLUB::GetClubBattle( DWORD dwClubID )
{
	CLUB_BATTLE_ITER pos = m_mapBattle.find(dwClubID);
	if ( pos==m_mapBattle.end() )		return NULL;

	return &(*pos).second;
}

GLCLUBBATTLE* GLCLUB::GetClubBattleReady( DWORD dwClubID )
{
	CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
	if ( pos==m_mapBattleReady.end() )		return NULL;

	return &(*pos).second;
}

GLCLUBBATTLE_DEL* GLCLUB::GetClubBattleDel( DWORD dwClubID )
{
	CLUB_BATTLE_DEL_ITER pos = m_mapBattleDel.find(dwClubID);
	if ( pos==m_mapBattleDel.end() )		return NULL;

	return &(*pos).second;
}

bool GLCLUB::IsBattleStop( DWORD dwClubID )
{
	GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
	if ( !pClubBattle ) return false;

	CTime tStopTime = CTime::GetCurrentTime();
	CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME,0);
	CTime tDisTime ( pClubBattle->m_tStartTime);
	tDisTime += tDisTimeSpan;

	if ( tDisTime < tStopTime )	return true;

	return false;
}

bool GLCLUB::IsGuidBattle()
{
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_GUID_TIME,0);
	CTime tDisTime ( m_tLastBattle );
	tDisTime += tDisTimeSpan;

	if ( tDisTime < tCurTime )	return true;

	return false;
}

bool GLCLUB::IsBattle( DWORD dwClubID )
{
	if ( dwClubID == 0 ) return false;

	GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
	if ( !pClubBattle || pClubBattle->m_bAlliance ) return false;

	return true;
}

bool GLCLUB::IsBattleAlliance( DWORD dwClubID )
{
	if ( dwClubID == 0 ) return false;

	GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
	if ( !pClubBattle || !pClubBattle->m_bAlliance ) return false;

	return true;
}

bool GLCLUB::IsBattleReady( DWORD dwClubID )
{
	if ( dwClubID == 0 ) return false;

	CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
	return pos!=m_mapBattleReady.end();
}

void GLCLUB::KillBattleClubMem( DWORD dwClubID, WORD nKill )
{
	GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
	if( !pClubBattle ) return;
	pClubBattle->m_wKillPoint += nKill;
	pClubBattle->m_bKillUpdate = true;
}
void GLCLUB::DeathBattleClubMem( DWORD dwClubID, WORD nDeath )
{
	GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
	if( !pClubBattle ) return;
	pClubBattle->m_wDeathPoint += nDeath;
	pClubBattle->m_bKillUpdate = true;
}

void GLCLUB::SetLastBattlePoint( DWORD dwClubID, WORD wKillPoint, WORD wDeathPoint )
{
	CLUB_BATTLE_DEL_ITER pos = m_mapBattleDel.find(dwClubID);
	if ( pos==m_mapBattleDel.end() )		return;

	GLCLUBBATTLE_DEL * pClubBattleDel = &(*pos).second;
	pClubBattleDel->m_dwField++;
	pClubBattleDel->m_sClubBattle.m_wKillPoint += wKillPoint;
	pClubBattleDel->m_sClubBattle.m_wDeathPoint += wDeathPoint;
	
	DWORD dwFieldNum = GLAgentServer::GetInstance().GetFieldNum();
	
	if ( pClubBattleDel->m_dwField >= dwFieldNum )	pClubBattleDel->m_bIsComplated = true;
}

//	동맹배틀 전적 초기화
void GLCLUB::ReSetAllianceBattle()
{
	m_dwAllianceBattleWin = 0;
	m_dwAllianceBattleLose = 0;
	m_dwAllianceBattleDraw = 0;
}

DWORD GLClubMan::FindClub ( std::string strName )
{
	CLUBSNAME_ITER pos = m_mapCLUBNAME.find ( strName );
	if ( pos==m_mapCLUBNAME.end() )		return CLUB_NULL;

	return (*pos).second;
}

GLCLUB* GLClubMan::GetClub ( DWORD dwID )
{
	CLUBS_ITER pos = m_mapCLUB.find ( dwID );
	if ( pos==m_mapCLUB.end() )		return NULL;

	return &(*pos).second;
}

void GLClubMan::AddClub ( GLCLUB &cCLUB )
{
	if ( cCLUB.m_dwID==CLUB_NULL )	return;

	m_mapCLUB.insert ( std::make_pair ( cCLUB.m_dwID, cCLUB ) );
	//m_mapCLUB[cCLUB.m_dwID] = cCLUB;
	m_mapCLUBNAME.insert ( std::make_pair ( std::string(cCLUB.m_szName), cCLUB.m_dwID ) );
	//m_mapCLUBNAME[std::string(cCLUB.m_szName)] = cCLUB.m_dwID;
}

bool GLClubMan::DelClub ( DWORD dwID )
{
	CLUBS_ITER pos = m_mapCLUB.find(dwID);
	if ( pos==m_mapCLUB.end() )		return false;

	GLCLUB cCLUB = (*pos).second;
	CLUBSNAME_ITER pos_name = m_mapCLUBNAME.find ( std::string(cCLUB.m_szName) );
	if ( pos_name!=m_mapCLUBNAME.end() )	m_mapCLUBNAME.erase ( pos_name );

	m_mapCLUB.erase ( pos );

	return true;
}

bool GLClubMan::LoadFromDB ( GLDBMan *pDBMan, bool bFieldServer )
{
	if ( !pDBMan )	return false;

	std::vector<GLCLUBINFO> vecClubInfo;
	int nRET(0);
	
	nRET = pDBMan->GetClubInfo ( vecClubInfo );
	if ( nRET!=0 )
	{
		return false;
	}

	for ( DWORD i=0; i<vecClubInfo.size(); ++i )
	{
		GLCLUBINFO &sClubInfo = vecClubInfo[i];

		GLCLUB sCLUB;
		sCLUB.SetInfo ( sClubInfo );

		std::vector<GLCLUBMEMBER>	vecMEMBER;
		nRET = pDBMan->GetClubMember ( sClubInfo.m_dwID, vecMEMBER );
		if ( nRET!=0 )
		{
			continue;
		}

		for ( DWORD m=0; m<vecMEMBER.size(); ++m )
		{
			const GLCLUBMEMBER &sMEMBER = vecMEMBER[m];
			sCLUB.AddMember ( sMEMBER.dwID, sMEMBER.m_szName, sMEMBER.m_dwFlags );
			
			//	클럽 CDM 권한
			if ( sCLUB.IsMemberFlgCDM( sMEMBER.dwID ) )
			{
				sCLUB.m_listCDMMembers.push_back( sMEMBER.dwID );
			}
		}

		//	인증 가능자가 멤버에서 제외 되었을 경우 혹은 케릭터가 삭제되었을 경우. 인증권한 회수.
		if ( sCLUB.m_dwCDCertifior!=0 )
		{
			if ( !sCLUB.IsMember(sCLUB.m_dwCDCertifior) )
			{
				sCLUB.m_dwCDCertifior = 0;
			}
		}

		std::vector<GLCLUBBATTLE>	vecBATTLE;
		nRET = pDBMan->GetClubBattleInfo ( sClubInfo.m_dwID, vecBATTLE );
		if ( nRET!=0 )
		{
			continue;
		}

		for ( DWORD m=0; m<vecBATTLE.size(); ++m )
		{
			const GLCLUBBATTLE &sBATTLE = vecBATTLE[m];
			sCLUB.ADDBATTLECLUB ( sBATTLE );
		}

		// Memo : 필드 서버이면 클럽 수입은 0으로 초기화 시킨다.
		//		AgentServer에서도 클럽 수입을 가지고 있기 때문이다.
		if( bFieldServer )
		{
			sCLUB.m_lnIncomeMoney = 0;
		}

		//	Note : 클럽 등록.
		//
		AddClub ( sCLUB );
	}

	// 클럽 배틀 관련 설정을 초기화 한다.
	InitClubBattle( bFieldServer );

	//	Note : 연합 정보 가져오기.
	std::vector< std::pair<DWORD,DWORD> > vecAlliance;
	pDBMan->GetClubAlliance ( vecAlliance );
	for ( size_t i=0; i<vecAlliance.size(); ++i )
	{
		const std::pair<DWORD,DWORD> &pairAlliance = vecAlliance[i];

		GLCLUB* pCLUB_A = GetClub ( pairAlliance.first );
		if ( !pCLUB_A )	continue;

		GLCLUB* pCLUB_B = GetClub ( pairAlliance.second );
		if ( !pCLUB_B )	continue;

		pCLUB_A->m_dwAlliance = pairAlliance.first;
		pCLUB_B->m_dwAlliance = pairAlliance.first;

		pCLUB_B->m_dwAllianceBattleWin = pCLUB_A->m_dwAllianceBattleWin;
		pCLUB_B->m_dwAllianceBattleLose = pCLUB_A->m_dwAllianceBattleLose;
		pCLUB_B->m_dwAllianceBattleDraw = pCLUB_A->m_dwAllianceBattleDraw;

		pCLUB_A->AddAlliance ( pCLUB_A->m_dwAlliance, pCLUB_A->m_szName );
		pCLUB_A->AddAlliance ( pCLUB_B->m_dwID, pCLUB_B->m_szName );		
	}

	// 동맹배틀을 설정한다.
	InitAllianceBattle ();

	return true;
}

// 배틀 정보에 이름을 입력한다.
bool GLClubMan::InitClubBattle( bool bFieldServer )
{
	// 수정요망

	CLUBS_ITER pos = m_mapCLUB.begin();
	CLUBS_ITER end = m_mapCLUB.end();
	for ( ; pos!=end; ++pos )
	{
		GLCLUB &cCLUB = (*pos).second;

		CLUB_BATTLE_ITER pos_B = cCLUB.m_mapBattle.begin();
		CLUB_BATTLE_ITER end_B = cCLUB.m_mapBattle.end();
		CLUB_BATTLE_ITER del_B;
		for ( ; pos_B!=end_B; pos_B )
		{
			GLCLUBBATTLE &sClubBattle = (*pos_B).second;
			del_B = pos_B++;
			
			GLCLUB* pClub = GetClub( sClubBattle.m_dwCLUBID );
			if ( pClub )	StringCchCopy( sClubBattle.m_szClubName, CHAR_SZNAME, pClub->m_szName );

			__time64_t curTime = CTime::GetCurrentTime().GetTime();

			// 배틀 종료				
			if ( sClubBattle.m_tEndTime < curTime )
			{
				if ( bFieldServer )
				{
					EndClubBattleField( cCLUB.m_dwID, sClubBattle.m_dwCLUBID );
				}
				else
				{
					EndClubBattleAgent( cCLUB.m_dwID, sClubBattle.m_dwCLUBID );
				}
				
				cCLUB.DELBATTLECLUB( (*del_B).first );	
				GLCLUB* pCLUB = GetClub( sClubBattle.m_dwCLUBID );
				if ( pCLUB ) pCLUB->DELBATTLECLUB( cCLUB.m_dwID );	
			}
		}
	}

	return true;
}

HRESULT GLClubMan::FrameMoveAgent( float fTime, float fElapsedTime )
{
	//	클럽 유지 관리.
	GLAgentServer& cAgentServer = GLAgentServer::GetInstance();

	CLUBS_ITER del;
	CLUBS_ITER pos = m_mapCLUB.begin();
	CLUBS_ITER end = m_mapCLUB.end();
	for ( ; pos!=end; )
	{
		GLCLUB &cCLUB = (*pos).second;
		del = pos++;

		//	Note : 대련 패배 점검.
		//
		bool bCONFT_END(false);
		std::string strCLUB_A, strCLUB_B;
		EMCONFRONT_END emEND_A(EMCONFRONT_END_CLOSS);
		EMCONFRONT_END emEND_B(EMCONFRONT_END_CWIN);

		if ( cCLUB.ISCONFTING() )
		{
			if ( cCLUB.ISCONFT_LOSS() )
			{
				GLCLUB *pConftClub = GetClub(cCLUB.m_dwconftCLUB);

				if ( !pConftClub )
				{
					emEND_A = EMCONFRONT_END_FAIL;
					emEND_B = EMCONFRONT_END_FAIL;
				}
				else if ( pConftClub->ISCONFT_LOSS() )
				{
					emEND_A = EMCONFRONT_END_CLOSS;
					emEND_B = EMCONFRONT_END_CLOSS;
				}

				bCONFT_END = true;
				strCLUB_A = cCLUB.m_szName;
				if ( pConftClub )	strCLUB_B = pConftClub->m_szName;


				//	Note : 클럽 대련이 종료됨을 알림 to (필드)
				//
				GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld;
				NetMsgFld.dwCLUB_A = cCLUB.m_dwID;
				NetMsgFld.dwCLUB_B = cCLUB.m_dwconftCLUB;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				cAgentServer.SENDTOALLCHANNEL(&NetMsgFld);

				{
					CLUBCONFT_ITER conft_pos = cCLUB.m_setConftCURRENT.begin();
					CLUBCONFT_ITER conft_end = cCLUB.m_setConftCURRENT.end();
					for ( ; conft_pos!=conft_end; ++conft_pos )
					{
						PGLCHARAG pMEMBER = cAgentServer.GetCharID ( *conft_pos );
						if ( pMEMBER )	pMEMBER->ResetConfrontOk ();
					}
				}

				if ( pConftClub )
				{
					CLUBCONFT_ITER conft_pos = pConftClub->m_setConftCURRENT.begin();
					CLUBCONFT_ITER conft_end = pConftClub->m_setConftCURRENT.end();
					for ( ; conft_pos!=conft_end; ++conft_pos )
					{
						PGLCHARAG pMEMBER = cAgentServer.GetCharID ( *conft_pos );
						if ( pMEMBER )	pMEMBER->ResetConfrontOk ();
					}
				}

				cCLUB.CLEARCONFT();
				if ( pConftClub )		pConftClub->CLEARCONFT();
			}


			cCLUB.m_fconftTIMER += fElapsedTime;
			if ( cCLUB.m_fconftTIMER > GLCONST_CHAR::fCONFRONT_TIME )
			{
				GLCLUB *pConftClub = GetClub(cCLUB.m_dwconftCLUB);

				emEND_A = (EMCONFRONT_END_CTIME);
				emEND_B = (EMCONFRONT_END_CTIME);

				bCONFT_END = true;
				strCLUB_A = cCLUB.m_szName;
				if ( pConftClub )	strCLUB_B = pConftClub->m_szName;

				//	Note : 클럽 대련이 종료됨을 알림 to (필드)
				//
				GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld;
				NetMsgFld.dwCLUB_A = cCLUB.m_dwID;
				NetMsgFld.dwCLUB_B = cCLUB.m_dwconftCLUB;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				cAgentServer.SENDTOALLCHANNEL(&NetMsgFld);

				{
					CLUBCONFT_ITER conft_pos = cCLUB.m_setConftCURRENT.begin();
					CLUBCONFT_ITER conft_end = cCLUB.m_setConftCURRENT.end();
					for ( ; conft_pos!=conft_end; ++conft_pos )
					{
						PGLCHARAG pMEMBER = cAgentServer.GetCharID ( *conft_pos );
						if ( pMEMBER )	pMEMBER->ResetConfrontOk ();
					}
				}

				if ( pConftClub )
				{
					CLUBCONFT_ITER conft_pos = pConftClub->m_setConftCURRENT.begin();
					CLUBCONFT_ITER conft_end = pConftClub->m_setConftCURRENT.end();
					for ( ; conft_pos!=conft_end; ++conft_pos )
					{
						PGLCHARAG pMEMBER = cAgentServer.GetCharID ( *conft_pos );
						if ( pMEMBER )	pMEMBER->ResetConfrontOk ();
					}
				}

				cCLUB.CLEARCONFT();
				if ( pConftClub )		pConftClub->CLEARCONFT();
			}

			if ( bCONFT_END )
			{
				std::string strCUR_MAP;
				CString strTEXT;

				PGLCHARAG pCHAR_MASTER = cAgentServer.GetCharID ( cCLUB.m_dwMasterID );
				if ( pCHAR_MASTER ) strCUR_MAP = cAgentServer.GetMapName ( pCHAR_MASTER->m_sCurMapID );

				if ( emEND_B==EMCONFRONT_END_CWIN )
				{
					strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_END_CWIN"),
						strCUR_MAP.c_str(), strCLUB_B.c_str(), strCLUB_A.c_str(), strCLUB_B.c_str() );
				}
				else
				{
					strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_END_CDRAWN"),
						strCUR_MAP.c_str(), strCLUB_A.c_str(), strCLUB_B.c_str() );
				}

				GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
				NetMsg.SETTEXT ( strTEXT.GetString() );
				cAgentServer.SENDTOALLCLIENT ( &NetMsg );
			}
		}
	
		if ( GLCONST_CHAR::bCLUB_BATTLE )
		{
			// Note : 클럽 배틀 점검.
			if ( cCLUB.GetBattleNum() > 0 )
			{
				CLUB_BATTLE_ITER del;
				CLUB_BATTLE_ITER pos = cCLUB.m_mapBattle.begin();
				CLUB_BATTLE_ITER end = cCLUB.m_mapBattle.end();
				for ( ; pos!=end; )
				{
					GLCLUBBATTLE &sClubBattle = (*pos).second;
					del = pos++;

					__time64_t curTime = CTime::GetCurrentTime().GetTime();

					//	배틀 종료				
					if ( sClubBattle.m_tEndTime < curTime )
					{
						DWORD dwClubID = CLUB_NULL;
						
						if ( sClubBattle.m_bAlliance ) 
						{
							dwClubID = cCLUB.m_dwAlliance;

							DelAllianceBattle( dwClubID, sClubBattle.m_dwCLUBID );
                        }
						else
						{
							cCLUB.DELBATTLECLUB( (*del).first );
							GLCLUB* pCLUB = GetClub( sClubBattle.m_dwCLUBID );
							if ( pCLUB ) pCLUB->DELBATTLECLUB( cCLUB.m_dwID );
							dwClubID = cCLUB.m_dwID;
						}
						
						GLCLUBBATTLE_DEL sClubBattleDel;
						sClubBattleDel.m_sClubBattle = sClubBattle;

						cCLUB.m_mapBattleDel[sClubBattle.m_dwCLUBID] = sClubBattleDel;

						GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD NetMsg;
						NetMsg.dwClubID_A = dwClubID;
						NetMsg.dwClubID_B = sClubBattle.m_dwCLUBID;

						cAgentServer.SENDTOALLCHANNEL ( &NetMsg );
					}				
				}
			}

			// 종료 예정인 클럽 배틀 확인
			if ( cCLUB.GetBattleDelNum() > 0 )
			{
				CLUB_BATTLE_DEL_ITER del;
				CLUB_BATTLE_DEL_ITER pos = cCLUB.m_mapBattleDel.begin();
				CLUB_BATTLE_DEL_ITER end = cCLUB.m_mapBattleDel.end();
				
				for ( ; pos!=end; )
				{
					GLCLUBBATTLE_DEL &sClubBattleDel = (*pos).second;
					del = pos++;
					
					//	배틀 종료				
					if ( sClubBattleDel.m_bIsComplated )
					{
						if ( sClubBattleDel.m_sClubBattle.m_bAlliance )
						{
							EndAllianceBattle( cCLUB.m_dwID, sClubBattleDel.m_sClubBattle.m_dwCLUBID );
						}
						else
						{							
							EndClubBattle( cCLUB.m_dwID, sClubBattleDel.m_sClubBattle.m_dwCLUBID );
						}
						cCLUB.m_mapBattleDel.erase(del);
					}

				}
			}
			

			cCLUB.m_fTIMER_BATTLE += fElapsedTime;
			// 클럽 배틀 관련 Timer 
			if ( cCLUB.m_fTIMER_BATTLE > 300.0f )
			{
				//	0 ~ 10.0f 사이의 값으로 초기화, 클럽마다 갱신 속도를 조절한다.
				cCLUB.m_fTIMER_BATTLE = seqrandom::getpercent() / 10.0f;

						// Note : 클럽 배틀 점검.
				if ( cCLUB.GetBattleNum() > 0  )
				{
					CLUB_BATTLE_ITER pos = cCLUB.m_mapBattle.begin();
					CLUB_BATTLE_ITER end = cCLUB.m_mapBattle.end();
					for ( ; pos!=end; pos++ )
					{
						GLCLUBBATTLE &sClubBattle = (*pos).second;

						// Kill수 변화	
						if ( sClubBattle.m_bKillUpdate )
						{
							GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE NetMsg;

							NetMsg.dwBattleClubID = sClubBattle.m_dwCLUBID;
							NetMsg.wKillPoint = sClubBattle.m_wKillPoint;
							NetMsg.wDeathPoint = sClubBattle.m_wDeathPoint;						
							
							cAgentServer.SENDTOCLUBCLIENT ( cCLUB.m_dwID, &NetMsg );

							GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD	NetMsgFld;

							NetMsgFld.dwClubID = cCLUB.m_dwID;
							NetMsgFld.dwBattleClubID = sClubBattle.m_dwCLUBID;
							NetMsgFld.wKillPoint = sClubBattle.m_wKillPoint;
							NetMsgFld.wDeathPoint = sClubBattle.m_wDeathPoint;

							cAgentServer.SENDTOALLCHANNEL( &NetMsgFld );

							sClubBattle.m_bKillUpdate = false;


						}

					}
				}
			}


			// 시작 예정중인 클럽 배틀
			{
				CLUB_BATTLE_ITER del;
				CLUB_BATTLE_ITER pos = cCLUB.m_mapBattleReady.begin();
				CLUB_BATTLE_ITER end = cCLUB.m_mapBattleReady.end();
				CTime cCurTime( CTime::GetCurrentTime() );

				for ( ; pos!=end; )
				{
					GLCLUBBATTLE& sClubBattle = (*pos).second;
					del = pos++;

					if ( cCurTime > sClubBattle.m_tStartTime )
					{
						// 클럽 배틀 시작
						cAgentServer.StartClubBattle( cCLUB.m_dwID, sClubBattle );
						cCLUB.m_mapBattleReady.erase( del );
					}
				}
			}
		}

		//	Note : 클럽 해체 점검.
		//
		if ( !cCLUB.CheckDissolution() )	continue;

		//	Note 대련도중이면 대련 취소.
		//
		if ( cCLUB.ISCONFTING() )
		{
			GLCLUB *pConftClub = GetClub(cCLUB.m_dwconftCLUB);

			emEND_A = (EMCONFRONT_END_FAIL);
			emEND_B = (EMCONFRONT_END_FAIL);

			//	Note : 클럽 대련이 종료됨을 알림 to (필드)
			//
			GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld;
			NetMsgFld.dwCLUB_A = cCLUB.m_dwID;
			NetMsgFld.dwCLUB_B = cCLUB.m_dwconftCLUB;
			NetMsgFld.emEND_A = emEND_A;
			NetMsgFld.emEND_B = emEND_B;
			cAgentServer.SENDTOALLCHANNEL(&NetMsgFld);

			cCLUB.CLEARCONFT();
			if ( pConftClub )		pConftClub->CLEARCONFT();

			std::string strCUR_MAP;
			CString strTEXT;

			PGLCHARAG pCHAR_MASTER = cAgentServer.GetCharID ( cCLUB.m_dwMasterID );
			if ( pCHAR_MASTER ) strCUR_MAP = cAgentServer.GetMapName ( pCHAR_MASTER->m_sCurMapID );

			strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_END_CDRAWN"),
				strCUR_MAP.c_str(), strCLUB_A.c_str(), strCLUB_B.c_str() );

			GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
			NetMsg.SETTEXT ( strTEXT.GetString() );
			cAgentServer.SENDTOALLCLIENT ( &NetMsg );
		}

		PGLCHARAG pCHAR = cAgentServer.GetCharID ( cCLUB.m_dwMasterID );
		if ( pCHAR )
		{
			//	Note : 클럽 정보 제거.
			pCHAR->m_dwGuild = CLUB_NULL;
		}
		
		{
			CLUBMEMBERS_ITER pos = cCLUB.m_mapMembers.begin();
			CLUBMEMBERS_ITER end = cCLUB.m_mapMembers.end();
			for ( ; pos!=end; ++pos )
			{
				const GLCLUBMEMBER &cMEMBER = (*pos).second;
				PGLCHARAG pMember = cAgentServer.GetCharID ( cMEMBER.dwID );
				if ( !pMember )	continue;

				//	Note : 클럽 정보 제거.
				pMember->m_dwGuild = CLUB_NULL;
			}
		}

		//	Note : 클럽 삭제 로그 기록.
		GLITEMLMT::GetInstance().ReqAction
		(
			cCLUB.m_dwMasterID,			//	당사자.
			EMLOGACT_CLUB_DELETE,		//	행위.
			ID_CLUB, cCLUB.m_dwID,		//	상대방.
			0,							//	exp
			0,							//	bright
			0,							//	life
			0							//	money
		);

		//	Note : 클럽 삭제 메시지.
		//
		GLMSG::SNET_CLUB_DEL_2FLD NetMsgFld;
		NetMsgFld.dwClubID = cCLUB.m_dwID;
		cAgentServer.SENDTOALLCHANNEL ( &NetMsgFld );

		//	Note : DB에서 제거.
		//
		CDeleteClub *pDbAction = new CDeleteClub(cCLUB.m_dwID,cCLUB.m_dwMasterID);
		cAgentServer.GetDBMan()->AddJob ( pDbAction );

		//	Note : 클럽 정보 제거.
		//
		DelClub ( (*del).first );
	}

	return S_OK;
}

HRESULT GLClubMan::FrameMoveField( float fTime, float fElapsedTime )
{
	//	클럽 유지 관리.
	GLMSG::SNET_CLUB_MEMBER_POS NetMsgPos;

		//	클럽 유지 관리.
	GLGaeaServer& cGaeaServer = GLGaeaServer::GetInstance();

	CLUBS_ITER pos = m_mapCLUB.begin();
	CLUBS_ITER end = m_mapCLUB.end();
	for ( ; pos!=end; ++pos )
	{
		GLCLUB &cCLUB = (*pos).second;

		//	수익 갱신 타이머.
		cCLUB.m_fTIMER_MONEY += fElapsedTime;

		//	Note : 클럽의 수익을 에이젼트 서버에 반영.
		if ( cCLUB.m_fTIMER_MONEY > 1800 || cCLUB.m_fTIMER_MONEY < 0 )
		{
			cCLUB.m_fTIMER_MONEY = 0.0f;

			if ( cCLUB.m_lnIncomeMoney > 0 )
			{
				GLMSG::SNET_CLUB_INCOME_MONEY_AGT	NetMsgAgt;
				NetMsgAgt.dwID = cCLUB.m_dwID;
				NetMsgAgt.lnMoney = cCLUB.m_lnIncomeMoney;
				cGaeaServer.SENDTOAGENT ( &NetMsgAgt );

				CDebugSet::ToFile ( "club_income_gaea.txt", "{FrameMove}, ClubID[%u], IncomeMoney[%I64d], Time[%f]",
									cCLUB.m_dwID, cCLUB.m_lnIncomeMoney, cCLUB.m_fTIMER_MONEY );

				cCLUB.m_lnIncomeMoney = 0;
			}
		}

		cCLUB.m_fTIMER_POS += fElapsedTime;
		if ( cCLUB.m_fTIMER_POS > 10.0f )
		{
			//	0 ~ 5.0f 사이의 값으로 초기화, 클럽마다 갱신 속도를 조절한다.
			cCLUB.m_fTIMER_POS = seqrandom::getpercent() / 20.0f;
			NetMsgPos.RESET();

			CLUBMEMBERS_ITER mem_pos = cCLUB.m_mapMembers.begin ();
			CLUBMEMBERS_ITER mem_end = cCLUB.m_mapMembers.end ();
			for ( ; mem_pos!=mem_end; ++mem_pos )
			{
				PGLCHAR pCHAR = cGaeaServer.GetCharID ( (*mem_pos).first );
				if ( pCHAR )
				{
					bool bOK = NetMsgPos.ADD ( pCHAR->m_dwCharID, D3DXVECTOR2(pCHAR->m_vPos.x,pCHAR->m_vPos.z) );
					if ( !bOK )
					{
						cGaeaServer.SENDTOCLUBCLIENT ( cCLUB.m_dwID, &NetMsgPos );

						NetMsgPos.RESET();
						NetMsgPos.ADD ( pCHAR->m_dwCharID, D3DXVECTOR2(pCHAR->m_vPos.x,pCHAR->m_vPos.z) );
					}
				}
			}

			if ( NetMsgPos.dwNumber > 0 )
			{
				cGaeaServer.SENDTOCLUBCLIENT ( cCLUB.m_dwID, &NetMsgPos );
			}
		}

		if ( GLCONST_CHAR::bCLUB_BATTLE )
		{

			cCLUB.m_fTIMER_BATTLE += fElapsedTime;
			if ( cCLUB.m_fTIMER_BATTLE > 300.0f )
			{
				//	0 ~ 10.0f 사이의 값으로 초기화, 클럽마다 갱신 속도를 조절한다.
				cCLUB.m_fTIMER_BATTLE = seqrandom::getpercent() / 20.0f;

						// Note : 클럽 배틀 점검.
				if ( cCLUB.GetBattleNum() > 0 )
				{
					CLUB_BATTLE_ITER pos_B = cCLUB.m_mapBattle.begin();
					CLUB_BATTLE_ITER end_B = cCLUB.m_mapBattle.end();
					for ( ; pos_B!=end_B; pos_B++ )
					{
						GLCLUBBATTLE &sClubBattle = (*pos_B).second;

						// Kill수 변화	
						if ( sClubBattle.m_bKillUpdate )
						{
							GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_AGT NetMsgAgt;
							NetMsgAgt.dwClubID = cCLUB.m_dwID;
							NetMsgAgt.dwBattleClubID = sClubBattle.m_dwCLUBID;
							NetMsgAgt.wKillPoint = sClubBattle.m_wKillPointTemp;
							NetMsgAgt.wDeathPoint = sClubBattle.m_wDeathPointTemp;

							sClubBattle.m_wKillPointDB += sClubBattle.m_wKillPointTemp;
							sClubBattle.m_wDeathPointDB += sClubBattle.m_wDeathPointTemp;

							sClubBattle.m_wKillPointTemp = 0;
							sClubBattle.m_wDeathPointTemp = 0;
							sClubBattle.m_bKillUpdate = false;

							cGaeaServer.SENDTOAGENT( &NetMsgAgt );
						}

					}
				}
			}
		}
	}

	return S_OK;
}

HRESULT GLClubMan::EndClubBattle( DWORD dwClubID_P, DWORD dwClubID_S )
{
	//  수정 요망
	//	클럽 유지 관리.
	GLAgentServer& cAgentServer = GLAgentServer::GetInstance();

	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsg;					
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltA;
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltB;

	CString strText;
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

	GLCLUB* pClub_P = GetClub( dwClubID_P );
	if ( !pClub_P ) return E_FAIL;

	GLCLUB* pClub_S = GetClub( dwClubID_S );
	if ( !pClub_S ) return E_FAIL;

	GLCLUBBATTLE_DEL* pClubBattleDel = pClub_P->GetClubBattleDel( dwClubID_S );	
	if ( !pClubBattleDel ) return E_FAIL;
	GLCLUBBATTLE& sClubBattle = pClubBattleDel->m_sClubBattle;
	
	if ( sClubBattle.m_wKillPoint == sClubBattle.m_wDeathPoint )
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_DRAW;
		NetMsg.dwWinClubID = pClub_P->m_dwID;
		NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_DRAW;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_DRAW;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
		strText.Format( ID2SERVERTEXT( "CLUB_BATTLE_OVER_DRAW" ), pClub_P->m_szName, sClubBattle.m_szClubName );

		pClub_P->m_dwBattleDraw++;
		pClub_S->m_dwBattleDraw++;
	}
	else if ( sClubBattle.m_wKillPoint > sClubBattle.m_wDeathPoint ) 
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsg.dwWinClubID = pClub_P->m_dwID;
		NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_LOSE;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
		strText.Format( ID2SERVERTEXT( "CLUB_BATTLE_OVER_WIN" ), pClub_P->m_szName, sClubBattle.m_szClubName );

		pClub_P->m_dwBattleWin++;
		pClub_S->m_dwBattleLose++;
	}
	else
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsg.dwWinClubID = sClubBattle.m_dwCLUBID;
		NetMsg.dwLoseClubID = pClub_P->m_dwID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_LOSE;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_WIN;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;
		strText.Format( ID2SERVERTEXT( "CLUB_BATTLE_OVER_WIN" ), sClubBattle.m_szClubName, pClub_P->m_szName );

		pClub_P->m_dwBattleLose++;
		pClub_S->m_dwBattleWin++;
	}

	NetMsgCltA.dwClubID = sClubBattle.m_dwCLUBID;
	NetMsgCltA.wKillPoint = sClubBattle.m_wKillPoint;
	NetMsgCltA.wDeathPoint = sClubBattle.m_wDeathPoint;
	
	NetMsgCltB.dwClubID = pClub_P->m_dwID;
	NetMsgCltB.wKillPoint = sClubBattle.m_wDeathPoint;
	NetMsgCltB.wDeathPoint = sClubBattle.m_wKillPoint;

	cAgentServer.SENDTOALLCHANNEL ( &NetMsg );
	
	cAgentServer.SENDTOCLUBCLIENT ( pClub_P->m_dwID, &NetMsgCltA );
	cAgentServer.SENDTOCLUBCLIENT ( sClubBattle.m_dwCLUBID, &NetMsgCltB );

	GLMSG::SNET_SERVER_GENERALCHAT NetMsgChat;
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );

	strText.Format ( ID2SERVERTEXT( "CLUB_BATTLE_RESULT" ), pClub_P->m_szName, 
					   pClub_P->m_dwBattleWin, pClub_P->m_dwBattleLose, pClub_P->m_dwBattleDraw );
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );

	
	strText.Format ( ID2SERVERTEXT( "CLUB_BATTLE_RESULT" ), pClub_S->m_szName, 
					   pClub_S->m_dwBattleWin, pClub_S->m_dwBattleLose, pClub_S->m_dwBattleDraw );
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );


	
	//	db에 저장 ( A기준, B기준 저장 )
	CEndClubBattle *pDbActionA = new CEndClubBattle ( pClub_P->m_dwID, sClubBattle.m_dwCLUBID, EnumA, 
		sClubBattle.m_wKillPoint, sClubBattle.m_wDeathPoint, sClubBattle.m_bAlliance );
	CEndClubBattle *pDbActionB = new CEndClubBattle ( sClubBattle.m_dwCLUBID, pClub_P->m_dwID, EnumB, 
		sClubBattle.m_wDeathPoint, sClubBattle.m_wKillPoint, sClubBattle.m_bAlliance );
	
	GLDBMan* pDbMan = GLAgentServer::GetInstance().GetDBMan();
	if ( pDbMan )
	{
		pDbMan->AddJob ( pDbActionA );
		pDbMan->AddJob ( pDbActionB );
	}

	return S_OK;
}

HRESULT GLClubMan::EndAllianceBattle ( DWORD dwClubID_P, DWORD dwClubID_S )
{
	GLAgentServer& cAgentServer = GLAgentServer::GetInstance();

	GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsg;					
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltA;
	GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltB;

	CString strText;
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

	GLCLUB* pClub_P = GetClub( dwClubID_P );
	if ( !pClub_P ) return E_FAIL;

	GLCLUB* pClub_S = GetClub( dwClubID_S );
	if ( !pClub_S ) return E_FAIL;

	GLCLUBBATTLE_DEL* pClubBattleDel = pClub_P->GetClubBattleDel( dwClubID_S );	
	if ( !pClubBattleDel ) return E_FAIL;
	GLCLUBBATTLE& sClubBattle = pClubBattleDel->m_sClubBattle;
	
	if ( sClubBattle.m_wKillPoint == sClubBattle.m_wDeathPoint )
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_DRAW;
		NetMsg.dwWinClubID = pClub_P->m_dwID;
		NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_DRAW;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_DRAW;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
		strText.Format( ID2SERVERTEXT( "ALLIANCE_BATTLE_OVER_DRAW" ), pClub_P->m_szName, sClubBattle.m_szClubName );

		pClub_P->m_dwAllianceBattleDraw++;
		pClub_S->m_dwAllianceBattleDraw++;
	}
	else if ( sClubBattle.m_wKillPoint > sClubBattle.m_wDeathPoint ) 
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsg.dwWinClubID = pClub_P->m_dwID;
		NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_LOSE;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
		strText.Format( ID2SERVERTEXT( "ALLIANCE_BATTLE_OVER_WIN" ), pClub_P->m_szName, sClubBattle.m_szClubName );

		pClub_P->m_dwAllianceBattleWin++;
		pClub_S->m_dwAllianceBattleLose++;
	}
	else
	{
		NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
		NetMsg.dwWinClubID = sClubBattle.m_dwCLUBID;
		NetMsg.dwLoseClubID = pClub_P->m_dwID;

		NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_LOSE;
		NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_WIN;
		EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
		EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;
		strText.Format( ID2SERVERTEXT( "ALLIANCE_BATTLE_OVER_WIN" ), sClubBattle.m_szClubName, pClub_P->m_szName );

		pClub_P->m_dwAllianceBattleLose++;
		pClub_S->m_dwAllianceBattleWin++;
	}


	//	전적 공유
	CLUB_ALLIANCE_ITER pos = pClub_P->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pClub_P->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{

		const GLCLUBALLIANCE &sALLIANCE = *pos;
		GLCLUB *pCLUB = GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB ) continue;

		pCLUB->m_dwAllianceBattleWin = pClub_P->m_dwAllianceBattleWin; 
		pCLUB->m_dwAllianceBattleLose = pClub_P->m_dwAllianceBattleLose; 
		pCLUB->m_dwAllianceBattleDraw = pClub_P->m_dwAllianceBattleDraw; 
	}

	pos = pClub_S->m_setAlliance.begin();
	end = pClub_S->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{

		const GLCLUBALLIANCE &sALLIANCE = *pos;
		GLCLUB *pCLUB = GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB ) continue;

		pCLUB->m_dwAllianceBattleWin = pClub_S->m_dwAllianceBattleWin; 
		pCLUB->m_dwAllianceBattleLose = pClub_S->m_dwAllianceBattleLose; 
		pCLUB->m_dwAllianceBattleDraw = pClub_S->m_dwAllianceBattleDraw; 
	}

	NetMsgCltA.dwClubID = sClubBattle.m_dwCLUBID;
	NetMsgCltA.wKillPoint = sClubBattle.m_wKillPoint;
	NetMsgCltA.wDeathPoint = sClubBattle.m_wDeathPoint;
	
	NetMsgCltB.dwClubID = pClub_P->m_dwID;
	NetMsgCltB.wKillPoint = sClubBattle.m_wDeathPoint;
	NetMsgCltB.wDeathPoint = sClubBattle.m_wKillPoint;

	cAgentServer.SENDTOALLCHANNEL ( &NetMsg );
	
	// 전체 동맹원들에게 모두 알림
	cAgentServer.SENDTOALLIANCECLIENT ( pClub_P->m_dwID, &NetMsgCltA );
	cAgentServer.SENDTOALLIANCECLIENT ( sClubBattle.m_dwCLUBID, &NetMsgCltB );
	

	GLMSG::SNET_SERVER_GENERALCHAT NetMsgChat;
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );

	strText.Format ( ID2SERVERTEXT( "ALLIANCE_BATTLE_RESULT" ), pClub_P->m_szName, 
					   pClub_P->m_dwAllianceBattleWin, pClub_P->m_dwAllianceBattleLose, pClub_P->m_dwAllianceBattleDraw );
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );

	
	strText.Format ( ID2SERVERTEXT( "ALLIANCE_BATTLE_RESULT" ), pClub_S->m_szName, 
					   pClub_S->m_dwAllianceBattleWin, pClub_S->m_dwAllianceBattleLose, pClub_S->m_dwAllianceBattleDraw );
	NetMsgChat.SETTEXT ( strText.GetString() );
	cAgentServer.SENDTOALLCLIENT ( &NetMsgChat );


	
	//	db에 저장 ( A기준, B기준 저장 )
	CEndClubBattle *pDbActionA = new CEndClubBattle ( pClub_P->m_dwID, sClubBattle.m_dwCLUBID, EnumA, 
		sClubBattle.m_wKillPoint, sClubBattle.m_wDeathPoint, sClubBattle.m_bAlliance );
	CEndClubBattle *pDbActionB = new CEndClubBattle ( sClubBattle.m_dwCLUBID, pClub_P->m_dwID, EnumB, 
		sClubBattle.m_wDeathPoint, sClubBattle.m_wKillPoint, sClubBattle.m_bAlliance );
	
	GLDBMan* pDbMan = GLAgentServer::GetInstance().GetDBMan();
	if ( pDbMan )
	{
		pDbMan->AddJob ( pDbActionA );
		pDbMan->AddJob ( pDbActionB );
	}

	return S_OK;

}

HRESULT GLClubMan::EndClubBattleAgent( DWORD dwClubID_P, DWORD dwClubID_S )
{
	// 수정요망
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
	GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

	GLCLUB* pClub_P = GetClub( dwClubID_P );
	if ( !pClub_P ) return E_FAIL;

	GLCLUB* pClub_S = GetClub( dwClubID_S );
	if ( !pClub_S ) return E_FAIL;
	
	GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClubID_S );
	if ( !pClubBattle )	return E_FAIL;

	if ( pClubBattle->m_bAlliance ) 
	{
		if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
		{
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
						
            pClub_P->m_dwAllianceBattleDraw++;
			pClub_S->m_dwAllianceBattleDraw++;

		}
		else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
		{	
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;

			pClub_P->m_dwAllianceBattleWin++;
			pClub_S->m_dwAllianceBattleLose++;
		}
		else
		{
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;

			pClub_P->m_dwAllianceBattleLose++;
			pClub_S->m_dwAllianceBattleWin++;
		}
	}
	else
	{
		if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
		{
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
						
			pClub_P->m_dwBattleDraw++;
			pClub_S->m_dwBattleDraw++;

		}
		else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
		{	
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;

			pClub_P->m_dwBattleWin++;
			pClub_S->m_dwBattleLose++;
		}
		else
		{
			EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
			EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;

			pClub_P->m_dwBattleLose++;
			pClub_S->m_dwBattleWin++;
		}
	}

	//	db에 저장 ( P기준, S기준 저장 )
	CEndClubBattle *pDbActionP = new CEndClubBattle ( pClub_P->m_dwID, pClubBattle->m_dwCLUBID, 
		EnumA, pClubBattle->m_wKillPoint, pClubBattle->m_wDeathPoint, pClubBattle->m_bAlliance );
	CEndClubBattle *pDbActionS = new CEndClubBattle ( pClubBattle->m_dwCLUBID, pClub_P->m_dwID,
		EnumB, pClubBattle->m_wDeathPoint, pClubBattle->m_wKillPoint, pClubBattle->m_bAlliance );
	
	GLDBMan* pDbMan = GLAgentServer::GetInstance().GetDBMan();
	if ( pDbMan )
	{
		pDbMan->AddJob ( pDbActionP );
		pDbMan->AddJob ( pDbActionS );
	}

	return S_OK;
}

// 배틀 정보만 삭제
HRESULT GLClubMan::EndClubBattleField( DWORD dwClubID_P, DWORD dwClubID_S )
{
	GLCLUB* pClub_P = GetClub( dwClubID_P );
	if ( !pClub_P ) return E_FAIL;

	GLCLUB* pClub_S = GetClub( dwClubID_S );
	if ( !pClub_S ) return E_FAIL;

	GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClubID_S );
	if ( !pClubBattle )	return E_FAIL;
	
	if ( pClubBattle->m_bAlliance )
	{
		if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
		{
			pClub_P->m_dwAllianceBattleDraw++;
			pClub_S->m_dwAllianceBattleDraw++;
		}
		else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
		{
			pClub_P->m_dwAllianceBattleWin++;
			pClub_S->m_dwAllianceBattleLose++;
		}
		else
		{
			pClub_P->m_dwAllianceBattleLose++;
			pClub_S->m_dwAllianceBattleWin++;
		}	
	}
	else
	{
		if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
		{
			pClub_P->m_dwBattleDraw++;
			pClub_S->m_dwBattleDraw++;
		}
		else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
		{
			pClub_P->m_dwBattleWin++;
			pClub_S->m_dwBattleLose++;
		}
		else
		{
			pClub_P->m_dwBattleLose++;
			pClub_S->m_dwBattleWin++;
		}	
	}

	return S_OK;
}

void GLClubMan::DelAllianceBattle( DWORD dwClub_P, DWORD dwClub_S )
{
	GLCLUB* pCLUB_P = GetClub( dwClub_P );
	GLCLUB* pCLUB_S = GetClub( dwClub_S );

	if ( !pCLUB_P || !pCLUB_S ) return;

	CLUB_ALLIANCE_ITER pos = pCLUB_P->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_P->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = *pos;

		GLCLUB *pCLUB = GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB ) continue;
		pCLUB->DELBATTLECLUB( pCLUB_S->m_dwID );
	}

	pos = pCLUB_S->m_setAlliance.begin();
	end = pCLUB_S->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE = *pos;

		GLCLUB *pCLUB = GetClub ( sALLIANCE.m_dwID );
		if ( !pCLUB ) continue;
		pCLUB->DELBATTLECLUB( pCLUB_P->m_dwID );
	}
}

void GLClubMan::InitAllianceBattle ()
{
	CLUBS_ITER pos = m_mapCLUB.begin();
	CLUBS_ITER end = m_mapCLUB.end();
	
	for ( ; pos!=end; ++pos )
	{
		GLCLUB &cCLUB = (*pos).second;

		if ( !cCLUB.IsChief() ) continue;

		CLUB_BATTLE_ITER pos_B = cCLUB.m_mapBattle.begin();
		CLUB_BATTLE_ITER end_B = cCLUB.m_mapBattle.end();
		for ( ; pos_B!=end_B; ++pos_B)
		{
			GLCLUBBATTLE &sClubBattle = (*pos_B).second;
			
			if ( !sClubBattle.m_bAlliance ) continue;
			
			CLUB_ALLIANCE_ITER pos_A = cCLUB.m_setAlliance.begin();
			CLUB_ALLIANCE_ITER end_A = cCLUB.m_setAlliance.end();
			for ( ; pos_A!=end_A; ++pos_A )
			{
				const GLCLUBALLIANCE &sALLIANCE = *pos_A;
				GLCLUB *pCLUB = GetClub ( sALLIANCE.m_dwID );
				
				if ( !pCLUB ) continue;
				if ( cCLUB.m_dwID == pCLUB->m_dwID ) continue;

				pCLUB->ADDBATTLECLUB( sClubBattle );
			}
		}
	}
}