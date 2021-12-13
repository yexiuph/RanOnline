#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"
#include "./GLItemLMT.h"
#include "./GLItemMan.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "RanFilter.h"
#include "./GLItemMixMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : 인벤아이템을 소모시킴.
//		2. 소모를 클라이언트에 알림.
//		3. b2AGENT 변수는 클라이언트에 알릴때 필드와의 연결성이 불확실 할 경우를 대비하여
//			에이젼트를 통해서 알릴수 있도록 함.
bool GLChar::DoDrugInvenItem ( WORD wPosX, WORD wPosY, bool b2AGENT )
{
	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pINVENITEM )		return false;

	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem )			return false;

	if ( pItem->sDrugOp.bInstance )
	{
		if ( pINVENITEM->sItemCustom.wTurnNum > 0 )		pINVENITEM->sItemCustom.wTurnNum--;
	
		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pINVENITEM->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 1 );

		if ( pINVENITEM->sItemCustom.wTurnNum==0 )
		{
			//	아이탬 제거.
			m_cInventory.DeleteItem ( wPosX, wPosY );

			//	[자신에게] 인밴에 아이탬 제거.
			GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
			NetMsg_Inven_Delete.wPosX = wPosX;
			NetMsg_Inven_Delete.wPosY = wPosY;
			if ( b2AGENT )	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg_Inven_Delete);
			else			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;

			if ( b2AGENT )	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&NetMsg_Inven_Update);
			else			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Update);
		}
	}


	// 쿨타임 적용
	if ( pItem->sBasicOp.IsCoolTime() ) 
	{
		SetCoolTime( pINVENITEM->sItemCustom.sNativeID , pItem->sBasicOp.emCoolType );
	}

	return true;
}

bool GLChar::DoDrugSlotItem ( EMSLOT emSLOT,  WORD wNum )
{
	if ( emSLOT >= SLOT_TSIZE )		return false;

	SITEMCUSTOM& sSLOTITEM = m_PutOnItems[emSLOT];
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sSLOTITEM.sNativeID );
	if ( !pItem )					return false;

	// 소모성 아이템일 경우
	if ( pItem->ISINSTANCE() )
	{
		if ( sSLOTITEM.wTurnNum >= wNum )		sSLOTITEM.wTurnNum -= wNum;

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( sSLOTITEM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sSLOTITEM.wTurnNum );

		if ( sSLOTITEM.wTurnNum==0 )
		{
			//	HOLD 아이탬 제거.
			RELEASE_SLOT_ITEM ( emSLOT );

			//	[자신에게] 슬롯에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release(emSLOT);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Release);
		}
		else
		{
			//	[자신에게] 연마제의 연마가능 횟수 감소.
			GLMSG::SNETPC_PUTON_DRUG_UPDATE NetMsg_Update;
			NetMsg_Update.emSlot = emSLOT;
			NetMsg_Update.wTurnNum = sSLOTITEM.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Update);
		}
	}
	else
	{
		//	기한 아이템일 경우
		GLITEMLMT::GetInstance().ReqItemRoute ( sSLOTITEM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 0 );
	}

	// 쿨타임 적용
	if ( pItem->sBasicOp.IsCoolTime() ) 
	{
		SetCoolTime( sSLOTITEM.sNativeID , pItem->sBasicOp.emCoolType );
	}

	return true;
}

bool GLChar::IsInsertToInven ( PITEMDROP pItemDrop, bool bVietnamInven )
{
	GASSERT(pItemDrop&&"GLChar::IsItemToInven()");
	if ( !pItemDrop )	return false;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItem )		return false;

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		BOOL bITEM_SPACE = TRUE;
		if( bVietnamInven )
			bITEM_SPACE = m_cVietnamInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY );
		else
			bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY );

		if ( !bITEM_SPACE )		return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = TRUE;
		if( bVietnamInven )
			bOk = m_cVietnamInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		else
			bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );

		if ( !bOk )				return false;
	}

	return true;
}

/* 비활성화 인벤토리에도 무언가 넣을수 있다.*/
bool GLChar::IsInsertToInvenEx ( PITEMDROP pItemDrop )
{
	GASSERT(pItemDrop&&"GLChar::IsItemToInven()");
	if ( !pItemDrop )	return false;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItem )		return false;

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		BOOL bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY, true );
		if ( !bITEM_SPACE )		return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY, true );
		if ( !bOk )				return false;
	}

	return true;
}

HRESULT GLChar::InsertToVNGainInven( PITEMDROP pItemDrop )
{

	GASSERT(pItemDrop&&"GLChar::InsertToInven()");
	if ( !pItemDrop )	return E_FAIL;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItem )		return E_FAIL;

 	bool bITEM_SPACE = IsInsertToInven ( pItemDrop, TRUE );
	if ( !bITEM_SPACE )
	{
		//	인밴이 가득차서 아이탬을 넣을수 없습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_INVEN_ERROR;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}


	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );



		//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
		//		아이템 점검 숫자를 감소시킴.
		GLInventory::CELL_MAP &ItemMap = *m_cVietnamInventory.GetItemList();
		GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM &sINVENITEM = *(*iter).second;
			SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.sNativeID != sNID )		continue;
			if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;

			//	기존 아이템 겹침 여유분.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : 기존 아이템의 겹침을 변경. ( 꽉참 )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.bVietnamInven = TRUE;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 잔여 수량 감소.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				//	Note : 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.bVietnamInven = TRUE;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
				//
				GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
				if ( pParty )
				{
					GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
					NetMsg.dwGaeaID = m_dwGaeaID;
					NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
					pParty->SENDMSG2NEAR ( this, &NetMsg );
				}

				//	Note : 아이탬 Land에서 삭제.
				m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

				return S_OK;
			}
		}

		//	Note : 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

		//	최대겹침아이템 수량.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	여분겹침아이템의 겹침수.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX(0), wInsertPosY(0);
			BOOL bSPACE = m_cVietnamInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			GASSERT(bSPACE&&"넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함.");
			if ( !bSPACE )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			//	Note : 새로운 아이템을 넣어줌.
			//
			SITEMCUSTOM sITEMCUSTOM = pItemDrop->sItemCustom;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

			//	인밴토리에 넣습니다.
			sITEMCUSTOM.bVietnamGainItem = true;
			m_cVietnamInventory.InsertItem ( sITEMCUSTOM, wInsertPosX, wInsertPosY );
			SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem ( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )		return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			//	인밴에 아이탬 넣어주는 메시지.
			GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
			NetMsgInven.bVietnamInven = TRUE;
			NetMsgInven.Data = *pInvenItem;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);
		}

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//  나중에 살펴봐야 할곳 
		//QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : PILE 겹침 아이템이므로 로그 작성 불가능.
		//

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = m_cVietnamInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 아이탬 줍기 성공.
		pItemDrop->sItemCustom.bVietnamGainItem = true;
		m_cVietnamInventory.InsertItem ( pItemDrop->sItemCustom, wPosX, wPosY );
		SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem ( wPosX, wPosY );
		if ( !pInvenItem )	return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}



		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.bVietnamInven = TRUE;
		NetMsg_Inven.Data = *pInvenItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//  나중에 살펴봐야 할곳 
		//QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}


	return S_OK;
}

HRESULT GLChar::InsertToInven ( PITEMDROP pItemDrop )
{
	//NeedToSaveValue _C

	GASSERT(pItemDrop&&"GLChar::InsertToInven()");
	if ( !pItemDrop )	return E_FAIL;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItem )		return E_FAIL;

	bool bITEM_SPACE = IsInsertToInven ( pItemDrop );
	if ( !bITEM_SPACE )
	{
		//	인밴이 가득차서 아이탬을 넣을수 없습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_INVEN_ERROR;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pItemDrop->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum );

		//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
		//		아이템 점검 숫자를 감소시킴.
		GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
		GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM &sINVENITEM = *(*iter).second;
			SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.sNativeID != sNID )		continue;
			if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;
		
			//	기존 아이템 겹침 여유분.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : 기존 아이템의 겹침을 변경. ( 꽉참 )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 잔여 수량 감소.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				//	Note : 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
				//
				GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
				if ( pParty )
				{
					GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
					NetMsg.dwGaeaID = m_dwGaeaID;
					NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
					pParty->SENDMSG2NEAR ( this, &NetMsg );
				}

				GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
				NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

				//	Note : 아이탬 Land에서 삭제.
				m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

				return S_OK;
			}
		}

		//	Note : 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

		//	최대겹침아이템 수량.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	여분겹침아이템의 겹침수.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX(0), wInsertPosY(0);
			BOOL bSPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			GASSERT(bSPACE&&"넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함.");
			if ( !bSPACE )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			//	Note : 새로운 아이템을 넣어줌.
			//
			SITEMCUSTOM sITEMCUSTOM = pItemDrop->sItemCustom;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

			//	인밴토리에 넣습니다.
			m_cInventory.InsertItem ( sITEMCUSTOM, wInsertPosX, wInsertPosY );
			SINVENITEM *pInvenItem = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )		return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			//	인밴에 아이탬 넣어주는 메시지.
			GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
			NetMsgInven.Data = *pInvenItem;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);
		}

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}

		GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
		NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : PILE 겹침 아이템이므로 로그 작성 불가능.
		//

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 아이탬 줍기 성공.
		m_cInventory.InsertItem ( pItemDrop->sItemCustom, wPosX, wPosY );
		SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
		if ( !pInvenItem )	return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}

		GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
		NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum );

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pInvenItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}

	return S_OK;
}


HRESULT GLChar::InsertToInvenEx ( PITEMDROP pItemDrop )
{
	GASSERT(pItemDrop&&"GLChar::InsertToInven()");
	if ( !pItemDrop )	return E_FAIL;

	SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItem )		return E_FAIL;

	bool bITEM_SPACE = IsInsertToInvenEx ( pItemDrop );
	if ( !bITEM_SPACE )
	{
		//	인밴이 가득차서 아이탬을 넣을수 없습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_INVEN_ERROR;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return E_FAIL;
	}

	if ( pItem->ISPILE() )
	{
		WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
		WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pItemDrop->sItemCustom.wTurnNum );

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pItemDrop->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum );

		//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
		//		아이템 점검 숫자를 감소시킴.
		GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
		GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
		for ( ; iter!=ItemMap.end(); ++iter )
		{
			SINVENITEM &sINVENITEM = *(*iter).second;
			SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
			if ( sITEMCUSTOM.sNativeID != sNID )		continue;
			if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;
		
			//	기존 아이템 겹침 여유분.
			WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

			if ( wREQINSRTNUM > wSURPLUSNUM )
			{
				//	Note : 기존 아이템의 겹침을 변경. ( 꽉참 )
				sITEMCUSTOM.wTurnNum = wPILENUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 잔여 수량 감소.
				wREQINSRTNUM -= wSURPLUSNUM;
			}
			else
			{
				//	Note : 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
				sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

				//	Note : 클라이언트에 갯수 변경을 알림.
				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
				NetMsg.wPosX = sINVENITEM.wPosX;
				NetMsg.wPosY = sINVENITEM.wPosY;
				NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

				//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
				//
				GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
				if ( pParty )
				{
					GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
					NetMsg.dwGaeaID = m_dwGaeaID;
					NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
					pParty->SENDMSG2NEAR ( this, &NetMsg );
				}

				GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
				NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

				//	Note : 아이탬 Land에서 삭제.
				m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

				return S_OK;
			}
		}

		//	Note : 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

		//	최대겹침아이템 수량.
		WORD wONENUM = wREQINSRTNUM / wPILENUM;
		WORD wITEMNUM = wONENUM;

		//	여분겹침아이템의 겹침수.
		WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
		if ( wSPLITNUM > 0 )				wITEMNUM += 1;
		if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

		for ( WORD i=0; i<wITEMNUM; ++i )
		{
			WORD wInsertPosX(0), wInsertPosY(0);

			// 활성화된 인벤에 우선적 배치...( 기존 검색순서가 세로 우선이라서 변경 하기 애매함. ) 
			BOOL bSPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
			if ( !bSPACE )
			{
				bSPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY, true );
				
				GASSERT(bSPACE&&"넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함.");
				if ( !bSPACE )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
			}

			//	Note : 새로운 아이템을 넣어줌.
			//
			SITEMCUSTOM sITEMCUSTOM = pItemDrop->sItemCustom;
			if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
			else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

			//	인밴토리에 넣습니다.
			m_cInventory.InsertItem ( sITEMCUSTOM, wInsertPosX, wInsertPosY, true );
			SINVENITEM *pInvenItem = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
			if ( !pInvenItem )		return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

			//	인밴에 아이탬 넣어주는 메시지.
			GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
			NetMsgInven.Data = *pInvenItem;
			NetMsgInven.bAllLine = true;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);
		}

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}

		GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
		NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : PILE 겹침 아이템이므로 로그 작성 불가능.
		//

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}
	else
	{
		WORD wPosX = 0, wPosY = 0;

		// 활성화된 인벤에 우선적 배치...( 기존 검색순서가 세로 우선이라서 변경 하기 애매함. ) 
		BOOL bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
				bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY, true );
				
				GASSERT(bOk&&"넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함.");
				if ( !bOk )			return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.
		}

		//	Note : 아이탬 줍기 성공.
		m_cInventory.InsertItem ( pItemDrop->sItemCustom, wPosX, wPosY, true );
		SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
		if ( !pInvenItem )	return E_FAIL;	//	실제로는 점검이 끝낫기 때문에 오류발생 가능성이 없음.

		//	Note : 파티원일 경우 다른 파티원에게 아이템 습득 알림.
		//
		GLPARTY_FIELD *pParty = GLGaeaServer::GetInstance().GetParty ( m_dwPartyID );
		if ( pParty )
		{
			GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
			pParty->SENDMSG2NEAR ( this, &NetMsg );
		}

		GLMSG::SNETPC_PICKUP_ITEM NetMsgPickUp;
		NetMsgPickUp.sNID_ITEM = pItemDrop->sItemCustom.sNativeID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPickUp);

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum );

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pInvenItem;
		NetMsg_Inven.bAllLine = true;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

		//	Note : 아이템 습득시 자동 퀘스트 시작 점검.
		//
		QuestStartFromGetITEM ( pItemDrop->sItemCustom.sNativeID );

		//	Note : 아이탬 Land에서 삭제.
		m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );
	}

	return S_OK;
}

void GLChar::DisableSkillFact()
{
	EMSLOT emRHand = GetCurRHand();	
	SITEM* pRightItem = GET_SLOT_ITEMDATA(emRHand);		

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		// 스킬 자신 버프
		if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_SELF || pSkill->m_sBASIC.emIMPACT_REALM != REALM_SELF ) continue;
				
		GLITEM_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

		// 스킬 도구 종속 없음
		if ( emSKILL_RITEM == ITEMATT_NOCARE )	continue;

		// 스킬 과 무기가 불일치
		if( !pRightItem || !CHECHSKILL_ITEM(emSKILL_RITEM,pRightItem->sSuitOp.emAttack) )
		{
			DISABLESKEFF( i );
		}
	}
}

//************************************
// Method:    InsertMoney
// FullName:  GLChar::InsertMoney
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LONGLONG lnAmount
// Explanation: 현재는 파티에서 금액 분배시 사용되는 함수이다
//************************************
void GLChar::InsertMoney ( LONGLONG lnAmount )
{
	//	돈 습득.

	lnAmount = CalculateInsertmoney( lnAmount );

	CheckMoneyUpdate( m_lnMoney, lnAmount, TRUE, "Insert Party Money." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney += lnAmount;

	//	Note : 금액 로그.
	//
	if ( lnAmount>EMMONEY_LOG )
	{
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CHAR, m_dwCharID, lnAmount, EMITEM_ROUTE_GROUND );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM)// ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "Insert Party Money, [%s][%s], Insert Amount:[%I64d], Have Money:[%I64d]",
				 m_szUID, m_szName,  lnAmount, m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

#if defined(VN_PARAM) //vietnamtest%%%
	if( lnAmount != 0 )
#endif
	{
		//	금액 변화 클라이언트에 알려줌.
		GLMSG::SNETPC_UPDATE_MONEY NetMsg;
		NetMsg.lnMoney = m_lnMoney;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
	}

#if defined(VN_PARAM) //vietnamtest%%%
	GLMSG::SNETPC_VIETNAM_GAIN_MONEY NetMsg2;
	NetMsg2.gainMoney = m_lVNGainSysMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg2 );
#endif
}

LONGLONG GLChar::CalculateInsertmoney( LONGLONG lnAmount )
{
	LONGLONG lnMoney = lnAmount;
	//	돈 습득.	
	lnMoney = lnMoney * (LONGLONG)m_pGLGaeaServer->GetMoneyGainRate(m_CHARINDEX,m_wLevel);

#if defined(TH_PARAM) || defined(MYE_PARAM ) || defined(MY_PARAM)
	if( m_dwThaiCCafeClass == 1 || m_nMyCCafeClass == 1 ) // A등급
	{
		lnMoney *= 2;
	}else if( m_dwThaiCCafeClass == 2 || m_nMyCCafeClass == 2 ) // B등급
	{
		lnMoney *= 1.75f;
	}else if( m_dwThaiCCafeClass == 3 || m_nMyCCafeClass == 3 ) // C등급
	{
		lnMoney *= 1.5f;
	}
#endif

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( m_ChinaGainType == GAINTYPE_HALF )
	{
		lnMoney /= 2;
	}else if( m_ChinaGainType == GAINTYPE_EMPTY )
	{
		lnMoney = 0;
	}
#endif

	//NeedToSaveValue_B
#if defined(VN_PARAM) //vietnamtest%%%
	if( m_dwVietnamGainType == GAINTYPE_HALF )
	{
		lnMoney /= 2;
		m_lVNGainSysMoney += lnMoney;
	}else if( m_dwVietnamGainType == GAINTYPE_EMPTY )
	{
		m_lVNGainSysMoney += lnMoney;
		lnMoney = 0;		
	}
#endif

	return lnMoney;
}

HRESULT GLChar::InsertToInven ( PMONEYDROP pMoneyDrop )
{
	//	돈 습득.	
	if( pMoneyDrop->bDropMonster )
	{
		pMoneyDrop->lnAmount = CalculateInsertmoney( pMoneyDrop->lnAmount );
	}
	

	CheckMoneyUpdate( m_lnMoney, pMoneyDrop->lnAmount, TRUE, "Insert Money." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney += pMoneyDrop->lnAmount;

	//	Note : 금액 로그.
	//
	if ( pMoneyDrop->lnAmount>EMMONEY_LOG )
	{
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CHAR, m_dwCharID, pMoneyDrop->lnAmount, EMITEM_ROUTE_GROUND );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "Insert Money, [%s][%s], Insert Amount:[%I64d], Have Money:[%I64d]",
			m_szUID, m_szName, pMoneyDrop->lnAmount, m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

#if defined(VN_PARAM) //vietnamtest%%%
	if( pMoneyDrop->lnAmount != 0 )
#endif
	{
		GLMSG::SNETPC_PICKUP_MONEY NetMsg;
		NetMsg.lnMoney = m_lnMoney;
		NetMsg.lnPickUp = pMoneyDrop->lnAmount;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
	}

#if defined(VN_PARAM) //vietnamtest%%%
	GLMSG::SNETPC_VIETNAM_GAIN_MONEY NetMsg2;
	NetMsg2.gainMoney = m_lVNGainSysMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg2 );
#endif

	//	돈을 Land에서 삭제.
	m_pLandMan->DropOutMoney ( pMoneyDrop->dwGlobID );

	return S_OK;
}

//	Note : Field의 아이탬,돈을 Inventory 에 넣기 시도.
//
HRESULT GLChar::MsgReqFieldToInven ( NET_MSG_GENERIC* nmg )
{
	if ( !m_pLandMan )	return E_FAIL;

	GLMSG::SNETPC_REQ_FIELD_TO_INVEN *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_FIELD_TO_INVEN*> ( nmg );

	

	if ( pNetMsg->emCrow==CROW_ITEM )
	{
		PITEMDROP pItemDrop = m_pLandMan->GetItem ( pNetMsg->dwID );

		if ( !pItemDrop )
		{
			//	아이탬이 사라졌습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			return E_FAIL;
		}

		// 사망확인
		if ( !IsValidBody() )	return E_FAIL;

		//	거리 체크
		D3DXVECTOR3 vPos;

		if ( pNetMsg->bPet )	
		{
			PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
			if ( pMyPet )	vPos = pMyPet->m_vPos;
		}	
		else vPos = m_vPos;

		const D3DXVECTOR3 &vTarPos = pItemDrop->vPos;

		D3DXVECTOR3 vDistance = vPos - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wTakeAbleDis = wTakeRange + 15;

		if ( fDistance>wTakeAbleDis )
		{
			//	거리가 멉니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
						
			return E_FAIL;
		}

        // 필드에서 주운경우 시효성 체크( 클럽락커 버그 대처 )
		{	
			GLITEMLMT & glItemmt = GLITEMLMT::GetInstance();
			GLItemMan & glItemMan = GLItemMan::GetInstance();
			SITEMCUSTOM sCUSTOM = pItemDrop->sItemCustom;
			const CTime cTIME_CUR = CTime::GetCurrentTime();
			
			SITEM *pITEM = glItemMan.GetItem ( sCUSTOM.sNativeID );
			if ( !pITEM )	return E_FAIL;

			bool bDELETE(false);

			if ( pITEM->IsTIMELMT() )
			{			
				
				CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
				CTime cTIME_LMT(sCUSTOM.tBORNTIME);
				cTIME_LMT += cSPAN;

				if ( cTIME_CUR > cTIME_LMT )
				{
					//	시간 제한으로 아이템 삭제 로그 남김.
					glItemmt.ReqItemRoute ( sCUSTOM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sCUSTOM.wTurnNum );

					//	시간 제한으로 아이템 삭제 알림.
					GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
					NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.sNativeID;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

					bDELETE = true;				

					//	Note : 코스툼 복구.
					if ( sCUSTOM.nidDISGUISE!=SNATIVEID(false) )
					{
						SITEM *pONE = glItemMan.GetItem ( sCUSTOM.nidDISGUISE );
						if ( !pONE )		return E_FAIL;

						SITEMCUSTOM sITEM_NEW;
						sITEM_NEW.sNativeID = sCUSTOM.nidDISGUISE;
						CTime cTIME = CTime::GetCurrentTime();
						if ( sCUSTOM.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
						{
							cTIME = CTime(sCUSTOM.tDISGUISE);

							CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
							cTIME -= tLMT;
						}

						//	Note : 아이템 생성.
						//
						sITEM_NEW.tBORNTIME = cTIME.GetTime();
						sITEM_NEW.wTurnNum = 1;
						sITEM_NEW.cGenType = EMGEN_BILLING;
						sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
						sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
						sITEM_NEW.lnGenNum = glItemmt.RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

						// 생성된 코스툼은 바닥에서 다시 떨군다.
						CItemDrop cDropItem;
						cDropItem.sItemCustom = sITEM_NEW;
						m_pLandMan->DropItem ( m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, m_dwGaeaID );

						//	Note : 분리된 코스툼 로그.
						glItemmt.ReqItemRoute ( sITEM_NEW, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum );												
					}
					
					m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

					return E_FAIL;
				}
			}
			
			if ( !bDELETE && sCUSTOM.nidDISGUISE!=SNATIVEID(false) )
			{
				if ( sCUSTOM.tDISGUISE != 0 )
				{
					CTime cTIME_LMT(sCUSTOM.tDISGUISE);
					if ( cTIME_LMT.GetYear()!=1970 )
					{
						if ( cTIME_CUR > cTIME_LMT )
						{
							//	시간 제한으로 아이템 삭제 알림.
							GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
							NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.nidDISGUISE;
							m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

							//	Note : 코스툼 정보 리셋.
							pItemDrop->sItemCustom.tDISGUISE = 0;
							pItemDrop->sItemCustom.nidDISGUISE = SNATIVEID(false);
						}
					}
				}
			}		
		}
		// 아이템이 자신과 동일한 맵에 있는지 ( 선도전 종료후 비인증클럽원이 선도무기 획득하는 버그 대처 )
		if ( pItemDrop->sMapID != m_sMapID )	return E_FAIL;

		if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) )
		{
			//	아이탬이 다른 사람에게 예약되어 있습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_RESERVED;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			return E_FAIL;
		}

		SITEM *pItem = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
		if ( !pItem )		return E_FAIL;

		if ( pItem->sBasicOp.emItemType==ITEM_QITEM )
		{
			TourchQItem ( pNetMsg->dwID );
			return S_OK;
		}

		//	Note : 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
		//
		GLPARTY_FIELD* pParty = GLGaeaServer::GetInstance().GetParty(m_dwPartyID);
		if ( pParty && !pItem->ISEVENTITEM() )
		{
			bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop );
			if ( ballot )		return S_OK;
		}

		//	Note : 이벤트 아이템 중복으로 가져가기 방지.
		//
		if ( pItem->ISEVENTITEM() && m_cInventory.HaveEventItem() )
		{
			//	이미 이벤트 아이템을 가지고 있습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

			return E_FAIL;
		}

		//	Note : 아이템 인벤에 넣음 ( 인벤이 가득 차있을 때는 습득 불가능. ) 베트남일 경우 탐닉 인벤토리에 넣을 수도 있다.
		//

