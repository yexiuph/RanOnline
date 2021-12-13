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
// �� ��������!! �̺�Ʈ 
// ���ϰ� :
// 1  1 �� P-4 PC
// 2  2 �� ȿ����
// 3  3 �� SONY ��ī
// 4  4 �� ���̸��� MP �÷��̾�
// 5  5 �� MAX ���� ��ȭ ��ǰ�� 1 ������
// 6  6 �� MBC Game ���� ���� ��ǰ��
// 7  �̹� ������ �Է��� �����
// 8  PC �� IP �� �ƴ�
// 9  �̹� ����� ����
// 10 �˼����� ����
CLogLottery::CLogLottery(CString strLottery, // ���ǹ�ȣ
				         CString strUserUID, // ����� UID
				         int nUserNum,       // ����� ��ȣ
				         DWORD dwClient,     // Ŭ���̾�Ʈ ��ȣ
				         const char* szUserIP, // ����� IP
				         USHORT uPort)         // ��Ʈ
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
    // ���������� �̺�Ʈ ��ó��
    pAgentServer->MsgLogLotteryBack(nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// ������ ���� �̺�Ʈ ����� �����.
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
// �������� PC �� ���θ�� ������ '����' �̺�Ʈ
// PC �濡�� �Ϸ翡 �ѻ���� �ѹ��� ����
// ���ϰ�
// 1 : �̺�Ʈ ���𰡴�
// 2 : �̹� �̺�Ʈ�� ��������, �Ϸ翡 �ѹ��� ����.
// 3 : ����Ұ� PC �� IP �ƴ�
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

    // ������ �����̺�Ʈ ó��...
    pAgentServer->MsgLogPungPungCheckBack(nResult, m_dwClient, m_strUserIP.GetString(), m_uPort);
    return NET_OK;
}


