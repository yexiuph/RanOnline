#include "pch.h"
#include "./GLChar.h"

#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool GLChar::DoDrugStorageItem ( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( dwChannel >= MAX_CLUBSTORAGE )		return false;

	//	�κ��� ������ Ȯ��.
	SINVENITEM *pINVENITEM = m_cStorage[dwChannel].FindPosItem ( wPosX, wPosY );
	if ( !pINVENITEM )		return false;

	//	������ ���� ������.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pItem )			return false;

	if ( pItem->sDrugOp.bInstance )
	{
		if ( pINVENITEM->sItemCustom.wTurnNum > 0 )		pINVENITEM->sItemCustom.wTurnNum--;
	
		//	Note :�������� ���� ���� ��� ���.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pINVENITEM->sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, 1 );

		if ( pINVENITEM->sItemCustom.wTurnNum==0 )
		{
			//	������ ����.
			m_cStorage[dwChannel].DeleteItem ( wPosX, wPosY );

			//	[�ڽſ���]â�� ������ ����.
			GLMSG::SNETPC_STORAGE_DELETE NetMsg_Delete;
			NetMsg_Delete.dwChannel = dwChannel;
			NetMsg_Delete.wPosX = wPosX;
			NetMsg_Delete.wPosY = wPosY;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Delete);
		}
		else
		{
			//	[�ڽſ���] �ι� ������ �Ҹ��.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsg_Inven_Update;
			NetMsg_Inven_Update.wPosX = wPosX;
			NetMsg_Inven_Update.wPosY = wPosY;
			NetMsg_Inven_Update.wTurnNum = pINVENITEM->sItemCustom.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Update);
		}
	}

	// ��Ÿ�� ����
	if ( pItem->sBasicOp.IsCoolTime() ) 
	{
		SetCoolTime( pINVENITEM->sItemCustom.sNativeID , pItem->sBasicOp.emCoolType );
	}

	return true;
}

// *****************************************************
// Desc: â������ ���� (ä�κ���)
// *****************************************************
HRESULT GLChar::MsgReqGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_GETSTORAGE *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	// ��û ä�� ��ȿ�� üũ
	if ( pNetMsg->dwChannel >= EMSTORAGE_CHANNEL )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	Note : â���� �⺻ ���� ����.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_FB NetMsg_GetFb;
	NetMsg_GetFb.lnMoney = m_lnStorageMoney;
	NetMsg_GetFb.dwChannel = dwChannel;
	NetMsg_GetFb.dwNumStorageItem = m_cStorage[dwChannel].GetNumItems ();
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_GetFb);

	//	Note : �������� �ϳ��� ����.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE_ITEM NetMsgItem;
	NetMsgItem.dwChannel = dwChannel;

	// Ŭ���̾�Ʈ�� â�� ����
	// ������ ���鼭 �ϳ��� ����
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
// Desc: â������� ��� ( �๰ )
// *****************************************************
HRESULT GLChar::MsgReqStorageDrug ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_STORAGEDRUG *pNetMsg = (GLMSG::SNETPC_REQ_STORAGEDRUG *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
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

	//	Note : pk ����� ���θ� ��� �̻��� ��� ȸ������ ����� ���´�.
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
			//	Note : ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg;
			NetMsg.wRECOVER = m_sCONFTING.wRECOVER;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

			return E_FAIL;
		}

		if ( m_sCONFTING.sOption.wRECOVER!=USHRT_MAX )
		{
			m_sCONFTING.COUNTRECOVE();

			//	Note : ȸ���� ��� ���� ���� Ŭ���̾�Ʈ�� �˸�.
			//
			GLMSG::SNETPC_CONFRONT_RECOVE NetMsg;
			NetMsg.wRECOVER = m_sCONFTING.wRECOVER;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);
		}
	}

	//	â�������� DB���� �������� �ʾ�����.
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


	//	â�� ������ �Ҹ��.
	DoDrugStorageItem ( dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : ü�� ��ġ ��ȭ�� [�ڽ�,��Ƽ��,����]�� Ŭ���̾�Ʈ�鿡 �˸�.
	MsgSendUpdateState ();

	return S_OK;
}