#if defined(VN_PARAM) //vietnamtest%%%
		if( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
 			InsertToVNGainInven( pItemDrop );
		}else{
			InsertToInven ( pItemDrop );
		}
#else
		InsertToInven ( pItemDrop );
#endif


/*
		// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
		// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pItemDrop->sItemCustom.dwPetID != 0 )
		{
			CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, pItemDrop->sItemCustom.dwPetID );
			GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
*/

	}
	else if ( pNetMsg->emCrow==CROW_MONEY )
	{
		PMONEYDROP pMoneyDrop = m_pLandMan->GetMoney ( pNetMsg->dwID );

		  
		if ( !pMoneyDrop )
		{
			//	돈이 사라졌습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_OFF;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			return E_FAIL;
		}

		// 사망확인
		if ( !IsValidBody() )	return E_FAIL;

		//	거리 체크
		D3DXVECTOR3 vPos;

		if ( pNetMsg->bPet )	
		{
			PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
			if ( pMyPet )	vPos = pMyPet->m_vPos;
		}	
		else vPos = m_vPos;

		const D3DXVECTOR3 &vTarPos = pMoneyDrop->vPos;

		D3DXVECTOR3 vDistance = vPos - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wTakeAbleDis = wTakeRange + 15;

		if ( fDistance>wTakeAbleDis )
		{
			//	거리가 멉니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_ITEM;
			NetMsg.emTakeFB = EMTAKE_FB_DISTANCE;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			return E_FAIL;			
			
		}

		if ( !pMoneyDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) )
		{
			//	돈이 다른 사람에게 예약되어 있습니다.
			GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
			NetMsg.emCrow = CROW_MONEY;
			NetMsg.emTakeFB = EMTAKE_FB_RESERVED;

			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
			return E_FAIL;
		}


		//	Note : 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
		//
		GLPARTY_FIELD* pParty = GLGaeaServer::GetInstance().GetParty(m_dwPartyID);
		if ( pParty )
		{
			bool ballot = pParty->DOMONEY_ALLOT ( this, pMoneyDrop, m_pLandMan );
			if ( ballot )
			{
				return S_OK;
			}
		}

		//	돈 줍기 성공.
		InsertToInven ( pMoneyDrop );
	}

	return S_OK;
}

//	Note : Field의 아이탬,돈을 손에 들기 시도.	( 손에 든 아이탬이 없다고 가정함. )
//
HRESULT GLChar::MsgReqFieldToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FIELD_TO_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_FIELD_TO_HOLD*> ( nmg );

	PITEMDROP pItemDrop = m_pLandMan->GetItem ( pNetMsg->dwGlobID );

	// 필드에서 아이템을 손에 드는경우는 인벤이 가득 찼을 경우 뿐이다.
	// 베트남탐닉일 경우 손에 드는걸 방지해야 한다.
#if defined(VN_PARAM) //vietnamtest%%%
	if( m_dwVietnamGainType == GAINTYPE_EMPTY )	return E_FAIL;
#endif

	if ( !pItemDrop )
	{
		//	아이탬이 사라졌습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_OFF;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return S_FALSE;
	}

	// 아이템이 자신과 동일한 맵에 있는지 ( 선도전 종료후 비인증클럽원이 선도무기 획득하는 버그 대처 )
	if ( pItemDrop->sMapID != m_sMapID )	return S_FALSE;

	const SITEM *pItemData = GLItemMan::GetInstance().GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItemData )	return S_FALSE;

	if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) )
	{
		//	아이탬이 다른 사람에게 예약되어 있습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_RESERVED;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );
		return S_FALSE;
	}

	if ( pItemData->sBasicOp.emItemType==ITEM_QITEM )
	{
		TourchQItem ( pNetMsg->dwGlobID );
		return S_OK;
	}

	//	Note : 파티원이고 이벤트 아이템이 아닐때 파티 루팅인지 검사후 처리.
	//
	GLPARTY_FIELD* pParty = GLGaeaServer::GetInstance().GetParty(m_dwPartyID);
	if ( pParty && !pItemData->ISEVENTITEM() )
	{
		bool ballot = pParty->DOITEM_ALLOT ( this, pItemDrop );
		if ( ballot )		return S_OK;
	}

	//	Note : 이벤트 아이템 중복으로 가져가기 방지.
	//
	if ( pItemData->ISEVENTITEM() && m_cInventory.HaveEventItem() )
	{
		//	이미 이벤트 아이템을 가지고 있습니다.
		GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
		NetMsg.emCrow = CROW_ITEM;
		NetMsg.emTakeFB = EMTAKE_FB_TOO_EVENT_ITEM;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsg );

		return S_FALSE;
	}

	//	Field에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( pItemDrop->sItemCustom );

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( pItemDrop->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pItemDrop->sItemCustom.wTurnNum );

	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = pItemDrop->sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	//	Note : 아이탬 Land에서 삭제.
	m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

	return S_OK;
}

//	Note : Inventory의 아이탬을 손에 들기 시도.
//
HRESULT GLChar::MsgReqInvenToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_INVEN_TO_HOLD*> ( nmg );
	
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_INVEN_TO_HOLD,EMHOLD_FB_OFF);

	//	요청한 아이탬 유효성 검사.
	SINVENITEM *pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	손에 든 아이탬이 있을때.
	if ( VALID_HOLD_ITEM() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	SITEM* pITEM = GLItemMan::GetInstance().GetItem( pInvenItem->sItemCustom.sNativeID );
	if ( !pITEM ) return S_FALSE;
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && m_bGetPetFromDB )
	{
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write( "PetCard Inven to Hold Error, m_bGetPetFromDB : %d",
															 m_bGetPetFromDB );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	인밴에 있던 아이탬 백업.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	인밴에서 빼낼 아이탬을 삭제.
	m_cInventory.DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[자신에게] 메시지 발생.	- 인밴에서 아이탬 삭제.
	GLMSG::SNETPC_INVEN_DELETE NetMsg;
	NetMsg.wPosX = sInven_BackUp.wPosX;
	NetMsg.wPosY = sInven_BackUp.wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	[자신에게] 인밴에 있었던 아이탬을 손에 듬.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenExHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_EX_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_INVEN_EX_HOLD*> ( nmg );

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SINVENITEM *pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	인밴에 있던 아이탬 백업.
	
	WORD wPosX = sInven_BackUp.wPosX;			//	아이탬 위치.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	손에 든 아이탬 백업.

#if defined(VN_PARAM) //vietnamtest%%%
		if ( sItemHold.bVietnamGainItem )	return S_FALSE;
#endif

	SITEM* pInvenData = GLItemMan::GetInstance().GetItem ( sInven_BackUp.sItemCustom.sNativeID );
	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( sItemHold.sNativeID );
	if ( !pInvenData || !pHoldData )	return S_FALSE;

	//	Note : 병합 할수 있는 아이템인지 점검.
	bool bMERGE(false);
	bMERGE = (pInvenData->sBasicOp.sNativeID==pHoldData->sBasicOp.sNativeID);
	if (bMERGE)	bMERGE = ( pInvenData->ISPILE() );
	if (bMERGE) bMERGE = ( sItemHold.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if (bMERGE) bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sItemHold.wTurnNum;
		
		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
			//	Note : 겹침 수 조정.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	손 있었던 아이탬 제거.
			RELEASE_HOLD_ITEM ();

			//	Note : 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenItem);

			//	[자신에게] 손 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	기존 아이템 갯수 변경.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경.
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM ( sItemHold );

			//	Note : 손에든 아이템 변경.
			GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
			NetMsgPutOn.emSlot = SLOT_HOLD;
			NetMsgPutOn.sItemCustom = sItemHold;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPutOn);

			//	Note : 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenItem);
		}

		return S_OK;
	}

	//	인밴에 있던 아이탬 삭제.
	BOOL bOk = m_cInventory.DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	Note : 종전 손에든 아이탬과 들려는 아이탬의 크기가 틀릴 경우.
	//	들고 있는 아이탬이 들어갈 공간을 찾는다.
//	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
//	{
		//	동일위치에 넣을수 있는지 검사.
		bOk = m_cInventory.IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
			//	들고있던 아이탬 인밴에 넣을 공간이 있는지 검색.
			bOk = m_cInventory.FindInsrtable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : 인밴에서 삭제했던 아이탬을 다시 넣음.
				m_cInventory.InsertItem ( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

				return S_FALSE;
			}
		}
//}

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( sItemHold, wPosX, wPosY );
	SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem(wPosX,wPosY);
	if ( !pINSERT_ITEM )
	{
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write ( "error MsgReqInvenExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.sNativeID.wMainID, sItemHold.sNativeID.wSubID, wPosX, wPosY );
		return S_FALSE;
	}

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[자신에게] 인밴에 있던 아이탬 삭제, 및 아이탬 삽입.
	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이탬.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	if ( pINSERT_ITEM )	NetMsg_Del_Insert.sInsert = *pINSERT_ITEM;	//	삽입되는 아이탬.

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Del_Insert);

	//	[자신에게] 인밴에 있었던 아이탬을 손에 듬.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);


	//	유효기간 초과 점검.	
	const SINVENITEM& sINVENITEM = *pINSERT_ITEM;
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.sNativeID );
	if ( pITEM && pITEM->IsTIMELMT() )
	{

		if ( pITEM->sDrugOp.tTIME_LMT != 0 )
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sINVENITEM.sItemCustom.tBORNTIME);
			cTIME_LMT += cSPAN;

			WORD wInsertPosX = sINVENITEM.wPosX, wInsertPosY = sINVENITEM.wPosY;


			if ( cTIME_CUR > cTIME_LMT )
			{
				// 탈것일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sINVENITEM.sItemCustom.dwVehicleID != 0 )
				{
					GLVEHICLE* pNewVehicle = new GLVEHICLE();
					CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
												 sINVENITEM.sItemCustom.dwVehicleID, 
												 m_dwClientID, 
												 m_dwCharID, 
												 true );
					GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );
				}

				// 팻카드일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && sINVENITEM.sItemCustom.dwPetID != 0 )
				{
					CDeletePet* pDbAction = new CDeletePet ( m_dwCharID, sINVENITEM.sItemCustom.dwPetID );
					GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pDbAction );

					// 활동중이면 사라지게 해준다.
					PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
					if ( pMyPet && sINVENITEM.sItemCustom.dwPetID == pMyPet->m_dwPetID )
					{
						// 팻의 악세사리를 검사한다.
						pMyPet->UpdateTimeLmtItem ( this );

						// 넣어줄꺼 넣어준다.
						for ( WORD i = 0; i < ACCETYPESIZE; ++i )
						{
							CItemDrop cDropItem;
							cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
							if ( IsInsertToInven ( &cDropItem ) ) InsertToInven ( &cDropItem );
							else
							{
								if ( m_pLandMan )
								{
									m_pLandMan->DropItem ( m_vPos, 
														   &(cDropItem.sItemCustom), 
														   EMGROUP_ONE, 
														   m_dwGaeaID );
								}
							}
						}

						GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(pMyPet->m_dwGUID,true,false) );

						// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
						CGetRestorePetList *pDbAction = new CGetRestorePetList ( m_dwCharID, m_dwClientID );
						if ( pDBMan ) pDBMan->AddJob ( pDbAction );
					}
					else
					{
						// 악세사리를 체크후 팻을 DB에서 삭제해준다.
						GLPET* pNewPet = new GLPET ();
						CGetPet* pGetPet = new CGetPet ( pNewPet, 
														 sINVENITEM.sItemCustom.dwPetID, 
														 m_dwClientID, 
														 m_dwCharID,
														 sINVENITEM.wPosX,
														 sINVENITEM.wPosY,
														 true );
						GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
						if ( pDBMan ) pDBMan->AddJob ( pGetPet );
					}
				}

				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNETPC_INVEN_DELETE NetMsgInvenDel;
				NetMsgInvenDel.wPosX = sINVENITEM.wPosX;
				NetMsgInvenDel.wPosY = sINVENITEM.wPosY;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDel);

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sINVENITEM.sItemCustom.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

				//	Note : 아이템 삭제.
				//
				m_cInventory.DeleteItem ( sINVENITEM.wPosX, sINVENITEM.wPosY );
			
				//	Note : 코스툼 복구.
				if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
				{
					SITEM *pONE = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )		return E_FAIL;

					SITEMCUSTOM sITEM_NEW;
					sITEM_NEW.sNativeID = sINVENITEM.sItemCustom.nidDISGUISE;
					CTime cTIME = CTime::GetCurrentTime();
					if ( sINVENITEM.sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
					{
						cTIME = CTime(sINVENITEM.sItemCustom.tDISGUISE);

						CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
						cTIME -= tLMT;
					}

					//	Note : 아이템 생성.
					//
					sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
					sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

					//	Note : 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cInventory.IsInsertable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
					if ( !bOk )			return E_FAIL;

					//	Note : 인벤에 넣음.
					m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

					GLMSG::SNETPC_INVEN_INSERT NetItemInsert;
					NetItemInsert.Data = *pINSERT_ITEM;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemInsert);

					//	Note : 분리된 코스툼 로그.
					GLITEMLMT::GetInstance().ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );
				}
			}
		}
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenToSlot ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_TO_SLOT *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_INVEN_TO_SLOT*> ( nmg );
	if ( pNetMsg->emToSlot >= SLOT_TSIZE )	return S_FALSE;

	SINVENITEM *pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )	return E_FAIL;

	SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;

	BOOL bOk(FALSE);

	if( sItemCustom.nidDISGUISE!=SNATIVEID(false) )
	{
		//	코스튬을 입힌 아이템일 경우
		bOk = ACCEPT_ITEM ( sItemCustom.sNativeID, sItemCustom.nidDISGUISE );
		if ( !bOk )	return E_FAIL;
	}
	else
	{
		//	자격조건에 부합하는지 검사.
		bOk = ACCEPT_ITEM ( sItemCustom.sNativeID );
		if ( !bOk )	return E_FAIL;
	}

	//	해당 슬롯에 장착 가능한지 검사.
	bOk = CHECKSLOT_ITEM ( sItemCustom.sNativeID, pNetMsg->emToSlot );
	if ( !bOk )	return E_FAIL;

	//	해당 슬롯이 비였는지 점검.
	bOk = ISEMPTY_SLOT ( sItemCustom.sNativeID, pNetMsg->emToSlot );
	if ( !bOk ) return E_FAIL;

	//	슬롯에 아이탬 삽입.
	SLOT_ITEM ( sItemCustom, pNetMsg->emToSlot );

	//	인밴에서 아이탬 삭제.
	m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	[자신에게] 해당 아이탬을 인밴에서 삭제.
	GLMSG::SNETPC_INVEN_DELETE NetMsgInvenDel;
	NetMsgInvenDel.wPosX = pNetMsg->wPosX;
	NetMsgInvenDel.wPosY = pNetMsg->wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDel);

	//	[자신에게] 해당 아이탬을 인밴에 넣어줌.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = pNetMsg->emToSlot;
	NetMsg_PutOn.sItemCustom = GET_SLOT_ITEM(pNetMsg->emToSlot);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

HRESULT GLChar::MsgReqVNGainToHold(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD*> ( nmg );

	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD,EMHOLD_FB_OFF);

	//	요청한 아이탬 유효성 검사.
	SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	손에 든 아이탬이 있을때.
	if ( VALID_HOLD_ITEM() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	인밴에 있던 아이탬 백업.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	인밴에서 빼낼 아이탬을 삭제.
	m_cVietnamInventory.DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[자신에게] 메시지 발생.	- 인밴에서 아이탬 삭제.
	GLMSG::SNETPC_INVEN_DELETE NetMsg;
	NetMsg.wPosX = sInven_BackUp.wPosX;
	NetMsg.wPosY = sInven_BackUp.wPosY;
	NetMsg.bVietnamInven = TRUE;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	[자신에게] 인밴에 있었던 아이탬을 손에 듬.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);
	return S_OK;
}

HRESULT GLChar::MsgReqVNGainExHold(NET_MSG_GENERIC* nmg)
{

	GLMSG::SNETPC_REQ_VNGAIN_EX_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_VNGAIN_EX_HOLD*> ( nmg );

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SINVENITEM *pInvenItem = m_cVietnamInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	인밴에 있던 아이탬 백업.

	WORD wPosX = sInven_BackUp.wPosX;			//	아이탬 위치.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	손에 든 아이탬 백업.

	SITEM* pInvenData = GLItemMan::GetInstance().GetItem ( sInven_BackUp.sItemCustom.sNativeID );
	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( sItemHold.sNativeID );
	if ( !pInvenData || !pHoldData )	return S_FALSE;

	//	Note : 병합 할수 있는 아이템인지 점검.
	bool bMERGE(false);
	bMERGE = (pInvenData->sBasicOp.sNativeID==pHoldData->sBasicOp.sNativeID);
	if (bMERGE)	bMERGE = ( pInvenData->ISPILE() );
	if (bMERGE) bMERGE = ( sItemHold.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if (bMERGE) bMERGE = ( sInven_BackUp.sItemCustom.wTurnNum<pInvenData->sDrugOp.wPileNum );
	if ( bMERGE )
	{
		WORD wTurnNum = sInven_BackUp.sItemCustom.wTurnNum + sItemHold.wTurnNum;

		if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
		{
			//	Note : 겹침 수 조정.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	손 있었던 아이탬 제거.
			RELEASE_HOLD_ITEM ();

			//	Note : 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenItem);

			//	[자신에게] 손 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;

			//	기존 아이템 갯수 변경.
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경.
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM ( sItemHold );

			//	Note : 손에든 아이템 변경.
			GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
			NetMsgPutOn.emSlot = SLOT_HOLD;
			NetMsgPutOn.sItemCustom = sItemHold;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPutOn);

			//	Note : 인벤의 아이템 업데이트.
			GLMSG::SNET_INVEN_ITEM_UPDATE NetMsgInvenItem;
			NetMsgInvenItem.wPosX = pNetMsg->wPosX;
			NetMsgInvenItem.wPosY = pNetMsg->wPosY;
			NetMsgInvenItem.sItemCustom = pInvenItem->sItemCustom;
			NetMsgInvenItem.bVietnamInven = TRUE;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenItem);
		}

		return S_OK;
	}

	//	인밴에 있던 아이탬 삭제.
	BOOL bOk = m_cVietnamInventory.DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	Note : 종전 손에든 아이탬과 들려는 아이탬의 크기가 틀릴 경우.
	//	들고 있는 아이탬이 들어갈 공간을 찾는다.
//	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	동일위치에 넣을수 있는지 검사.
		BOOL bOk = m_cVietnamInventory.IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
			//	들고있던 아이탬 인밴에 넣을 공간이 있는지 검색.
			bOk = m_cVietnamInventory.FindInsrtable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : 인밴에서 삭제했던 아이탬을 다시 넣음.
				sInven_BackUp.sItemCustom.bVietnamGainItem = true;
				m_cVietnamInventory.InsertItem ( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

				return S_FALSE;
			}
		}
	}

	//	들고있던 아이탬 인밴에 넣음.
	sItemHold.bVietnamGainItem = true;
	m_cVietnamInventory.InsertItem ( sItemHold, wPosX, wPosY );
	SINVENITEM *pINSERT_ITEM = m_cVietnamInventory.GetItem(wPosX,wPosY);
	if ( !pINSERT_ITEM )
	{
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write ( "error MsgReqInvenExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.sNativeID.wMainID, sItemHold.sNativeID.wSubID, wPosX, wPosY );
		return S_FALSE;
	}

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[자신에게] 인밴에 있던 아이탬 삭제, 및 아이탬 삽입.
	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이탬.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	NetMsg_Del_Insert.bVietnamInven = TRUE;
	if ( pINSERT_ITEM )	NetMsg_Del_Insert.sInsert = *pINSERT_ITEM;	//	삽입되는 아이탬.

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Del_Insert);

	//	[자신에게] 인밴에 있었던 아이탬을 손에 듬.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

HRESULT GLChar::MsgReqHoldToVNGain(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_HOLD_TO_VNGAIN *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_VNGAIN*> ( nmg );

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	인벤 위치.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;

	// 해킹 재현 ( 아이템의 사이즈가 0 일때 )
	//pHoldData->sBasicOp.wInvenSizeX = 0, pHoldData->sBasicOp.wInvenSizeY = 0;

	BOOL bOk = m_cVietnamInventory.IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		return E_FAIL;
	}

	//	들고있던 아이탬 인밴에 넣음.
	SITEMCUSTOM& sITEM_NEW = m_PutOnItems[SLOT_HOLD];
	sITEM_NEW.bVietnamGainItem = true;
	bOk = m_cVietnamInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
	if ( !bOk	 ) return E_FAIL;

	SINVENITEM sINVENITEM = *m_cVietnamInventory.FindPosItem ( wInsertPosX, wInsertPosY );

	//	성공시 아이탬 제거.
	RELEASE_HOLD_ITEM ();

	//	[자신에게] 인밴에 아이탬 삽입.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.bVietnamInven = TRUE;
	NetMsg_Inven_Insert.Data = sINVENITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

	//	[자신에게] 손 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);	

	return S_OK;
}

