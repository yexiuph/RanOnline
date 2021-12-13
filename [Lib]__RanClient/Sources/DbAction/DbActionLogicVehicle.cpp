#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CSessionServer.h"
#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"
#include "../[Lib]__NetServer/Sources/s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 새로운 탈것을 생성한다.
* \param nChaNum  탈것 소유자 캐릭터 번호
* \param szVehicleName 탈것 이름
* \param nVehicleType 탈것의 타입
* \param nVehicleMID 탈것 MID
* \param nVehicleSID 탈것 SID
* \param nVehicleCardMID 탈것의 아이템 MID
* \param nVehicleCardSID 탈것의 아이템 SID
* \return DB_ERROR 또는 성공했을때는 탈것의 고유번호
*/
CCreateVehicle::CCreateVehicle(
	DWORD dwClientID,
	DWORD dwCharNum,
	const TCHAR* szVehicleName,
	int nVehicleType,
	int nVehicleCardMID,
	int nVehicleCardSID
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleType(nVehicleType)
	, m_nVehicleCardMID(nVehicleCardMID)
	, m_nVehicleCardSID(nVehicleCardSID)
{
	m_dwClient = dwClientID;
	m_strVehicleName = szVehicleName;
	m_strVehicleName.Trim(_T(" ")); // 앞뒤 공백제거
}

int CCreateVehicle::Execute(CServer* pServer)
{
	int nResult = m_pDbManager->CreateVehicle( m_dwCharNum,
		m_strVehicleName.GetString(),
		m_nVehicleType,
		m_nVehicleCardMID, 
		m_nVehicleCardSID
		);	
	
	GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB InternalMsg;
	
	// 성공했을때만 내부 메시지 발송
	if ( nResult != DB_ERROR )
	{
		InternalMsg.emTYPE = (VEHICLE_TYPE)m_nVehicleType;
		InternalMsg.sVehicleID.wMainID = m_nVehicleCardMID;
		InternalMsg.sVehicleID.wSubID = m_nVehicleCardSID;
		InternalMsg.dwVehicleID = nResult;
		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	else
	{

		// 실패하면 보드카드의 팻번호를 다시 0으로 되돌려 놓는다. (재생성 가능하게 하기 위해)
		PGLCHAR pOwner = GLGaeaServer::GetInstance().GetCharID ( m_dwCharNum );
		if ( pOwner )
		{
			SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
			if ( sItemCustom.sNativeID != NATIVEID_NULL() ) sItemCustom.dwVehicleID = 0;

			pOwner->m_bGetVehicleFromDB = false;
		}
	}

	return nResult;
}


/**
* 탈것의 정보를 가져온다.
*/
CGetVehicle::CGetVehicle(
	GLVEHICLE* pVehicle,
	DWORD dwVehicleNum,
	DWORD dwClientID,
	DWORD dwCharNum,
	bool  bLMTItemCheck,
	bool  bCardInfo,
	bool  bTrade
	)
	: m_pVehicle( pVehicle )
	, m_dwVehicleNum ( dwVehicleNum )
	, m_dwCharNum ( dwCharNum )
	, m_bLMTItemCheck ( bLMTItemCheck )
	, m_bCardInfo ( bCardInfo )
	, m_bTrade ( bTrade )

{
	m_dwClient = dwClientID;
}

int CGetVehicle::Execute(CServer* pServer)
{
	int nResult = m_pDbManager->GetVehicle( m_pVehicle,
		m_dwVehicleNum,
		m_dwCharNum );

	if ( nResult == DB_OK )
	{
		GLMSG::SNET_VEHICLE_GET_FROMDB_FB InternalMsg;
		InternalMsg.pVehicle = m_pVehicle;
		InternalMsg.dwVehicleNum = m_dwVehicleNum;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo = m_bCardInfo;
		InternalMsg.bTrade = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	else if ( nResult == DB_ROWCNT_ERROR )
	{

		GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR InternalMsg;		

		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo	  = m_bCardInfo;
		InternalMsg.bTrade		  = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );

		SAFE_DELETE ( m_pVehicle );		
	}
	else
	{
		PGLCHAR pOwner = GLGaeaServer::GetInstance().GetCharID ( m_dwCharNum );
		if ( pOwner )
		{
			pOwner->m_bGetVehicleFromDB = false;
		}
		
		SAFE_DELETE ( m_pVehicle );
	}

	

	return nResult;
}

/**
* 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
* \param nVehicleBttery 배터리를 설정할 탈것의 번호
* \param nVehicleBttery 배터리
*/
CSetVehicleBattery::CSetVehicleBattery(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,
	int nVehicleBttery
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
	, m_nVehicleBttery( nVehicleBttery )
{
	m_dwClient = dwClientID;
}


int CSetVehicleBattery::Execute(CServer* pServer)
{
	return m_pDbManager->SetVehicleBattery( m_nVehicleNum, m_dwCharNum, m_nVehicleBttery );
}


CGetVehicleBattery::CGetVehicleBattery( 
	DWORD dwClientID, 
	DWORD dwCharNum,
	int	  nVehicleNum, 
	SNATIVEID	sItemID,
	WORD  wCureVolume, 
	BOOL  bRatio, 
	SNATIVEID sBatteryID,
	VEHICLE_TYPE emType )
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )	
	, m_sItemID ( sItemID )
	, m_wCureVolume( wCureVolume )
	, m_bRatio ( bRatio )
	, m_sBatteryID ( sBatteryID ) 
	, m_emType( emType )
{
	m_dwClient = dwClientID;
}


int CGetVehicleBattery::Execute(CServer* pServer)
{
	
	int nResult = m_pDbManager->GetVehicleBattery( m_nVehicleNum, m_dwCharNum );

	GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB InternalMsg;

	if ( nResult != DB_ERROR )
	{
		InternalMsg.dwVehicleID	 = m_nVehicleNum;
		InternalMsg.sItemID		 = m_sItemID;	
		InternalMsg.nFull		 = nResult;
		InternalMsg.wCureVolume  = m_wCureVolume;
		InternalMsg.bRatio		 = m_bRatio;
		InternalMsg.sBatteryID   = m_sBatteryID;
		InternalMsg.emFB		 = EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK;
		InternalMsg.emType		 = m_emType;
	}
	else
	{
		InternalMsg.emFB		 = EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL;
	}

	CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
	pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );

	
	return 0;
}




/**
* 탈것을 삭제한다.
* \param nVehicle 삭제할 탈것의 번호
*/
CDeleteVehicle::CDeleteVehicle(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
{
	m_dwClient = dwClientID;
}

int CDeleteVehicle::Execute(CServer* pServer)
{
	return m_pDbManager->DeleteVehicle( m_nVehicleNum, m_dwCharNum );
}



/**
* 탈것의 인벤토리 업데이트
*/
CSetVehicleInven::CSetVehicleInven(		
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,	
	GLVEHICLE* pVehicle
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum(nVehicleNum)
{	
	m_dwClient = dwClientID;
	pVehicle->GETPUTONITEMS_BYBUF( m_ByteStream );
}

int CSetVehicleInven::Execute(CServer* pServer)
{	
	return m_pDbManager->SetVehicleInven(
		m_nVehicleNum,
		m_dwCharNum,
		m_ByteStream);
}