// *****************************************************
// Desc: â��ų ��� ( �๰ )
// *****************************************************
HRESULT GLChar::MsgReqStorageSkill ( NET_MSG_GENERIC* nmg )
{
	if ( !IsValidBody() )	return E_FAIL;

	GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE *pNetMsg = (GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE *) nmg;
	static GLMSG::SNETPC_REQ_HOLD_FB NetMsg_fb(NET_MSG_GCTRL_REQ_GETSTORAGE,EMHOLD_FB_OFF);

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
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

	//	â�������� DB���� �������� �ʾ�����.
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
		//	�̹� ������ ��ų.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = EMSKILL_LEARN_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL(sSKILL_ID);
	if ( emSKILL_LEARNCHECK!=EMSKILL_LEARN_OK )
	{
		//	��ų ���� �䱸 ������ �������� ���մϴ�.
		GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
		NetMsgFB.skill_id = sSKILL_ID;
		NetMsgFB.emCHECK = emSKILL_LEARNCHECK;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

		return E_FAIL;
	}

	//	Note : ��ų ������.
	//
	LEARN_SKILL(sSKILL_ID);

	//	Note : ��ų �������� ���� ����Ʈ ���� ����.
	//
	QuestStartFromGetSKILL ( sSKILL_ID );

	//	Note : �Ҹ� ������ ����.
	//
	DoDrugStorageItem ( pNetMsg->dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );

	//	��ų ��� ����.
	GLMSG::SNETPC_REQ_LEARNSKILL_FB	NetMsgFB;
	NetMsgFB.skill_id = sSKILL_ID;
	NetMsgFB.emCHECK = EMSKILL_LEARN_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgFB );

	//	��ų ��ﶧ �Ҹ�� ����Ʈ ������Ʈ.
	GLMSG::SNETPC_UPDATE_SKP NetMsgSkp;
	NetMsgSkp.dwSkillPoint = m_dwSkillPoint;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgSkp );

	return S_OK;
}