HRESULT GLChar::MsgReqVNInvenToInven( NET_MSG_GENERIC* nmg )
{

	if ( m_dwVietnamInvenCount <= 0 ) return E_FAIL;

	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN *pNetMsg = (GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN *)nmg;

	SINVENITEM* pInvenItem(NULL);
	pInvenItem = m_cVietnamInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	살려고 하는 아이탬이 없습니다.
	if ( !pInvenItem )											return E_FAIL;
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )												return E_FAIL;

	WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
	WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

	WORD wInsertPosX(0), wInsertPosY(0);
	BOOL bITEM_SPACE(false);
	bITEM_SPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );

	//	인벤에 여유공간이 없을 경우 취소됨.
	if ( !bITEM_SPACE )		return E_FAIL;

	SITEMCUSTOM sITEM_NEW = pInvenItem->sItemCustom;

	sITEM_NEW.bVietnamGainItem = false;
	m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
	m_cVietnamInventory.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );

	SINVENITEM sINVENITEM = *m_cInventory.FindPosItem ( wInsertPosX, wInsertPosY );

	m_dwVietnamInvenCount--;
	GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE NetMsgFB;
	NetMsgFB.dwVietnamInvenCount = m_dwVietnamInvenCount;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB NetMsg;
	NetMsg.wPosX    = pNetMsg->wPosX;
	NetMsg.wPosY    = pNetMsg->wPosY;
	NetMsg.wNewPosX = wInsertPosX;
	NetMsg.wNewPosY = wInsertPosY;
	NetMsg.Data		= sINVENITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);	

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.sNativeID );
	if ( pITEM && pITEM->IsTIMELMT() )
	{

		if ( pITEM->sDrugOp.tTIME_LMT != 0 )
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sINVENITEM.sItemCustom.tBORNTIME);
			cTIME_LMT += cSPAN;

			WORD wInsertPosX = sINVENITEM.wPosX, wInsertPosY = sINVENITEM.wPosY;


			if ( cTIME_CUR > cTIME_LMT )
			{
				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNETPC_INVEN_DELETE NetMsgInvenDel;
				NetMsgInvenDel.wPosX = sINVENITEM.wPosX;
				NetMsgInvenDel.wPosY = sINVENITEM.wPosY;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDel);

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sINVENITEM.sItemCustom.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

				//	Note : 아이템 삭제.
				//
				m_cInventory.DeleteItem ( sINVENITEM.wPosX, sINVENITEM.wPosY );
			
				//	Note : 코스툼 복구.
				if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
				{
					SITEM *pONE = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )		return E_FAIL;

					SITEMCUSTOM sITEM_NEW;
					sITEM_NEW.sNativeID = sINVENITEM.sItemCustom.nidDISGUISE;
					CTime cTIME = CTime::GetCurrentTime();
					if ( sINVENITEM.sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
					{
						cTIME = CTime(sINVENITEM.sItemCustom.tDISGUISE);

						CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
						cTIME -= tLMT;
					}

					//	Note : 아이템 생성.
					//
					sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
					sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

					//	Note : 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cInventory.IsInsertable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
					if ( !bOk )			return E_FAIL;

					//	Note : 인벤에 넣음.
					m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

					GLMSG::SNETPC_INVEN_INSERT NetItemInsert;
					NetItemInsert.Data = *pINSERT_ITEM;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemInsert);

					//	Note : 분리된 코스툼 로그.
					GLITEMLMT::GetInstance().ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );
				}
			}
		}
	}	

	return S_OK;
}

HRESULT GLChar::MsgReqVNGainInvenReset(NET_MSG_GENERIC* nmg)
{
	m_cVietnamInventory.DeleteItemAll();

	return S_OK;
}


HRESULT GLChar::MsgReqInvenSplit ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_SPLIT *pNetMsg = (GLMSG::SNETPC_REQ_INVEN_SPLIT *)nmg;
	
	BOOL bOk(FALSE);

	//	Note : 분리 요청 아이템 찾음.
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : 아이템 정보 가져오기.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )													return E_FAIL;

	//	Note : 분리 가능한 아이템인지 점검.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISINSTANCE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= 1 )					return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : 원본 아이템의 변화된 갯수.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : 분리되어 새로 생기는 아이템.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : 새로 생기는 아이템 위치 찾기.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : 새로 생긴 아이템 넣기.
	bOk = m_cInventory.InsertItem ( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : 새로 생긴 아이템 정보 가져오기.
	SINVENITEM* pNewItem = m_cInventory.GetItem ( wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : 원본 아이템의 갯수 변경 시킴.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : 인벤에 새로 분리되어 들어 가는 아이템.
	GLMSG::SNETPC_INVEN_INSERT NetMsgNew;
	NetMsgNew.Data = *pNewItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgNew);

	//	Note : 원본 아이템 갯수 변경.
	GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsgInven;
	NetMsgInven.wPosX = pNetMsg->wPosX;
	NetMsgInven.wPosY = pNetMsg->wPosY;
	NetMsgInven.wTurnNum = wORGIN_SPLIT;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);

	return S_OK;
}

HRESULT GLChar::MsgReqHoldToInven ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_HOLD_TO_INVEN *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_INVEN*> ( nmg );

#if defined(VN_PARAM) //vietnamtest%%%
	if ( pNetMsg->bUseVietnamInven && m_dwVietnamInvenCount <= 0 ) return E_FAIL;
#endif

	if ( !VALID_HOLD_ITEM() )
	{
		return S_FALSE;
	}

	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pHoldData )
	{
		return E_FAIL;
	}

	//	인벤 위치.
	WORD wInsertPosX = pNetMsg->wPosX, wInsertPosY = pNetMsg->wPosY;

	// 해킹 재현 ( 아이템의 사이즈가 0 일때 )
	//pHoldData->sBasicOp.wInvenSizeX = 0, pHoldData->sBasicOp.wInvenSizeY = 0;

	BOOL bOk = m_cInventory.IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		return E_FAIL;
	}

	//	들고있던 아이탬 인밴에 넣음.	
#if defined(VN_PARAM) //vietnamtest%%%   
    SITEMCUSTOM& sCustom = m_PutOnItems[SLOT_HOLD];
	if ( sCustom.bVietnamGainItem && m_dwVietnamInvenCount <= 0 ) return E_FAIL;
	
	sCustom.bVietnamGainItem = false;
#endif	

	bOk = m_cInventory.InsertItem ( GET_HOLD_ITEM(), wInsertPosX, wInsertPosY );
	if ( !bOk	 ) return E_FAIL;

	SINVENITEM sINVENITEM = *m_cInventory.FindPosItem ( wInsertPosX, wInsertPosY );

	//	성공시 아이탬 제거.
	RELEASE_HOLD_ITEM ();

#if defined(VN_PARAM) //vietnamtest%%%
	if( pNetMsg->bUseVietnamInven )
	{
		m_dwVietnamInvenCount--;

		GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE NetMsgFB;
		NetMsgFB.dwVietnamInvenCount = m_dwVietnamInvenCount;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
	}
#endif

	//	[자신에게] 인밴에 아이탬 삽입.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.Data = sINVENITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

	//	[자신에게] 손 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);


	//	유효기간 초과 점검.
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.sNativeID );
	if ( pITEM && pITEM->IsTIMELMT() )
	{
		if ( pITEM->sDrugOp.tTIME_LMT != 0 )
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(sINVENITEM.sItemCustom.tBORNTIME);
			cTIME_LMT += cSPAN;

			if ( cTIME_CUR > cTIME_LMT )
			{
				// 탈것일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sINVENITEM.sItemCustom.dwVehicleID != 0 )
				{
					GLVEHICLE* pNewVehicle = new GLVEHICLE();
					CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
												 sINVENITEM.sItemCustom.dwVehicleID, 
												 m_dwClientID, 
												 m_dwCharID, 
												 true );
					GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );
				}

				// 팻카드일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && sINVENITEM.sItemCustom.dwPetID != 0 )
				{
					CDeletePet* pDbAction = new CDeletePet ( m_dwCharID, sINVENITEM.sItemCustom.dwPetID );
					GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pDbAction );

					// 활동중이면 사라지게 해준다.
					PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
					if ( pMyPet && sINVENITEM.sItemCustom.dwPetID == pMyPet->m_dwPetID )
					{
						// 팻의 악세사리를 검사한다.
						pMyPet->UpdateTimeLmtItem ( this );

						// 넣어줄꺼 넣어준다.
						for ( WORD i = 0; i < ACCETYPESIZE; ++i )
						{
							CItemDrop cDropItem;
							cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
							if ( IsInsertToInven ( &cDropItem ) ) InsertToInven ( &cDropItem );
							else
							{
								if ( m_pLandMan )
								{
									m_pLandMan->DropItem ( m_vPos, 
														   &(cDropItem.sItemCustom), 
														   EMGROUP_ONE, 
														   m_dwGaeaID );
								}
							}
						}

						GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(pMyPet->m_dwGUID,true,false) );

						// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
						CGetRestorePetList *pDbAction = new CGetRestorePetList ( m_dwCharID, m_dwClientID );
						if ( pDBMan ) pDBMan->AddJob ( pDbAction );
					}
					else
					{
						// 악세사리를 체크후 팻을 DB에서 삭제해준다.
						GLPET* pNewPet = new GLPET ();
						CGetPet* pGetPet = new CGetPet ( pNewPet, 
														 sINVENITEM.sItemCustom.dwPetID, 
														 m_dwClientID, 
														 m_dwCharID,
														 sINVENITEM.wPosX,
														 sINVENITEM.wPosY,
														 true );
						GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
						if ( pDBMan ) pDBMan->AddJob ( pGetPet );
					}
				}

				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNETPC_INVEN_DELETE NetMsgInvenDel;
				NetMsgInvenDel.wPosX = sINVENITEM.wPosX;
				NetMsgInvenDel.wPosY = sINVENITEM.wPosY;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDel);

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sINVENITEM.sItemCustom.sNativeID;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

				//	Note : 아이템 삭제.
				//
				m_cInventory.DeleteItem ( sINVENITEM.wPosX, sINVENITEM.wPosY );
			
				//	Note : 코스툼 복구.
				if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
				{
					SITEM *pONE = GLItemMan::GetInstance().GetItem ( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )		return E_FAIL;

					SITEMCUSTOM sITEM_NEW;
					sITEM_NEW.sNativeID = sINVENITEM.sItemCustom.nidDISGUISE;
					CTime cTIME = CTime::GetCurrentTime();
					if ( sINVENITEM.sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
					{
						cTIME = CTime(sINVENITEM.sItemCustom.tDISGUISE);

						CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
						cTIME -= tLMT;
					}

					//	Note : 아이템 생성.
					//
					sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
					sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

					//	Note : 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cInventory.IsInsertable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
					if ( !bOk )			return E_FAIL;

					//	Note : 인벤에 넣음.
					m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

					GLMSG::SNETPC_INVEN_INSERT NetItemInsert;
					NetItemInsert.Data = *pINSERT_ITEM;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemInsert);

					//	Note : 분리된 코스툼 로그.
					GLITEMLMT::GetInstance().ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );
				}
			}
		}
	}

/*
	// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
	// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD && sINVENITEM.sItemCustom.dwPetID != 0 )
	{
		CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, sINVENITEM.sItemCustom.dwPetID );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );
	}
*/


	return S_OK;
}

HRESULT GLChar::MsgReqHoldToField ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_HOLD_TO_FIELD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_FIELD*> ( nmg );

	if ( !VALID_HOLD_ITEM () )	return S_FALSE;

	D3DXVECTOR3 vCollisionPos;
	BOOL bCollision = m_pLandMan->IsCollisionNavi
		(
		pNetMsg->vPos + D3DXVECTOR3(0,+5,0),
		pNetMsg->vPos + D3DXVECTOR3(0,-5,0),
		vCollisionPos
		);

	if ( !bCollision )			return S_FALSE;

	const SITEMCUSTOM& sITEMCUSTOM = GET_HOLD_ITEM();

#if defined(VN_PARAM) //vietnamtest%%%
	if ( sITEMCUSTOM.bVietnamGainItem )
	{
		//	아이탬 제거.
		RELEASE_HOLD_ITEM ();

		//	[자신에게] 손 있었던 아이탬 제거.
		GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

		return S_OK;
	}
#endif



	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sITEMCUSTOM.sNativeID );
	if ( !pITEM )		return false;

	// 버리는 아이템이 팻카드 && 팻이 활성상태이면 버릴 수 없다
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && sITEMCUSTOM.dwPetID == pMyPet->m_dwPetID ) return FALSE;
	}
	
	//	거래옵션
	if ( !pITEM->sBasicOp.IsTHROW() )						return FALSE;

	// GMCharEdit 로 넣은 아이템은 버리기 불가능하다.
	if ( sITEMCUSTOM.IsGM_GENITEM() )						return FALSE;

	//	Item Drop 시도.
	BOOL bDrop = m_pLandMan->DropItem ( vCollisionPos, const_cast<SITEMCUSTOM*> ( &sITEMCUSTOM ) );
	if ( !bDrop )				return S_FALSE;

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sITEMCUSTOM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_GROUND, sITEMCUSTOM.wTurnNum );

	//	Drop 시 아이탬 제거.
	RELEASE_HOLD_ITEM ();

	//	[자신에게] 손 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	return S_OK;
}

BOOL ISBOTH_HANDHELD ( SITEM *pITEM )
{
	GASSERT ( pITEM );
	return ( (pITEM->sSuitOp.emSuit==SUIT_HANDHELD) && pITEM->sSuitOp.IsBOTHHAND() );
}

HRESULT GLChar::MsgReqSlotToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SLOT_TO_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_TO_HOLD*> ( nmg );
	if ( pNetMsg->emSlot >= SLOT_TSIZE )		return S_FALSE;

	if ( VALID_HOLD_ITEM () )					return S_FALSE;
	if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )	return S_FALSE;
	if ( m_sCONFTING.IsCONFRONTING() )			return S_FALSE;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM *pITEM_LEFT=NULL, *pITEM_RIGHT=NULL;
	if ( VALID_SLOT_ITEM(emLHand) )	pITEM_LEFT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emLHand).sNativeID);
	if ( VALID_SLOT_ITEM(emRHand) )	pITEM_RIGHT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emRHand).sNativeID);

	//	양손 모두 착용 무기 있을 경우.	활+화살, 창+부적. 세트로 동작하는 무기류가 있음 이때 모두 빼내야함.
	//
	if ( pNetMsg->emSlot==emRHand )
	{
		if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_BOW) || (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_SPEAR) )
		{
			if ( pITEM_LEFT )	//	화살, 부적 빼내야 함.
			{
				SITEMCUSTOM sITEM_LHAND = GET_SLOT_ITEM(emLHand);

				WORD wInvenPosX, wInvenPosY;
				BOOL bOk = m_cInventory.FindInsrtable ( pITEM_LEFT->sBasicOp.wInvenSizeX, pITEM_LEFT->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
				if ( bOk )
				{
					//	종전 아이탬 인밴에 넣음.
					m_cInventory.InsertItem ( sITEM_LHAND, wInvenPosX, wInvenPosY );

					//	[자신에게] 인밴에 아이탬 삽입.
					GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven_Insert;
					NetMsg_Inven_Insert.Data = *m_cInventory.GetItem(wInvenPosX, wInvenPosY);
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

					//	[모두에게] 디스플레이 아이템이 아닌걸로 가정하여 무시.
				}
				//	Note : 인벤토리 넣기에 실패하였을 경우.
				else
				{
					//	Item Drop 시도.
					BOOL bDrop = m_pLandMan->DropItem ( m_vPos, &sITEM_LHAND );

					//	Note :아이템의 소유 이전 경로 기록.
					//
					GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_LHAND, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_GROUND, sITEM_LHAND.wTurnNum );
				}

				//	왼쪽 슬롯 지워줌.
				RELEASE_SLOT_ITEM(emLHand);

				//	[자신에게] SLOT에 있었던 아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emLHand);
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);
			}
		}
	}

	if ( pNetMsg->emSlot== SLOT_VEHICLE && m_bGetVehicleFromDB )
	{
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write( "Vehilce Slot Release Error, m_bGetVehicleFromDB : %d",
															 m_bGetVehicleFromDB );
		return S_FALSE;
	}

	//	손에 들게 함.
	HOLD_ITEM ( GET_SLOT_ITEM(pNetMsg->emSlot) );

	//	SLOT 아이탬 제거.
	RELEASE_SLOT_ITEM ( pNetMsg->emSlot );

	//	아이탬 변경시 값 재 환산 필요함.
	INIT_DATA ( FALSE, FALSE );

	//	[자신에게] SLOT에 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(pNetMsg->emSlot);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

	//	[모두에게] 자신의 착용 복장이 바뀜을 알림.
	GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutOn_Release_Brd;
	NetMsg_PutOn_Release_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Release_Brd.emSlot = pNetMsg->emSlot;
	SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsg_PutOn_Release_Brd) );

	//	[자신에게] 손에 아이탬을 들게.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = GET_HOLD_ITEM();
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);


	if ( pNetMsg->emSlot == SLOT_VEHICLE )
	{
		GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, false );
	}

	// 무기에 따른 버프를 초기화 한다.
	DisableSkillFact();
	

	return S_OK;
}

HRESULT GLChar::MsgReqSlotExHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SLOT_EX_HOLD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_EX_HOLD*> ( nmg );
	if ( pNetMsg->emSlot >= SLOT_TSIZE )		return S_FALSE;

	if ( !VALID_HOLD_ITEM () )					return S_FALSE;
	if ( m_sCONFTING.IsCONFRONTING() )			return S_FALSE;

	BOOL bOk(FALSE);

#if defined(VN_PARAM) //vietnamtest%%%
	if ( GET_HOLD_ITEM().bVietnamGainItem )	return S_FALSE;
