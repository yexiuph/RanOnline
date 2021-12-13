#include "pch.h"
#include "./GLChar.h"
#include "./GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT GLChar::MsgPMarketTitle ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_TITLE *pNetMsg = (GLMSG::SNETPC_PMARKET_TITLE *) nmg;

	GLMSG::SNETPC_PMARKET_TITLE_FB	NetMsgFB;

	if ( m_sPMarket.IsOpen() )
	{
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 초기화를 행한다.
	m_sPMarket.DoMarketClose();

	m_sPMarket.SetTITLE ( pNetMsg->szPMarketTitle );

	StringCchCopy ( NetMsgFB.szPMarketTitle, CHAT_MSG_SIZE+1, m_sPMarket.GetTitle().c_str() );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgPMarketReqItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_REGITEM *pNetMsg = (GLMSG::SNETPC_PMARKET_REGITEM *) nmg;
	GLMSG::SNETPC_PMARKET_REGITEM_FB NetMsgFB;

	if ( m_sPMarket.IsOpen() )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 등록할 수 있는 한도를 넘어서고 있습니다.
	if ( m_sPMarket.GetItemNum() >= GLPrivateMarket::EMMAX_SALE_NUM )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_MAXNUM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	SNATIVEID nidITEM = pINVENITEM->sItemCustom.sNativeID;
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	// 팻카드이면서 팻이 활성화 되어 있으면 상점에 등록할 수 없다.
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GLGaeaServer::GetInstance().GetPET ( m_dwPetGUID );
		if ( pMyPet && pMyPet->IsValid () && pINVENITEM->sItemCustom.dwPetID == pMyPet->m_dwPetID ) return S_FALSE;
	}

	if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pINVENITEM->sItemCustom.dwVehicleID != 0 )
	{
		return E_FAIL;
	}

	DWORD dwNum = pNetMsg->dwNum;

	//	Note : 이미 등록된 아이템인지 검사.
	bool bREGPOS = m_sPMarket.IsRegInvenPos ( SNATIVEID(pNetMsg->wPosX,pNetMsg->wPosY) );
	if ( bREGPOS )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_REGITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	거래옵션
	if ( !pITEM->sBasicOp.IsEXCHANGE() )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_NOSALE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return FALSE;
	}
	
	// GMCharEdit 로 넣은 아이템은 판매가 불가능하다.
	if ( pINVENITEM->sItemCustom.IsGM_GENITEM() )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_NOSALE;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return FALSE;
	}

	//	Note : 겹침 가능일 경우 동일한 종류의 아이템이 이미 등록되어 있는지 검사.
	if ( pITEM->ISPILE() )
	{
		bool bREG = m_sPMarket.IsRegItem ( nidITEM );
		if ( bREG )
		{
			NetMsgFB.emFB = EMPMARKET_REGITEM_FB_REGITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return S_FALSE;
		}

		//	Note : 소지하고 있는 개수 많큼 판매 가능.
		DWORD dwTURN = m_cInventory.CountTurnItem ( nidITEM );
		if ( dwNum >= dwTURN )
		{
			dwNum = dwTURN;
		}
	}

	if ( !pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	SNATIVEID sSALEPOS;
	bool bPOS = m_sPMarket.FindInsertPos ( nidITEM, sSALEPOS );
	if ( !bPOS )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_MAXNUM;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	// Add Search Item Data
	bool bREG;
	if( m_sMapID.wMainID == 22 && m_sMapID.wSubID == 0 )
	{
		bREG = m_sPMarket.RegItem ( *pINVENITEM, pNetMsg->dwMoney, dwNum, sSALEPOS, TRUE );
	}else{
		bREG = m_sPMarket.RegItem ( *pINVENITEM, pNetMsg->dwMoney, dwNum, sSALEPOS, FALSE );
	}

	
	if ( !bREG )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.emFB = EMPMARKET_REGITEM_FB_OK;
	NetMsgFB.wPosX = pNetMsg->wPosX;
	NetMsgFB.wPosY = pNetMsg->wPosY;
	NetMsgFB.dwMoney = pNetMsg->dwMoney;
	NetMsgFB.dwNum = dwNum;
	NetMsgFB.sSALEPOS = sSALEPOS;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgPMarketDisItem ( NET_MSG_GENERIC* nmg )
{
	if ( m_sPMarket.IsOpen() )	return S_FALSE;

	GLMSG::SNETPC_PMARKET_DISITEM *pNetMsg = (GLMSG::SNETPC_PMARKET_DISITEM *) nmg;

	const SSALEITEM* pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
	if ( !pSALEITEM )		return S_FALSE;

	// Add Search Item Data
	// bool bREG;
	if( m_sMapID.wMainID == 22 && m_sMapID.wSubID == 0 )
	{
		m_sPMarket.DisItem ( pNetMsg->sSALEPOS , TRUE );
	}else{
		m_sPMarket.DisItem ( pNetMsg->sSALEPOS , FALSE );
	}
	
	GLMSG::SNETPC_PMARKET_DISITEM_FB NetMsgFB;
	NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgPMarketOpen ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_OPEN *pNetMsg = (GLMSG::SNETPC_PMARKET_OPEN *) nmg;
	GLMSG::SNETPC_PMARKET_OPEN_FB	NetMsgFB;

	//// 상점 열고 움직이는것 방지
	//if ( !IsACTION ( GLAT_IDLE ) )
	//{
	//	NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
	//	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
	//	return S_FALSE;
	//}

	if ( m_sPMarket.IsOpen() )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return S_FALSE;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	if ( pITEM->sBasicOp.emItemType!=ITEM_PRIVATEMARKET )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	if ( m_sPMarket.GetItemNum() == 0 )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_EMPTY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	bool bOPEN = m_sPMarket.DoMarketOpen();
	if ( !bOPEN )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	[자신에게] 인밴 아이탬 소모시킴.
	DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : 자기 자신에게.
	NetMsgFB.emFB = EMPMARKET_OPEN_FB_OK;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	//	Note : 주변 모두에게.
	GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	StringCchCopy ( NetMsgBRD.szPMarketTitle, CHAT_MSG_SIZE+1, m_sPMarket.GetTitle().c_str() );
	SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBRD );


	// Add Search Item Data
	// 만일 상점을 학원 광장에서 열었다면 아이템 검색 리스트에 추가한다.
	if( m_sMapID.wMainID == 22 && m_sMapID.wSubID == 0 )
	{
		GLGaeaServer::GetInstance().InsertSearchShop( m_dwGaeaID );
	}


	return S_OK;
}

