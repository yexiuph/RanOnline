#include "pch.h"
#include "./DbActionLogic.h"

#include "./GLChar.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* Shop 에서 구입한 아이템을 가져온다.
* 가져온 아이템은 실제로 가져 갈 수 있는 아이템이 아니다.
* 아이템을 가져갈때는 실제로 가져갈 수 있는지 다시 확인해야 한다.
* 웹을 통한 구매와 취소 및 게임내 캐릭터의 동기화 문제.
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
 * 실제로 가져갈 수 있는 상품인지 확인한다.
 * 입력값
 * nFlag (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
 * 출력값
 * 1 : 가져갈수 있음
 * 이외의값 : 가져갈수 없음
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
*	구입한 아이템을 인벤에 넣기에 실패했을 경우 flag 리셋.
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
* 구입한 아이템을 인벤으로 가져옵니다.
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
			//	Note : 인벤에 아이템 넣기를 요청한다.
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