#endif

	if( GET_HOLD_ITEM().nidDISGUISE!=SNATIVEID(false) )
	{
		//	코스튬을 입힌 아이템일 경우
		bOk = ACCEPT_ITEM ( GET_HOLD_ITEM().sNativeID, GET_HOLD_ITEM().nidDISGUISE );
		if ( !bOk )	return E_FAIL;
	}
	else
	{
		//	자격조건에 부합하는지 검사.
		bOk = ACCEPT_ITEM ( GET_HOLD_ITEM().sNativeID );
		if ( !bOk )	return E_FAIL;
	}

	//	해당 슬롯에 장착 가능한지 검사.
	bOk = CHECKSLOT_ITEM ( GET_HOLD_ITEM().sNativeID, pNetMsg->emSlot );
	if ( !bOk )	return E_FAIL;
	
	WORD wInvenPosX, wInvenPosY;
	SITEMCUSTOM sToHoldItem(NATIVEID_NULL());
	SITEMCUSTOM sToInvenItem(NATIVEID_NULL());
	SITEMCUSTOM sToSlotItem(NATIVEID_NULL());

	SITEM *pITEM_TOSLOT = GLItemMan::GetInstance().GetItem(GET_HOLD_ITEM().sNativeID);
	if ( !pITEM_TOSLOT )	return E_FAIL;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	//	Note : 일반 슬롯일 경우.
	//
	if ( pITEM_TOSLOT->sSuitOp.emSuit!=SUIT_HANDHELD )
	{
		if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )	return S_FALSE;

		//	종전 아이탬 손에 들고 있게.	#1 백업
		sToHoldItem = GET_SLOT_ITEM ( pNetMsg->emSlot );

		//	장착할 아이템 지정.
		sToSlotItem = GET_HOLD_ITEM();
	}
	//	손에 드는 도구일 경우.
	else
	{
		SITEM *pITEM_LEFT=NULL, *pITEM_RIGHT=NULL;
		if ( VALID_SLOT_ITEM(emLHand) )	pITEM_LEFT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emLHand).sNativeID);
		if ( VALID_SLOT_ITEM(emRHand) )	pITEM_RIGHT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emRHand).sNativeID);

		//	양손 도구를 착용하려 하는 경우.
		if ( ISBOTH_HANDHELD(pITEM_TOSLOT) )
		{
			if ( pITEM_LEFT && pITEM_RIGHT )
			{
				//	Memo :	넣을려는 아이템이 활이고 왼쪽에 착용한 아이템이 화살이 아닌 경우.
				if( ( (pITEM_TOSLOT->sSuitOp.emAttack==ITEMATT_BOW) && (pITEM_LEFT->sBasicOp.emItemType!=ITEM_ARROW) ) ||
				//	Memo :	넣을려는 아이템이 봉이고 왼쪽에 착용한 아이템이 부적이 아닌 경우.
					( (pITEM_TOSLOT->sSuitOp.emAttack==ITEMATT_SPEAR) && (pITEM_LEFT->sBasicOp.emItemType!=ITEM_CHARM) ) ||
				//	Memo :	넣을려는 아이탬이 활이나 지팡이가 아닐 경우.
					( (pITEM_TOSLOT->sSuitOp.emAttack!=ITEMATT_BOW) && (pITEM_TOSLOT->sSuitOp.emAttack!=ITEMATT_SPEAR) ) )
				{
					bOk = m_cInventory.FindInsrtable ( pITEM_LEFT->sBasicOp.wInvenSizeX, pITEM_LEFT->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
					if ( !bOk )	return E_FAIL;

					//	종전 아이탬 인밴에 넣음.	#1 백업		//	왼쪽 슬롯 지워줌.
					sToInvenItem = GET_SLOT_ITEM(emLHand);
				}

				//	종전 아이탬 손에 들고 있게.	#2 백업
				sToHoldItem = GET_SLOT_ITEM(emRHand);

				//	장착할 아이템 지정.
				sToSlotItem = GET_HOLD_ITEM();
			}
			else
			{
				if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )	return S_FALSE;

				//	손에 들게.
				sToHoldItem = GET_SLOT_ITEM(pNetMsg->emSlot);

				//	아이탬 장착할 아이템 지정.
				sToSlotItem = GET_HOLD_ITEM();
			}
		}
		//	한손 도구를 착용하려 하는 경우.
		else
		{
			//	Note : 착용할려하는 도구가 화살이나 부적의 경우.
			//
			if ( (pITEM_TOSLOT->sBasicOp.emItemType==ITEM_ARROW) || (pITEM_TOSLOT->sBasicOp.emItemType==ITEM_CHARM) )
			{
				//	왼손에만 장착 가능.
				if ( pNetMsg->emSlot != emLHand )
					return E_FAIL;

				//	Note : 양손무기이고 활이나 화살일 경우만 착용 가능.
				//
				if ( pITEM_RIGHT && ISBOTH_HANDHELD(pITEM_RIGHT) )
				{
					if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_BOW) && (pITEM_TOSLOT->sBasicOp.emItemType!=ITEM_ARROW) )		return E_FAIL;
					if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_SPEAR) && (pITEM_TOSLOT->sBasicOp.emItemType!=ITEM_CHARM) )	return E_FAIL;
				}
				else	return E_FAIL;

				if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )	return S_FALSE;

				//	종전 아이탬 손에 들고 있게.	#1 백업
				sToHoldItem = GET_SLOT_ITEM(pNetMsg->emSlot);

				//	장착할 아이템 지정.
				sToSlotItem = GET_HOLD_ITEM();
			}
			else 
			{
				//	종전 아이템이 양손류일 경우.
				if ( pITEM_RIGHT && ISBOTH_HANDHELD(pITEM_RIGHT) )
				{
					//	양손 모두 착용 무기 있을 경우.	활+화살, 창+부적. 세트로 동작하는 무기류가 있음 이때 모두 빼내야함.
					//
					if ( pITEM_LEFT )
					{
						bOk = m_cInventory.FindInsrtable ( pITEM_LEFT->sBasicOp.wInvenSizeX, pITEM_LEFT->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
						if ( !bOk )	return E_FAIL;

						//	종전 아이탬 인밴에 넣음. #1 백업	//	왼쪽 슬롯 지워줌.
						sToInvenItem = GET_SLOT_ITEM(emLHand);
					}

					//	종전 아이탬 손에 들고 있게.	#2 백업
					sToHoldItem = GET_SLOT_ITEM(emRHand);

					//	장착할 아이템 지정.
					sToSlotItem = GET_HOLD_ITEM();
				}
				//	일반적일 경우.
				else
				{
					if ( !VALID_SLOT_ITEM(pNetMsg->emSlot) )	return S_FALSE;

					//	종전 아이탬 손에 들고 있게.	#1 백업
					sToHoldItem = GET_SLOT_ITEM(pNetMsg->emSlot);

					//	장착할 아이템 지정.
					sToSlotItem = GET_HOLD_ITEM();
				}
			}
		}
	}

	//	Note : 실제로 이동 시킴, 메시지 발생
	//

	//	Note : 인벤에 보내질 아이템.
	//
	if ( sToInvenItem.sNativeID != NATIVEID_NULL() )
	{
		m_cInventory.InsertItem ( sToInvenItem, wInvenPosX, wInvenPosY );

		//	[자신에게] 인밴에 아이탬 삽입.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven_Insert;
		NetMsg_Inven_Insert.Data = *m_cInventory.GetItem(wInvenPosX, wInvenPosY);
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);		
	}

	//	Note : 손에들기.
	//

	if ( sToHoldItem.sNativeID != NATIVEID_NULL() )
	{
		HOLD_ITEM ( sToHoldItem );

		//	[자신에게] 손에 아이탬을 들게.
		GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
		NetMsg_PutOn.emSlot = SLOT_HOLD;
		NetMsg_PutOn.sItemCustom = sToHoldItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

		// 무기에 따른 버프를 초기화 한다.
		DisableSkillFact();
	}

	if ( pNetMsg->emSlot == SLOT_VEHICLE )
	{
		GLGaeaServer::GetInstance().SaveVehicle( m_dwClientID, m_dwGaeaID, false );		
	}

	//	Note : 슬롯에 넣기. 슬롯에서 빼낼 아이템 있으면 제거. (인벤에 보내질 아이템.)
	//
	if ( sToSlotItem.sNativeID != NATIVEID_NULL() )
	{
		//	Note : 슬롯에서 제거할 아이템이 있을 경우.
		//
		EMSLOT emSlotRelease(SLOT_TSIZE);
		if ( sToInvenItem.sNativeID!=NATIVEID_NULL() )
		{
			emSlotRelease = emLHand;
			RELEASE_SLOT_ITEM(emLHand);		//	왼쪽 슬롯 지워줌.
		}

		//	슬롯에 넣음.
		SLOT_ITEM ( sToSlotItem, pNetMsg->emSlot );

		//	[자신에게] SLOT 에 아이탬을 착용.
		//
		GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
		NetMsg_PutOn.emSlot = pNetMsg->emSlot;
		NetMsg_PutOn.sItemCustom = sToSlotItem;
		NetMsg_PutOn.emSlotRelease = emSlotRelease;					//	아이템 삭제할 슬롯.
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

		//	[모두에게] 자신의 착용 복장이 바뀜을 알림.
		//
		GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsg_PutOn_Update_Brd;
		NetMsg_PutOn_Update_Brd.dwGaeaID = m_dwGaeaID;
		NetMsg_PutOn_Update_Brd.emSlot = pNetMsg->emSlot;
		NetMsg_PutOn_Update_Brd.sItemClient.Assign ( sToSlotItem );
		NetMsg_PutOn_Update_Brd.emSlotRelease = emSlotRelease;		//	아이템 삭제할 슬롯.
		SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsg_PutOn_Update_Brd) );

		// 무기에 따른 버프를 초기화 한다.
		DisableSkillFact();
	}

	//	Note : 아이탬 변경시 값 재 환산 필요함.
	//
	INIT_DATA ( FALSE, FALSE );

	return S_OK;
}

HRESULT GLChar::MsgReqHoldToSlot ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_HOLD_TO_SLOT *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_HOLD_TO_SLOT*> ( nmg );
	if ( pNetMsg->emSlot >= SLOT_TSIZE )	return S_FALSE;

	if ( !VALID_HOLD_ITEM () )				return S_FALSE;
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pItem )							return S_FALSE;
	if ( m_sCONFTING.IsCONFRONTING() )		return S_FALSE;

	const SITEMCUSTOM sItemCustom = GET_HOLD_ITEM ();

	BOOL bOk(FALSE);

#if defined(VN_PARAM) //vietnamtest%%%
	if ( sItemCustom.bVietnamGainItem )	return S_FALSE;
#endif
	
	if( sItemCustom.nidDISGUISE!=SNATIVEID(false) )
	{
		//	코스튬을 입힌 아이템일 경우
		bOk = ACCEPT_ITEM ( sItemCustom.sNativeID, sItemCustom.nidDISGUISE );
		if ( !bOk )	return E_FAIL;
	}
	else
	{
		//	자격조건에 부합하는지 검사.
		bOk = ACCEPT_ITEM ( sItemCustom.sNativeID );
		if ( !bOk )	return E_FAIL;
	}
	
	//	해당 슬롯에 장착 가능한지 검사.
	bOk = CHECKSLOT_ITEM ( sItemCustom.sNativeID, pNetMsg->emSlot );
	if ( !bOk )	return E_FAIL;

	//	해당 슬롯이 비였는지 점검.
	bOk = ISEMPTY_SLOT ( sItemCustom.sNativeID, pNetMsg->emSlot );
	if ( !bOk ) return E_FAIL;

	//	만약 양손 무기일 경우 슬롯을 "오른손 들기" 슬롯으로.
	if ( ISBOTH_HANDHELD(pItem) )	pNetMsg->emSlot = GetCurRHand();

	//	해당 슬롯에 아이탬 장착.
	SLOT_ITEM ( sItemCustom, pNetMsg->emSlot );

	//	손에 든 아이템 제거
	RELEASE_HOLD_ITEM ();

	//	아이탬 변경시 값 재 환산 필요함.
	INIT_DATA ( FALSE, FALSE );

	//	[자신에게] 손에 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

	//	[자신에게] 손에 있었던 아이탬을 Slot에 착용.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = pNetMsg->emSlot;
	NetMsg_PutOn.sItemCustom = sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	//	[모두에게] 복장이 바뀜을 알려줌.
	GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.emSlot = pNetMsg->emSlot;
	NetMsg_PutOn_Brd.sItemClient.Assign ( sItemCustom );
	SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsg_PutOn_Brd) );

	// 무기에 따른 버프를 초기화 한다.
	DisableSkillFact();

	return S_OK;
}


HRESULT GLChar::MsgReqSlotChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SLOT_CHANGE *pNetMsg = reinterpret_cast<GLMSG::SNETPC_REQ_SLOT_CHANGE*> ( nmg );

	if( IsUseArmSub() ) SetUseArmSub( FALSE ); // 주무기
	else				SetUseArmSub( TRUE );  // 보조무기

	//	아이탬 변경시 값 재 환산 필요함.
	INIT_DATA ( FALSE, FALSE, m_sCONFTING.sOption.fHP_RATE );

	//	[자신에게] 무기 스왑
	GLMSG::SNETPC_PUTON_CHANGE NetMsg_PutOn;
	NetMsg_PutOn.bUseArmSub = IsUseArmSub();
	NetMsg_PutOn.fCONFT_HP_RATE = m_sCONFTING.sOption.fHP_RATE;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	//	[모두에게] 무기 스왑을 알림.
	GLMSG::SNETPC_PUTON_CHANGE_BRD NetMsg_PutOn_Brd;
	NetMsg_PutOn_Brd.dwGaeaID = m_dwGaeaID;
	NetMsg_PutOn_Brd.bUseArmSub = IsUseArmSub();
	SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsg_PutOn_Brd) );

	GLMSG::SNETPC_PUTON_CHANGE_AG NetMsg_PutOn_AG;
	NetMsg_PutOn_AG.bUseArmSub = IsUseArmSub();
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsg_PutOn_AG );


	// 무기에 따른 버프를 초기화 한다.
	DisableSkillFact();
	
	return S_OK;
}

bool GLChar::InsertPileItem ( const SITEMCUSTOM& sItemCustom, WORD wREQINSRTNUM )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );
	if ( !pItem )				return false;
	if ( !pItem->ISPILE() )		return false;

	WORD wPILENUM = pItem->sDrugOp.wPileNum;
	WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
	WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

	//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	//		아이템 점검 숫자를 감소시킴.
	GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
	GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
	for ( ; iter!=ItemMap.end(); ++iter )
	{
		SINVENITEM &sINVENITEM = *(*iter).second;
		SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
		if ( sITEMCUSTOM.sNativeID != sItemCustom.sNativeID )	continue;
		if ( sITEMCUSTOM.wTurnNum>=wPILENUM )		continue;
	
		//	기존 아이템 겹침 여유분.
		WORD wSURPLUSNUM = wPILENUM - sITEMCUSTOM.wTurnNum;

		if ( wREQINSRTNUM > wSURPLUSNUM )
		{
			//	Note : 기존 아이템의 겹침을 변경. ( 꽉참 )
			sITEMCUSTOM.wTurnNum = wPILENUM;

			//	Note : 클라이언트에 갯수 변경을 알림.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
			NetMsg.wPosX = sINVENITEM.wPosX;
			NetMsg.wPosY = sINVENITEM.wPosY;
			NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

			//	Note : 잔여 수량 감소.
			wREQINSRTNUM -= wSURPLUSNUM;
		}
		else
		{
			//	Note : 기존 아이템에 겹치는 것 만으로도 새로 넣을 아이템이 충분히 들어감.
			sITEMCUSTOM.wTurnNum += wREQINSRTNUM;

			//	Note : 클라이언트에 갯수 변경을 알림.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg;
			NetMsg.wPosX = sINVENITEM.wPosX;
			NetMsg.wPosY = sINVENITEM.wPosY;
			NetMsg.wTurnNum = sITEMCUSTOM.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

			//	금액 변화 메시지.
			GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
			NetMsgMoney.lnMoney = m_lnMoney;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgMoney);

			return true;
		}
	}

	//	Note : 새로이 인벤에 들어가야할 아이템의 갯수 파악후 인벤에 들어갈 공간이 있는지 검사.

	//	최대겹침아이템 수량.
	WORD wONENUM = wREQINSRTNUM / wPILENUM;
	WORD wITEMNUM = wONENUM;

	//	여분겹침아이템의 겹침수.
	WORD wSPLITNUM = wREQINSRTNUM % wPILENUM;
	if ( wSPLITNUM > 0 )				wITEMNUM += 1;
	if ( wSPLITNUM==0 && wITEMNUM>=1 )	wSPLITNUM = wPILENUM;

	for ( WORD i=0; i<wITEMNUM; ++i )
	{
		WORD wInsertPosX(0), wInsertPosY(0);
		BOOL bITEM_SPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );
		GASSERT(bITEM_SPACE&&"넣을 공간을 미리 체크를 하고 아이템을 넣었으나 공간이 부족함.");

		//	Note : 새로운 아이템을 넣어줌.
		//
		SITEMCUSTOM sITEMCUSTOM(sItemCustom.sNativeID);
		sITEMCUSTOM.tBORNTIME = sItemCustom.tBORNTIME;

		sITEMCUSTOM.cGenType = sItemCustom.cGenType;
		sITEMCUSTOM.cChnID = sItemCustom.cChnID;
		sITEMCUSTOM.cFieldID = sItemCustom.cFieldID;

		if ( wITEMNUM==(i+1) )	sITEMCUSTOM.wTurnNum = wSPLITNUM;	//	마지막 아이템은 잔여량.
		else					sITEMCUSTOM.wTurnNum = wPILENUM;	//	아닐 경우는 꽉찬량.

		m_cInventory.InsertItem ( sITEMCUSTOM, wInsertPosX, wInsertPosY );	//	인밴토리에 넣습니다.

		//	인밴에 아이탬 넣어주는 메시지.
		GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
		NetMsgInven.Data = *m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);
	}

	return true;
}

bool GLChar::DeletePileItem ( SNATIVEID sNID, WORD wREQDELNUM )
{
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNID );
	if ( !pItem )				return false;
	if ( !pItem->ISPILE() )		return false;

	WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
	WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

	std::vector<SNATIVEID>	vecDEL;

	//	Note : 더 겹쳐질수 있는 아이템이 있는지 검사하여 인벤에 들어가야하는
	//		아이템 점검 숫자를 감소시킴.
	GLInventory::CELL_MAP &ItemMap = *m_cInventory.GetItemList();
	GLInventory::CELL_MAP_ITER iter = ItemMap.begin();
	for ( ; iter!=ItemMap.end(); ++iter )
	{
		if ( wREQDELNUM==0 )						break;

		SINVENITEM &sINVENITEM = *(*iter).second;
		SITEMCUSTOM &sITEMCUSTOM = sINVENITEM.sItemCustom;
		if ( sITEMCUSTOM.sNativeID != sNID )		continue;

		if ( sITEMCUSTOM.wTurnNum>wREQDELNUM )
		{
			sITEMCUSTOM.wTurnNum -= wREQDELNUM;
			wREQDELNUM = 0;

			//	Note : 갯수변경 메시지.
			GLMSG::SNETPC_INVEN_DRUG_UPDATE	NetMsgUpdate;
			NetMsgUpdate.wPosX = sINVENITEM.wPosX;
			NetMsgUpdate.wPosY = sINVENITEM.wPosY;
			NetMsgUpdate.wTurnNum = sITEMCUSTOM.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgUpdate);
		}
		else
		{
			wREQDELNUM -= sITEMCUSTOM.wTurnNum;
			sITEMCUSTOM.wTurnNum = 0;

			vecDEL.push_back ( SNATIVEID(sINVENITEM.wPosX,sINVENITEM.wPosY) );
		}
	}

	if ( !vecDEL.size() )	return true;

	for ( DWORD i=0; i<vecDEL.size(); ++i )
	{
		m_cInventory.DeleteItem ( vecDEL[i].wMainID, vecDEL[i].wSubID );

		//	Note : 삭제 메시지.
		GLMSG::SNETPC_INVEN_DELETE NetMsgDelete;
		NetMsgDelete.wPosX = vecDEL[i].wMainID;
		NetMsgDelete.wPosY = vecDEL[i].wSubID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgDelete);
	}

	return true;
}

// *****************************************************
// Desc: NPC 로부터 아이템 구입
// *****************************************************
HRESULT GLChar::MsgReqBuyFromNpc ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_BUY_FROM_NPC *pNetMsg = (GLMSG::SNETPC_REQ_BUY_FROM_NPC *) nmg;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pNetMsg->sNID );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();

	if ( !m_pLandMan )
	{
		return E_FAIL;
	}

	// 상점 NPC 유효성 체크
	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
	if ( !pCrow )					return E_FAIL;

	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if ( fDist>fTalkableDis )		return E_FAIL;
	

	if ( !pCrowData )
	{
		//	상인 NPC가 존제하지 않습니다.
		return E_FAIL;
	}

	if ( !pCrowData->m_sNpcTalkDlg.IsMARKET() )
	{
		//	상인이 아닙니다.
		return E_FAIL;
	}

	if ( pCrowData->GetSaleNum() <= pNetMsg->dwChannel )		return E_FAIL;

	GLInventory *pInven = pCrowData->GetSaleInven(pNetMsg->dwChannel);
	if ( !pInven )												return E_FAIL;

	SINVENITEM* pSaleItem(NULL);
	pSaleItem = pInven->GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	살려고 하는 아이탬이 없습니다.
	if ( !pSaleItem )											return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pSaleItem->sItemCustom.sNativeID );
	if ( !pItem )												return E_FAIL;

	volatile float fSHOP_RATE = GET_PK_SHOP2BUY();
	if ( fSHOP_RATE==0.0f )										return E_FAIL;

	volatile float fSHOP_2RATE = fSHOP_RATE + m_pLandMan->m_fCommissionRate;
	volatile float fSHOP_2RATE_C = fSHOP_2RATE * 0.01f;

	LONGLONG dwPrice = pCrowData->GetNpcSellPrice(pItem->sBasicOp.sNativeID.dwID);
	volatile LONGLONG llPRICE2;
	if( dwPrice == 0 )
	{
		dwPrice = pItem->sBasicOp.dwBuyPrice;
		llPRICE2 = (LONGLONG)(dwPrice * fSHOP_2RATE_C);
	}else{
		llPRICE2 = dwPrice;
	}

	
	volatile LONGLONG llPRICE3 = (LONGLONG)(llPRICE2*pNetMsg->wBuyNum);

	if ( m_lnMoney < llPRICE3 )
	{
		//	돈이 부족합니다.
		return E_FAIL;
	}

	WORD wINVENX = pItem->sBasicOp.wInvenSizeX;
	WORD wINVENY = pItem->sBasicOp.wInvenSizeY;

	//	Note : 인벤에 여유 공간이 있는지 검사.
	//
	BOOL bITEM_SPACE(FALSE);
	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( pNetMsg->wBuyNum * pItem->GETAPPLYNUM() );

		bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY );

		//	인벤에 여유공간이 없을 경우 취소됨.
		if ( !bITEM_SPACE )		return E_FAIL;

		//	Note :아이템의 소유 이전 경로 기록.
		//
		SITEMCUSTOM sDB_ITEM(sNID);
		sDB_ITEM.cGenType = EMGEN_SHOP;
		sDB_ITEM.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
		sDB_ITEM.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
		sDB_ITEM.wTurnNum = wPILENUM;
		sDB_ITEM.tBORNTIME = CTime::GetCurrentTime().GetTime();

		GLITEMLMT::GetInstance().ReqItemRoute ( sDB_ITEM, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sDB_ITEM.wTurnNum );

		//	Note : 구입을 수행합니다.
		//	
		CheckMoneyUpdate( m_lnMoney, llPRICE3, FALSE, "Buy From Npc." );
		m_bMoneyUpdate = TRUE;

		m_lnMoney -= llPRICE3;					//	돈을 지불.

		//	돈 로그.
		if ( llPRICE3>EMMONEY_LOG )
		{
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, -LONGLONG(llPRICE3), EMITEM_ROUTE_DELETE );
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
		}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
		if ( m_bTracingUser )
		{
			NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

			CString strTemp;
			strTemp.Format( "Buy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]",
					 m_szUID, m_szName,  llPRICE3, m_lnMoney );

			StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

			m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
		}
#endif

		//	Note : 아이템을 인벤에 넣어준다.
		//
		InsertPileItem ( sDB_ITEM, wREQINSRTNUM );
	}
	else
	{
		//	일반 아이템의 경우.
		WORD wInsertPosX(0), wInsertPosY(0);
		bITEM_SPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wInsertPosX, wInsertPosY );

		//	인벤에 여유공간이 없을 경우 취소됨.
		if ( !bITEM_SPACE )		return E_FAIL;

		//	Note : 새로운 아이템을 생성합니다.
		//
		SNATIVEID sNID = pSaleItem->sItemCustom.sNativeID;
		SITEMCUSTOM sITEM_NEW = pSaleItem->sItemCustom;
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = 1;
		sITEM_NEW.cGenType = (BYTE) EMGEN_SHOP;
		sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
		sITEM_NEW.cFieldID = (BYTE) GLGaeaServer::GetInstance().GetFieldSvrID();
		sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sNID, EMGEN_SHOP );

		//	랜덤 옵션 발생.
		//
		//if( sITEM_NEW.GENERATE_RANDOM_OPT () )
		//{
		//	GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
		//}

		//	Note : 돈을 지불합니다.
		//
		CheckMoneyUpdate( m_lnMoney, llPRICE2, FALSE, "Buy From Npc." );
		m_bMoneyUpdate = TRUE;

		m_lnMoney -= llPRICE2;

		//	돈 로그.
		if ( llPRICE2>EMMONEY_LOG )
		{
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, -LONGLONG(llPRICE2), EMITEM_ROUTE_DELETE );
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
		}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
		if ( m_bTracingUser )
		{
			NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

			CString strTemp;
			strTemp.Format( "Buy From Npc, [%s][%s], Buy price:[%I64d], Have Money:[%I64d]",
					 m_szUID, m_szName,  llPRICE3, m_lnMoney );

			StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

			m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
		}
#endif

		//	Note : 인벤토리에 넣어줍니다.
		//
		BOOL bOK = m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
		if ( !bOK )		return S_OK;

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

		//	인밴에 아이탬 넣어주는 메시지.
		GLMSG::SNETPC_INVEN_INSERT NetMsgInven;
		NetMsgInven.Data = *m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgInven);
	}

	if ( pCrow->m_bNpcRecall )
	{
		volatile float fSHOP_RATE_C = fSHOP_RATE * 0.01f;
		volatile LONGLONG llPRICE1 = (LONGLONG)( dwPrice * fSHOP_RATE_C );
		volatile LONGLONG llCommission = llPRICE2 - llPRICE1;

		llCommission = LONGLONG ( llCommission * GLCONST_CHAR::fEARNING_RATE_NPC + 0.1f );

		pCrow->m_lnCommission += llCommission;
	}
	else if ( m_pLandMan->m_dwGuidClubID != CLUB_NULL )
	{
		GLGuidance *pGuid = m_pLandMan->m_pGuidance;
		if ( pGuid && !pGuid->m_bBattle )
		{
			//	Note : 클럽의 수익 발생.
			volatile float fSHOP_RATE_C = fSHOP_RATE * 0.01f;
			volatile LONGLONG llPRICE1 = (LONGLONG)( dwPrice * fSHOP_RATE_C );
			volatile LONGLONG llCommission = llPRICE2 - llPRICE1;

			GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub ( m_pLandMan->m_dwGuidClubID );
			if ( pCLUB )
			{
				pCLUB->m_lnIncomeMoney += llCommission;

				// 클럽 수입이 1억 이상일 경우 로그를 남긴다.
				if( llCommission > 	EMCLUBINCOMEMONEY_LOG )
					GLITEMLMT::GetInstance().ReqMoneyExc( ID_CHAR, 0, 
														  ID_CLUB, pCLUB->m_dwID,
														  llCommission,
														  EMITEM_ROUTE_CLUBINCOME );

				CDebugSet::ToFile ( "club_income_char.txt", "{BuyFromNpc}, ClubID[%u], CharID[%u], Commission[%I64d], IncomeMoney[%I64d]",
									pCLUB->m_dwID, m_dwCharID, llCommission, pCLUB->m_lnIncomeMoney );
			}
		}
	}

	//	금액 변화 메시지.
	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgMoney);

	return S_OK;
}