HRESULT GLChar::MsgPMarketClose ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_CLOSE *pNetMsg = (GLMSG::SNETPC_PMARKET_CLOSE *) nmg;

	m_sPMarket.DoMarketClose();

	//	주변에게 개인상점 종료 알림.
	GLMSG::SNETPC_PMARKET_CLOSE_BRD	NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBRD );

	//	자신에게 개인상점 종료 알림.
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBRD );

	// Add Search Item Data
	// 만일 상점을 학원 광장에서 열었다가 닫았다면 아이템 검색 리스트에서 삭제한다.
	if( m_sMapID.wMainID == 22 && m_sMapID.wSubID == 0 )
	{
		GLGaeaServer::GetInstance().EraseSearchShop( m_dwGaeaID );
	}

	return S_OK;
}

void GLChar::ReqNetMsg_PMarketItem ( DWORD dwClientID )
{
	GLMSG::SNETPC_PMARKET_ITEM_INFO_BRD NetMsgBrd;
	GLPrivateMarket::MAPITEM& mapITEM = m_sPMarket.GetItemList();
	GLPrivateMarket::MAPITEM_ITER pos = mapITEM.begin();
	GLPrivateMarket::MAPITEM_ITER end = mapITEM.end();
	for ( ; pos!=end; ++pos )
	{
		const SSALEITEM &sSALEITEM = (*pos).second;

		
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.sSALEPOS = sSALEITEM.sSALEPOS;
		NetMsgBrd.sINVENPOS = sSALEITEM.sINVENPOS;
		NetMsgBrd.sITEMCUSTOM = sSALEITEM.sITEMCUSTOM;
		NetMsgBrd.llPRICE = sSALEITEM.llPRICE;
		NetMsgBrd.dwNUMBER = sSALEITEM.dwNUMBER;
		NetMsgBrd.bSOLD = sSALEITEM.bSOLD;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );

		// 상점의 아이템이 팻카드이면 팻카드 정보 요청
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sSALEITEM.sITEMCUSTOM.sNativeID );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			PGLPET pPetInfo = new GLPET ();

			// DB작업 실패시 혹은 생성후 메모리 해제해줘라
			CGetPet* pDbAction = new CGetPet ( pPetInfo, sSALEITEM.sITEMCUSTOM.dwPetID, dwClientID, m_dwCharID, 
											   0,0,false, false, true );
			GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
	}
}

