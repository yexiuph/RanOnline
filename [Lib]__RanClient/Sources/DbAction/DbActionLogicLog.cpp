#include "pch.h"
#include "./DbActionLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
* �б�vs�б� ��Ƽ��ð������
*/
CLogPartyMatch::CLogPartyMatch(
	int nSGNum,
	int nSvrNum,
	WORD wWin,
	WORD wLost )
	: m_nSGNum(nSGNum)
	, m_nSvrNum(nSvrNum)
	, m_wWin(wWin)
	, m_wLost(wLost)
{
}

int CLogPartyMatch::Execute(
	CServer* pServer )
{
	m_pDbManager->LogPartyMatch( m_nSGNum, m_nSvrNum, m_wWin, m_wLost );
	return NET_OK;
}

/*
* ������ �ŷ� �α�
*/
CLogItemExchange::CLogItemExchange(
	const SLOGITEMEXCHANGE &sEXCHANGE )
{
	m_sEXCHANGE = sEXCHANGE;
}

int CLogItemExchange::Execute(
	CServer* pServer )
{
    m_pDbManager->LogItemExchange( m_sEXCHANGE );
	return NET_OK;
}

/*
* ���ӸӴ� �ŷ� �α�
*/
CLogMoneyExchange::CLogMoneyExchange(
	const SLOGMONEYEXCHANGE &_sLOG )
{
	m_sLOG = _sLOG;
}

int CLogMoneyExchange::Execute(
	CServer* pServer )
{
	m_pDbManager->LogMoneyExchange( m_sLOG );
	return NET_OK;
}

/*
* ������ ��þƮ �α�
*/
CLogItemConversion::CLogItemConversion(
	const SLOGITEMCONVERSION &_sLOG )
{
	m_sLOG = _sLOG;
}

int CLogItemConversion::Execute(
	CServer* pServer )
{
	m_pDbManager->LogItemConversion( m_sLOG );
	return NET_OK;
}

/**
* �������� �����ɼ� �α׸� �����.
* \param _sLOG �����ɼ� ����ü
*/
CLogRandomItem::CLogRandomItem(
	const SLOGRANDOMOPTION& sLOG )
{
	m_sLOG = sLOG;
}

int CLogRandomItem::Execute(
	CServer* pServer )
{
	m_pDbManager->LogRandomItem( m_sLOG );
	return NET_OK;
}

/**
* ���� �׼� �α׸� �����.
* \param _sLOG ��׼Ƿα� ����ü
*/
CLogPetAction::CLogPetAction( const SLOGPETACTION& sLOG )
{
	m_sLOG = sLOG;
}

int CLogPetAction::Execute( CServer* pServer )
{
	m_pDbManager->LogPetAction( m_sLOG );
	return NET_OK;
}


/**
* Ż���� �׼� �α׸� �����.
* \param _sLOG Ż�;׼Ƿα� ����ü
*/
CLogVehicleAction::CLogVehicleAction( const SLOGVEHICLEACTION& sLOG )
{
	m_sLOG = sLOG;
}

int CLogVehicleAction::Execute( CServer* pServer )
{
	m_pDbManager->LogVehicleAction( m_sLOG );
	return NET_OK;
}

/**
* ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.
* \param nSGNum �����׷�
* \param ChaNum �Ű��� ĳ���͹�ȣ
* \param strMsg �Ű��� (�ִ�ũ��� CHAT_MSG_SIZE)
*/
CLogAppeal::CLogAppeal(
	int nSGNum,
	int nChaNum,
	CString strMsg )
	: m_nSGNum( nSGNum )
	, m_nChaNum( nChaNum )
{    
    m_strMsg  = strMsg;
}

int CLogAppeal::Execute(
	CServer* pServer )
{
    return m_pDbManager->LogAppeal( m_nSGNum, m_nChaNum, m_strMsg );
}

/**
* �ҹ����α׷� ��볻���� ����Ѵ�.
* \param nUserNum ������ȣ
* \param nChaNum ĳ���͹�ȣ
* \param nHackProgramNum ��ŷ���α׷���ȣ
*/
CLogHackProgram::CLogHackProgram(
	int nUserNum,
	int nChaNum,
	int nHackProgramNum,
	TCHAR* szINFO )
	: m_nUserNum( nUserNum )
	, m_nChaNum( nChaNum )
	, m_nHackProgramNum( nHackProgramNum )
{
	StringCchCopy( m_szINFO, CLogHackProgram::MAX_INFO, szINFO );
}

int CLogHackProgram::Execute(
	CServer* pServer )
{
	return m_pDbManager->LogHackProgram(
							pServer->GetServerGroup(),
		                    pServer->GetServerNum(),
							m_nUserNum,
							m_nChaNum,
							m_nHackProgramNum,
							m_szINFO );
}

/**
* ĳ���� Ȱ�� �α׸� ����Ѵ�.
*/
CLogAction::CLogAction(
	const SLOGACTION &_sLOG )
{
	m_sLOG = _sLOG;
}

int CLogAction::Execute(
	CServer* pServer )
{
	return m_pDbManager->LogAction( m_sLOG );
}