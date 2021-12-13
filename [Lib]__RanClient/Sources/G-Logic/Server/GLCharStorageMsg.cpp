#include "pch.h"
#include "./GLChar.h"

#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool GLChar::DoDrugStorageItem ( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( dwChannel >= MAX_CLUBSTORAGE )		return false;

	//	인벤의 아이템 확인.
	SINVENITEM *pINVENITEM = m_cStorage[dwChannel].FindPosItem ( wPosX, wPosY );
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
			m_cStorage[dwChannel].DeleteItem ( wPosX, wPosY );

			//	[자신에게]창고 아이탬 제거.
			GLMSG::SNETPC_STORAGE_DELETE NetMsg_Delete;
			NetMsg_Delete.dwChannel = dwChannel;
			NetMsg_Delete.wPosX = wPosX;
			NetMsg_Delete.wPosY = wPosY;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Delete);
		}
		else
		{
			//	[자신에게] 인밴 아이탬 소모됨.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Update);
		}
	}

	// 쿨타임 적용
	if ( pItem->sBasicOp.IsCoolTime() ) 
	{
		SetCoolTime( pINVENITEM->sItemCustom.sNativeID , pItem->sBasicOp.emCoolType );
	}

	return true;
}

// *****************************************************
// Desc: 창고정보 전송 (채널별로)
// *****************************************************
HRESULT GLChar::MsgReqGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_GETSTORAGE *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	// 요청 채널 유효성 체크
	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	Note : 창고의 기본 정보 전송.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_FB NetMsg_GetFb;
	NetMsg_GetFb.lnMoney = m_lnStorageMoney;
	NetMsg_GetFb.dwChannel = dwChannel;
	NetMsg_GetFb.dwNumStorageItem = m_cStorage[dwChannel].GetNumItems ();
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_GetFb);

	//	Note : 아이템을 하나씩 전송.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_ITEM NetMsgItem;
	NetMsgItem.dwChannel = dwChannel;

	// 클라이언트에 창고 전송
	// 루프를 돌면서 하나씩 전송
	{
		SINVENITEM* pInvenItem = NULL;
		GLInventory::CELL_MAP* pInvenList = m_cStorage[dwChannel].GetItemList();
		GLInventory::CELL_MAP_ITER iter = pInvenList->begin();
		GLInventory::CELL_MAP_ITER iter_end = pInvenList->end();
		for ( ; iter!=iter_end; ++iter )
		{
			pInvenItem = (*iter).second;
			NetMsgItem.Data = *pInvenItem;			
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgItem );
		}
	}
	return S_OK;
}

// *****************************************************
// Desc: 창고아이템 사용 ( 약물 )
// *****************************************************
HRESULT GLChar::MsgReqStorageDrug ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_STORAGEDRUG *pNetMsg = (GLMSG::SNETPC_REQ_STORAGEDRUG *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE (pNetMsg->dwChannel) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	Note : pk 등급이 살인마 등급 이상일 경우 회복약의 사용을 막는다.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
	{
		return E_FAIL;
	}

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

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )	return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sDrugOp.emDrug==ITEM_DRUG_NUNE )	return E_FAIL;

	switch ( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_HP:
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_MP:
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_SP:
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_HP_MP:
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_MP_SP:
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_HP_MP_SP:
		m_sHP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sMP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		m_sSP.INCREASE ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );
		break;

	case ITEM_DRUG_CURE:
	case ITEM_DRUG_HP_CURE:
	case ITEM_DRUG_HP_MP_SP_CURE:
		//GASSERT(0&&"GLChar::MsgReqInvenDrug - ITEM_DRUG_CURE");
		break; 
	};


	//	창고 아이탬 소모됨.
	DoDrugStorageItem ( dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 체력 수치 변화를 [자신,파티원,주위]의 클라이언트들에 알림.
	MsgSendUpdateState ();

	return S_OK;
}