HRESULT GLChar::MsgPMarketItemInfo ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_ITEM_INFO *pNetMsg = (GLMSG::SNETPC_PMARKET_ITEM_INFO *) nmg;
	
	PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )	return S_FALSE;

	pCHAR->ReqNetMsg_PMarketItem(m_dwClientID);

	return S_OK;
}

HRESULT GLChar::MsgPMarketBuy ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_BUY *pNetMsg = (GLMSG::SNETPC_PMARKET_BUY *) nmg;
	GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;

	PGLCHAR pCHAR = GLGaeaServer::GetInstance().GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 개인상점이 오픈상태인지 검사.
	if ( !pCHAR->m_sPMarket.IsOpen() )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 판매중인 물품 정보.
	const SSALEITEM *pSALEITEM = pCHAR->m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
	if ( !pSALEITEM )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 판매중인 물품이 이미 판매완료됬는지 검사.
	if ( pSALEITEM->bSOLD )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_SOLD;
		NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsgFB.dwNum = 0;
		NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pSALEITEM->sITEMCUSTOM.sNativeID );
	if ( !pITEM )					return S_FALSE;

	// 시한부 아이템
	if( pITEM->IsTIMELMT() )
	{
		CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
		CTime cTIME_LMT(pSALEITEM->sITEMCUSTOM.tBORNTIME);
		cTIME_LMT += cSPAN;

		const CTime cTIME_CUR = CTime::GetCurrentTime();

		if ( cTIME_CUR > cTIME_LMT )
		{
			NetMsgFB.emFB = EMPMARKET_BUY_FB_NOTIME;
			GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return S_FALSE;
		}
	}

	// 코스튬
	if( pSALEITEM->sITEMCUSTOM.nidDISGUISE != SNATIVEID(false) )
	{
		if( pSALEITEM->sITEMCUSTOM.tDISGUISE != 0 )
		{
			CTime cTIME_LMT( pSALEITEM->sITEMCUSTOM.tDISGUISE );
			if( cTIME_LMT.GetYear() != 1970 )
			{
				const CTime cTIME_CUR = CTime::GetCurrentTime();

				if ( cTIME_CUR > cTIME_LMT )
				{
					NetMsgFB.emFB = EMPMARKET_BUY_FB_NOTIME;
					GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	DWORD dwNum = pNetMsg->dwNum;
	if ( !pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	//	Note : 구입할 수량 많큼 남아 있는지 검사.
	if ( pSALEITEM->dwNUMBER < dwNum )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_NUM;
		NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsgFB.dwNum = pSALEITEM->dwNUMBER;
		NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 자금이 충분한지 검사.
	if ( m_lnMoney < (dwNum*pSALEITEM->llPRICE) )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_LOWMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	Note : 인벤의 공간이 충분한지 검사.
	WORD wINVENX = pITEM->sBasicOp.wInvenSizeX;
	WORD wINVENY = pITEM->sBasicOp.wInvenSizeY;

	BOOL bITEM_SPACE(FALSE);
	if ( pITEM->ISPILE() )
	{
		WORD wPILENUM = pITEM->sDrugOp.wPileNum;
		SNATIVEID sNID = pITEM->sBasicOp.sNativeID;
		WORD wREQINSRTNUM = (WORD) dwNum;

		bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM, wINVENX, wINVENY );
	}
	else
	{
		WORD wPosX(0), wPosY(0);
		bITEM_SPACE = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wPosX, wPosY );
	}

	if ( !bITEM_SPACE )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return E_FAIL;
	}

	//	Note : 개인상점에 등록된 아이템이랑 실제로 인벤토리에 있는 아이템이 일치하는지 검사.
	//
	BOOL bVALID_ITEM(FALSE);
	if ( pITEM->ISPILE() )
	{
		DWORD dwTURN = pCHAR->m_cInventory.CountTurnItem ( pSALEITEM->sITEMCUSTOM.sNativeID );
		if ( dwTURN >= dwNum )		bVALID_ITEM = TRUE;
	}
	else
	{
		SINVENITEM *pINVENITEM = pCHAR->m_cInventory.GetItem ( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID );
		if ( pINVENITEM )
		{
			int nCMP = memcmp ( &pINVENITEM->sItemCustom, &pSALEITEM->sITEMCUSTOM, sizeof(SITEMCUSTOM) );
			if ( nCMP==0 )			bVALID_ITEM = TRUE;
		}
	}

	if ( !bVALID_ITEM )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	// Add Search Item Data
	//	Note : 개인 상점의 정보 갱신.
	bool bSALE;
	if( m_sMapID.wMainID == 22 && m_sMapID.wSubID == 0 )
	{
		bSALE = pCHAR->m_sPMarket.DoSale ( pNetMsg->sSALEPOS, (WORD) dwNum, TRUE );
	}else{
		bSALE = pCHAR->m_sPMarket.DoSale ( pNetMsg->sSALEPOS, (WORD) dwNum, FALSE );
	}
	if ( !bSALE )
	{
		GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return E_FAIL;
	}

	//	Note : 거래 수행.
	if ( !pITEM->ISPILE() )
	{
		BOOL bDEL = pCHAR->m_cInventory.DeleteItem ( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID );
		if ( !bDEL )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}

		//	Note : 삭제 메시지.
		GLMSG::SNETPC_INVEN_DELETE NetMsgDelete;
		NetMsgDelete.wPosX = pSALEITEM->sINVENPOS.wMainID;
		NetMsgDelete.wPosY = pSALEITEM->sINVENPOS.wSubID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgDelete);

		WORD wPosX, wPosY;
		BOOL bFOUND = m_cInventory.FindInsrtable ( wINVENX, wINVENY, wPosX, wPosY );
		if ( !bFOUND )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}

		BOOL bINSERT = m_cInventory.InsertItem ( pSALEITEM->sITEMCUSTOM, wPosX, wPosY );
		if ( !bINSERT )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}

		SINVENITEM *pINSERTITEM = m_cInventory.GetItem ( wPosX, wPosY );
		if ( !pINSERTITEM )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}

		//	Note : 구입자 인벤에 아이템 삽입 메시지.
		GLMSG::SNETPC_INVEN_INSERT NetMsg_Inven;
		NetMsg_Inven.Data = *pINSERTITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg_Inven);

		//	Note :아이템의 소유 이전 경로 기록.
		// 운영팀에서 상점 아이템 거래에 대한 아이템 정보요청으로 인해서 아이템 겹침 상관없이 무조건 로그를 남기게 수정
		//GLITEMLMT::GetInstance().ReqItemRoute ( pSALEITEM->sITEMCUSTOM, ID_CHAR, pCHAR->m_dwCharID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pSALEITEM->sITEMCUSTOM.wTurnNum );

