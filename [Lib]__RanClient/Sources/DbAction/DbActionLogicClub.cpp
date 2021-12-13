#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CAgentServer.h"
#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 새로운 클럽을 생성한다.
* \param strClubName : 클럽이름
* \param dwChaNum : 클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)
* \return -1 : 이미 길드마스터로 등록되어 있음 생성불가.
*         -2 : 길드생성중 에러발생 (중복된길드이름)
*         0 보다 클때 : 생성된 길드번호임
*/
CCreateClub::CCreateClub(
	DWORD dwClientID,
	CString strClubName,
	DWORD dwChaNum )
{
	m_dwClient = dwClientID;
    m_strClubName = strClubName;
    m_dwChaNum = dwChaNum;
}

int CCreateClub::Execute(
	CServer* pServer )
{
    int nRETCODE = m_pDbManager->CreateClub(m_strClubName, m_dwChaNum);    
	
	GLMSG::SNET_CLUB_NEW_DB2AGT NetMsgDb2Agt;
	NetMsgDb2Agt.dwMasterID = m_dwChaNum;
	NetMsgDb2Agt.dwClubID = 0;
	StringCchCopy ( NetMsgDb2Agt.szClubName, CHAR_SZNAME, m_strClubName.GetString() );

	switch ( nRETCODE )
	{
	case -1:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_ALREADY;
		break;

	case -2:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_NAME;
		break;

	case CLUB_NULL:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_FAIL;
		break;

	default:
		NetMsgDb2Agt.emFB = EMCLUB_NEW_FB_OK;
		NetMsgDb2Agt.dwClubID = nRETCODE;
		break;
	};

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->InsertMsg ( m_dwClient, (char*) &NetMsgDb2Agt );

	return nRETCODE;
}

/**
* 클럽을 삭제한다.
* \param dwClub : 클럽번호
* \param dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
* \return 0 : 길드 삭제 실패
*         1 : 길드 삭제 성공
*/
CDeleteClub::CDeleteClub(
	DWORD dwClub, 
	DWORD dwChaNum)
	: m_dwClub( dwClub )
	, m_dwChaNum( dwChaNum )
{
}

int CDeleteClub::Execute(
	CServer* pServer )
{
    return m_pDbManager->DeleteClub(m_dwClub, m_dwChaNum);
}

/**
* 클럽랭크를 세팅한다.
* \param dwClub : 클럽번호
* \param dwRank : 랭킹
*/
CSetClubRank::CSetClubRank(
	DWORD dwClub,
	DWORD dwRank )
	: m_dwClub( dwClub )
	, m_dwRank( dwRank )
{   
}
 
int CSetClubRank::Execute(CServer* pServer)
{
    return m_pDbManager->SetClubRank(m_dwClub, m_dwRank);
}