// *****************************************************
// Desc: NPC 에게 아이템 판매
// *****************************************************
HRESULT GLChar::MsgReqSaleToNpc ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_SALE_TO_NPC *pNetMsg = (GLMSG::SNETPC_REQ_SALE_TO_NPC *) nmg;
	
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pNetMsg->sNID );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();

	// 상점 NPC 유효성 체크
	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
	if ( !pCrow )						return E_FAIL;
	

	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if ( fDist>fTalkableDis )			return E_FAIL;
	

	if ( !pCrowData )
	{
		//	상인 NPC가 존제하지 않습니다.
		return E_FAIL;
	}

	if ( !pCrowData->m_sNpcTalkDlg.IsMARKET() )
	{
		//	상인이 아닙니다.
		return E_FAIL;
	}

	if ( !VALID_HOLD_ITEM() )
	{
		//	팔려고 하는 아이탬이 없습니다.
		return E_FAIL;
	}

	const SITEMCUSTOM& sHOLD_ITEM = GET_HOLD_ITEM();
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sHOLD_ITEM.sNativeID );
	if ( pItem==NULL )						return E_FAIL;

	// 팻아이디(DB접근용)
	DWORD dwPetID = sHOLD_ITEM.dwPetID;
	DWORD dwVehicleID = sHOLD_ITEM.dwVehicleID;

	//	거래옵션
	if ( !pItem->sBasicOp.IsSALE() )
	{
		//	팔기 가능하지 않음.
		return E_FAIL;
	}

//  클라이언트와 서버 아이템 계산 공식이 틀려서 수정함 
/* 
	volatile float fSHOP_SALE_RATE = GET_PK_SHOP2SALE();
	if ( fSHOP_SALE_RATE==0.0f )			return E_FAIL;

	volatile LONGLONG llPRICE = LONGLONG( pItem->GETPRICE(sHOLD_ITEM.wTurnNum) * GLCONST_CHAR::fDISPRICE );

	volatile float fSHOP_SALE_2RATE = fSHOP_SALE_RATE - m_pLandMan->m_fCommissionRate;
	volatile float fSHOP_SALE_2RATE_C = (DOUBLE(fSHOP_SALE_2RATE) * 0.01f );
	volatile LONGLONG llDisPRICE2 = LONGLONG( llPRICE * fSHOP_SALE_2RATE_C );
*/

	volatile float fSHOP_SALE_RATE = GET_PK_SHOP2SALE();
	if ( fSHOP_SALE_RATE==0.0f )			return E_FAIL;
	
	volatile LONGLONG dwPrice = pItem->GETSELLPRICE ( sHOLD_ITEM.wTurnNum );  

	volatile float fSHOP_SALE_2RATE = fSHOP_SALE_RATE - m_pLandMan->m_fCommissionRate;
	volatile float fSHOP_SALE_2RATE_C = fSHOP_SALE_2RATE * 0.01f;

	volatile LONGLONG llDisPRICE2 = LONGLONG( dwPrice * fSHOP_SALE_2RATE_C );


	if ( pCrow->m_bNpcRecall )
	{
		volatile float fSHOP_RATE_C = fSHOP_SALE_RATE * 0.01f;
		volatile LONGLONG llDisPRICE1 = LONGLONG ( dwPrice * fSHOP_RATE_C );
		volatile LONGLONG llCommission = llDisPRICE1-llDisPRICE2;

		llCommission = LONGLONG ( llCommission * GLCONST_CHAR::fEARNING_RATE_NPC + 0.1f );

		pCrow->m_lnCommission += llCommission;
	}
	else if ( m_pLandMan->m_dwGuidClubID != CLUB_NULL )
	{
		GLGuidance *pGuid = m_pLandMan->m_pGuidance;
		if ( pGuid && !pGuid->m_bBattle )
		{
			//	Note : 클럽의 수익 발생.
			volatile float fSHOP_RATE_C = fSHOP_SALE_RATE * 0.01f;
			volatile LONGLONG llDisPRICE1 = LONGLONG ( dwPrice * fSHOP_RATE_C );
			volatile LONGLONG llCommission = llDisPRICE1-llDisPRICE2;
		
			GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub ( m_pLandMan->m_dwGuidClubID );
			if ( pCLUB )
			{
				pCLUB->m_lnIncomeMoney += llCommission;

				// 클럽 수입이 1억 이상일 경우 로그를 남긴다.
				if( llCommission > 	EMCLUBINCOMEMONEY_LOG )
					GLITEMLMT::GetInstance().ReqMoneyExc(	ID_CHAR, m_dwCharID, 
															ID_CLUB, pCLUB->m_dwID,
															llCommission,
															EMITEM_ROUTE_CLUBINCOME );

				CDebugSet::ToFile ( "club_income_char.txt", "{SaleToNpc}, ClubID[%u], CharID[%u], Commission[%I64d], IncomeMoney[%I64d]",
									pCLUB->m_dwID, m_dwCharID, llCommission, pCLUB->m_lnIncomeMoney );
			}
		}
	}

	//	금액 변화.
	CheckMoneyUpdate( m_lnMoney, llDisPRICE2, TRUE, "Sale To Npc." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney += llDisPRICE2;

	//	돈 로그.
	if ( llDisPRICE2>EMMONEY_LOG )
	{
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CHAR, m_dwCharID, llDisPRICE2, EMITEM_ROUTE_CHAR );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "Sale To Npc, [%s][%s], Sale price:[%I64d], Have Money:[%I64d]",
				 m_szUID, m_szName,  llDisPRICE2, m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sHOLD_ITEM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_SHOP, sHOLD_ITEM.wTurnNum );

	//	아이탬 삭제.
	RELEASE_HOLD_ITEM();

	//	금액 변화 메시지.
	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgMoney);

	//	[자신에게] 손에 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

	// 팻카드일 경우 아이템을 상점에 팔면 팻DB 삭제
	if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && dwPetID != 0 )
	{
		// 팻을 사라지게 해준다.
		GLGaeaServer::GetInstance().ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,false) );

		CDeletePet* pDbAction = new CDeletePet ( m_dwCharID, dwPetID );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );

		// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
		CGetRestorePetList *pDbAction1 = new CGetRestorePetList ( m_dwCharID, m_dwClientID );
		if ( pDBMan ) pDBMan->AddJob ( pDbAction1 );
	}

	if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && dwVehicleID != 0 )
	{
		CDeleteVehicle* pDbAction = new CDeleteVehicle ( m_dwClientID, m_dwCharID, dwVehicleID );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );		
	}

	return S_OK;
}


HRESULT GLChar::MsgReqFireCracker ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FIRECRACKER_BRD *pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER_BRD *) nmg;

	GLMSG::SNETPC_REQ_FIRECRACKER_FB NetMsgFB;

	if ( !VALID_HOLD_ITEM () )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( m_sTrade.Valid() )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM* pITEM = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pITEM->sBasicOp.sNativeID ) ) return E_FAIL;

	SITEMCUSTOM &sCUSTOM = m_PutOnItems[SLOT_HOLD];

	if ( pITEM->sBasicOp.emItemType!=ITEM_FIRECRACKER )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( pITEM->sBasicOp.strTargetEffect.empty() )
	{
		NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	Note : 폭죽 사용.
	//
	GLMSG::SNETPC_REQ_FIRECRACKER_BRD NetMsgBrd;
	NetMsgBrd.nidITEM = sCUSTOM.sNativeID;
	NetMsgBrd.vPOS = pNetMsg->vPOS;
	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBrd );

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgBrd);

	//	Note : 폭죽 사용을 알림.
	//
	NetMsgFB.emFB = EMREQ_FIRECRACKER_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	//	Note : 아이템 소모.
	//
	DoDrugSlotItem (SLOT_HOLD );

	return S_OK;
}

HRESULT GLChar::MsgInvenVietnamGet ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;
//	if( m_dwVietnamGainType == GAINTYPE_EMPTY ) return E_FAIL;
//	if( m_dwVietnamGainType == GAINTYPE_HALF ) return E_FAIL;

	GLMSG::SNETPC_INVEN_VIETNAM_INVENGET *pNetMsg = (GLMSG::SNETPC_INVEN_VIETNAM_INVENGET *) nmg;


	SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )	return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sDrugOp.wCureVolume == 0 )	return E_FAIL;

    if( pNetMsg->bGetExp )
	{
		// 확률 
		if ( pItem->sDrugOp.bRatio ) 
		{
			LONGLONG lAddExp;
			LONGLONG lAddMoney;

			float fRate = (float)pItem->sDrugOp.wCureVolume / 100.0f;

			lAddExp = m_lVNGainSysExp * (LONGLONG)fRate;
			lAddMoney = m_lVNGainSysMoney * (LONGLONG)fRate;

			if ( lAddExp > m_lVNGainSysExp ) lAddExp = m_lVNGainSysExp;
			if ( lAddMoney > m_lVNGainSysMoney ) lAddMoney = m_lVNGainSysMoney;

			m_lVNGainSysExp -= lAddExp;
			m_lVNGainSysMoney -= lAddMoney;

			m_sExperience.lnNow += lAddExp;
			m_lnMoney			+= lAddMoney;

			m_bVietnamLevelUp = TRUE; 
		}
		else 
		{
			LONGLONG lAddExp;
			if( m_lVNGainSysExp < pItem->sDrugOp.wCureVolume )
			{
				lAddExp = m_lVNGainSysExp;
				m_lVNGainSysExp = 0;
			}else{
				lAddExp = pItem->sDrugOp.wCureVolume;
				m_lVNGainSysExp -= pItem->sDrugOp.wCureVolume;
			}
			m_sExperience.lnNow += lAddExp;
			m_lnMoney			+= m_lVNGainSysMoney;

			m_lVNGainSysMoney = 0;
			m_bVietnamLevelUp = TRUE; 
		}

		DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

		GLMSG::SNETPC_INVEN_VIETNAM_EXPGET_FB NetMsgFB;
		NetMsgFB.lnVnMoney = m_lVNGainSysMoney;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

		//	Note : 클라이언트에 돈 액수 변화를 알려줌.
		GLMSG::SNETPC_UPDATE_MONEY NetMsg;
		NetMsg.lnMoney = m_lnMoney;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

		//	Note : 금액 로그.
		//
		/*if ( lnAmount>EMMONEY_LOG )
		{
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, 0, ID_CHAR, m_dwCharID, lnAmount, EMITEM_ROUTE_VNINVEN );
			GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
		}*/
	}else{
		m_dwVietnamInvenCount += pItem->sDrugOp.wCureVolume;
		DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		GLMSG::SNETPC_INVEN_VIETNAM_ITEMGET_FB NetMsgFB;
		NetMsgFB.dwVietnamInvenCount = m_dwVietnamInvenCount;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	}

	//	[자신에게] 인밴 아이탬 소모시킴.


	return S_OK;
}

HRESULT GLChar::MsgReqInvenDrug ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_INVENDRUG *pNetMsg = (GLMSG::SNETPC_REQ_INVENDRUG *) nmg;

	SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )	return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sDrugOp.emDrug==ITEM_DRUG_NUNE )	return E_FAIL;

	if ( m_sCONFTING.IsFIGHTING() )
	{
		if ( !m_sCONFTING.IsRECOVE() )
		{
			//	Note : 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg;
			NetMsg.wRECOVER = m_sCONFTING.wRECOVER;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER!=USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			//	Note : 회복약 사용 가능 갯수 클라이언트에 알림.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg;
			NetMsg.wRECOVER = m_sCONFTING.wRECOVER;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);
		}
	}

	//	Note : pk 등급이 살인마 등급 이상일 경우 회복약의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
	{
		return E_FAIL;
	}

	switch ( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		if ( m_sHP.wNow == m_sHP.wMax ) return E_FAIL;
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_MP:
		if ( m_sMP.wNow == m_sMP.wMax ) return E_FAIL;
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_SP:
		if ( m_sSP.wNow == m_sSP.wMax ) return E_FAIL;
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_HP_MP:
		if ( m_sHP.wNow == m_sHP.wMax && m_sMP.wNow == m_sMP.wMax ) return E_FAIL;
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_MP_SP:
		if ( m_sMP.wNow == m_sMP.wMax && m_sSP.wNow == m_sSP.wMax ) return E_FAIL;
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_HP_MP_SP:
		if ( m_sHP.wNow == m_sHP.wMax && m_sMP.wNow == m_sMP.wMax && m_sSP.wNow == m_sSP.wMax ) return E_FAIL;
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	//case ITEM_DRUG_HP:
	//case ITEM_DRUG_MP:
	//case ITEM_DRUG_SP:
	//case ITEM_DRUG_HP_MP:
	//case ITEM_DRUG_MP_SP:
	//case ITEM_DRUG_HP_MP_SP:
	//	RECEIVE_DRUGFACT(pItem->sDrugOp.emDrug,pItem->sDrugOp.wCureVolume,pItem->sDrugOp.bRatio);
	//	break;

	case ITEM_DRUG_CURE:
	case ITEM_DRUG_HP_CURE:
	case ITEM_DRUG_HP_MP_SP_CURE:
		{
			BOOL bSTATEBLOW = ISSTATEBLOW();

			if( pItem->sDrugOp.emDrug == ITEM_DRUG_HP_CURE )
			{
				if ( m_sHP.wNow == m_sHP.wMax && !bSTATEBLOW ) return E_FAIL;
				m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
				bSTATEBLOW = TRUE;
			}

			if( pItem->sDrugOp.emDrug == ITEM_DRUG_HP_MP_SP_CURE )
			{
				if ( m_sHP.wNow == m_sHP.wMax && m_sMP.wNow == m_sMP.wMax 
					&& m_sSP.wNow == m_sSP.wMax && !bSTATEBLOW ) return E_FAIL;
				m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
				m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
				m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
				bSTATEBLOW = TRUE;
			}

			if ( !bSTATEBLOW ) return E_FAIL;

			GLMSG::SNETPC_CURESTATEBLOW_BRD	NetMsgBrd;
			for ( int i=0; i<EMBLOW_MULTI; ++i )
			{
				if ( m_sSTATEBLOWS[i].emBLOW==EMBLOW_NONE )		continue;

				EMDISORDER emDIS = STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW);
				if ( !(pItem->sDrugOp.dwCureDISORDER&emDIS) )	continue;

				//	상태이상 지속시간을 종료시킴.
				m_sSTATEBLOWS[i].fAGE = 0.0f;
			}

			//	상태이상 치료됨을 자신에게 알림.
			NetMsgBrd.emCrow = CROW_PC;
			NetMsgBrd.dwID = m_dwGaeaID;
			NetMsgBrd.dwCUREFLAG = pItem->sDrugOp.dwCureDISORDER;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgBrd);

			//	상태이상 치료됨을 모두에게 알림.
			SNDMSGAROUND ( (NET_MSG_GENERIC*)&NetMsgBrd );
		}
		break;
	};


	//	Note : 아이템 소모.
	//
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 체력 수치 변화를 [자신,파티원,주위]의 클라이언트들에 알림.
	MsgSendUpdateState ();

	return S_OK;
}
HRESULT GLChar::MsgReqInvenBoxInfo ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_GET_CHARGEDITEM_FROMDB* pNetMsg = (GLMSG::SNET_GET_CHARGEDITEM_FROMDB*) nmg;

	CString strUID = m_szUID;
    std::vector<SHOPPURCHASE> vItem;

	// DB에서 빌링아이템 정보 가져오기

	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CGetPurchaseItem* pDbAction = new CGetPurchaseItem ( strUID, m_dwCharID );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}
		
	return S_OK;
}

HRESULT GLChar::MsgReqInvenBoxOpen ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_BOXOPEN *pNetMsg = (GLMSG::SNET_INVEN_BOXOPEN *) nmg;

	GLMSG::SNET_INVEN_BOXOPEN_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_BOX )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOBOX;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		NetMsgFB.emFB = EMREQ_BOXOPEN_FB_EMPTY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	Note : 인벤의 여유 공간 측정.
	//
	GLInventory cInvenTemp;
	cInvenTemp.Assign ( m_cInventory );

	for ( int i=0; i<ITEM::SBOX::ITEM_SIZE; ++i )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.sNativeID = pItem->sBox.sITEMS[i].nidITEM;
		if ( sCUSTOM.sNativeID==SNATIVEID(false) )				continue;

		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sCUSTOM.sNativeID );
		if ( !pITEM )
		{
			NetMsgFB.emFB = EMREQ_BOXOPEN_FB_INVALIDITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}

		BOOL bOK = cInvenTemp.InsertItem ( sCUSTOM );
		if ( !bOK )
		{
			//	Note : 인벤에 공간이 없는 것으로 판단됨.
			//
			NetMsgFB.emFB = EMREQ_BOXOPEN_FB_NOTINVEN;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}
	}

	//	Note : 상자안에 있는 아이템을 꺼내어서 넣는다.
	//
	for ( int i=0; i<ITEM::SBOX::ITEM_SIZE; ++i )
	{
		SITEMCUSTOM sITEM_NEW;
		sITEM_NEW.sNativeID = pItem->sBox.sITEMS[i].nidITEM;
		DWORD dwAMOUNT = pItem->sBox.sITEMS[i].dwAMOUNT;
		if ( sITEM_NEW.sNativeID==SNATIVEID(false) )				continue;

		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sITEM_NEW.sNativeID );
		if ( !pITEM )											return E_FAIL;

		//	Note : 아이템 생성.
		//
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = (WORD) dwAMOUNT;
		sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
		sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
		sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
		sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

		// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 )
		sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

		//	랜덤 옵션 생성.
		if( sITEM_NEW.GENERATE_RANDOM_OPT() )
		{
			GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
		}

		//	Note : 인벤에 넣을 위치 찾음.
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
		if ( !bOk )		return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

		//	생성한 아이탬 인밴에 넣음.
		m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
		SINVENITEM *pInsertItem = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

		GLITEMLMT::GetInstance().ReqItemRoute ( pInsertItem->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInsertItem->sItemCustom.wTurnNum );

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pInsertItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);
	}

	//	상자 삭제 로그.
	GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum );

	//	Note : 상자 아이템 삭제.
	//
	m_cInventory.DeleteItem ( wPosX, wPosY );

	//	[자신에게] 인밴에 아이탬 제거.
	GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
	NetMsg_Inven_Delete.wPosX = wPosX;
	NetMsg_Inven_Delete.wPosY = wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);

	//	Note : 상자를 성공적으로 열었음.
	//
	NetMsgFB.emFB = EMREQ_BOXOPEN_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenRandomBoxOpen ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_RANDOMBOXOPEN *pNetMsg = (GLMSG::SNET_INVEN_RANDOMBOXOPEN *) nmg;

	GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_RANDOMITEM )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( !pItem->sRandomBox.VALID() )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_EMPTY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	float fNowRate = seqrandom::getpercent();

	float fLOW = 0;
	ITEM::SRANDOMITEM sGENITEM;
	sGENITEM.nidITEM = SNATIVEID(false);
	for ( DWORD i=0; i<pItem->sRandomBox.vecBOX.size(); ++i )
	{
		ITEM::SRANDOMITEM sITEM = pItem->sRandomBox.vecBOX[i];

		if ( fLOW <= fNowRate && fNowRate < (fLOW+sITEM.fRATE) )
		{
			sGENITEM = sITEM;
			break;
		}

		fLOW += sITEM.fRATE;
	}

	if ( sGENITEM.nidITEM == SNATIVEID(false) )
	{
		//	상자 아이템 삭제 로그 남김.
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum );

		//	Note : 상자 아이템 삭제.
		//
		m_cInventory.DeleteItem ( wPosX, wPosY );

		//	[자신에게] 인밴에 아이탬 제거.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
		NetMsg_Inven_Delete.wPosX = wPosX;
		NetMsg_Inven_Delete.wPosY = wPosY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);

		//	Note : 처리 결과 전송.
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_MISS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM *pITEM_DATA = GLItemMan::GetInstance().GetItem ( sGENITEM.nidITEM );
	if ( !pITEM_DATA )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	Note : 아이탬 발생.
	//
	WORD wINSERTX(0), wINSERTY(0);
	BOOL bOK = m_cInventory.FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wINSERTX, wINSERTY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	Note : 아이템 생성.
	//
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = sGENITEM.nidITEM;

	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = pITEM_DATA->GETAPPLYNUM();
	sITEM_NEW.cGenType = pInvenItem->sItemCustom.cGenType;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 )
	sITEM_NEW.cDAMAGE = (BYTE)pITEM_DATA->sBasicOp.wGradeAttack;
	sITEM_NEW.cDEFENSE = (BYTE)pITEM_DATA->sBasicOp.wGradeDefense;

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	//	생성한 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( sITEM_NEW, wINSERTX, wINSERTY );
	SINVENITEM *pInsertItem = m_cInventory.GetItem ( wINSERTX, wINSERTY );

	if (!pInsertItem) return E_FAIL;


	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pInsertItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	//	상자 아이템 삭제 로그 남김.
	GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum );

	//	랜덤 아이템 생성 로그 남김.
	GLITEMLMT::GetInstance().ReqItemRoute ( pInsertItem->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum );

	//	Note : 상자 아이템 삭제.
	//
	m_cInventory.DeleteItem ( wPosX, wPosY );

	//	[자신에게] 인밴에 아이탬 제거.
	GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
	NetMsg_Inven_Delete.wPosX = wPosX;
	NetMsg_Inven_Delete.wPosY = wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);

	//	Note : 상자를 성공적으로 열었음.
	//
	NetMsgFB.emFB = EMINVEN_RANDOMBOXOPEN_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenDisJunction ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_DISJUNCTION *pNetMsg = (GLMSG::SNET_INVEN_DISJUNCTION *) nmg;

	GLMSG::SNET_INVEN_DISJUNCTION_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	if ( pHold->sBasicOp.emItemType!=ITEM_DISJUNCTION )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( pInvenItem->sItemCustom.nidDISGUISE==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_NONEED;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = pInvenItem->sItemCustom.nidDISGUISE;
	if ( sITEM_NEW.sNativeID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM *pONE = GLItemMan::GetInstance().GetItem ( sITEM_NEW.sNativeID );
	if ( !pONE )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	CTime cTIME = CTime::GetCurrentTime();

	if ( pInvenItem->sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
	{
		cTIME = CTime(pInvenItem->sItemCustom.tDISGUISE);

		CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
		cTIME -= tLMT;
	}

	//	Note : 아이템 생성.
	//
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	sITEM_NEW.wTurnNum = 1;
	sITEM_NEW.cGenType = EMGEN_BILLING;
	sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

	//	Note : hold 아이템 인벤에 되돌림.
	WORD wInsertPosX, wInsertPosY;
	BOOL bOk = m_cInventory.FindInsrtable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;	//	인벤 공간 부족으로 되돌림 실패.
	}

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
	SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pINSERT_ITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	//	Note : 망각의 세탁을 함.
	//
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID(false);

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemUpdate);

	//	Note : 분리된 코스툼 로그.
	GLITEMLMT::GetInstance().ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );

	//	Note : 일반 복장 아이템이 세탁하는 것을 로그 남김.
	//
	GLITEMLMT::GetInstance().ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );

	//	Note : 아이템 소모.
	//
	DoDrugSlotItem ( SLOT_HOLD );

	NetMsgFB.emFB = EMINVEN_DISJUNCTION_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}