/*
		// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
		// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
		if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pSALEITEM->sITEMCUSTOM.dwPetID != 0 )
		{
			CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, pSALEITEM->sITEMCUSTOM.dwPetID );
			GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
*/
	}
	else
	{
		bool bDELETE = pCHAR->DeletePileItem ( pSALEITEM->sITEMCUSTOM.sNativeID, (WORD) dwNum );
		if ( !bDELETE )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}

		//	Note : 아이템을 인벤에 넣어준다.
		//
		bool bINSERT = InsertPileItem ( pSALEITEM->sITEMCUSTOM, (WORD) dwNum );
		if ( !bINSERT )
		{
			//	Note : 치명적오류. ( 사전에 data의 무결성을 점검하였기 때문에 문제가 발생하면 안됨. )
			GASSERT(0&&"GLChar::MsgPMarketBuy()에서 치명적 오류 발견.");
			NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
			return E_FAIL;
		}
	}

	if ( pSALEITEM )
	{
		GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;

		NetMsgBrd.sSALEPOS = pNetMsg->sSALEPOS;
		NetMsgBrd.dwNUMBER = pSALEITEM->dwNUMBER;
		NetMsgBrd.bSOLD = pSALEITEM->bSOLD;

		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );
		GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );

		pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}

	//	Note : 구입, 판매 성공 FB.
	NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
	NetMsgFB.emFB = EMPMARKET_BUY_FB_OK;
	NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
	NetMsgFB.dwNum = pNetMsg->dwNum;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	//	Note : 돈 이전.
	LONGLONG lnPRICE = dwNum * pSALEITEM->llPRICE;

	CheckMoneyUpdate( m_lnMoney, lnPRICE, FALSE, "Buy From PC." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= lnPRICE;

	volatile float fCommissionRate = m_pLandMan->m_fCommissionRate;
	volatile float fCommissionRate_C = (100.0f-fCommissionRate)*0.01f;
	volatile LONGLONG lnGIVE = LONGLONG( DOUBLE(lnPRICE) * fCommissionRate_C );

	if ( m_pLandMan->m_dwGuidClubID != CLUB_NULL )
	{
		GLGuidance *pGuid = m_pLandMan->m_pGuidance;
		if ( pGuid && !pGuid->m_bBattle )
		{
			//	Note : 클럽의 수익 발생.
			LONGLONG lnCommission = lnPRICE-lnGIVE;
		
			GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub ( m_pLandMan->m_dwGuidClubID );
			if ( pCLUB )
			{
				pCLUB->m_lnIncomeMoney += lnCommission;

				// 클럽 수입이 1억 이상일 경우 로그를 남긴다.
				if( lnCommission > 	EMCLUBINCOMEMONEY_LOG )
					GLITEMLMT::GetInstance().ReqMoneyExc( ID_USER, pCHAR->m_dwCharID, 
														  ID_CLUB, pCLUB->m_dwID,
														  lnCommission,
														  EMITEM_ROUTE_CLUBINCOME );

				CDebugSet::ToFile ( "club_income_char.txt", "{PMarketBuy}, ClubID[%u], ToCharID[%u], FromCharID[%u], Commission[%I64d], IncomeMoney[%I64d]",
									pCLUB->m_dwID, m_dwCharID, pCHAR->m_dwCharID, lnCommission, pCLUB->m_lnIncomeMoney );
			}
		}
	}

	pCHAR->CheckMoneyUpdate( pCHAR->m_lnMoney, lnGIVE, TRUE, "Sale To PC." );
	pCHAR->m_bMoneyUpdate = TRUE;

	pCHAR->m_lnMoney += lnGIVE;

	//	Note :아이템의 소유 이전 경로 기록.
	//  운영팀에서 상점 아이템 거래에 대한 아이템 정보요청으로 인해서 아이템 겹침 상관없이 무조건 로그를 남기게 수정
	GLITEMLMT::GetInstance().ReqItemRoute ( pSALEITEM->sITEMCUSTOM, ID_CHAR, pCHAR->m_dwCharID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pSALEITEM->sITEMCUSTOM.wTurnNum );

	//	Note : 돈 기록.
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, pCHAR->m_dwCharID, lnGIVE, EMITEM_ROUTE_CHAR );

	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, pCHAR->m_dwCharID, ID_CHAR, 0, pCHAR->m_lnMoney, EMITEM_ROUTE_CHAR );

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		strTemp.Format( "Buy From PC, [%s][%s], PC Have Money:[%I64d], MarketPC[%s][%s], MarketPC Have Money:[%I64d], Buy price:[%I64d] ",
			     m_szUID, m_szName, m_lnMoney, pCHAR->m_szUID, pCHAR->m_szName, m_lnMoney, lnGIVE );

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

	//	Note : 구입자 인벤 금액 변화.
	//	금액 변화 클라이언트에 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsgM1;
	NetMsgM1.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, &NetMsgM1 );

	//	Note : 판매자 인벤 금액 변화.
	//	금액 변화 클라이언트에 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsgM2;
	NetMsgM2.lnMoney = pCHAR->m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsgM2 );

	return S_OK;
}