// *****************************************************
// Desc: 창고스킬 사용 ( 약물 )
// *****************************************************
HRESULT GLChar::MsgReqStorageSkill ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE *pNetMsg = (GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE (pNetMsg->dwChannel) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )	return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_SKILL )	return E_FAIL;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL(sSKILL_ID) )
	{
		//	이미 습득한 스킬.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = EMSKILL_LEARN_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL(sSKILL_ID);
	if ( emSKILL_LEARNCHECK!=EMSKILL_LEARN_OK )
	{
		//	스킬 습득 요구 조건을 충족하지 못합니다.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = emSKILL_LEARNCHECK;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	//	Note : 스킬 습득함.
	//
	LEARN_SKILL(sSKILL_ID);

	//	Note : 스킬 습득으로 인한 퀘스트 시작 점검.
	//
	QuestStartFromGetSKILL ( sSKILL_ID );

	//	Note : 소모성 아이템 제거.
	//
	DoDrugStorageItem ( pNetMsg->dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	스킬 배움 성공.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	//	스킬 배울때 소모된 포인트 업데이트.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgSkp );

	return S_OK;
}

// *****************************************************
// Desc: 아이템 들기 시도
// *****************************************************
HRESULT GLChar::MsgReqStorageToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_TO_HOLD *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_TO_HOLD *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD,EMHOLD_FB_OFF);

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	요청한 아이탬 유효성 검사.
	SINVENITEM *pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
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

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_USER, m_dwUserID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	인밴에서 빼낼 아이탬을 삭제.
	m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[자신에게] 메시지 발생.	- 창고에서 아이탬 삭제.
	GLMSG::SNETPC_STORAGE_DELETE NetMsg;
	NetMsg.dwChannel = dwChannel;
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

// *****************************************************
// Desc: 창고에 아이템 들기 놓기 병합 (확장기능)
// *****************************************************
HRESULT GLChar::MsgReqStorageExHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_EX_HOLD *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_EX_HOLD *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_STORAGE_EX_HOLD,EMHOLD_FB_OFF);

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE(pNetMsg->dwChannel) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	if ( !VALID_HOLD_ITEM() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

#if defined(VN_PARAM) //vietnamtest%%%
	if ( GET_HOLD_ITEM().bVietnamGainItem  )	return S_FALSE;
#endif

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pITEM )		return false;
	
	//	거래옵션
	if ( !pITEM->sBasicOp.IsEXCHANGE() )		return FALSE;
	
	SINVENITEM *pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
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

			//	Note : 들고 있는 아이템 지움.
			RELEASE_HOLD_ITEM();

			//	Note : 창고 아이템 업데이트.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageUpdate;
			NetMsgStorageUpdate.dwChannel = dwChannel;
			NetMsgStorageUpdate.wPosX = pNetMsg->wPosX;
			NetMsgStorageUpdate.wPosY = pNetMsg->wPosY;
			NetMsgStorageUpdate.wTurnNum = wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageUpdate);
		
			//	Note : 클라이언트에 들고 있는 아이템 지움.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	기존 아이템 갯수 변경. ( 꽉채움. )
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	손에 들고 있던 아이템 변경. ( 갯수 줄임. )
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM ( sItemHold );

			//	Note : 손에든 아이템 변경.
			GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
			NetMsgPutOn.emSlot = SLOT_HOLD;
			NetMsgPutOn.sItemCustom = sItemHold;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPutOn);

			//	Note : 창고 아이템 업데이트.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
			NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
			NetMsgStorageItem.wPosX = pNetMsg->wPosX;
			NetMsgStorageItem.wPosY = pNetMsg->wPosY;
			NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageItem);
		}

		return S_OK;
	}

	//	창고 있던 아이탬 삭제.
	BOOL bOk = m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	Note : 종전 손에든 아이탬과 들려는 아이탬의 크기가 틀릴 경우.
	//	들고 있는 아이탬이 들어갈 공간을 찾는다.
	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	동일위치에 넣을수 있는지 검사.
		BOOL bOk = m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
			//	들고있던 아이탬 인밴에 넣을 공간이 있는지 검색.
			bOk = m_cStorage[dwChannel].FindInsrtable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : 인밴에서 삭제했던 아이탬을 다시 넣음.
				m_cStorage[dwChannel].InsertItem ( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY );

				return S_FALSE;
			}
		}
	}

	//	들고있던 아이탬 인밴에 넣음.
	m_cStorage[dwChannel].InsertItem ( sItemHold, wPosX, wPosY );
	SINVENITEM *pINSERTITEM = m_cStorage[dwChannel].GetItem(wPosX,wPosY);
	if ( !pINSERTITEM )
	{
		CDebugSet::ToLogFile ( "error MsgReqStorageExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.sNativeID.wMainID, sItemHold.sNativeID.wSubID, wPosX, wPosY );
		return S_FALSE;
	}

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sItemHold, ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, EMITEM_ROUTE_USERINVEN, sItemHold.wTurnNum );

	//	인밴에 있었던 아이탬을 손에 듬.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_USER, m_dwUserID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	[자신에게] 인밴에 있던 아이탬 삭제, 및 아이탬 삽입.
	GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.dwChannel = dwChannel;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이탬.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	if ( pINSERTITEM )	NetMsg_Del_Insert.sInsert = *pINSERTITEM;	//	삽입되는 아이탬.
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Del_Insert);

	//	[자신에게] 인밴에 있었던 아이탬을 손에 듬.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

