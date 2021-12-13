#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* SMS ���ڸ޽����� �߼��Ѵ�.
* \param nSendChaNum SMS �� �߼��ϴ� ĳ���� ��ȣ
* \param szSendUserID SMS �� �߼��ϴ� ����� ID
* \param nReceiveChaNum SMS �� ���� ĳ���� ��ȣ
* \param szReceivePhone SMS �� ���� ĳ������ ��ȭ��ȣ
* \param szSmsMsg SMS �޽��� ����
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

		if( nRESULT == DB_OK ) // �̸����� ����
		{
			NetMsgDB2Fld.emFB = EMSMS_SEND_FROM_DB_OK;
			NetMsgDB2Fld.wPosX = m_wPosX;
			NetMsgDB2Fld.wPosY = m_wPosY;
		}
		else // �̸����� ����
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
* ĳ������ �޴��� ��ȣ�� �����Ѵ�.
* \param nChaNum ĳ���� ��ȣ
* \param szPhoneNumber ��ȭ��ȣ NULL ���� ���� 14�� SMS_RECEIVER
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

		if( nRESULT == DB_OK ) // �̸����� ����
		{
			NetMsgDB2Fld.emFB = EMSMS_PHONE_NUMBER_FROM_DB_OK;
			StringCchCopy( NetMsgDB2Fld.szPhoneNumber, SMS_RECEIVER, m_szPhoneNumber );
		}
		else // �̸����� ����
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