#include "pch.h"

#include "s_CDbAction.h"
#include "s_CSessionServer.h"
#include "s_CFieldServer.h"
#include "s_CAgentServer.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
** Databse Action for Log Database
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// 란 선물펑펑!! 이벤트 
// 리턴값 :
// 1  1 등 P-4 PC
// 2  2 등 효리폰
// 3  3 등 SONY 디카
// 4  4 등 아이리버 MP 플레이어
// 5  5 등 MAX 무비 영화 상품권 1 만원권
// 6  6 등 MBC Game 일일 무료 상품권
// 7  이미 복권을 입력한 사용자
// 8  PC 방 IP 가 아님
// 9  이미 사용한 복권
// 10 알수없는 오류
CLogLottery::CLogLottery(CString strLottery, // 복권번호
				         CString strUserUID, // 사용자 UID
				         int nUserNum,       // 사용자 번호
				         DWORD dwClient,     // 클라이언트 번호
				         const char* szUserIP, // 사용자 IP
				         USHORT uPort)         // 포트
{
	m_strLottery = strLottery;
	m_strUserIP  = szUserIP;
	m_strUserUID = strUserUID;
	m_nUserNum   = nUserNum;
	m_dwClient   = dwClient;
	m_uPort      = uPort;
}
	
int CLogLottery::Execute(CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	int nResult;
	nResult = COdbcManager::GetInstance()->LogLottery(m_strLottery, m_strUserIP, m_strUserUID, m_nUserNum);
	
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    // 아이템팡팡 이벤트 뒷처리
    pAgentServer->MsgLogLotteryBack(nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// 아이템 펑펑 이벤트 기록을 남긴다.
CLogPungPungWrite::CLogPungPungWrite(CString strUserUID, int nUserNum, int nPrize, CString strIP)
{
    m_strUserIP  = strIP;
    m_strUserUID = strUserUID ;
    m_nUserNum   = nUserNum;
    m_nPrize     = nPrize;
}

int CLogPungPungWrite::Execute(CServer* pServer)
{
    COdbcManager::GetInstance()->LogPungPungWrite(m_strUserIP, m_strUserUID, m_nUserNum, m_nPrize);
    return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 여름방학 PC 방 프로모션 아이템 '펑펑' 이벤트
// PC 방에서 하루에 한사람이 한번만 가능
// 리턴값
// 1 : 이벤트 응모가능
// 2 : 이미 이벤트에 응모했음, 하루에 한번만 가능.
// 3 : 응모불가 PC 방 IP 아님
CLogPungPungCheck::CLogPungPungCheck(int nUserNum, DWORD dwClient, CString strIP, USHORT uPort)
{
    m_nUserNum  = nUserNum;
    m_dwClient  = dwClient;
    m_strUserIP = strIP;
    m_uPort     = uPort;       
}

int CLogPungPungCheck::Execute(CServer* pServer)
{    
    if (pServer == NULL) return NET_ERROR;

    int nResult = 0;
    nResult = COdbcManager::GetInstance()->LogPungPungCheck(m_strUserIP, m_nUserNum);

    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);

    // 아이템 팡팡이벤트 처리...
    pAgentServer->MsgLogPungPungCheckBack(nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
    return NET_OK;
}