// *****************************************************
// Desc: 손에든 아이템 창고에 놓기
// *****************************************************
HRESULT GLChar::MsgReqHoldToStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_HOLD_TO_STORAGE *pNetMsg = (GLMSG::SNETPC_REQ_HOLD_TO_STORAGE *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE,EMHOLD_FB_OFF);

	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	if ( !IsKEEP_STORAGE(pNetMsg->dwChannel) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	창고정보를 DB에서 가져오지 않았을때.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	if ( !VALID_HOLD_ITEM() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

#if defined(VN_PARAM) //vietnamtest%%%
	if ( GET_HOLD_ITEM().bVietnamGainItem  )	return FALSE;
#endif

	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( GET_HOLD_ITEM().sNativeID );
	if ( !pHoldData )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	거래옵션
	if ( !pHoldData->sBasicOp.IsEXCHANGE() )		return FALSE;

	// 팻카드일경우 팻이 활동중이면
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && pMyPet->m_dwPetID == GET_HOLD_ITEM().dwPetID )	return FALSE;
	}


	BOOL bOk = m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	들고있던 아이탬 인밴에 넣음.
	m_cStorage[dwChannel].InsertItem ( GET_HOLD_ITEM(), pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note :아이템의 소유 이전 경로 기록.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( GET_HOLD_ITEM(), ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, EMITEM_ROUTE_USERINVEN, GET_HOLD_ITEM().wTurnNum );

	//	성공시 아이탬 제거.
	RELEASE_HOLD_ITEM ();

	//	[자신에게] 인밴에 아이탬 삽입.
	GLMSG::SNETPC_STORAGE_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.dwChannel = dwChannel;
	NetMsg_Inven_Insert.Data = *m_cStorage[dwChannel].GetItem(pNetMsg->wPosX,pNetMsg->wPosY);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

	//	[자신에게] 손 있었던 아이탬 제거.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	return S_OK;
}

// *****************************************************
// Desc: 돈 맡기기
// *****************************************************
HRESULT GLChar::MsgReqStorageSaveMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY *)nmg;

	if ( pNetMsg->lnMoney < 0 )				return S_FALSE;
	if ( m_lnMoney < pNetMsg->lnMoney )		return S_FALSE;

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	CheckMoneyUpdate( m_lnMoney, pNetMsg->lnMoney, FALSE, "Storage Save Money." );
	m_bMoneyUpdate = TRUE;
	m_bStorageMoneyUpdate = TRUE;

	m_lnMoney -= pNetMsg->lnMoney;
	m_lnStorageMoney += pNetMsg->lnMoney;

	//	Note : 금액 로그.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, pNetMsg->lnMoney, EMITEM_ROUTE_USERINVEN );

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

// *****************************************************
// Desc: 돈 찾기
// *****************************************************
HRESULT GLChar::MsgReqStorageDrawMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *)nmg;

	LONGLONG lnMoney = pNetMsg->lnMoney;
	if ( lnMoney < 0 )						return S_FALSE;
	if ( m_lnStorageMoney < lnMoney )		return S_FALSE;

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	CheckMoneyUpdate( m_lnMoney, lnMoney, TRUE, "Storage Draw Money." );
	m_bMoneyUpdate = TRUE;
	m_bStorageMoneyUpdate = TRUE;

	m_lnMoney += lnMoney;
	m_lnStorageMoney -= lnMoney;

	//	Note : 금액 로그.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, -lnMoney, EMITEM_ROUTE_USERINVEN );

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

// *****************************************************
// Desc: 소모성 아이템 개수 분리
// *****************************************************
HRESULT GLChar::MsgReqStorageSplit ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_SPLIT *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_SPLIT *)nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE,EMHOLD_FB_OFF);
	
	BOOL bOk(FALSE);
	if ( EMSTORAGE_CHANNEL <= pNetMsg->dwChannel )		return E_FAIL;

	if ( !IsKEEP_STORAGE (pNetMsg->dwChannel) )
	{
		NetMsg_fb.emHoldFB = EMHOLD_FB_NONKEEPSTORAGE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	if ( !m_bUsedStorageCard )
	{
		// 창고지기 유효성 체크
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	GLInventory &sInventory = m_cStorage[pNetMsg->dwChannel];

	//	Note : 분리 요청 아이템 찾음.
	SINVENITEM* pInvenItem = sInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : 아이템 정보 가져오기.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )										return E_FAIL;

	//	Note : 분리 가능한 아이템인지 점검.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : 원본 아이템의 변화된 갯수.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : 분리되어 새로 생기는 아이템.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : 새로 생기는 아이템 위치 찾기.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = sInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : 새로 생긴 아이템 넣기.
	bOk = sInventory.InsertItem ( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : 새로 생긴 아이템 정보 가져오기.
	SINVENITEM* pNewItem = sInventory.GetItem ( wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : 원본 아이템의 갯수 변경 시킴.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : 인벤에 새로 분리되어 들어 가는 아이템.
	GLMSG::SNETPC_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgNew);

	//	Note : 원본 아이템 갯수 변경.
	GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

