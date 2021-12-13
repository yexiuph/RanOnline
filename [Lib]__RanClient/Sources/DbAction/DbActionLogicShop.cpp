#include "pch.h"
#include "./DbActionLogic.h"

#include "./GLChar.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* Shop ���� ������ �������� �����´�.
* ������ �������� ������ ���� �� �� �ִ� �������� �ƴϴ�.
* �������� ���������� ������ ������ �� �ִ��� �ٽ� Ȯ���ؾ� �Ѵ�.
* ���� ���� ���ſ� ��� �� ���ӳ� ĳ������ ����ȭ ����.
*/
CGetPurchaseItem::CGetPurchaseItem(
	CString strUID,
	DWORD dwCharID )
{
    m_strUID = strUID;
	m_dwCharID = dwCharID;
}

int CGetPurchaseItem::Execute(
	CServer* pServer )
{
    m_pDbManager->GetPurchaseItem( m_strUID, m_vItem );

	PGLCHAR pChar = GLGaeaServer::GetInstance().GetCharID( m_dwCharID );
	if ( pChar )
	{
		pChar->SETSHOPPURCHASE( m_vItem );
		pChar->SENDBOXITEMINFO();
	}
	
    return NET_OK;
}

/**
 * ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
 * �Է°�
 * nFlag (0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
 * ��°�
 * 1 : �������� ����
 * �̿��ǰ� : �������� ����
 */
CSetPurchaseItem::CSetPurchaseItem(
	CString strPurKey,
	int nFlag )
{
    m_strPurKey = strPurKey;
    m_nFlag = nFlag;
}

int CSetPurchaseItem::Execute(
	CServer* pServer )
{
    return m_pDbManager->SetPurchaseItem( m_strPurKey, m_nFlag );
}

/**
*	������ �������� �κ��� �ֱ⿡ �������� ��� flag ����.
*/
CPurchaseItem_Restore::CPurchaseItem_Restore(
	CString strPurKey )
{
    m_strPurKey = strPurKey;
}

int CPurchaseItem_Restore::Execute(
	CServer* pServer )
{
    return m_pDbManager->SetPurchaseItem( m_strPurKey, 0 );
}

/**
* ������ �������� �κ����� �����ɴϴ�.
*/
CPurchaseItem_Get::CPurchaseItem_Get(
	DWORD dwClient,
	DWORD dwUserID,
	CString strPurKey,
	SNATIVEID nidITEM,
	DWORD dwInvenPos )
	: m_dwClient( dwClient )
	, m_dwUserID( dwUserID )
	, m_dwInvenPos( dwInvenPos )
{	
	m_strPurKey = strPurKey;
	m_nidITEM = nidITEM;	
}

int CPurchaseItem_Get::Execute(
	CServer* pServer )
{
	if (pServer == NULL)
	{
		return DB_ERROR;
	}
	else
	{
		int nRET = m_pDbManager->SetPurchaseItem( m_strPurKey, 1 );
		if ( nRET==1 )
		{
			//	Note : �κ��� ������ �ֱ⸦ ��û�Ѵ�.
			//
			GLMSG::SNET_CHARGED_ITEM2_INVEN NetMsg;
			NetMsg.dwUserID = m_dwUserID;
			NetMsg.nidITEM = m_nidITEM;
			StringCchCopy ( NetMsg.szPurKey, PURKEY_LENGTH+1, m_strPurKey.GetString() );
			NetMsg.dwInvenPos = m_dwInvenPos;

			CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
			pTemp->InsertMsg ( m_dwClient, (char*) &NetMsg );
		}
		return nRET;
	}
}