/**
* 클럽마크 이미지를 설정한다.
* \param dwClub : 클럽번호
* \param dwMarkVer : 마크버전
* \param pData : 데이터 포인터
* \param nSize : 데이터 size
*/
CWriteClubMarkImage::CWriteClubMarkImage(
	DWORD dwClub,
	DWORD dwMarkVer,
	BYTE* pData,
	int nSize) 
	: m_nSize(0)
	, m_dwMarkVer(0)
	, m_dwClub(0)
{
	if (pData != NULL)
	{
		int nMaxSize = (int) (sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

		m_dwClub = dwClub;
		m_dwMarkVer = dwMarkVer;
	    
		if ( nSize <= 0 || nSize > nMaxSize )
		{
			m_nSize = 0;
		}
		else
		{
			m_nSize = nSize;
			memcpy ( m_aryMark, pData, nSize );
		}
	}
}

CWriteClubMarkImage::~CWriteClubMarkImage()
{    
}

int CWriteClubMarkImage::Execute(CServer* pServer)
{
    if (m_nSize > 0)
        return m_pDbManager->WriteClubMarkImage(m_dwClub, m_dwMarkVer, m_aryMark, m_nSize);
    else
        return DB_ERROR;
}

/**
* 클럽 해체시간을 설정한다.
* dwClub : 클럽번호
* tDiss : 해체시간
*/
CSetClubDissolutionTime::CSetClubDissolutionTime(
	DWORD dwClub,
	__time64_t tDiss )
	: m_dwClub( dwClub )
{    
    m_tDiss = tDiss;
}

int CSetClubDissolutionTime::Execute(
	CServer* pServer )
{
    return m_pDbManager->SetClubDissolutionTime(m_dwClub, m_tDiss);
}

/**
* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
* \param dwClub 클럽번호
* \param tSec 시간
* \return 
*/
CSetClubAllianceSec::CSetClubAllianceSec(
	DWORD dwClub,
	__time64_t tSec )
	: m_dwClub( dwClub )
{
	m_tSec   = tSec;
}

int CSetClubAllianceSec::Execute(
	CServer* pServer )
{
	return m_pDbManager->SetClubAllianceSec( m_dwClub, m_tSec );
}

/**
* 클럽의 동맹해산 시간을 정한다.
* \param dwClub 클럽번호
* \param tDis 해산시간
* \return 
*/
CSetClubAllianceDis::CSetClubAllianceDis(
	DWORD dwClub,
	__time64_t tDis )
	: m_dwClub( dwClub )
{
	m_tDis   = tDis;
}
    
int CSetClubAllianceDis::Execute(
	CServer* pServer )
{
	return m_pDbManager->SetClubAllianceDis( m_dwClub, m_tDis );
}


/**
* 클럽의 마스터 위임 시간을 설정한다.
* \param dwClub 클럽번호
* \param tDis 마스터 위임 시간
* \return 
*/
CSetClubAuthorityTime::CSetClubAuthorityTime(
	DWORD dwClub,
	__time64_t tAuthority )
	: m_dwClub( dwClub )
{
	m_tAuthority   = tAuthority;
}
    
int CSetClubAuthorityTime::Execute(
	CServer* pServer )
{
	return m_pDbManager->SetClubAuthorityTime( m_dwClub, m_tAuthority );
}



/**
* 새로운 캐릭터를 클럽에 가입시킨다
* \param dwClub : 클럽번호
* \param dwChaNum : 가입시키려는 캐릭터번호
*/
CAddClubMember::CAddClubMember(
	DWORD dwClub,
	DWORD dwChaNum )
	: m_dwClub( dwClub )
	, m_dwChaNum( dwChaNum )
{    
}

int CAddClubMember::Execute(CServer* pServer)
{
    return m_pDbManager->AddClubMember(m_dwClub, m_dwChaNum);
}

/**
* 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
* \param dwChaNum : 캐릭터번호
*/
CDeleteClubMember::CDeleteClubMember(
	DWORD dwChaNum )
	: m_dwChaNum( dwChaNum )
{
}

int CDeleteClubMember::Execute(
	CServer* pServer )
{
    return m_pDbManager->DeleteClubMember(m_dwChaNum);
}

/**
* 해당클럽의 보유금액을 세팅한다. (Storage)
* \param dwClub : 클럽번호
* \param llMoney : 보유금액 (금액은 >= 0)
*/
CSetClubMoney::CSetClubMoney(
	DWORD dwClub,
	LONGLONG llMoney )
	: m_dwClub( dwClub )
	, m_llMoney( llMoney )
{	
}

int CSetClubMoney::Execute(
	CServer* pServer )
{
	return m_pDbManager->SetClubMoney( m_dwClub, m_llMoney );
}

/**
* 해당클럽의 수입금액을 세팅한다. (Income money)
* \param dwClub : 클럽번호
* \param llMoney : 수입금액 (금액은 >= 0)
*/
CSetClubIncomeMoney::CSetClubIncomeMoney(
	DWORD dwClub,
	LONGLONG llMoney )
	: m_dwClub( dwClub )
	, m_llMoney( llMoney )
{	
}

int CSetClubIncomeMoney::Execute(
	CServer* pServer )
{
	return m_pDbManager->SetClubIncomeMoney( m_dwClub, m_llMoney );
}

/**
* 해당클럽의 클럽창고를 저장한다.
* \param dwClub : 클럽번호
* \param pData : 창고위치
* \param nSize : 버퍼 전체크기
*/
CWriteClubStorage::CWriteClubStorage(
	DWORD dwClub,
	const BYTE* pData,
	int nSize )
	: m_dwClub(0)
	, m_pData(NULL)
	, m_nSize(0)
{	
	if ((pData != NULL) && (nSize > 0) && (dwClub > 0))
	{
		m_dwClub = dwClub;
		m_nSize  = nSize;
		m_pData  = new BYTE[nSize];
		memcpy(m_pData, pData, nSize);
	}	
}

CWriteClubStorage::~CWriteClubStorage()
{
	SAFE_DELETE_ARRAY(m_pData);
}
	
int CWriteClubStorage::Execute(CServer* pServer)
{
	if ( (m_pData!=NULL) && (m_nSize>0) )
		return m_pDbManager->WriteClubStorage(m_dwClub, m_pData, m_nSize);
	else
		return DB_ERROR;
}

CGetClubStorage::CGetClubStorage(
	DWORD _dwClientID,
	DWORD _dwCharID,
	DWORD _dwClubID )
	: m_dwClientID( _dwClientID )
	, m_dwCharID( _dwCharID )
	, m_dwClubID( _dwClubID )
{
}

int CGetClubStorage::Execute(
	CServer* pServer )
{
	int nRetCode;

	LONGLONG lnMoney = m_pDbManager->GetClubMoney ( m_dwClubID );
	if ( lnMoney < 0 )
	{
		//CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:Club Storage Money DB Read Fail.");
		return NET_OK;
	}

	CByteStream *pSTREAM = new CByteStream;
	nRetCode = m_pDbManager->ReadClubStorage ( m_dwClubID, *pSTREAM );
	if (nRetCode == DB_ERROR)
	{
		//CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:Club Storate Stream DB Read Fail.");

		SAFE_DELETE(pSTREAM);
		return NET_OK;
	}

	GLMSG::SNET_CLUB_STORAGE_GET_DB NetMsg;
	NetMsg.m_dwCharID = m_dwCharID;
	NetMsg.m_dwClubID = m_dwClubID;
	NetMsg.m_lnStorageMoney = lnMoney;
	NetMsg.m_pStream = pSTREAM;
	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
	pFieldServer->InsertMsg ( m_dwClientID, (char*) &NetMsg );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////
// 부 클럽장을 세팅한다
// dwClub : 클럽번호
// dwChaNum : 캐릭터번호
// dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
CSetClubMasterFlags::CSetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags)
{
	m_dwClub           = dwClub;
	m_dwChaNum         = dwChaNum;
	m_dwSubMasterFlags = dwSubMasterFlags;
}

int CSetClubMasterFlags::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubMasterFlags(m_dwClub, m_dwChaNum, m_dwSubMasterFlags);
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
CSetClubAlliance::CSetClubAlliance(DWORD dwClubP, DWORD dwClubS)
{
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
}

int CSetClubAlliance::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubAlliance(m_dwClubP, m_dwClubS);
}