// *****************************************************
// Desc: ������ ��� �õ�
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
		// â������ ��ȿ�� üũ
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	//	â�������� DB���� �������� �ʾ�����.
	if ( !m_bServerStorage )	LoadStorageFromDB ();

	const DWORD dwChannel = pNetMsg->dwChannel;

	//	��û�� ������ ��ȿ�� �˻�.
	SINVENITEM *pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	�տ� �� �������� ������.
	if ( VALID_HOLD_ITEM() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	//	�ι꿡 �ִ� ������ ���.
	SINVENITEM sInven_BackUp = *pInvenItem;

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_USER, m_dwUserID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	�ι꿡�� ���� �������� ����.
	m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	�ι꿡 �־��� �������� �տ� ��.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	[�ڽſ���] �޽��� �߻�.	- â���� ������ ����.
	GLMSG::SNETPC_STORAGE_DELETE NetMsg;
	NetMsg.dwChannel = dwChannel;
	NetMsg.wPosX = sInven_BackUp.wPosX;
	NetMsg.wPosY = sInven_BackUp.wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	[�ڽſ���] �ι꿡 �־��� �������� �տ� ��.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

// *****************************************************
// Desc: â�� ������ ��� ���� ���� (Ȯ����)
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
		// â������ ��ȿ�� üũ
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

	//	â�������� DB���� �������� �ʾ�����.
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
	
	//	�ŷ��ɼ�
	if ( !pITEM->sBasicOp.IsEXCHANGE() )		return FALSE;
	
	SINVENITEM *pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return S_FALSE;
	}

	SINVENITEM sInven_BackUp = *pInvenItem;		//	�ι꿡 �ִ� ������ ���.
	
	WORD wPosX = sInven_BackUp.wPosX;			//	������ ��ġ.
	WORD wPosY = sInven_BackUp.wPosY;

	SITEMCUSTOM sItemHold = GET_HOLD_ITEM();	//	�տ� �� ������ ���.

	SITEM* pInvenData = GLItemMan::GetInstance().GetItem ( sInven_BackUp.sItemCustom.sNativeID );
	SITEM* pHoldData = GLItemMan::GetInstance().GetItem ( sItemHold.sNativeID );
	if ( !pInvenData || !pHoldData )	return S_FALSE;

	//	Note : ���� �Ҽ� �ִ� ���������� ����.
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
			//	Note : ��ħ �� ����.
			pInvenItem->sItemCustom.wTurnNum = wTurnNum;

			//	Note : ��� �ִ� ������ ����.
			RELEASE_HOLD_ITEM();

			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageUpdate;
			NetMsgStorageUpdate.dwChannel = dwChannel;
			NetMsgStorageUpdate.wPosX = pNetMsg->wPosX;
			NetMsgStorageUpdate.wPosY = pNetMsg->wPosY;
			NetMsgStorageUpdate.wTurnNum = wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageUpdate);
		
			//	Note : Ŭ���̾�Ʈ�� ��� �ִ� ������ ����.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);
		}
		else
		{
			WORD wSplit = wTurnNum - pInvenData->sDrugOp.wPileNum;
			
			//	���� ������ ���� ����. ( ��ä��. )
			pInvenItem->sItemCustom.wTurnNum = pInvenData->sDrugOp.wPileNum;

			//	�տ� ��� �ִ� ������ ����. ( ���� ����. )
			sItemHold.wTurnNum = wSplit;
			HOLD_ITEM ( sItemHold );

			//	Note : �տ��� ������ ����.
			GLMSG::SNETPC_PUTON_UPDATE NetMsgPutOn;
			NetMsgPutOn.emSlot = SLOT_HOLD;
			NetMsgPutOn.sItemCustom = sItemHold;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgPutOn);

			//	Note : â�� ������ ������Ʈ.
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorageItem;
			NetMsgStorageItem.dwChannel = pNetMsg->dwChannel;
			NetMsgStorageItem.wPosX = pNetMsg->wPosX;
			NetMsgStorageItem.wPosY = pNetMsg->wPosY;
			NetMsgStorageItem.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorageItem);
		}

		return S_OK;
	}

	//	â�� �ִ� ������ ����.
	BOOL bOk = m_cStorage[dwChannel].DeleteItem ( sInven_BackUp.wPosX, sInven_BackUp.wPosY );

	//	Note : ���� �տ��� �����ư� ����� �������� ũ�Ⱑ Ʋ�� ���.
	//	��� �ִ� �������� �� ������ ã�´�.
	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
	{
		//	������ġ�� ������ �ִ��� �˻�.
		BOOL bOk = m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )
		{
			//	����ִ� ������ �ι꿡 ���� ������ �ִ��� �˻�.
			bOk = m_cStorage[dwChannel].FindInsrtable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	roll-back : �ι꿡�� �����ߴ� �������� �ٽ� ����.
				m_cStorage[dwChannel].InsertItem ( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY );

				return S_FALSE;
			}
		}
	}

	//	����ִ� ������ �ι꿡 ����.
	m_cStorage[dwChannel].InsertItem ( sItemHold, wPosX, wPosY );
	SINVENITEM *pINSERTITEM = m_cStorage[dwChannel].GetItem(wPosX,wPosY);
	if ( !pINSERTITEM )
	{
		CDebugSet::ToLogFile ( "error MsgReqStorageExHold(), sItemHold, id(%d,%d) to pos(%d,%d)",
			sItemHold.sNativeID.wMainID, sItemHold.sNativeID.wSubID, wPosX, wPosY );
		return S_FALSE;
	}

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sItemHold, ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, EMITEM_ROUTE_USERINVEN, sItemHold.wTurnNum );

	//	�ι꿡 �־��� �������� �տ� ��.
	HOLD_ITEM ( sInven_BackUp.sItemCustom );

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( sInven_BackUp.sItemCustom, ID_USER, m_dwUserID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sInven_BackUp.sItemCustom.wTurnNum );

	//	[�ڽſ���] �ι꿡 �ִ� ������ ����, �� ������ ����.
	GLMSG::SNETPC_STORAGE_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.dwChannel = dwChannel;
	NetMsg_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	������ ������.
	NetMsg_Del_Insert.wDelY = sInven_BackUp.wPosY;
	if ( pINSERTITEM )	NetMsg_Del_Insert.sInsert = *pINSERTITEM;	//	���ԵǴ� ������.
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Del_Insert);

	//	[�ڽſ���] �ι꿡 �־��� �������� �տ� ��.
	GLMSG::SNETPC_PUTON_UPDATE NetMsg_PutOn;
	NetMsg_PutOn.emSlot = SLOT_HOLD;
	NetMsg_PutOn.sItemCustom = sInven_BackUp.sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn);

	return S_OK;
}