// *****************************************************
// Desc: 코스튬 입힘
// *****************************************************
HRESULT GLChar::MsgReqInvenDisguise ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_DISGUISE *pNetMsg = (GLMSG::SNET_INVEN_DISGUISE *) nmg;

	GLMSG::SNET_INVEN_DISGUISE_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	// 유효한 인벤토리 인지 검사
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	// 유효한 아이템인지 검사
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 손에 들고 있는 아이템이 유효한지 검사
	const SITEMCUSTOM& sITEM_HOLD = GET_SLOT_ITEM(SLOT_HOLD);
	if ( sITEM_HOLD.sNativeID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 손에든 아이템의 정보 갖고 오기
	SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 손에든 아이템이 코스튬인지 검사
	if ( !pHold->sBasicOp.IsDISGUISE() )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NODISGUISE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 인벤토리 아이템이 연마가능한지 검사
	if ( pItem->sBasicOp.IsDISGUISE() )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 같은 클래스 인지 검사
	if ( ( pHold->sBasicOp.dwReqCharClass & pItem->sBasicOp.dwReqCharClass ) == NULL )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 복장류인지 검사
	if ( pHold->sBasicOp.emItemType != ITEM_SUIT )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOTSUIT;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_SUIT )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_NOTSUIT;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 같은 종류의 복장인지 검사
	if ( pHold->sSuitOp.emSuit != pItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_DEFSUIT;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 인벤토리의 복장이 코스튬이 입혀져 있는지 검사
	if ( pInvenItem->sItemCustom.nidDISGUISE!=SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMREQ_DISGUISE_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	Note : 시한 코스툼일 경우.
	//
	if ( pHold->sDrugOp.tTIME_LMT != 0 )
	{
		__time64_t &tTIME = pInvenItem->sItemCustom.tDISGUISE;

		CTime tCurTime = sITEM_HOLD.tBORNTIME;
		CTime tSeedTime(tTIME);
		CTimeSpan tLMT (pHold->sDrugOp.tTIME_LMT);
		tLMT += CTimeSpan(0,0,05,0);

		//	Note : 시간 충전.
		tSeedTime = tCurTime + tLMT;

		tTIME = tSeedTime.GetTime();
	}

	//	Note : 코스툼 스킨 지정.
	//
	pInvenItem->sItemCustom.nidDISGUISE = pHold->sBasicOp.sNativeID;

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemUpdate);

	//	Note : 일반 복장 아이템이 코스툼 스킨으로 변환되는 값 로그 남김.
	//
	GLITEMLMT::GetInstance().ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( GET_HOLD_ITEM(), ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, GET_HOLD_ITEM().wTurnNum );

	//	Note : 손에든 아이템 삭제.
	//
	RELEASE_SLOT_ITEM ( SLOT_HOLD );

	//	[자신에게] SLOT에 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

	//	Note : 코스툼 스킨으로 변환 성공 알림.
	//
	NetMsgFB.emFB = EMREQ_DISGUISE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenCleanser ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_INVEN_CLEANSER *pNetMsg = (GLMSG::SNET_INVEN_CLEANSER *) nmg;

	GLMSG::SNET_INVEN_CLEANSER_FB NetMsgFB;

	WORD wPosX = pNetMsg->wPosX;
	WORD wPosY = pNetMsg->wPosY;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}	

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	if ( pInvenItem->sItemCustom.nidDISGUISE==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NONEED;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( pHold->sBasicOp.emItemType!=ITEM_CLEANSER )
	{
		NetMsgFB.emFB = EMREQ_CLEANSER_FB_NONEED;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;	
	}

	//	Note : 망각의 세탁을 함.
	//
	pInvenItem->sItemCustom.tDISGUISE = 0;
	pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID(false);

	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemUpdate);

	//	Note : 일반 복장 아이템이 세탁하는 것을 로그 남김.
	//
	GLITEMLMT::GetInstance().ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );

	//	Note : 아이템 소모.
	//
	DoDrugSlotItem ( SLOT_HOLD );

	//	Note : 코스툼 스킨 리셋 변환 성공 알림.
	//
	NetMsgFB.emFB = EMREQ_CLEANSER_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqMoneyToField ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_MONEY_TO_FIELD *pNetMsg = (GLMSG::SNETPC_REQ_MONEY_TO_FIELD *)nmg;

	if ( !GLCONST_CHAR::bMONEY_DROP2FIELD )	return S_FALSE;
	if ( pNetMsg->lnMoney < 0 )				return S_FALSE;
	if ( m_lnMoney < pNetMsg->lnMoney )		return S_FALSE;

	D3DXVECTOR3 vCollisionPos;
	BOOL bCollision = m_pLandMan->IsCollisionNavi
	(
		pNetMsg->vPos + D3DXVECTOR3(0,+5,0),
		pNetMsg->vPos + D3DXVECTOR3(0,-5,0),
		vCollisionPos
	);

	if ( !bCollision )			return S_FALSE;

	//	Note : 가지고 있는 돈의 액수를 조절.
	CheckMoneyUpdate( m_lnMoney, pNetMsg->lnMoney, FALSE, "Drop Money." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= pNetMsg->lnMoney;

	//	Note : 금액 로그.
	//
	if ( pNetMsg->lnMoney > EMMONEY_LOG )
	{
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, pNetMsg->lnMoney, EMITEM_ROUTE_GROUND );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "DropMoney!!, [%s][%s], Drop Amount:[%I64d], Have Money:[%I64d]",
			m_szUID, m_szName,  pNetMsg->lnMoney, m_lnMoney );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

	//	Note : 바닥에 떨어트린다.
	m_pLandMan->DropMoney ( vCollisionPos, pNetMsg->lnMoney, FALSE );

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);
	
	return S_OK;
}

// *****************************************************
// Desc: 아이템 인첸트 요청 처리
// *****************************************************
HRESULT GLChar::MsgReqInvenGrinding ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_GRINDING *pNetMsg = (GLMSG::SNET_INVEN_GRINDING *)nmg;

	if ( !VALID_HOLD_ITEM() )							return S_FALSE;

	SITEMCUSTOM& sHOLDITEM = m_PutOnItems[SLOT_HOLD];
	SITEM* pHold = GLItemMan::GetInstance().GetItem ( sHOLDITEM.sNativeID );
	if ( !pHold )										return E_FAIL;

	if ( pHold->sBasicOp.emItemType != ITEM_GRINDING )			return S_FALSE;

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )										return S_FALSE;

	if ( pItem->sSuitOp.gdDamage == GLPADATA(0,0) )
	{
		if ( pHold->sGrindingOp.emCLASS != EMGRINDING_CLASS_CLOTH )	return S_FALSE;
	}
	else
	{
		if ( pHold->sGrindingOp.emCLASS != EMGRINDING_CLASS_ARM )	return S_FALSE;
	}

	BOOL bGrinding = pItem->sBasicOp.emItemType==ITEM_SUIT && pItem->sSuitOp.wReModelNum>0;
	if ( !bGrinding )									return E_FAIL;

	if ( pHold->sGrindingOp.emTYPE == EMGRINDING_DAMAGE || pHold->sGrindingOp.emTYPE == EMGRINDING_DEFENSE )
	{
		if ( pInvenItem->sItemCustom.GETGRADE(pHold->sGrindingOp.emTYPE)>=GLCONST_CHAR::wGRADE_MAX )
		{
			return S_FALSE;
		}
	}
	else
	{
		if ( pInvenItem->sItemCustom.GETGRADE(pHold->sGrindingOp.emTYPE)>=GLCONST_CHAR::wGRADE_MAX_REGI )
		{
			return S_FALSE;
		}
	}
	
	//	Note : 연마제 등급, 낮은 연마제로 높은 연마 불가능
	//
	
	BYTE cGrade = 0;

	cGrade = pInvenItem->sItemCustom.GETGRADE(pHold->sGrindingOp.emTYPE);

	if ( cGrade >= GRADE_HIGH && pHold->sGrindingOp.emGRINDER_TYPE != EMGRINDER_TOP )
	{
		return E_FAIL;
	}
	else if ( cGrade >=GRADE_NORMAL && pHold->sGrindingOp.emGRINDER_TYPE < EMGRINDER_HIGH )
	{
		return E_FAIL;
	}

	// 최상위 등급까지 인챈트 할수 있는 아이템인지 점검
	if ( cGrade >= GRADE_HIGH && pItem->sGrindingOp.emGRINDER_TYPE != EMGRINDER_TOP )
	{
		return S_FALSE;
	}


	//	Note : 연마제 수량 확인
	WORD wGradeNum = 1;	
	
	if ( cGrade >= GRADE_HIGH )
	{
		wGradeNum = GLCONST_CHAR::wUSE_GRADE_NUM[cGrade-GRADE_HIGH];

		if ( wGradeNum > sHOLDITEM.wTurnNum )
		{
			return E_FAIL;
		}

	}

	//	Note : 고급 연마제일 경우 GRADE_NORMAL 등급 미만 연마 불가능.
	//
	//if ( pInvenItem->sItemCustom.GETGRADE(pHold->sGrindingOp.emTYPE)<GRADE_NORMAL && pHold->sGrindingOp.bHIGH )
	//{
	//	return E_FAIL;
	//}

	//	Note : 연마 수행.
	//
	bool bRESET(false), bTERMINATE(false);
	EMANTIDISAPPEAR emANTIDISAPPEAR(EMANTIDISAPPEAR_OFF);

	// 소방주가 있다면 bANTIDISAPPEAR를 1로 셋팅해서 아이템이 소멸되는것을 방지한다.
	SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_NECK);
	if ( pITEM && pITEM->sBasicOp.emItemType == ITEM_ANTI_DISAPPEAR )
	{
		if ( !IsCoolTime( pITEM->sBasicOp.sNativeID ) )	emANTIDISAPPEAR = EMANTIDISAPPEAR_ON;
	}

	BOOL bSucceeded = DOGRINDING ( pInvenItem->sItemCustom, pHold, bRESET, bTERMINATE, emANTIDISAPPEAR );

	//	Note : 소모된 연마제 처리.
	//
	DoDrugSlotItem(SLOT_HOLD, wGradeNum);

	//	Note :	소모된 소방주 처리.
	//			bANTIDISAPPEAR가 false면 소방주가 사용되었다.
	if( emANTIDISAPPEAR == EMANTIDISAPPEAR_USE )
	{
		DoDrugSlotItem(SLOT_NECK);
	}

	//	Note : 로그 기록.
	//
	GLITEMLMT::GetInstance().ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );

	//	Note : 연마 성공 여부 반환.
	//
	GLMSG::SNET_INVEN_GRINDING_FB NetMsgFB;
	NetMsgFB.bRESET = bRESET;
	NetMsgFB.bTERMINATE = bTERMINATE;
	NetMsgFB.emANTIDISAPPEAR = emANTIDISAPPEAR;
	NetMsgFB.emGrindFB = bSucceeded ? EMGRINDING_SUCCEED : EMGRINDING_FAIL;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);

	//	Note : 아이템의 속성값 클라이언트에 알림.
	//
	if ( !bTERMINATE )
	{
		GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
		NetItemUpdate.wPosX = pNetMsg->wPosX;
		NetItemUpdate.wPosY = pNetMsg->wPosY;
		NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetItemUpdate);
	}
	else
	{
		// 만약 코스튬이 발라져 있으면 코스튬을 분리하고 삭제한다.
		if ( pInvenItem->sItemCustom.nidDISGUISE!=SNATIVEID(false) )
		{
			GLITEMLMT & glItemmt = GLITEMLMT::GetInstance();			
			GLItemMan & glItemMan = GLItemMan::GetInstance();

			SITEM *pONE = glItemMan.GetItem ( pInvenItem->sItemCustom.nidDISGUISE );
			if ( !pONE )		return E_FAIL;

			SITEMCUSTOM sITEM_NEW;
			sITEM_NEW.sNativeID = pInvenItem->sItemCustom.nidDISGUISE;
			CTime cTIME = CTime::GetCurrentTime();
			if ( pInvenItem->sItemCustom.tDISGUISE!=0 && pONE->sDrugOp.tTIME_LMT!= 0 )
			{
				cTIME = CTime(pInvenItem->sItemCustom.tDISGUISE);

				CTimeSpan tLMT(pONE->sDrugOp.tTIME_LMT);
				cTIME -= tLMT;
			}

			//	Note : 아이템 생성.
			//
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.wTurnNum = 1;
			sITEM_NEW.cGenType = EMGEN_BILLING;
			sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
			sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
			sITEM_NEW.lnGenNum = glItemmt.RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

			//	Note : hold 아이템 인벤에 되돌림.
			WORD wInsertPosX, wInsertPosY;
			BOOL bOk = m_cInventory.FindInsrtable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
			if ( !bOk )
			{
				// 생성된 코스툼은 바닥에서 다시 떨군다.
				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				m_pLandMan->DropItem ( m_vPos, &(cDropItem.sItemCustom), EMGROUP_ONE, m_dwGaeaID );

				//	Note : 분리된 코스툼 로그.
				glItemmt.ReqItemRoute ( sITEM_NEW, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_GROUND, sITEM_NEW.wTurnNum );
			}else{
				//	들고있던 아이탬 인밴에 넣음.
				m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
				SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

				//	[자신에게] 메시지 발생.
				GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
				NetMsg_Inven.Data = *pINSERT_ITEM;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);
			}			
		}

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, pInvenItem->sItemCustom.wTurnNum );

		//	[자신에게] 인밴에 아이탬 제거.
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
		NetMsg_Inven_Delete.wPosX = pNetMsg->wPosX;
		NetMsg_Inven_Delete.wPosY = pNetMsg->wPosY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Delete);

		//	아이탬 제거.
		m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenResetSkSt ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_RESET_SKST *pNetMsg = (GLMSG::SNET_INVEN_RESET_SKST *)nmg;

	GLMSG::SNET_INVEN_RESET_SKST_FB	MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;



	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_SKP_STAT_RESET )
	{
		MsgFB.emFB = EMREQ_RESET_SKST_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// 레벨 조건 확인
	if ( !SIMPLE_CHECK_ITEM( pITEM->sBasicOp.sNativeID ) )
	{		
		MsgFB.emFB = EMREQ_RESET_SKST_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);		
		return E_FAIL;
	}

	enum
	{
		EMGRADE				= 31,
		EMMARBLE_MID		= 104,

		EMGRADE_MIN			= 4,
	};

	WORD wNUM(0);
	WORD wGRADE[EMGRADE];
	memset(wGRADE,0,sizeof(WORD)*EMGRADE);

	SKILL_MAP_ITER pos = m_ExpSkills.begin();
	SKILL_MAP_ITER end = m_ExpSkills.end();
	for ( ; pos!=end; ++pos )
	{
		SNATIVEID sNID ( (*pos).first );

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( sNID );
		if ( !pSKILL )									continue;
		if ( EMGRADE_MIN > pSKILL->m_sBASIC.dwGRADE )	continue;

		wGRADE[pSKILL->m_sBASIC.dwGRADE] += 1;
		++wNUM;
	}

	BOOL bINSERT(TRUE);
	GLInventory cInven;
	cInven.Assign ( m_cInventory );
	for ( int i=EMGRADE_MIN; i<EMGRADE; ++i )
	for ( int j=0; j<wGRADE[i]; ++j )
	{
		SITEM *pITEM_SKILL = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
		if ( !pITEM_SKILL )		continue;

		SITEMCUSTOM sITEM_NEW;
		sITEM_NEW.sNativeID = SNATIVEID(EMMARBLE_MID,i);

		//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
		sITEM_NEW.wTurnNum = pITEM_SKILL->GETAPPLYNUM();

		BOOL bOK = cInven.InsertItem ( sITEM_NEW );
		if ( !bOK )
		{
			bINSERT = FALSE;
			break;
		}
	}

	if ( !bINSERT )
	{
		MsgFB.wITEM_NUM = wNUM;
		MsgFB.emFB = EMREQ_RESET_SKST_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	for ( int i=EMGRADE_MIN; i<EMGRADE; ++i )
	for ( int j=0; j<wGRADE[i]; ++j )
	{
		SITEM *pITEM_SKILL = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
		if ( !pITEM_SKILL )		continue;

		SITEMCUSTOM sITEM_NEW;
		CTime cTIME = CTime::GetCurrentTime();

		//	아이템 발생.
		sITEM_NEW.sNativeID = SNATIVEID(EMMARBLE_MID,i);
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
		sITEM_NEW.wTurnNum = pITEM_SKILL->GETAPPLYNUM();

		//	발생된 정보 등록.
		sITEM_NEW.cGenType = EMGEN_INIT;
		sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
		sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
		sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, EMGEN_INIT );

		BOOL bOK;
		WORD wPosX(0), wPosY(0);
		bOK = m_cInventory.FindInsrtable ( pITEM_SKILL->sBasicOp.wInvenSizeX, pITEM_SKILL->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOK )		continue;

		bOK = m_cInventory.InsertItem ( sITEM_NEW, wPosX, wPosY );
		if ( !bOK )		continue;

		SINVENITEM *pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );
		if (!pInvenItem) continue;

		//	아이템 획득 로그 남김.
		GLITEMLMT::GetInstance().ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pInvenItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

	}

	//	Note : 스킬, 스텟 리셋.
	//
	RESET_STATS_SKILL ( pITEM->sDrugOp.wCureVolume );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 리셋 메시지 보냄.
	//
	MsgFB.emFB = EMREQ_RESET_SKST_FB_OK;
	MsgFB.wITEM_NUM = wNUM;
	MsgFB.wSTATS_POINT = pITEM->sDrugOp.wCureVolume;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}


HRESULT GLChar::MsgReqCharCard ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_CHARCARD *pNetMsg = (GLMSG::SNET_INVEN_CHARCARD *)nmg;

	GLMSG::SNET_INVEN_CHARCARD_FB	MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_CHARCARD_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_CHARACTER_CARD )
	{
		MsgFB.emFB = EMREQ_CHARCARD_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 케릭터 생성 슬롯 증가.
	//
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CItemChaCreateNumIncrease *pDbAction = new CItemChaCreateNumIncrease ( m_dwUserID );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 처리 성공 알림.
	MsgFB.emFB = EMREQ_CHARCARD_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqStorageCard ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_STORAGECARD *pNetMsg = (GLMSG::SNET_INVEN_STORAGECARD *)nmg;

	GLMSG::SNET_INVEN_STORAGECARD_FB	MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_STORAGE_CARD )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( pNetMsg->wSTORAGE < EMSTORAGE_CHANNEL_SPAN || pNetMsg->wSTORAGE>=(EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE) )
	{
		MsgFB.emFB = EMREQ_STORAGECARD_FB_INVNUM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 창고 제한 시간 변경.
	//
	int nINDEX = pNetMsg->wSTORAGE-EMSTORAGE_CHANNEL_SPAN;
	__time64_t &tTIME = m_tSTORAGE[nINDEX];

	CTime tCurTime = CTime::GetCurrentTime();
	CTime tSeedTime(tTIME);
	CTimeSpan tLMT (pITEM->sDrugOp.tTIME_LMT);
	CTimeSpan tADD(0,1,30,0);

	//	Note : 아직 초기화 되지 않았거나, 충전시간 초과되었을 경우.
	if ( tTIME == 0 || tSeedTime < tCurTime )
	{
		tSeedTime = tCurTime + tLMT + tADD;
	}
	//	Note : 충전시간에 아직 여유가 있을때.
	else
	{
		tSeedTime = tSeedTime + tLMT + tADD;
	}

	//	Note : 사용시간 충전됨, 사용 가능으로 변경.
	tTIME = tSeedTime.GetTime();	//	m_tSTORAGE[nINDEX] = time;
	m_bSTORAGE[nINDEX] = true;

	//	Note : DB에 스토리지 충전시간 저장.
	//
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CSetChaStorageDate *pDbAction = new CSetChaStorageDate ( m_dwUserID, nINDEX+EMSTORAGE_CHANNEL_SPAN, tTIME );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 처리 결과 알림.
	CTimeSpan tDX = tSeedTime - tCurTime;
	MsgFB.emFB = EMREQ_STORAGECARD_FB_OK;
	MsgFB.tSTORAGE_LIMIT = tSeedTime.GetTime();
	MsgFB.tSPAN = tDX.GetTimeSpan();
	MsgFB.wSTORAGE = pNetMsg->wSTORAGE;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenLine ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_INVENLINE *pNetMsg = (GLMSG::SNET_INVEN_INVENLINE *)nmg;

	GLMSG::SNET_INVEN_INVENLINE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_INVENLINE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_INVEN_CARD )
	{
		MsgFB.emFB = EMREQ_INVENLINE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( m_wINVENLINE >= (EM_INVENSIZE_Y - EM_INVEN_DEF_SIZE_Y - EM_INVEN_PREMIUM_SIZE) )
	{
		MsgFB.emFB = EMREQ_INVENLINE_FB_MAXLINE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 인벤 라인 증가.
	//
	m_wINVENLINE += 1;

	//	Note : 현제 활성화된 인벤 라인 설정.
	//
	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );

	//	Note : DB에 저장.
	//
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CSetChaInvenNum *pDbAction = new CSetChaInvenNum ( m_dwCharID, m_wINVENLINE );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	//	[자신에게] 인밴 아이템 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 처리 결과 알림.
	MsgFB.emFB = EMREQ_INVENLINE_FB_OK;
	MsgFB.wINVENLINE = m_wINVENLINE;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

// *****************************************************
// Desc: 개조 기능 이용카드 요청 ( 카드만 소모시킴 )
// *****************************************************
HRESULT GLChar::MsgReqInvenRemodelOpen ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_REMODELOPEN *pNetMsg = (GLMSG::SNET_INVEN_REMODELOPEN *)nmg;

	GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_REMODEL )
	{
		MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	m_ItemRemodelPosX = pNetMsg->wPosX;
	m_ItemRemodelPosY = pNetMsg->wPosY;

	MsgFB.emFB = EMREQ_REMODELOPEN_FB_OK;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}