///////////////////////////////////////////////////////////////////////////
// 동맹 클럽을 해체한다.
// dwClubP : 주 클럽번호 (동맹 클럽의 주)
// dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
CDelClubAlliance::CDelClubAlliance(DWORD dwClubP, DWORD dwClubS)
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
}

int CDelClubAlliance::Execute(CServer* pServer)
{
	return m_pDbManager->DelClubAlliance(m_dwClubP, m_dwClubS);
}


///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 시작한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
CSetClubBattle::CSetClubBattle(DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, bool bAlliance )
	: m_dwClubP ( dwClubP )
	, m_dwClubS ( dwClubS )
	, m_dwEndTime ( dwEndTime )
	, m_bAlliance ( bAlliance )
{	
}

int CSetClubBattle::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubBattle(m_dwClubP, m_dwClubS, m_dwEndTime, (int)m_bAlliance );
}

///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 종료한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
// nFlag :	종료 Flag
// nGuKillNum : 킬수
// nGuDeathNum : 데스수
CEndClubBattle::CEndClubBattle(
	DWORD dwClubP, 
	DWORD dwClubS, 
	int nFlag, 
	int nGuKillNum, 
	int nGuDeathNum, 
	bool bAlliance )
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
	m_nFlag = nFlag;
	m_nGuKillNum = nGuKillNum;
	m_nGuDeathNum = nGuDeathNum;
	m_bAlliance = bAlliance;
}

int CEndClubBattle::Execute(CServer* pServer)
{
	return m_pDbManager->EndClubBattle(m_dwClubP, m_dwClubS, m_nFlag, m_nGuKillNum, 
									m_nGuDeathNum, m_bAlliance);
}


///////////////////////////////////////////////////////////////////////////
// 클럽배틀을 종료한다.
// dwClubP : 주 클럽번호 (자기클럽)
// dwClubS : 보조 클럽번호 (상대클럽)
// nFlag :	종료 Flag
// nGuKillNum : 킬수
// nGuDeathNum : 데스수
CSaveClubBattle::CSaveClubBattle(
	DWORD dwClubP, 
	DWORD dwClubS, 
	int nGuKillNum, 
	int nGuDeathNum )
{	
	m_dwClubP = dwClubP;
	m_dwClubS = dwClubS;
	m_nGuKillNum = nGuKillNum;
	m_nGuDeathNum = nGuDeathNum;
}

int CSaveClubBattle::Execute(CServer* pServer)
{
	return m_pDbManager->SaveClubBattle(m_dwClubP, m_dwClubS, m_nGuKillNum, m_nGuDeathNum);
}

CReSetAllianceBattle:: CReSetAllianceBattle( DWORD dwClub )
	: m_dwClub ( dwClub )
{
}

int CReSetAllianceBattle::Execute(CServer* pServer)
{
	return m_pDbManager->ReSetAllianceBattle( m_dwClub );
}


///////////////////////////////////////////////////////////////////////////
// 클럽의 공지사항을 세팅한다.
CSetClubNotice::CSetClubNotice(DWORD dwClub, const char* szClubNotice)
{
	memset(m_szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
	
	m_dwClub = dwClub;

	CString strTemp;
	strTemp = szClubNotice;
    strTemp.Replace(_T("'"), _T("''"));	
	StringCchCopy(m_szNotice, EMCLUB_NOTICE_LEN+1, strTemp.GetString());
}

int CSetClubNotice::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubNotice(m_dwClub, (const char*) m_szNotice);
}

///////////////////////////////////////////////////////////////////////////
// 클럽의 선도클럽 인증 대리자를 세팅한다.
CSetClubDeputy::CSetClubDeputy(DWORD dwClub, DWORD dwDeputy)
{
	m_dwClub   = dwClub;
	m_dwDeputy = dwDeputy;
}

int CSetClubDeputy::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubDeputy(m_dwClub, m_dwDeputy);
}

CSetClubAuthority::CSetClubAuthority(DWORD dwClub, DWORD dwMasterID)
{
	m_dwClub   = dwClub;
	m_dwMasterID = dwMasterID;
}

int CSetClubAuthority::Execute(CServer* pServer)
{
	return m_pDbManager->SetClubAuthority(m_dwClub, m_dwMasterID);
}