// *****************************************************
// Desc: �տ��� ������ â�� ����
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
		// â������ ��ȿ�� üũ
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

	//	â�������� DB���� �������� �ʾ�����.
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

	//	�ŷ��ɼ�
	if ( !pHoldData->sBasicOp.IsEXCHANGE() )		return FALSE;

	// ��ī���ϰ�� ���� Ȱ�����̸�
	if ( pHoldData->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && pMyPet->m_dwPetID == GET_HOLD_ITEM().dwPetID )	return FALSE;
	}


	BOOL bOk = m_cStorage[dwChannel].IsInsertable ( pHoldData->sBasicOp.wInvenSizeX, pHoldData->sBasicOp.wInvenSizeY, pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_fb);
		return E_FAIL;
	}

	//	����ִ� ������ �ι꿡 ����.
	m_cStorage[dwChannel].InsertItem ( GET_HOLD_ITEM(), pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note :�������� ���� ���� ��� ���.
	//
	GLITEMLMT::GetInstance().ReqItemRoute ( GET_HOLD_ITEM(), ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, EMITEM_ROUTE_USERINVEN, GET_HOLD_ITEM().wTurnNum );

	//	������ ������ ����.
	RELEASE_HOLD_ITEM ();

	//	[�ڽſ���] �ι꿡 ������ ����.
	GLMSG::SNETPC_STORAGE_INSERT NetMsg_Inven_Insert;
	NetMsg_Inven_Insert.dwChannel = dwChannel;
	NetMsg_Inven_Insert.Data = *m_cStorage[dwChannel].GetItem(pNetMsg->wPosX,pNetMsg->wPosY);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven_Insert);

	//	[�ڽſ���] �� �־��� ������ ����.
	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_ReleaseHold);

	return S_OK;
}

// *****************************************************
// Desc: �� �ñ��
// *****************************************************
HRESULT GLChar::MsgReqStorageSaveMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY *)nmg;

	if ( pNetMsg->lnMoney < 0 )				return S_FALSE;
	if ( m_lnMoney < pNetMsg->lnMoney )		return S_FALSE;

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
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

	//	Note : �ݾ� �α�.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, pNetMsg->lnMoney, EMITEM_ROUTE_USERINVEN );

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

// *****************************************************
// Desc: �� ã��
// *****************************************************
HRESULT GLChar::MsgReqStorageDrawMoney ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY *)nmg;

	LONGLONG lnMoney = pNetMsg->lnMoney;
	if ( lnMoney < 0 )						return S_FALSE;
	if ( m_lnStorageMoney < lnMoney )		return S_FALSE;

	if ( !m_bUsedStorageCard )
	{
		// â������ ��ȿ�� üũ
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

	//	Note : �ݾ� �α�.
	//
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, -lnMoney, EMITEM_ROUTE_USERINVEN );

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

// *****************************************************
// Desc: �Ҹ� ������ ���� �и�
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
		// â������ ��ȿ�� üũ
		PGLCROW pCrow = m_pLandMan->GetCrow ( pNetMsg->dwNPCID );
		if ( !pCrow )					return E_FAIL;

		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-pCrow->GetPosition()) );
		float fTalkRange = (float) (pCrow->GetBodyRadius() + GETBODYRADIUS() + 30);
		float fTalkableDis = fTalkRange + 20;

		if ( fDist>fTalkableDis )		return E_FAIL;
	}

	GLInventory &sInventory = m_cStorage[pNetMsg->dwChannel];

	//	Note : �и� ��û ������ ã��.
	SINVENITEM* pInvenItem = sInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : ������ ���� ��������.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )										return E_FAIL;

	//	Note : �и� ������ ���������� ����.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : ���� �������� ��ȭ�� ����.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : �и��Ǿ� ���� ����� ������.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : ���� ����� ������ ��ġ ã��.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = sInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : ���� ���� ������ �ֱ�.
	bOk = sInventory.InsertItem ( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : ���� ���� ������ ���� ��������.
	SINVENITEM* pNewItem = sInventory.GetItem ( wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : ���� �������� ���� ���� ��Ŵ.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : �κ��� ���� �и��Ǿ� ��� ���� ������.
	GLMSG::SNETPC_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgNew);

	//	Note : ���� ������ ���� ����.
	GLMSG::SNETPC_STORAGE_DRUG_UPDATE NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgStorage);

	return S_OK;
}

