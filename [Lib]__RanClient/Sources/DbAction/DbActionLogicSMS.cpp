#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* SMS 문자메시지를 발송한다.
* \param nSendChaNum SMS 를 발송하는 캐릭터 번호
* \param szSendUserID SMS 를 발송하는 사용자 ID
* \param nReceiveChaNum SMS 를 받을 캐릭터 번호
* \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
* \param szSmsMsg SMS 메시지 내용
*/
CSendSMS::CSendSMS(
	DWORD dwClientNum,
	DWORD dwSendChaNum,
	DWORD dwReceiveChaNum,
	TCHAR* szReceivePhone,
	TCHAR* szSendUserID,
	TCHAR* szSmsMsg,
	WORD wPosX,
	WORD wPosY )	
{
	if (dwSendChaNum > 0 &&
		dwReceiveChaNum > 0 &&
		szReceivePhone != NULL &&
		szSendUserID != NULL &&
		szSmsMsg != NULL )
	{
		m_dwClient = dwClientNum;
		m_dwSendChaNum = dwSendChaNum;
		m_dwReceiveChaNum = dwReceiveChaNum;
		StringCchCopy( m_szReceivePhone, SMS_RECEIVER, szReceivePhone );
		StringCchPrintf(
			m_szSmsMsg,
			SMS_LENGTH,
			_T("RAN/%s/%s"),
			szSendUserID,
			szSmsMsg );
		m_wPosX = wPosX;
		m_wPosY = wPosY;

		m_bSMS = true;
	}
	else
	{
		m_bSMS = false;
	}
}

int CSendSMS::Execute( CServer* pServer )
{
	if (m_bSMS == true)
	{
		int nRESULT = m_pDbManager->LogSMS(	m_dwSendChaNum,
											m_dwReceiveChaNum,
											m_szReceivePhone,
											m_szSmsMsg );

		GLMSG::SNETPC_SEND_SMS_FROM_DB NetMsgDB2Fld;

		if( nRESULT == DB_OK ) // 이름변경 성공
		{
			NetMsgDB2Fld.emFB = EMSMS_SEND_FROM_DB_OK;
			NetMsgDB2Fld.wPosX = m_wPosX;
			NetMsgDB2Fld.wPosY = m_wPosY;
		}
		else // 이름변경 실패
		{
			NetMsgDB2Fld.emFB = EMSMS_SEND_FROM_DB_FAIL;
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_dwClient, (char*) &NetMsgDB2Fld );

		return nRESULT;
	}
	else
	{
		return DB_ERROR;
	}
}

/*
CSendSMS* pAction1 = new CSendSMS(
							111,
                            112,
							_T("0175391624"),
							_T("jgkim999"),
							_T("SMS Test Message") );

COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pAction1 );
*/

/**
* 캐릭터의 휴대폰 번호를 저장한다.
* \param nChaNum 캐릭터 번호
* \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
*/
CSetChaPhoneNumber::CSetChaPhoneNumber(
	DWORD dwClientNum,
	DWORD dwChaNum,
	const TCHAR* szPhoneNumber )
{
	if (dwChaNum > 0 &&
		m_szPhoneNumber != NULL)
	{
		m_dwClient = dwClientNum;
		m_dwChaNum = dwChaNum;
		StringCchCopy( m_szPhoneNumber, SMS_RECEIVER, szPhoneNumber );
		m_bSMS = true;
	}
	else
	{
		m_bSMS = false;
	}
}

int CSetChaPhoneNumber::Execute( CServer* pServer )
{
	if (m_bSMS == true)
	{
		int nRESULT = m_pDbManager->SetChaPhoneNumber( m_dwChaNum, m_szPhoneNumber );

		GLMSG::SNETPC_PHONE_NUMBER_FROM_DB NetMsgDB2Fld;

		if( nRESULT == DB_OK ) // 이름변경 성공
		{
			NetMsgDB2Fld.emFB = EMSMS_PHONE_NUMBER_FROM_DB_OK;
			StringCchCopy( NetMsgDB2Fld.szPhoneNumber, SMS_RECEIVER, m_szPhoneNumber );
		}
		else // 이름변경 실패
		{
			NetMsgDB2Fld.emFB = EMSMS_PHONE_NUMBER_FROM_DB_FAIL;
			StringCchCopy ( NetMsgDB2Fld.szPhoneNumber, SMS_RECEIVER, m_szPhoneNumber );
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_dwClient, (char*) &NetMsgDB2Fld );

		return nRESULT;
	}
	else
	{
		return DB_ERROR;
	}
}