HRESULT GLChar::MsgReqInvenGarbageOpen ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_GARBAGEOPEN *pNetMsg = (GLMSG::SNET_INVEN_GARBAGEOPEN *)nmg;

	GLMSG::SNET_INVEN_GARBAGEOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_GARBAGE_CARD )
	{
		MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	m_ItemGarbagePosX = pNetMsg->wPosX;
	m_ItemGarbagePosY = pNetMsg->wPosY;

	MsgFB.emFB = EMREQ_GARBAGEOPEN_FB_OK;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	return S_OK;
}

// *****************************************************
// Desc: 긴급 창고 이용카드 요청 ( 카드만 소모시킴 )
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageOpen ( NET_MSG_GENERIC* nmg )
{
	// 창고연결카드를 사용중이면
	if ( m_bUsedStorageCard ) return E_FAIL;

	GLMSG::SNET_INVEN_STORAGEOPEN *pNetMsg = (GLMSG::SNET_INVEN_STORAGEOPEN *)nmg;

	GLMSG::SNET_INVEN_STORAGEOPEN_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_STORAGEOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_STORAGE_CONNECT )
	{
		MsgFB.emFB = EMREQ_STORAGEOPEN_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	[자신에게] 인밴 아이템 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	MsgFB.emFB = EMREQ_STORAGEOPEN_FB_OK;
	MsgFB.wPosX = pNetMsg->wPosX;
	MsgFB.wPosY = pNetMsg->wPosY;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	// 창고연결카드 사용여부
	m_bUsedStorageCard = true;

	return S_OK;
}

// *****************************************************
// Desc: 긴급 창고 사용 완료
// *****************************************************
HRESULT GLChar::MsgReqInvenStorageClose ( NET_MSG_GENERIC* nmg )
{
	// 창고연결카드 사용여부
	m_bUsedStorageCard = false;

	return S_OK;
}

HRESULT GLChar::MsgReqInvenPremiumSet ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_PREMIUMSET *pNetMsg = (GLMSG::SNET_INVEN_PREMIUMSET *)nmg;

	GLMSG::SNET_INVEN_PREMIUMSET_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_PREMIUMSET )
	{
		MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 인벤의 여유 공간 측정.
	//
	GLInventory cInvenTemp;
	cInvenTemp.SetAddLine ( m_cInventory.GETAddLine(), true );
	cInvenTemp.Assign ( m_cInventory );

	for ( int i=0; i<ITEM::SBOX::ITEM_SIZE; ++i )
	{
		SITEMCUSTOM sCUSTOM;
		sCUSTOM.sNativeID = pITEM->sBox.sITEMS[i].nidITEM;
		if ( sCUSTOM.sNativeID==SNATIVEID(false) )				continue;

		SITEM *pONE = GLItemMan::GetInstance().GetItem ( sCUSTOM.sNativeID );
		if ( !pONE )
		{
			MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}

		BOOL bOK = cInvenTemp.InsertItem ( sCUSTOM );
		if ( !bOK )
		{
			//	Note : 인벤에 공간이 없는 것으로 판단됨.
			//
			MsgFB.emFB = EMREQ_PREMIUMSET_FB_NOTINVEN;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}
	}

	//	Note : 상자안에 있는 아이템을 꺼내어서 넣는다.
	//
	for ( int i=0; i<ITEM::SBOX::ITEM_SIZE; ++i )
	{
		SITEMCUSTOM sITEM_NEW;
		sITEM_NEW.sNativeID = pITEM->sBox.sITEMS[i].nidITEM;
		DWORD dwAMOUNT = pITEM->sBox.sITEMS[i].dwAMOUNT;
		if ( sITEM_NEW.sNativeID==SNATIVEID(false) )				continue;

		SITEM *pONE = GLItemMan::GetInstance().GetItem ( sITEM_NEW.sNativeID );
		if ( !pONE )											return E_FAIL;

		//	Note : 아이템 생성.
		//
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();

		sITEM_NEW.wTurnNum = (WORD) dwAMOUNT;
		sITEM_NEW.cGenType = pINVENITEM->sItemCustom.cGenType;
		sITEM_NEW.cChnID = (BYTE)GLGaeaServer::GetInstance().GetServerChannel();
		sITEM_NEW.cFieldID = (BYTE)GLGaeaServer::GetInstance().GetFieldSvrID();
		sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

		// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 )
		sITEM_NEW.cDAMAGE = (BYTE)pONE->sBasicOp.wGradeAttack;
		sITEM_NEW.cDEFENSE = (BYTE)pONE->sBasicOp.wGradeDefense;

		//	랜덤 옵션 생성.
		if( sITEM_NEW.GENERATE_RANDOM_OPT() )
		{
			GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
		}

		//	Note :.
		WORD wInsertPosX, wInsertPosY;
		BOOL bOk = m_cInventory.FindInsrtable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
		if ( !bOk )		return S_OK;	//	인벤 공간 부족으로 되돌림 실패.

		//	.
		m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
		SINVENITEM *pInvenItem = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );
		if ( !pInvenItem )	return E_FAIL;

		//	Note : 로그 남김.
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum );

		//	[자신에게] 메시지 발생.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pInvenItem;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);
	}

	//	Note : 프리미엄 설정.
	//
	CTime tPREMIUM(m_tPREMIUM);
	CTime tCurTime = CTime::GetCurrentTime();
	CTimeSpan tLMT(pITEM->sDrugOp.tTIME_LMT);
	CTimeSpan tADD(0,1,30,0);

	if ( m_tPREMIUM==0 || tPREMIUM < tCurTime )
	{
		tPREMIUM = tCurTime + tLMT + tADD;
	}
	else
	{
		tPREMIUM = tPREMIUM + tLMT + tADD;
	}

	//	Note : 프리미엄 모드 ON.
	//
	m_tPREMIUM = tPREMIUM.GetTime();
	m_bPREMIUM = true;

	//	Note : 현제 활성화된 인벤 라인 설정.
	//
	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );

	//	Note : DB에 저장.
	//
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CSetPremiumTime *pDbAction = new CSetPremiumTime ( m_dwUserID, m_tPREMIUM );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	// Note : CSetPremiumTime 클래스에서 모든 경우를 처리하도록 변경됨
	/*
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		CDaumSetPremiumTime *pDbAction = new CDaumSetPremiumTime ( m_dwUserID, m_tPREMIUM );
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}
	*/
	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );


	//	Note : 에이젼트로 거쳐서 메시지 전송.
	//		( 에이젼트에 기록되어 있는 프리미엄 기간을 갱신 시켜줘야 한다. )
	//
	CTimeSpan tDX = tPREMIUM - tCurTime;
	MsgFB.emFB = EMREQ_PREMIUMSET_FB_OK;
	MsgFB.tLMT = m_tPREMIUM;
	MsgFB.tSPAN = tDX.GetTimeSpan();
	GLGaeaServer::GetInstance().SENDTOAGENT(m_dwClientID,&MsgFB);

	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIR_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_HAIR_CHANGE *)nmg;

	GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_HAIR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	bool bVALIDCLASS = (NULL!=(pITEM->sBasicOp.dwReqCharClass&m_emClass));
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADCLASS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	WORD wHAIR = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHAIRNUM<=wHAIR || MAX_HAIR<=wHAIR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( sCHARCONST.strHAIR_CPS[wHAIR].empty() )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 헤어를 변화 시킴.
	//
	m_wHair = wHAIR;

	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	Note : 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairStyleChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIRSTYLE_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_HAIRSTYLE_CHANGE *)nmg;

	GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	
	if ( pITEM->sBasicOp.emItemType!=ITEM_HAIR_STYLE )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}


	bool bVALIDCLASS = (NULL!=(pITEM->sBasicOp.dwReqCharClass&m_emClass));
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADCLASS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//WORD wHAIR = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHAIRNUM<=pNetMsg->wHairStyle || MAX_HAIR<=pNetMsg->wHairStyle )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	m_wHair = pNetMsg->wHairStyle;

	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wHair;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	Note : 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wHair;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
	CSetChaHairStyle* pDBAction = new CSetChaHairStyle ( m_dwCharID, m_wHair );

	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}


	return S_OK;
}

HRESULT GLChar::MsgReqInvenHairColorChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE *)nmg;

	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );

	if ( pITEM->sBasicOp.emItemType!=ITEM_HAIR_COLOR )
	{
		MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 헤어를 변화 시킴.
	//
	m_wHairColor = pNetMsg->wHairColor;

	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.wHairColor = m_wHairColor;
	MsgFB.emFB = EMINVEN_HAIR_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	Note : 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.wHairColor = m_wHairColor;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
	CSetChaHairColor* pDBAction = new CSetChaHairColor ( m_dwCharID, m_wHairColor );

	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenFaceChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_FACE_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_FACE_CHANGE *)nmg;

	GLMSG::SNETPC_INVEN_FACE_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);		
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_FACE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	bool bVALIDCLASS = (NULL!=(pITEM->sBasicOp.dwReqCharClass&m_emClass));
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADCLASS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	WORD wFACE = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHEADNUM<=wFACE || MAX_HEAD<=wFACE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( sCHARCONST.strHEAD_CPS[wFACE].empty() )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//	Note : 얼굴을 변화 시킴.
	//
	m_wFace = wFACE;

	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	Note : 얼굴 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqInvenFaceStyleChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_FACESTYLE_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_FACESTYLE_CHANGE *)nmg;

	GLMSG::SNETPC_INVEN_FACE_CHANGE_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{		
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	
	if ( pITEM->sBasicOp.emItemType!=ITEM_FACE_STYLE )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}


	bool bVALIDCLASS = (NULL!=(pITEM->sBasicOp.dwReqCharClass&m_emClass));
	if ( !bVALIDCLASS )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADCLASS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	//WORD wHAIR = pITEM->sSuitOp.wReModelNum;

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];
	if ( sCHARCONST.dwHEADNUM<=pNetMsg->wFaceStyle || MAX_HEAD<=pNetMsg->wFaceStyle )
	{
		MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	m_wFace = pNetMsg->wFaceStyle;

	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.dwID = m_wFace;
	MsgFB.emFB = EMINVEN_FACE_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	Note : 헤어 변경을 다른 사람들에게 알림.
	GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	NetMsgBrd.dwID = m_wFace;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
	CSetChaFaceStyle* pDBAction = new CSetChaFaceStyle ( m_dwCharID, m_wFace );

	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}


	return S_OK;

}


HRESULT GLChar::MsgReqInvenGenderChange( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_GENDER_CHANGE *pNetMsg = (GLMSG::SNETPC_INVEN_GENDER_CHANGE *)nmg;
	GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB MsgFB;


	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	
	if ( pITEM->sBasicOp.emItemType!=ITEM_GENDER_CHANGE )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_ITEMTYPE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}


	if ( m_emClass == GLCC_EXTREME_M || m_emClass == GLCC_EXTREME_W )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_BADCLASS;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	EMCHARCLASS emClass;
    	
	if ( m_emClass > GLCC_EXTREME_M )	emClass = (EMCHARCLASS) (m_emClass / 64 );
	else emClass = (EMCHARCLASS) (m_emClass * 64);


	EMCHARINDEX emIndex = CharClassToIndex(emClass);
	const GLCONST_CHARCLASS& sCHARCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( pNetMsg->wFace >= sCHARCONST.dwHEADNUM || pNetMsg->wHair >= sCHARCONST.dwHAIRNUM_SELECT )
	{
		MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_NOTVALUE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

    m_emClass = emClass;
	m_wHair = pNetMsg->wHair;
	m_wFace = pNetMsg->wFace;	
	m_wHairColor = HAIRCOLOR::GetHairColor( emIndex, m_wHair );

	m_wSex = m_wSex ? 0 : 1;


	//	Note : 클라이언트에 처리 결과 전송.
	MsgFB.emClass	= m_emClass;
	MsgFB.wSex		= m_wSex;
	MsgFB.wFace = m_wFace;
	MsgFB.wHair = m_wHair;
	MsgFB.wHairColor = m_wHairColor;

	MsgFB.emFB = EMINVEN_GENDER_CHANGE_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// DB에 저장
	CSetChaGenderChange* pDBAction = new CSetChaGenderChange ( m_dwCharID, m_emClass, m_wSex, m_wFace, m_wHair, m_wHairColor );

	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}

	return S_OK;
}

HRESULT GLChar::MsgReqInvenChargedItem ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNET_CHARGED_ITEM_GET *pNetMsg = (GLMSG::SNET_CHARGED_ITEM_GET *) nmg;

	GLMSG::SNET_CHARGED_ITEM_GET_FB NetMsgFB;

	MAPSHOP_ITER iter = m_mapCharged.find ( pNetMsg->szPurKey );
	if ( iter==m_mapCharged.end() )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SHOPPURCHASE &sPURCHASE = (*iter).second;
	
	SNATIVEID nidITEM(sPURCHASE.wItemMain,sPURCHASE.wItemSub);
	
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	WORD wPosX, wPosY;
	BOOL bOK = m_cInventory.FindInsrtable ( pITEM->sBasicOp.wInvenSizeX, pITEM->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOK )
	{
		NetMsgFB.emFB = EMCHARGED_ITEM_GET_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	Note : 아이템 가져옴. DB Action.
	//
	CPurchaseItem_Get *pDbAction = new CPurchaseItem_Get(m_dwClientID,
		                                                 m_dwUserID,
														 pNetMsg->szPurKey,
														 nidITEM,pNetMsg->dwID);
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	return S_OK;
}

// *****************************************************
// Desc: 이름변경요청 처리
// *****************************************************
HRESULT GLChar::MsgReqInvenRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME*) nmg;

	GLMSG::SNETPC_INVEN_RENAME_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_RENAME )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}
	
	// 캐릭터명
	CString strChaName(pNetMsg->szName);
	strChaName.Trim(_T(" ")); // 앞뒤 공백제거

	// 스페이스 찾기, 캐릭터이름 4글자 이하 에러, 캐릭터 만들 수 없음.
	if ((strChaName.FindOneOf(" ") != -1) || (strChaName.GetLength() < 4)) 
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_LENGTH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strChaName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strChaName );
	if ( bFILTER0 || bFILTER1 )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_BAD;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &MsgFB );
		return S_FALSE;			
	}

#ifdef TH_PARAM	
	// 태국어 문자 조합 체크 
	if ( !m_pCheckString ) return E_FAIL;

	if ( !m_pCheckString(strChaName) )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_THAICHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}
#endif

#ifdef VN_PARAM
	// 베트남 문자 조합 체크 
	if( STRUTIL::CheckVietnamString( strChaName ) )
	{
		MsgFB.emFB = EMINVEN_RENAME_FB_VNCHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

#endif 

	// DB에 저장요청
	CRenameCharacter* pDBAction = new CRenameCharacter(m_dwClientID, 
		                                               m_dwCharID,
													   pNetMsg->szName,
													   pNetMsg->wPosX,
													   pNetMsg->wPosY);
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}

	return S_OK;
}

// *****************************************************
// Desc: 이름변경 결과 처리
// *****************************************************
HRESULT GLChar::MsgInvenRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME_FROM_DB *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_FROM_DB *)nmg;
	
	// 클라이언트에 처리 결과 전송.
	GLMSG::SNETPC_INVEN_RENAME_FB MsgFB;

	if( pNetMsg->emFB == EMINVEN_RENAME_FROM_DB_BAD )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_BAD;
		StringCchCopy ( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return S_FALSE;
	}
	else if ( pNetMsg->emFB == EMINVEN_RENAME_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMINVEN_RENAME_FROM_DB_FAIL;
		StringCchCopy ( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return S_FALSE;
	}

	StringCchCopy ( MsgFB.szName, CHAR_SZNAME, pNetMsg->szName );
	MsgFB.emFB = EMINVEN_RENAME_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	// 주변에 이름변경을 알림.
	GLMSG::SNETPC_INVEN_RENAME_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgBrd.szName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	// GLLandMan 이름맵 변경.
	GLMSG::SNETPC_CHANGE_NAMEMAP NetMsgNameMap;
	NetMsgNameMap.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgNameMap.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgNameMap.szNewName, CHAR_SZNAME, pNetMsg->szName );
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgNameMap );

	// 이름변경을 친구와 클럽 맴버들에게 알림
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD NetMsgAgt;
	NetMsgAgt.dwID = m_dwGaeaID;
	StringCchCopy ( NetMsgAgt.szOldName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgAgt.szNewName, CHAR_SZNAME, pNetMsg->szName );
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );

	GLGaeaServer::GetInstance().ChangeNameMap ( this, m_szName, pNetMsg->szName );

	//	[자신에게] 인밴에 아이탬 제거.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	return S_OK;
}

HRESULT GLChar::MsgReqSendSMS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SEND_SMS *pNetMsg = (GLMSG::SNETPC_SEND_SMS*) nmg;

	GLMSG::SNETPC_SEND_SMS_FB MsgFB;

	SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wItemPosX, pNetMsg->wItemPosY );
	if ( !pINVENITEM )
	{
		MsgFB.emFB = EMSMS_SEND_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType!=ITEM_SMS )
	{
		MsgFB.emFB = EMSMS_SEND_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return E_FAIL;
	}

	// DB에 저장요청
	CSendSMS* pDBAction = new CSendSMS( m_dwClientID,
										m_dwCharID,
										pNetMsg->dwReceiveChaNum,
										pNetMsg->szPhoneNumber,
										m_szName,
										pNetMsg->szSmsMsg,
										pNetMsg->wItemPosX,
										pNetMsg->wItemPosY );

	if( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob( pDBAction );
	}

	return S_OK;
}

// *****************************************************
// Desc: SMS 전송 결과 처리
// *****************************************************
HRESULT GLChar::MsgSendSMS( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_SEND_SMS_FROM_DB *pNetMsg = (GLMSG::SNETPC_SEND_SMS_FROM_DB *)nmg;

	// 클라이언트에 처리 결과 전송.
	GLMSG::SNETPC_SEND_SMS_FB MsgFB;

	if ( pNetMsg->emFB == EMSMS_SEND_FROM_DB_FAIL )
	{
		MsgFB.emFB = EMSMS_SEND_FROM_DB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
		return S_FALSE;
	}

	MsgFB.emFB = EMSMS_SEND_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	return TRUE;
}

HRESULT GLChar::MsgLoudSpeaker ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_CHAT_LOUDSPEAKER *pNetMsg = (GLMSG::SNETPC_CHAT_LOUDSPEAKER *) nmg;

	GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB	NetMsgFB;

	bool bchannel_all(false);

	if ( m_dwUserLvl < USER_GM3 )
	{
		SINVENITEM *pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !pINVENITEM )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return S_OK;
		}

		if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
		if ( !pITEM )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return S_OK;
		}

		if ( pITEM->sBasicOp.emItemType!=ITEM_LOUDSPEAKER )
		{
			NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return S_OK;
		}

		if ( IsCHATBLOCK() )	return S_FALSE;

		bchannel_all = pITEM->sBasicOp.IsCHANNEL_ALL();

		if ( m_sPMarket.IsOpen() ) 
		{
			DWORD dwTurn = m_cInventory.CountTurnItem( pINVENITEM->sItemCustom.sNativeID );
			DWORD dwMarketTurn = m_sPMarket.GetItemTurnNum( pINVENITEM->sItemCustom.sNativeID ) ;

			if ( dwTurn <= dwMarketTurn ) 
			{
				NetMsgFB.emFB = EMCHAT_LOUDSPEAKER_NOITEM;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
				return S_OK;
			}
		}

		//	[자신에게] 인밴 아이탬 소모시킴.
		DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	}

	//	Note : 확성기 사용.
	//
	GLMSG::SNETPC_CHAT_LOUDSPEAKER_AGT NetMsgAgt;
	StringCbCopy( NetMsgAgt.szMSG, CHAT_MSG_SIZE+1, pNetMsg->szMSG);
	NetMsgAgt.bchannel_all = bchannel_all;
	GLGaeaServer::GetInstance().SENDTOAGENT ( m_dwClientID, &NetMsgAgt );

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildResult( NET_MSG_GENERIC* nmg )	// ITEMREBUILD_MARK
{
	GLMSG::SNET_REBUILD_RESULT* pNetMsg = (GLMSG::SNET_REBUILD_RESULT*)nmg;

	switch( pNetMsg->emResult )
	{
	case EMREBUILD_RESULT_OPEN:		// 개조 인터페이스 열기
		{
			InitRebuildData();
			OpenRebuild();
		}
		break;

	case EMREBUILD_RESULT_SUCCESS:	// 클라이언트에서 개조 시도 요청
		{
			// 금액이 맞지 않아서 개조를 진행할 수 없다
			if( m_i64RebuildInput != m_i64RebuildCost || m_lnMoney < m_i64RebuildCost )
			{
				pNetMsg->emResult = EMREBUILD_RESULT_MONEY;
				GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, pNetMsg );
				return S_OK;
			}

			// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
			SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
			SITEM* pItem = GLItemMan::GetInstance().GetItem( sRebuildItem.sNativeID );
			if( !pItem )		return E_FAIL;

			// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
			SRANDOM_GEN* pRANDOM_SET = GLItemMan::GetInstance().GetItemRandomOpt( sRebuildItem.sNativeID );
			if( !pRANDOM_SET )	return E_FAIL;

			// 개조카드를 사용해서 시도할 경우 카드를 체크하고 소모 시킨다.
			if( m_ItemRemodelPosX < EM_INVENSIZE_X && m_ItemRemodelPosY < EM_INVENSIZE_Y )
			{
				GLMSG::SNET_INVEN_REMODELOPEN_FB MsgFB;

				SINVENITEM* pINVENITEM = m_cInventory.GetItem ( m_ItemRemodelPosX, m_ItemRemodelPosY );
				if ( !pINVENITEM )
				{
					MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
					return E_FAIL;
				}

				if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

				SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
				if ( pITEM->sBasicOp.emItemType!=ITEM_REMODEL )
				{
					MsgFB.emFB = EMREQ_REMODELOPEN_FB_NOITEM;
					GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
					return E_FAIL;
				}

				//	[자신에게] 인밴 아이템 소모시킴.
				DoDrugInvenItem ( m_ItemRemodelPosX, m_ItemRemodelPosY );
			}
			else // 개조 NPC를 이용할 경우 NPC를 체크한다.
			{
				PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
				if ( !pCrow )	
					return E_FAIL;

				float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
				float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
				float fTalkableDis = fTalkRange + 20;

				if ( fDist>fTalkableDis )
					return E_FAIL;
			}

			// 가진 돈에서 개조 금액을 빼고 클라이언트에 전송한다
			{
				CheckMoneyUpdate( m_lnMoney, m_i64RebuildCost, FALSE, "Rebuild Cost." );
				m_bMoneyUpdate = TRUE;

				m_lnMoney -= m_i64RebuildCost;

				GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
				NetMsgMoney.lnMoney = m_lnMoney;
				GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgMoney );

				// 개조 금액 로그 저장
				GLITEMLMT::GetInstance().ReqMoneyExc( ID_CHAR, m_dwCharID, ID_CHAR, 0, -m_i64RebuildCost, EMITEM_ROUTE_NPCREMAKE );
				GLITEMLMT::GetInstance().ReqMoneyExc( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
			}

			float fDestroyRate = seqrandom::getpercent();
			if( fDestroyRate < pRANDOM_SET->fD_point )
			{
				// 아이템 삭제 로그 남김
				GLITEMLMT::GetInstance().ReqItemRoute( sRebuildItem, ID_CHAR, m_dwCharID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_DELETE, sRebuildItem.wTurnNum );

				// 인벤토리에서 아이템 제거
				m_cInventory.DeleteItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );

				// 개조 실패 했으므로 아이템을 제거해야한다
				GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
				NetMsg_Inven_Delete.wPosX = m_sRebuildItem.wPosX;
				NetMsg_Inven_Delete.wPosY = m_sRebuildItem.wPosY;
				GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg_Inven_Delete );

				// 개조 실패로 아이템이 소멸되었다
				pNetMsg->emResult = EMREBUILD_RESULT_DESTROY;
				GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, pNetMsg );
			}
			else
			{
				SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
				if( pInvenItem )
				{
					// 랜덤 옵션을 재조정한다
					pInvenItem->sItemCustom.GENERATE_RANDOM_OPT( false );

					// 변경된 랜덤옵션의 로그를 저장한다.
					GLITEMLMT::GetInstance().ReqRandomItem( sRebuildItem );

					// 개조 되었으므로 아이템 정보를 전송해야한다
					GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
					NetItemUpdate.wPosX = m_sRebuildItem.wPosX;
					NetItemUpdate.wPosY = m_sRebuildItem.wPosY;
					NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
					GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetItemUpdate );

					// 개조 성공했다
					pNetMsg->emResult = EMREBUILD_RESULT_SUCCESS;
					GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, pNetMsg );
				}
			}

			InitRebuildData();
		}
		break;

	case EMREBUILD_RESULT_FAIL:		// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_DESTROY:	// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_MONEY:	// 이런 패킷은 올 수 없다
	case EMREBUILD_RESULT_CLOSE:	// 개조 인터페이스 닫기
		{
			InitRebuildData();
			CloseRebuild();

			// 개조카드 아이템의 위치를 초기화 시킨다.
			m_ItemRemodelPosX = EM_INVENSIZE_X;
			m_ItemRemodelPosY = EM_INVENSIZE_Y;
		}
		break;
	}

	return S_OK;
}

HRESULT GLChar::MsgReqGarbageResult( NET_MSG_GENERIC* nmg )	// ITEMREBUILD_MARK
{
	GLMSG::SNET_GARBAGE_RESULT* pNetMsg = (GLMSG::SNET_GARBAGE_RESULT*)nmg;

	if( m_ItemGarbagePosX < EM_INVENSIZE_X && m_ItemGarbagePosY < EM_INVENSIZE_Y )
	{
		GLMSG::SNET_GARBAGE_RESULT_FB MsgFB;
		
		SINVENITEM* pINVENITEM_DEL = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );

		if ( !pINVENITEM_DEL )
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}

		SITEM* pItem = GLItemMan::GetInstance().GetItem( pINVENITEM_DEL->sItemCustom.sNativeID );
		if( !pItem || !pItem->sBasicOp.IsGarbage() )		
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_ITEMTYPE;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}

		// 팻카드이면서 팻이 활성화 되어 있으면 삭제할수 없다.
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
			if ( pMyPet && pMyPet->IsValid () && pINVENITEM_DEL->sItemCustom.dwPetID == pMyPet->m_dwPetID ) 
			{
				MsgFB.emResult = EMGARBAGE_RESULT_FB_FAIL;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
				return E_FAIL;
			}
		}
		
		SINVENITEM* pINVENITEM = m_cInventory.GetItem ( m_ItemGarbagePosX, m_ItemGarbagePosY );
		if ( !pINVENITEM )
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}

		if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return E_FAIL;

		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
		if ( !pITEM || pITEM->sBasicOp.emItemType!=ITEM_GARBAGE_CARD )
		{
			MsgFB.emResult = EMGARBAGE_RESULT_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);
			return E_FAIL;
		}

		DoDrugInvenItem ( m_ItemGarbagePosX, m_ItemGarbagePosY );

		GLITEMLMT::GetInstance().ReqItemRoute( pINVENITEM_DEL->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_GARBAGE, pINVENITEM_DEL->sItemCustom.wTurnNum );

        
		{
			// 펫 카드 일경우
			if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pINVENITEM_DEL->sItemCustom.dwPetID != 0 )
			{
				PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );

				if ( pMyPet && pINVENITEM_DEL->sItemCustom.dwPetID == pMyPet->m_dwPetID ) 
				{
					m_pGLGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(pMyPet->m_dwGUID,true,false) );

						// 팻의 악세사리를 검사한다.
					pMyPet->UpdateTimeLmtItem ( this );

						// 넣어줄꺼 넣어준다.
					for ( WORD i = 0; i < ACCETYPESIZE; ++i )
					{
						CItemDrop cDropItem;
						cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
						if ( IsInsertToInven ( &cDropItem ) ) InsertToInven ( &cDropItem );
						else
						{
							if ( m_pLandMan )
							{
								m_pLandMan->DropItem ( m_vPos, 
														&(cDropItem.sItemCustom), 
														EMGROUP_ONE, 
														m_dwGaeaID );
							}
						}
					}

					CDeletePet* pDeletePet = new CDeletePet ( m_dwCharID, pINVENITEM_DEL->sItemCustom.dwPetID );
					GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pDeletePet );
					
					// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
					CGetRestorePetList *pGetRestorePetList = new CGetRestorePetList ( m_dwCharID, m_dwClientID );
					if ( pDBMan ) pDBMan->AddJob ( pGetRestorePetList );
				}
				else
				{
					// 악세사리를 체크후 팻을 DB에서 삭제해준다.
					GLPET* pNewPet = new GLPET ();
					CGetPet* pGetPet = new CGetPet ( pNewPet, 
														pINVENITEM_DEL->sItemCustom.dwPetID, 
														m_dwClientID, 
														m_dwCharID,
														pINVENITEM_DEL->wPosX,
														pINVENITEM_DEL->wPosY,
														true );
					GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pGetPet );
				}
			}
			// 탈것일 경우
			if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && pINVENITEM_DEL->sItemCustom.dwVehicleID != 0 )
			{
				// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
				GLVEHICLE* pNewVehicle = new GLVEHICLE();
				CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
												pINVENITEM_DEL->sItemCustom.dwVehicleID, 
												m_dwClientID,
												m_dwCharID, 
												true );
				GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
				if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );		
			}
		}

		// 인벤토리에서 아이템 제거
		m_cInventory.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );

		MsgFB.emResult = EMGARBAGE_RESULT_FB_OK;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&MsgFB);

		// 삭제한 아이템 제거
		GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;
		NetMsg_Inven_Delete.wPosX = pNetMsg->wPosX;
		NetMsg_Inven_Delete.wPosY = pNetMsg->wPosY;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg_Inven_Delete );	
		
		m_ItemGarbagePosX = EM_INVENSIZE_X;
		m_ItemGarbagePosY = EM_INVENSIZE_Y;

	}	

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildMoveItem( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_REBUILD_MOVE_ITEM* pNetMsg = (GLMSG::SNET_REBUILD_MOVE_ITEM*)nmg;
	GLMSG::SNET_REBUILD_MOVE_ITEM NetMsg;

	m_sRebuildItem.RESET();

	if( pNetMsg->wPosX != USHRT_MAX && pNetMsg->wPosY != USHRT_MAX )
	{
		SINVENITEM* pResistItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
		if( pResistItem )
		{
			// 장착 아이템이어야 바꿀 수 있다
			SITEM* pItem = GLItemMan::GetInstance().GetItem( pResistItem->sItemCustom.sNativeID );
			if( pItem && pItem->sBasicOp.emItemType == ITEM_SUIT )
			{
				// 랜덤옵션 파일이 지정되어 있어야 가능하다
				if( strlen( pItem->sRandomOpt.szNAME ) > 3 )
				{
					NetMsg.wPosX = pNetMsg->wPosX;
					NetMsg.wPosY = pNetMsg->wPosY;

					m_sRebuildItem.SET( NetMsg.wPosX, NetMsg.wPosY );
				}
			}
		}
	}
	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );

	// 개조 비용 전송
	MsgReqRebuildCostMoney();

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildCostMoney()
{
	GLMSG::SNET_REBUILD_COST_MONEY NetMsg;

	// 개조 아이템이 없으므로 0을 전송한다
	if( !m_sRebuildItem.VALID() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );
		return S_OK;
	}

	// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
	SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sRebuildItem.sNativeID );
	if( !pItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );
		return S_OK;
	}

	// 개조창에 아이템 올릴때 점검했으므로 이런 경우는 발생하지 말아야한다
	SRANDOM_GEN* pRANDOM_SET = GLItemMan::GetInstance().GetItemRandomOpt( sRebuildItem.sNativeID );
	if( !pRANDOM_SET )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );
		return S_OK;
	}

	m_i64RebuildCost = (LONGLONG)( pItem->sBasicOp.dwBuyPrice * pRANDOM_SET->fS_value );

	NetMsg.i64CostMoney = m_i64RebuildCost;

	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg );

	return S_OK;
}

HRESULT GLChar::MsgReqRebuildInputMoney( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_REBUILD_INPUT_MONEY* pNetMsg = (GLMSG::SNET_REBUILD_INPUT_MONEY*)nmg;

	m_i64RebuildInput = (LONGLONG)max( 0, pNetMsg->i64InputMoney );
	m_i64RebuildInput = (LONGLONG)min( m_i64RebuildInput, m_lnMoney );

	pNetMsg->i64InputMoney = m_i64RebuildInput;

	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, pNetMsg );

	return S_OK;
}

HRESULT	GLChar::MsgGiveFood ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GIVEFOOD* pNetMsg = ( GLMSG::SNETPET_REQ_GIVEFOOD* ) nmg;
	
	GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB;
	SNATIVEID sNativeID;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem ) 
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDCARD;
		return E_FAIL;
	}

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem ) 
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDCARD;
		return E_FAIL;
	}

	// 팻카드 여부 체크
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )	
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDCARD;
		return E_FAIL;
	}

	SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold ) 
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDFOOD;
		return E_FAIL;
	}

	if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	// 사료 여부 체크
	if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD )	
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDFOOD;
		return E_FAIL;
	}

	// 아이템 정보가 서로 틀리면 실행하지 않는다.
	SITEM* pPutOnItem = GLItemMan::GetInstance().GetItem ( m_PutOnItems[SLOT_HOLD].sNativeID );
	if( pHold != pPutOnItem )
	{
		NetMsgFB.emFB = EMPET_REQ_GIVEFOOD_FB_INVALIDFOOD;
		return E_FAIL;
	}

	sNativeID = pHold->sBasicOp.sNativeID;

	switch ( pHold->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		{
			PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );

			// Logic 에 팻이 있다면
			if ( pMyPet && pMyPet->m_dwPetID == pInvenItem->sItemCustom.dwPetID )
			{
				// 펫 사료먹기 이전 로그
				GLITEMLMT::GetInstance().ReqPetAction(	pMyPet->m_dwPetID, 
														pInvenItem->sItemCustom.sNativeID, 
														EMPET_ACTION_FOOD_BEFORE, 
														pMyPet->m_nFull );

				bool bOK = pMyPet->IncreaseFull ( pHold->sDrugOp.wCureVolume, pHold->sDrugOp.bRatio );
				if ( !bOK ) return E_FAIL;
				
				if ( pMyPet->IsValid () )	pMyPet->m_fTIMER = 0.0f;

				DoDrugSlotItem ( SLOT_HOLD );

				// 펫 사료먹기 이후 로그
				GLITEMLMT::GetInstance().ReqPetAction(	pMyPet->m_dwPetID, 
														pInvenItem->sItemCustom.sNativeID, 
														EMPET_ACTION_FOOD_AFTER, 
														pMyPet->m_nFull );

				// 팻에게 먹이를 준다.
				NetMsgFB.emFB	   = EMPET_REQ_GIVEFOOD_FB_OK;
				NetMsgFB.dwPetID   = pMyPet->m_dwPetID;
				NetMsgFB.sNativeID = sNativeID;
				NetMsgFB.nFull	   = pMyPet->m_nFull;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			}
			// 아직 Logic 에 팻이 없다면
			else
			{
				// 팻 생성여부 확인
				if (pInvenItem->sItemCustom.dwPetID <= 0 )
				{
					GLGaeaServer::GetInstance().GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, "Invalid ID, PetID %d dwClientID %d", 
						pInvenItem->sItemCustom.dwPetID, m_dwClientID );
					return E_FAIL;
				}

				// 팻의 포만감을 가져와서 갱신하고 다시 DB에 저장하는 코드
				CGetPetFull* pDbAction = new CGetPetFull ( m_dwCharID, 
														   pInvenItem->sItemCustom.dwPetID, 
														   m_dwClientID,
														   pHold->sDrugOp.wCureVolume, 
														   pHold->sDrugOp.bRatio,
														   sNativeID,
														   pNetMsg->emPetTYPE );

				GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
				if ( pDBMan ) pDBMan->AddJob ( pDbAction );
			}
		}
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT GLChar::MsgGetPetFullFromDB ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB* pIntMsg = ( GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB* ) nmg;
	switch ( pIntMsg->emFB )
	{
	case EMPET_REQ_GETFULL_FROMDB_FAIL:
		{
			GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB;
			NetMsgFB.emFB = EMPET_REQ_GETFULL_FROMDB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		}
		break;

	case EMPET_REQ_GETFULL_FROMDB_OK:
		{
			//	손에 든 아이템 확인한다.
			//	아이템이 바뀌는 경우가 있다.
			SITEM* pHold = GET_SLOT_ITEMDATA ( SLOT_HOLD );
			if ( !pHold ) 
			{
				GLGaeaServer::GetInstance().GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, 
					"Pet Give Full Error CharID : %d", m_dwCharID );
					
				return E_FAIL;
			}

			if (  pHold->sBasicOp.emItemType != ITEM_PET_FOOD || pHold->sBasicOp.sNativeID != pIntMsg->sNativeID ) 
			{
				GLGaeaServer::GetInstance().GetConsoleMsg()->Write ( LOG_TEXT_CONSOLE, 
					"Pet Give Full Error CharID : %d, Item %d/%d", m_dwCharID,
					pHold->sBasicOp.sNativeID.wMainID, pHold->sBasicOp.sNativeID.wSubID  );
					
				return E_FAIL;

			}

			// 펫 포만감 이전 로그
			GLITEMLMT::GetInstance().ReqPetAction(	pIntMsg->dwPetID, 
													pIntMsg->sNativeID,
													EMPET_ACTION_FOOD_BEFORE, 
													pIntMsg->nFull );

			int nFull = pIntMsg->nFull;
			int nMaxFull = GLCONST_PET::pGLPET[pIntMsg->emType]->m_nFull;
			if ( nFull >= nMaxFull ) break;

			if ( pIntMsg->bRatio )
			{
				nFull += ( nMaxFull*pIntMsg->wCureVolume )/100;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}
			else
			{
				nFull += pIntMsg->wCureVolume;
				if ( nFull > nMaxFull ) nFull = nMaxFull;
			}

			CSetPetFull* pDbAction = new CSetPetFull( m_dwCharID, pIntMsg->dwPetID, nFull );
			GLDBMan* pDbMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDbMan ) pDbMan->AddJob ( pDbAction );
			DoDrugSlotItem ( SLOT_HOLD );

			// 펫 포만감 이전 로그
			GLITEMLMT::GetInstance().ReqPetAction(	pIntMsg->dwPetID,
													pIntMsg->sNativeID,
													EMPET_ACTION_FOOD_AFTER,
													nFull );

			GLMSG::SNETPET_REQ_GIVEFOOD_FB NetMsgFB;
			NetMsgFB.emFB	   = EMPET_REQ_GIVEFOOD_FB_OK;
			NetMsgFB.dwPetID   = pIntMsg->dwPetID;
			NetMsgFB.sNativeID = pIntMsg->sNativeID;
			NetMsgFB.nFull	   = nFull;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
		}
	}

	return S_OK;
}

HRESULT GLChar::MsgReqMGameOddEvenEvent( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg = ( GLMSG::SNETPC_MGAME_ODDEVEN* )nmg;
	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;

	// 홀짝게임NPC 유효성 체크
	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
	if ( !pCrow )
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	// 홀짝게임NPC 거리 체크
	if ( fDist>fTalkableDis )
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	switch( pNetMsg->emEvent )
	{
	case EMMGAME_ODDEVEN_OK:		return MGOddEvenOK(pNetMsg);
	case EMMGAME_ODDEVEN_CANCEL:	return MGOddEvenCANCEL(pNetMsg);
	case EMMGAME_ODDEVEN_SELECT:	return MGOddEvenSelect(pNetMsg);
	case EMMGAME_ODDEVEN_AGAIN_OK:	return MGOddEvenAGAIN(pNetMsg);
	case EMMGAME_ODDEVEN_SHUFFLE:	return MGOddEvenShuffle(pNetMsg);
	case EMMGAME_ODDEVEN_FINISH:	return MGOddEvenFinish(pNetMsg);
	default:
		{
			// 적절한 이벤트가 아니라서 게임 취소
			NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgFB );

			return S_FALSE;
		}
		break;
	}

	return S_OK;
}


HRESULT GLChar::MsgReqItemMix( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_ITEM_MIX* pNetMsg = (GLMSG::SNET_INVEN_ITEM_MIX*)nmg;
	GLMSG::SNET_INVEN_ITEM_MIX_FB	NetMsgFB;


	SINVENITEM* pInvenItem[ITEMMIX_ITEMNUM];

	//	NPC확인
		// NPC 체크 
	PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pCrow )
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_NONPC;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
	float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
	float fTalkableDis = fTalkRange + 20;

	if ( fDist>fTalkableDis )
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_NONPC;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	조합 확인
	const ITEM_MIX* pItemMix = GLItemMixMan::GetInstance().GetItemMix( pNetMsg->dwKey );
	if ( !pItemMix ) 
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_NOMIX;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	금액 확인 
	if( m_lnMoney < pItemMix->dwPrice )
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_NOMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	//	인벤 아이템 확인
	for ( int i =0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		if( !pNetMsg->sInvenPos[i].VALID() ) continue;
		
		pInvenItem[i] = m_cInventory.GetItem ( pNetMsg->sInvenPos[i].wPosX, pNetMsg->sInvenPos[i].wPosY );
		if ( !pInvenItem[i] ) 
		{
			NetMsgFB.emFB = EMITEM_MIX_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}

		SITEM* pItem = GLItemMan::GetInstance().GetItem( pInvenItem[i]->sItemCustom.sNativeID );

		if ( !pItem ) 
		{
			NetMsgFB.emFB = EMITEM_MIX_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}
	}


	for ( int i =0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		if ( pItemMix->sMeterialItem[i].sNID == NATIVEID_NULL() ) continue;

		if ( pItemMix->sMeterialItem[i].nNum != pInvenItem[i]->sItemCustom.wTurnNum )
		{
			NetMsgFB.emFB = EMITEM_MIX_FB_NOMIXNUM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}	
	}


	//	결과 아이템 체크
	SITEM* pItemResult = GLItemMan::GetInstance().GetItem( pItemMix->sResultItem.sNID );
	if ( !pItemResult ) 
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}


	WORD wInsertPosX, wInsertPosY;
	BOOL bOk = m_cInventory.FindInsrtable ( pItemResult->sBasicOp.wInvenSizeX, pItemResult->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
	if ( !bOk )
	{
		//	불가능한 경우 입니다.
		NetMsgFB.emFB = EMITEM_MIX_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;	//	인벤 공간 부족으로 되돌림 실패.
	}

	GLMSG::SNETPC_INVEN_DELETE NetMsg_Inven_Delete;

	//	조합 아이템 삭제
	for ( int i =0; i < ITEMMIX_ITEMNUM; ++i )
	{
		if( !pNetMsg->sInvenPos[i].VALID() ) continue;

		GLITEMLMT::GetInstance().ReqItemRoute( pInvenItem[i]->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, 
			EMITEM_ROUTE_ITEMMIX, pInvenItem[i]->sItemCustom.wTurnNum );
		
		m_cInventory.DeleteItem( pInvenItem[i]->wPosX, pInvenItem[i]->wPosY );
		
		NetMsg_Inven_Delete.wPosX = pNetMsg->sInvenPos[i].wPosX;
		NetMsg_Inven_Delete.wPosY = pNetMsg->sInvenPos[i].wPosY;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsg_Inven_Delete );	
	}

	//	소지금 
	if ( pItemMix->dwPrice > 0 )
	{
		CheckMoneyUpdate( m_lnMoney, pItemMix->dwPrice, FALSE, "ITEM_MIX" );
		m_bMoneyUpdate = TRUE;

		m_lnMoney -= pItemMix->dwPrice;

		GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
		NetMsgMoney.lnMoney = m_lnMoney;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, &NetMsgMoney );
	}

	//	실패했을 경우
	float fRate = (float)pItemMix->dwRate;
	if ( !RANDOM_GEN( fRate ) )
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;	
	}

	//	결과 아이템 생성
	SITEMCUSTOM sITEM_NEW;
	sITEM_NEW.sNativeID = pItemMix->sResultItem.sNID;
	sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
	sITEM_NEW.wTurnNum = pItemMix->sResultItem.nNum;
	sITEM_NEW.cGenType = EMGEN_ITEMMIX;
	sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
	sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
	sITEM_NEW.lnGenNum = GLITEMLMT::GetInstance().RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
		GLITEMLMT::GetInstance().ReqRandomItem( sITEM_NEW );
	}

	//	들고있던 아이탬 인밴에 넣음.
	m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
	SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

	if ( !pINSERT_ITEM )
	{
		NetMsgFB.emFB = EMITEM_MIX_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	GLITEMLMT::GetInstance().ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, 
		EMITEM_ROUTE_ITEMMIX, pINSERT_ITEM->sItemCustom.wTurnNum );

	//	[자신에게] 메시지 발생.
	GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
	NetMsg_Inven.Data = *pINSERT_ITEM;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);


	NetMsgFB.emFB = EMITEM_MIX_FB_OK;
	NetMsgFB.sNativeID	= pINSERT_ITEM->sItemCustom.sNativeID;
	NetMsgFB.wTurnNum = pINSERT_ITEM->sItemCustom.wTurnNum;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
	
	return S_OK;
}
