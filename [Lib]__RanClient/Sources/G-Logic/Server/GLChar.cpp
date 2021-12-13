#include "pch.h"
#include "./GLChar.h"
#include "./GLCharData.h"
#include "./GLGaeaServer.h"
#include "./GLSchoolFreePK.h"
#include "GLClubDeathMatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BOX_R	(4)

GLChar::GLChar () :
	m_wTARNUM(0),

	m_dwANISUBSELECT(0),

	m_dwActState(NULL),
	m_Action(GLAT_IDLE),

	m_fIdleTime(0.0f),
	m_fMBR_TIMER(0.0f),
	m_fSTATE_TIMER(0.0f),
	m_fPERIOD_TIMER(0.0f),
	m_fQUEST_TIMER(60.0f),
	m_fITEM_TIMER(0.0f),
	m_fSAVEDB_TIMER(0.0f),
	m_fattTIMER(0.0f),
	m_fMoveDelay(0.0f),
	m_fBRIGHT_TIMER(0.0f),

	m_sAssault(CROW_PC,GAEAID_NULL),
	m_lnLastSendExp(0),

	m_lnLastSendVNExp(0),

	m_dwViewFrame(0),
	m_fAge(0.0f),
	m_fGenAge(0.0f),
	m_nDECEXP(0),
	m_vPos(0,0,0),
	
	m_dwClientID(0),
	m_dwGaeaID(0),
	m_dwPartyID(PARTY_NULL),

	m_dwCeID(0),

	m_wHealingNum(0),

	m_pLandMan(NULL),
	
	m_pGaeaNode(NULL),
	m_pLandNode(NULL),

	m_pQuadNode(NULL),
	m_pCellNode(NULL),

	m_SKILLMTYPE(AN_SKILL),
	m_SKILLSTYPE(AN_SUB_00),

	m_bUsedStorageCard(false),

	m_bNon_Rebirth(false),

	m_fSkillDelay(0.0f),

	m_dwPetGUID(UINT_MAX),
	m_dwPetID(0),

	m_dwSummonGUID(UINT_MAX),
	m_dwSummonID(0),

	m_bEntryFailed(FALSE),


	m_bProtectPutOnItem(false),
	m_bGetPetFromDB(false),
	m_bGetVehicleFromDB ( false ),
	m_fVehicleTimer( 0.0f ),

	m_ItemRemodelPosX(EM_INVENSIZE_X),
	m_ItemRemodelPosY(EM_INVENSIZE_Y),
	m_ItemGarbagePosX(EM_INVENSIZE_X),
	m_ItemGarbagePosY(EM_INVENSIZE_Y),

	m_ChinaGainType(255),
	m_dwVietnamGainType(GAINTYPE_MAX),

	m_bEventStart(FALSE),
	m_bEventApply(FALSE),
	m_hCheckStrDLL( NULL ),
	m_pCheckString( NULL ),

	m_pGLGaeaServer( NULL ),
	m_bItemShopOpen ( false ),
	m_bDefenseSkill ( false )
{


	m_sSaveMapID = NATIVEID_NULL();
	m_ARoundSlot.SetChar ( this );


	m_China15Time = CTime::GetCurrentTime();
	DWORD nMaxClient = GLGaeaServer::GetInstance().GetMaxClient();

	GASSERT(nMaxClient);
	m_arrayFieldPC = new PSFIELDCROW[nMaxClient];

	memset ( m_arrayFieldPC, 0, sizeof(PSFIELDCROW)*nMaxClient );
	memset ( m_arrayFieldCROW, 0, sizeof(PSFIELDCROW)*MAXCROW );
	memset ( m_arrayFieldMATERIAL, 0, sizeof(PSFIELDCROW)*MAXCROW );
	memset ( m_arrayFieldITEM, 0, sizeof(PSFIELDCROW)*MAXITEM );
	memset ( m_arrayFieldMONEY, 0, sizeof(PSFIELDCROW)*MAXMONEY );

	m_setLandEffectNum.clear();


	// PET
	m_arrayFieldPET = new PSFIELDCROW[nMaxClient];
	memset ( m_arrayFieldPET, 0, sizeof(PSFIELDCROW)*nMaxClient );

	m_arrayFieldSummon = new PSFIELDCROW[nMaxClient];
	memset ( m_arrayFieldSummon, 0, sizeof(PSFIELDCROW)*nMaxClient );

	MGOddEvenInit();

#ifdef TH_PARAM
	HMODULE m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}
#endif
}

GLChar::~GLChar ()
{
	delete[] m_arrayFieldPC;

	// PET
	delete[] m_arrayFieldPET;


	// Summon
	delete[] m_arrayFieldSummon;

#ifdef TH_PARAM
	if( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif
}

DWORD GLChar::GetBonusExp () const
{
	return GLCONST_CHAR::GETEXP_PC(m_wLevel);
}

void GLChar::RESET_DATA ()
{
	int i = 0;

	GLCHARLOGIC::RESET_DATA();

	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_vDir = D3DXVECTOR3(1,0,0);

	m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);
	m_sNPCID   = STARGETID(CROW_PC,GAEAID_NULL);
	m_wTARNUM = 0;

	for ( i=0; i<EMTARGET_NET; ++i )
		m_sTARIDS[i] = STARID(CROW_PC,GAEAID_NULL);

	m_dwANISUBSELECT = 0;

	m_Action = GLAT_IDLE;
	m_dwActState = NULL;
	m_fIdleTime = 0.0f;
	m_fMBR_TIMER = 0.0f;
	m_fSTATE_TIMER = 0.0f;
	m_fPERIOD_TIMER = 0.0f;
	m_fQUEST_TIMER = 60.0f;	// 처음 한번 무조건 실행되게 하기 위하여

	m_fattTIMER = 0.0f;
	m_fBRIGHT_TIMER = 0.0f;

	m_fMoveDelay = 0.0f;
	m_sAssault = STARGETID(CROW_PC,GAEAID_NULL);

	m_ARoundSlot.Reset();

	m_cDamageLog.clear();

	m_dwThaiCCafeClass	= 0;
	// 말레이시아 PC방 이벤트
	m_nMyCCafeClass		= 0;

	m_bVietnamLevelUp = false;

	m_ChinaGainType    = 255;

	m_bTracingUser     = FALSE;

	m_dwVietnamGainType = GAINTYPE_MAX;

	m_bEventStart	   = FALSE;

	m_bEventApply	   = FALSE;

	m_sChinaTime.Init();
	m_sEventTime.Init();
	m_sVietnamSystem.Init();

	

	m_fAge = 0.0f;
	m_fGenAge = 0.0f;
	m_vPos = D3DXVECTOR3(0,0,0);

	m_dwClientID = 0;
	m_dwGaeaID = GAEAID_NULL;
	m_dwPartyID = PARTY_NULL;

	m_sMapID    = SNATIVEID(false);
	m_sOldMapID = SNATIVEID(false);
	m_dwCeID = 0;
	
	m_pLandMan = NULL;

	m_pGaeaNode = NULL;
	m_pLandNode = NULL;

	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_sTrade.Reset();
	m_sPMarket.DoMarketClose();
	m_sCONFTING.RESET();

	m_sHITARRAY.clear();
	m_SKILLMTYPE = AN_SKILL;
	m_SKILLSTYPE = AN_SUB_00;

	m_bUsedStorageCard = false;

	m_bNon_Rebirth     = false;

	m_bGetPetFromDB    = false;

	m_dwPetGUID = UINT_MAX;
	m_dwPetID	= 0;

	m_dwSummonGUID = UINT_MAX;
	m_dwSummonID   = 0;

	m_bEntryFailed = FALSE;

	m_setLandEffectNum.clear();

	m_bProtectPutOnItem = false;

	m_China15Time = CTime::GetCurrentTime();

	MGOddEvenInit();

	m_sVehicle.RESET();
	m_bGetVehicleFromDB = false;
	m_fVehicleTimer = 0.0f;

	m_bItemShopOpen = false;
	m_bDefenseSkill = false;

	DWORD nMaxClient = GLGaeaServer::GetInstance().GetMaxClient();


	memset ( m_arrayFieldPC, 0, sizeof(PSFIELDCROW)*nMaxClient );
	memset ( m_arrayFieldCROW, 0, sizeof(PSFIELDCROW)*MAXCROW );
	memset ( m_arrayFieldMATERIAL, 0, sizeof(PSFIELDCROW)*MAXCROW );
	memset ( m_arrayFieldITEM, 0, sizeof(PSFIELDCROW)*MAXITEM );
	memset ( m_arrayFieldMONEY, 0, sizeof(PSFIELDCROW)*MAXMONEY );

	memset ( m_arrayFieldPET, 0, sizeof(PSFIELDCROW)*nMaxClient );
	memset ( m_arrayFieldSummon, 0, sizeof(PSFIELDCROW)*nMaxClient );
}

HRESULT GLChar::CreateChar( GLLandMan* pLandMan, D3DXVECTOR3 &vPos, PCHARDATA2 pCharData, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bNEW )
{
	HRESULT hr;
	//*/*/*/*
	m_vPos = vPos;

	

	m_dwThaiCCafeClass = pCharData->m_dwThaiCCafeClass;
	// 말레이시아 PC방 이벤트
	m_nMyCCafeClass = pCharData->m_nMyCCafeClass;

	m_sChinaTime	   = pCharData->m_sChinaTime;
	m_sEventTime	   = pCharData->m_sEventTime;
	m_sVietnamSystem   = pCharData->m_sVietnamSystem;

	m_bTracingUser	   = pCharData->m_bTracingUser;

//	m_sEventState	   = m_pGLGaeaServer->m_sEventState;

	m_bEventStart = FALSE;
	m_bEventApply = FALSE;
	if( m_pGLGaeaServer->m_sEventState.bEventStart == TRUE )
	{
		// 이벤트가 새로 시작 됨
		CTime	  crtTime     = CTime::GetCurrentTime();
		CTimeSpan crtGameSpan;
		CTime	  loginTime			 = m_sEventTime.loginTime;
		crtGameSpan					 = ( crtTime - loginTime );
		m_sEventTime.currentGameTime = crtGameSpan.GetTimeSpan();

		if( crtGameSpan.GetTotalSeconds() >= m_pGLGaeaServer->m_sEventState.EventPlayTime )
		{
			m_bEventStart = TRUE;
		}

		if( !(m_pGLGaeaServer->m_sEventState.MinEventLevel > pCharData->m_wLevel || 
			  m_pGLGaeaServer->m_sEventState.MaxEventLevel < pCharData->m_wLevel ) )
		{
			m_bEventApply = TRUE;
		}
	}


	hr = SetData ( pCharData, bNEW );
	if ( FAILED(hr) )	return E_FAIL;

	m_bEntryFailed = FALSE;
	// 주석돌려
	// 진입조건 검사
	if ( m_dwUserLvl < USER_GM3 )
	{
		if( pLandMan->GetMapID() != NATIVEID_NULL() )
		{
			const SLEVEL_REQUIRE* pLEVEL_REQUIRE = GLGaeaServer::GetInstance().GetLevelRequire(pLandMan->GetMapID().dwID);
			EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
			if( pLEVEL_REQUIRE ) 
			{
				emReqFail = pLEVEL_REQUIRE->ISCOMPLETE ( this );
			}else{
				TEXTCONSOLEMSG_WRITE( "[ERROR]Level Data Load failed! charID[%s] mapID[%d][%d] EMFail[%d]", m_szName, pLandMan->GetMapID().wMainID, pLandMan->GetMapID().wSubID, emReqFail );
			}
			if ( emReqFail != EMREQUIRE_COMPLETE )
			{
				TEXTCONSOLEMSG_WRITE( "[INFO]Entry failed! charID[%s] mapID[%d][%d] EMFail[%d]", m_szName, pLandMan->GetMapID().wMainID, pLandMan->GetMapID().wSubID, emReqFail );
				m_bEntryFailed = TRUE;
			}
		}else{
			TEXTCONSOLEMSG_WRITE( "[ERROR]Land ID is NATIVEID NULL! charID[%s]", m_szName );
		}
	}


	m_setLandEffectNum.clear();
	


	hr = SetNavi ( pLandMan->GetNavi(), m_vPos );
	if ( FAILED(hr) )	return E_FAIL;

	m_lnLastSendExp = m_sExperience.lnNow;

	m_lnLastSendVNExp = m_lVNGainSysExp;

	m_bServerStorage = TRUE;	

	DWORD Num1 = m_cInventory.GetNumItems();
	DWORD Num2 = m_cVietnamInventory.GetNumItems ();

	m_fMoveDelay = 0.0f;
	CTime cTIME_CUR = CTime::GetCurrentTime();

	//	Note : 창고의 유효성 검사.
	//
	for ( WORD i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
	{
		CTime cSTORAGE(m_tSTORAGE[i]);

		bool bVALID(true);
		if ( m_tSTORAGE[i]==0 || cSTORAGE<cTIME_CUR )	bVALID = false;
		
		m_bSTORAGE[i] = bVALID;
	}

	//	Note : 프리미엄 여부 검사.
	//
	bool bPREMIUM(true);
	CTime tPREMIUM(m_tPREMIUM);
	if ( m_tPREMIUM==0 || tPREMIUM<cTIME_CUR )	bPREMIUM = false;
	m_bPREMIUM = bPREMIUM;

	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );
	// 베트남 인벤토리는 세로가 10칸이므로 6칸을 더 추가한다.
#if defined(VN_PARAM) //vietnamtest%%%
	m_cVietnamInventory.SetAddLine ( 6, true );
#endif


	//DoQuestCheckLeaveMap();


	SetSTATE(EM_ACT_WAITING);

	


	return S_OK;
}

HRESULT GLChar::SetNavi ( NavigationMesh* pNavi, D3DXVECTOR3 &vPos )
{
	m_vPos = vPos;
	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();

	m_actorMove.Create ( pNavi, vPos, -1 );
	m_actorMove.SetMaxSpeed ( GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO );

	return S_OK;
}

HRESULT GLChar::SetData ( PCHARDATA2 pCharData, BOOL bNEW )
{
	SCHARDATA2::Assign ( *pCharData );
	
	if ( !GLCHARLOGIC::INIT_DATA(bNEW,TRUE) )	return E_FAIL;

	return S_OK;
}

HRESULT GLChar::SavePosition ()
{
	//	Note : 죽은상태일 때는 현제 포지션을 저장하지 않는다.
	//
	if ( IsDie() )
	{
		m_sSaveMapID = NATIVEID_NULL();
	}
	//	Note : 현제 위치 저장.
	//
	else
	{
		m_sSaveMapID = m_sMapID;
		m_vSavePos = m_vPos;
	}

	return S_OK;
}

//	Note : 직전 위치를 저장.
//
HRESULT GLChar::SaveLastCall ()
{
	if ( !m_pLandMan )	return S_FALSE;

	//	Note : 만약 선도 클럽전용 멥일 경우 직전 위치를 저장하지 않음.
	if ( !m_pLandMan->m_bGuidBattleMap && !m_pLandMan->m_bClubDeathMatchMap )
	{
		m_sLastCallMapID = m_sMapID;
		m_vLastCallPos = m_vPos;

		GLMSG::SNETPC_UPDATE_LASTCALL NetMsg;
		NetMsg.sLastCallMapID = m_sLastCallMapID;
		NetMsg.vLastCallPos = m_vLastCallPos;
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
	}

	return S_OK;
}

HRESULT GLChar::LoadStorageFromDB ()
{
	if ( m_bServerStorage )	return S_OK;

	GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan();
	if ( pDBMan )	pDBMan->ReadUserInven ( &GetCharData2() );
	m_bServerStorage = TRUE;

	return S_OK;
}

HRESULT GLChar::SetPosition ( D3DXVECTOR3 &vPos )
{
	m_actorMove.SetPosition ( vPos, -1 );
	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();

	m_vPos = m_actorMove.Position();

	return S_OK;
}

BOOL GLChar::IsValidBody () const
{
	if ( IsSTATE(EM_ACT_DIE) )			return FALSE;
	if ( IsSTATE(EM_ACT_WAITING) )		return FALSE;
	if ( IsSTATE(EM_REQ_LOGOUT) )		return FALSE;

	return ( m_Action < GLAT_FALLING );
}

BOOL GLChar::IsDie () const
{
	if ( !IsSTATE(EM_ACT_WAITING) )
	{
		if ( m_Action==GLAT_DIE )		return TRUE;
		if ( IsSTATE(EM_ACT_DIE) )		return TRUE;
	}

	return FALSE;
}

BOOL GLChar::IsVisibleDetect ( const BOOL bRECVISIBLE )
{
	if ( IsSTATE(EM_REQ_VISIBLENONE) || IsSTATE(EM_REQ_VISIBLEOFF) )	return FALSE;

	if ( m_bINVISIBLE && !bRECVISIBLE )									return FALSE;

	return TRUE;
}

BOOL GLChar::IsVaildTradeData ()
{
	//	금액 유효성 검사.
	if ( m_lnMoney < m_sTrade.GetMoney() )		return FALSE;

	GLInventory &sTradeBox = m_sTrade.GetTradeBox();
	GLInventory::CELL_MAP* pItemList = sTradeBox.GetItemList();

	GLInventory::CELL_MAP_ITER iter = pItemList->begin();
	GLInventory::CELL_MAP_ITER iter_end = pItemList->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;
		SINVENITEM* pInvenItem = m_cInventory.GetItem ( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !pInvenItem )														return FALSE;

		if ( memcmp ( &pTradeItem->sItemCustom, &pInvenItem->sItemCustom, sizeof(SITEMCUSTOM) ) )	return FALSE;
	}

	return TRUE;
}

BOOL GLChar::IsVaildTradeInvenSpace ( GLInventory &sTradeBoxTar )
{
	GLInventory sInvenTemp;
	sInvenTemp.SetAddLine ( m_cInventory.GETAddLine(), true );
	sInvenTemp.Assign ( m_cInventory );

	//	Note : 자신의 교환 목록에 올라간 아이템을 인밴에서 제외.
	//
	GLInventory &sTradeBoxMy = m_sTrade.GetTradeBox();
	GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();

	GLInventory::CELL_MAP_ITER iter = pItemListMy->begin();
	GLInventory::CELL_MAP_ITER iter_end = pItemListMy->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		BOOL bOk = sInvenTemp.DeleteItem ( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !bOk )		return FALSE;
	}

	//	Note : 상대방의 교환 물품이 들어 갈수 있는지 검사.
	//
	GLInventory::CELL_MAP* pItemListTar = sTradeBoxTar.GetItemList();

	iter = pItemListTar->begin();
	iter_end = pItemListTar->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		SITEM *pItem = GLItemMan::GetInstance().GetItem ( pTradeItem->sItemCustom.sNativeID );
		if ( !pItem )	return FALSE;

		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = sInvenTemp.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )		return FALSE;

		bOk = sInvenTemp.InsertItem ( pTradeItem->sItemCustom, wPosX, wPosY );
		if ( !bOk )		return FALSE;
	}
	
	return TRUE;
}

BOOL GLChar::DoTrade ( GLTrade &sTrade, DWORD dwFromCharID )
{
	//	Note : 줄 금액.
	//
	LONGLONG lnMoneyOut = m_sTrade.GetMoney();
	if ( m_lnMoney < lnMoneyOut ) return FALSE;
	CheckMoneyUpdate( m_lnMoney, lnMoneyOut, FALSE, "Do Trade Out Money." );
	m_lnMoney -= lnMoneyOut;	

	//	Note : 받을 금액.
	//
	LONGLONG lnMoneyIn = sTrade.GetMoney();
	CheckMoneyUpdate( m_lnMoney, lnMoneyIn, TRUE, "Do Trade In Money." );
	m_lnMoney += lnMoneyIn;

	m_bMoneyUpdate = TRUE;

	//	Note : 금액 로그.
	//
	LONGLONG lnMoneyDx = (lnMoneyIn-lnMoneyOut);
	if ( lnMoneyDx!=0 )
	{
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, dwFromCharID, ID_CHAR, m_dwCharID, lnMoneyDx, EMITEM_ROUTE_CHAR );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if ( m_bTracingUser )
	{
		NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

		CString strTemp;
		PGLCHAR pFromChar = m_pGLGaeaServer->GetCharID( dwFromCharID );
		if ( pFromChar != NULL )
		{
			strTemp.Format( "Do Trade! From, [%s][%s], Target[%s][%s], RecvMoney:[%I64d], SendMoney:[%I64d], HaveMoney:[%I64d], Target HaveMoney:[%I64d]",
					 m_szUID, m_szName, pFromChar->m_szUID, pFromChar->m_szName, lnMoneyIn, lnMoneyOut, m_lnMoney, pFromChar->m_lnMoney );
		}else{
			strTemp.Format( "Do Trade! From, [%s][%s], Target Not Found, RecvMoney:[%I64d], SendMoney:[%I64d], HaveMoney:[%I64d]",
					 m_szUID, m_szName, lnMoneyIn, lnMoneyOut, m_lnMoney );
		}

		StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

		m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
	}
#endif

	//	Note : 금액 변화 클라이언트에 알림.
	//
	GLMSG::SNETPC_UPDATE_MONEY NetMsgMoney;
	NetMsgMoney.lnMoney = m_lnMoney;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgMoney );


	//	Note : 자신의 교환 목록에 올라간 아이템을 인밴에서 제외.
	//
	GLInventory &sTradeBoxMy = m_sTrade.GetTradeBox();
	GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();
	
	SINVENITEM* pTradeItem = NULL;
	BOOL bOk = FALSE;
	GLMSG::SNETPC_INVEN_DELETE NetMsgDelete;
	GLInventory::CELL_MAP_ITER iter = pItemListMy->begin();
	GLInventory::CELL_MAP_ITER iter_end = pItemListMy->end();
	for ( ; iter!=iter_end; ++iter )
	{
		pTradeItem = (*iter).second;

		bOk = m_cInventory.DeleteItem ( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !bOk )
		{
			return FALSE;
		}
		else
		{
			//	Note : 아이템 변화를 클라이언트에 알림.
			//			
			NetMsgDelete.wPosX = pTradeItem->wBackX;
			NetMsgDelete.wPosY = pTradeItem->wBackY;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgDelete );
		}
	}

	//	Note : 상대방이 주는 물품을 인밴에 넣음.
	//
	GLInventory::CELL_MAP* pItemListTar = sTrade.GetTradeBox().GetItemList();

	pTradeItem = NULL;
	SITEM* pItem = NULL;
	WORD wPosX = 0, wPosY = 0;

	SINVENITEM* pInvenItem = NULL;
	GLMSG::SNETPC_INVEN_INSERT NetMsgInert;

	iter = pItemListTar->begin();
	iter_end = pItemListTar->end();	
	for ( ; iter!=iter_end; ++iter )
	{
		pTradeItem = (*iter).second;
		pItem = GLItemMan::GetInstance().GetItem ( pTradeItem->sItemCustom.sNativeID );
		if ( !pItem )	return FALSE;

		wPosX = 0;
		wPosY = 0;
		bOk = m_cInventory.FindInsrtable ( pItem->sBasicOp.wInvenSizeX, pItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
		if ( !bOk )		return FALSE;

		bOk = m_cInventory.InsertItem ( pTradeItem->sItemCustom, wPosX, wPosY );
		if ( !bOk )		return FALSE;


/*
		// 획득한 아이템이 팻카드 && 생성된 팻이라면 소유권을 이전한다.
		// 팻아이디가 0 이 아니면 팻카드이면서 DB에 팻이 생성된 것이다.
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD && pTradeItem->sItemCustom.dwPetID != 0 )
		{
			CExchangePet* pDbAction = new CExchangePet ( m_dwCharID, pTradeItem->sItemCustom.dwPetID );
			GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
			if ( pDBMan ) pDBMan->AddJob ( pDbAction );
		}
*/

		pInvenItem = m_cInventory.GetItem ( wPosX, wPosY );

		//	Note :아이템의 소유 이전 경로 기록.
		//
		GLITEMLMT::GetInstance().ReqItemRoute ( pInvenItem->sItemCustom, ID_CHAR, dwFromCharID, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum );

		//	Note : 아이템 변화를 클라이언트에 알림.
		//		
		NetMsgInert.Data = *pInvenItem;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgInert );
	}

	return TRUE;
}

// *****************************************************
// Desc: 시간제한 아이템 업데이트
// *****************************************************
void GLChar::RESET_TIMELMT_ITEM ()
{
	std::vector<GLInventory::CELL_KEY>	vecDELETE;

	const CTime cTIME_CUR = CTime::GetCurrentTime();

	SINVENITEM* pInvenItem = NULL;
	SITEM* pITEM = NULL;
	GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;

	// 팻 아이템 슬롯
	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( pMyPet && pMyPet->IsValid () ) pMyPet->UpdateTimeLmtItem ( this );

	// 탈것 아이템 슬롯 
	if ( m_sVehicle.IsActiveValue() ) m_sVehicle.UpdateTimeLmtItem( this );

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;
	GLITEMLMT & glItemmt = GLITEMLMT::GetInstance();
	GLItemMan & glItemMan = GLItemMan::GetInstance();
	
	// 인벤토리 아이템
	GLInventory::CELL_MAP* pMapItem = m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
	GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
	for ( ; iter!=iter_end; ++iter )
	{
		pInvenItem = (*iter).second;
		pITEM = glItemMan.GetItem ( pInvenItem->sItemCustom.sNativeID );
		if ( !pITEM )	continue;

		bool bDELETE(false);

		// 시한부 아이템
		if ( pITEM->IsTIMELMT() )
		{
			CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
			CTime cTIME_LMT(pInvenItem->sItemCustom.tBORNTIME);
			cTIME_LMT += cSPAN;

			if ( cTIME_CUR > cTIME_LMT )
			{
				vecDELETE.push_back ( (*iter).first );
				bDELETE = true;

				if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.dwVehicleID != 0 )
				{
					// 악세사리를 체크후 팻을 DB에서 삭제해준다.
					GLVEHICLE* pNewVehicle = new GLVEHICLE();
					CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
													 pInvenItem->sItemCustom.dwVehicleID, 
													 m_dwClientID, 
													 m_dwCharID, 
													 true );
					GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );				

				}

				// 팻카드일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.dwPetID != 0 )
				{
					// 활동중이면 사라지게 해준다.
					if ( pMyPet && pInvenItem->sItemCustom.dwPetID == pMyPet->m_dwPetID ) 
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

						CDeletePet* pDeletePet = new CDeletePet ( m_dwCharID, pInvenItem->sItemCustom.dwPetID );
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
														 pInvenItem->sItemCustom.dwPetID, 
														 m_dwClientID, 
														 m_dwCharID,
														 pInvenItem->wPosX,
														 pInvenItem->wPosY,
														 true );
						GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
						if ( pDBMan ) pDBMan->AddJob ( pGetPet );
					}
				}
			}
		}

		// 코스튬
		if ( !bDELETE && pInvenItem->sItemCustom.nidDISGUISE!=SNATIVEID(false) )
		{
			if ( pInvenItem->sItemCustom.tDISGUISE != 0 )
			{
				CTime cTIME_LMT(pInvenItem->sItemCustom.tDISGUISE);
				if ( cTIME_LMT.GetYear()!=1970 )
				{
					if ( cTIME_CUR > cTIME_LMT )
					{
						//	시간 제한으로 아이템 삭제 알림.						
						NetMsgInvenDelTimeLmt.nidITEM = pInvenItem->sItemCustom.nidDISGUISE;
						m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

						//	Note : 코스툼 정보 리셋.
						pInvenItem->sItemCustom.tDISGUISE = 0;
						pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID(false);

						//	Note : 클라이언트에 알림.						
						NetItemUpdate.wPosX = pInvenItem->wPosX;
						NetItemUpdate.wPosY = pInvenItem->wPosY;
						NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
						m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetItemUpdate);

						//	Note : 로그 저장.
						glItemmt.ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );
					}
				}
			}
		}
	}

	if ( !vecDELETE.empty() )
	{
		for ( DWORD i=0; i<vecDELETE.size(); ++i )
		{
			const GLInventory::CELL_KEY &cKEY = vecDELETE[i];
			SINVENITEM sINVENITEM = *m_cInventory.GetItem(cKEY.first,cKEY.second);

			SNATIVEID nidITEM = sINVENITEM.sItemCustom.sNativeID;

			//	시간 제한으로 아이템 삭제 로그 남김.
			glItemmt.ReqItemRoute ( sINVENITEM.sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sINVENITEM.sItemCustom.wTurnNum );

			//	Note : 아이템 삭제.
			//
			m_cInventory.DeleteItem ( cKEY.first, cKEY.second );

			//	[자신에게] 해당 아이탬을 인밴에서 삭제.
			GLMSG::SNETPC_INVEN_DELETE NetMsgInvenDel;
			NetMsgInvenDel.wPosX = cKEY.first;
			NetMsgInvenDel.wPosY = cKEY.second;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDel);

			//	시간 제한으로 아이템 삭제 알림.
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
			NetMsgInvenDelTimeLmt.nidITEM = nidITEM;
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

			//	Note : 코스툼 복구.
			if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
			{
				SITEM *pONE = glItemMan.GetItem ( sINVENITEM.sItemCustom.nidDISGUISE );
				if ( !pONE )		continue;

				//	인벤 위치.
				WORD wInsertPosX = cKEY.first, wInsertPosY = cKEY.second;

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
				sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
				sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
				sITEM_NEW.lnGenNum = glItemmt.RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

				//	Note : 인벤에 들어갈 공간 검사.
				BOOL bOk = m_cInventory.IsInsertable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
				if ( !bOk )		continue;

				//	Note : 인벤에 넣음.
				m_cInventory.InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
				SINVENITEM *pINSERT_ITEM = m_cInventory.GetItem ( wInsertPosX, wInsertPosY );

				GLMSG::SNETPC_INVEN_INSERT NetItemInsert;
				NetItemInsert.Data = *pINSERT_ITEM;
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetItemInsert);

				//	Note : 분리된 코스툼 로그.
				glItemmt.ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );
			}
		}
	}

	// 아이템 슬롯 정보 갱신
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		EMSLOT emSLOT = (EMSLOT) i;
		if( m_PutOnItems[emSLOT].sNativeID==NATIVEID_NULL() ) continue;

		SITEMCUSTOM sCUSTOM = m_PutOnItems[emSLOT];

		SITEM *pITEM = glItemMan.GetItem ( sCUSTOM.sNativeID );
		if ( !pITEM )								continue;

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

				if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sCUSTOM.dwVehicleID != 0 )
				{
					m_pGLGaeaServer->SaveVehicle( m_dwClientID, m_dwGaeaID, false );

                	// 악세사리를 체크후 팻을 DB에서 삭제해준다.
					GLVEHICLE* pNewVehicle = new GLVEHICLE();
					CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
													 sCUSTOM.dwVehicleID, 
													 m_dwClientID, 
													 m_dwCharID, 
													 true );

					GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
					if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );
				}

				//	아이템 제거.
				RELEASE_SLOT_ITEM ( emSLOT );

				//	[자신에게]  아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release(emSLOT);
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_Release);

				//	Note : 주변 사람들에게 아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsgReleaseBrd;
				NetMsgReleaseBrd.dwGaeaID = m_dwGaeaID;
				NetMsgReleaseBrd.emSlot = emSLOT;
				SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgReleaseBrd) );

				bDELETE = true;

				//	Note : 코스툼 복구.
				if ( sCUSTOM.nidDISGUISE!=SNATIVEID(false) )
				{
					SITEM *pONE = glItemMan.GetItem ( sCUSTOM.nidDISGUISE );
					if ( !pONE )		continue;

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

					//	Note : 복장 착용.
					SLOT_ITEM ( sITEM_NEW, emSLOT );

					//	Note : 자신에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
					NetMsgUpdate.emSlot = emSLOT;
					NetMsgUpdate.sItemCustom = sITEM_NEW;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgUpdate);

					//	Note : 주변 사람들에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsgUpdateBrd;
					NetMsgUpdateBrd.dwGaeaID = m_dwGaeaID;
					NetMsgUpdateBrd.emSlot = emSLOT;
					NetMsgUpdateBrd.sItemClient.Assign ( sITEM_NEW );
					SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgUpdateBrd) );

					//	Note : 분리된 코스툼 로그.
					glItemmt.ReqItemRoute ( sITEM_NEW, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum );
				}
			}
		}
		
		if ( !bDELETE && VALID_SLOT_ITEM(emSLOT) && sCUSTOM.nidDISGUISE!=SNATIVEID(false) )
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
						sCUSTOM.tDISGUISE = 0;
						sCUSTOM.nidDISGUISE = SNATIVEID(false);
						SLOT_ITEM ( sCUSTOM, emSLOT );			//	실제로 적용.

						//	Note : 자신에게 착용아이템 변경.
						GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
						NetMsgUpdate.emSlot = emSLOT;
						NetMsgUpdate.sItemCustom = sCUSTOM;
						m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgUpdate);

						//	Note : 주변 사람들에게 착용아이템 변경.
						GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsgUpdateBrd;
						NetMsgUpdateBrd.dwGaeaID = m_dwGaeaID;
						NetMsgUpdateBrd.emSlot = emSLOT;
						NetMsgUpdateBrd.sItemClient.Assign ( sCUSTOM );
						SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgUpdateBrd) );
					}
				}
			}
		}
	}

	//	Note : 인벤토리 추가라인 유효성 검사.
	//
	GLMSG::SNETPC_STORAGE_STATE NetMsgStorageState;
	for ( WORD i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
	{
		bool bVALID(true);
		CTime cSTORAGE(m_tSTORAGE[i]);

		if ( m_tSTORAGE[i]==0 )		bVALID = false;
		if ( cSTORAGE<cTIME_CUR )
		{
			bVALID = false;
		}

		m_bSTORAGE[i] = bVALID;
		NetMsgStorageState.bVALID[i] = bVALID;
	}
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgStorageState);

	bool bPREMIUM(true);
	CTime cPREMIUM(m_tPREMIUM);
	if ( m_tPREMIUM==0 )			bPREMIUM = false;
	if ( cPREMIUM<cTIME_CUR )
	{
		bPREMIUM = false;
	}

	m_bPREMIUM = bPREMIUM;

	GLMSG::SNETPC_PREMIUM_STATE NetMsgPremium;
	NetMsgPremium.bPREMIUM = bPREMIUM;
	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgPremium);

	//	Note : 현재 활성화된 인벤 라인 설정.
	//
	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );

	// 창고 아이템 유효성 갱신
	for ( WORD idx = 0; idx < EMSTORAGE_CHANNEL; ++idx )
	{
		GLInventory::CELL_MAP* pMapItem = m_cStorage[idx].GetItemList();
		GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
		GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();

		vecDELETE.clear();

		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM *pInvenItem = (*iter).second;

			SITEM *pITEM = glItemMan.GetItem ( pInvenItem->sItemCustom.sNativeID );
			if ( !pITEM )								continue;

			bool bDELETE(false);

			// 시한부 아이템
			if ( pITEM->IsTIMELMT() )
			{
				CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
				CTime cTIME_LMT(pInvenItem->sItemCustom.tBORNTIME);
				cTIME_LMT += cSPAN;

				if ( cTIME_CUR > cTIME_LMT )
				{
					vecDELETE.push_back ( (*iter).first );
					bDELETE = true;

					// 팻카드일 경우
					if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.dwPetID != 0 )
					{
						// 악세사리를 체크후 팻을 DB에서 삭제해준다.
						GLPET* pNewPet = new GLPET ();
						CGetPet* pGetPet = new CGetPet ( pNewPet, 
														 pInvenItem->sItemCustom.dwPetID, 
														 m_dwClientID, 
														 m_dwCharID,
														 pInvenItem->wPosX,
														 pInvenItem->wPosY,
														 true );
						GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
						if ( pDBMan ) pDBMan->AddJob ( pGetPet );
					}

					// 탈것일 경우
					if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.dwVehicleID != 0 )
					{
						// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
						GLVEHICLE* pNewVehicle = new GLVEHICLE();
						CGetVehicle* pGetVehicle = new CGetVehicle ( pNewVehicle, 
													 pInvenItem->sItemCustom.dwVehicleID, 
													 m_dwClientID,
													 m_dwCharID, 
													 true );
						GLDBMan* pDBMan = m_pGLGaeaServer->GetDBMan ();
						if ( pDBMan ) pDBMan->AddJob ( pGetVehicle );		
					}
				}
			}
			
			// 코스튬
			if ( !bDELETE && pInvenItem->sItemCustom.nidDISGUISE!=SNATIVEID(false) )
			{
				if ( pInvenItem->sItemCustom.tDISGUISE != 0 )
				{
					CTime cTIME_LMT(pInvenItem->sItemCustom.tDISGUISE);
					if ( cTIME_LMT.GetYear()!=1970 )
					{
						if ( cTIME_CUR > cTIME_LMT )
						{
							//	시간 제한으로 아이템 삭제 알림.
							GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
							NetMsgInvenDelTimeLmt.nidITEM = pInvenItem->sItemCustom.nidDISGUISE;
							m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

							//	Note : 코스툼 정보 리셋.
							pInvenItem->sItemCustom.tDISGUISE = 0;
							pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID(false);

							//	Note : 클라이언트에 알림.
							GLMSG::SNETPC_STORAGE_ITEM_UPDATE NetItemUpdate;
							NetItemUpdate.dwChannel = idx;
							NetItemUpdate.wPosX = pInvenItem->wPosX;
							NetItemUpdate.wPosY = pInvenItem->wPosY;
							NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
							m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetItemUpdate);

							//	Note : 로그 저장.
							glItemmt.ReqItemConversion ( pInvenItem->sItemCustom, ID_CHAR, m_dwCharID );
						}
					}
				}
			}
		}

		if ( !vecDELETE.empty() )
		{
			for ( DWORD i=0; i<vecDELETE.size(); ++i )
			{
				const GLInventory::CELL_KEY &cKEY = vecDELETE[i];
				SINVENITEM sINVENITEM = *m_cStorage[idx].GetItem(cKEY.first,cKEY.second);

				SNATIVEID nidITEM = sINVENITEM.sItemCustom.sNativeID;

				//	시간 제한으로 아이템 삭제 로그 남김.
				glItemmt.ReqItemRoute ( sINVENITEM.sItemCustom, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_DELETE, sINVENITEM.sItemCustom.wTurnNum );

				//	Note : 아이템 삭제.
				//
				m_cStorage[idx].DeleteItem ( cKEY.first, cKEY.second );

				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNETPC_STORAGE_DELETE NetMsgStorageDel;
				NetMsgStorageDel.dwChannel = idx;
				NetMsgStorageDel.wPosX = cKEY.first;
				NetMsgStorageDel.wPosY = cKEY.second;
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgStorageDel);

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = nidITEM;
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgInvenDelTimeLmt);

				//	Note : 코스툼 복구.
				if ( sINVENITEM.sItemCustom.nidDISGUISE!=SNATIVEID(false) )
				{
					SITEM *pONE = glItemMan.GetItem ( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )		continue;

					//	인벤 위치.
					WORD wInsertPosX = cKEY.first, wInsertPosY = cKEY.second;

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
					sITEM_NEW.cChnID = (BYTE)m_pGLGaeaServer->GetServerChannel();
					sITEM_NEW.cFieldID = (BYTE)m_pGLGaeaServer->GetFieldSvrID();
					sITEM_NEW.lnGenNum = glItemmt.RegItemGen ( sITEM_NEW.sNativeID, (EMITEMGEN)sITEM_NEW.cGenType );

					//	Note : 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cStorage[idx].IsInsertable ( pONE->sBasicOp.wInvenSizeX, pONE->sBasicOp.wInvenSizeY, wInsertPosX, wInsertPosY );
					if ( !bOk )		continue;

					//	Note : 인벤에 넣음.
					m_cStorage[idx].InsertItem ( sITEM_NEW, wInsertPosX, wInsertPosY );
					SINVENITEM *pINSERT_ITEM = m_cStorage[idx].GetItem ( wInsertPosX, wInsertPosY );

					GLMSG::SNETPC_STORAGE_INSERT NetItemInsert;
					NetItemInsert.dwChannel = idx;
					NetItemInsert.Data = *pINSERT_ITEM;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetItemInsert);

					//	Note : 분리된 코스툼 로그.
					glItemmt.ReqItemRoute ( pINSERT_ITEM->sItemCustom, ID_CHAR, 0, ID_CHAR, m_dwCharID, EMITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum );
				}
			}
		}
	}
}




// *****************************************************
// Desc: 아이템 조건 체크 ( 시간제한과 같이 작동 ) 
// *****************************************************
void GLChar::RESET_CHECK_ITEM ()
{

	// 아이템 슬롯 착용 조건을 체크한다.
	for ( int i=0; i<SLOT_HOLD; ++i )
	{
		EMSLOT emSLOT = (EMSLOT) i;
		
		if ( m_PutOnItems[emSLOT].sNativeID == NATIVEID_NULL() ) continue;

		SITEMCUSTOM sCUSTOM = m_PutOnItems[emSLOT];

		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sCUSTOM.sNativeID );
		if ( !pITEM )	continue;

		bool bRelease(false);

		// 비코스튬
		if ( sCUSTOM.nidDISGUISE == NATIVEID_NULL() )
		{
			if ( !SIMPLE_CHECK_ITEM( sCUSTOM.sNativeID ) )	bRelease = true;
		}
		// 코스튬
		else 
		{
			if ( !SIMPLE_CHECK_ITEM ( sCUSTOM.sNativeID, sCUSTOM.nidDISGUISE ) ) bRelease = true;
		}

		if ( bRelease ) 
		{
			EMSLOT emRHand = GetCurRHand();
			EMSLOT emLHand = GetCurLHand();

			if ( emSLOT == emRHand ) 
			{
				SITEM *pITEM_LEFT=NULL, *pITEM_RIGHT=NULL;
				if ( VALID_SLOT_ITEM(emLHand) )	pITEM_LEFT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emLHand).sNativeID);
				if ( VALID_SLOT_ITEM(emRHand) )	pITEM_RIGHT = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emRHand).sNativeID);

				if ( (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_BOW) || (pITEM_RIGHT->sSuitOp.emAttack==ITEMATT_SPEAR) )
				{
					if ( pITEM_LEFT )	//	화살, 부적 빼내야 함.
					{
						SITEMCUSTOM sITEM_LHAND = GET_SLOT_ITEM(emLHand);

						CItemDrop cDropItem;
						cDropItem.sItemCustom = m_PutOnItems[emLHand];

						if ( IsInsertToInvenEx ( &cDropItem ) )
						{
							InsertToInvenEx( &cDropItem );
						}
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

						//	왼쪽 슬롯 지워줌.
						RELEASE_SLOT_ITEM(emLHand);

						//	[자신에게] SLOT에 있었던 아이탬 제거.
						GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emLHand);
						m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);				
					}

				}
			}
			
			// 탈것 슬롯일경우 슬롯의 아이템 정리
			if ( emSLOT == SLOT_VEHICLE &&  m_sVehicle.IsActiveValue() )
			{
				for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
				{
					if ( m_sVehicle.m_PutOnItems[i].sNativeID == NATIVEID_NULL() ) continue;
					
					// 인벤 삽입
					CItemDrop cDropItem;
					cDropItem.sItemCustom = m_sVehicle.m_PutOnItems[i];
					
					if ( IsInsertToInvenEx ( &cDropItem ) )
					{
						InsertToInvenEx ( &cDropItem );
					}
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

					m_sVehicle.m_PutOnItems[i].sNativeID = NATIVEID_NULL();
					
					// 탈것 릴리즈 
					//	[자신에게] 해당 아이탬을 인밴에서 삭제.
					GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
					NetMsg.dwVehicleNum	= m_sVehicle.m_dwGUID;
					NetMsg.accetype = (VEHICLE_ACCESSORYTYPE)i;
					m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);

					// 주변에 알림.
					GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
					NetMsgBrd.dwGaeaID = m_dwGaeaID;
					NetMsgBrd.dwVehicleNum = m_sVehicle.m_dwGUID;
					NetMsgBrd.accetype = (VEHICLE_ACCESSORYTYPE)i;
					SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );				
				}

				m_pGLGaeaServer->SaveVehicle( m_dwClientID, m_dwGaeaID, false );
			}
			// 탈것 정보가 활성화 되지 않았을경우에는 제거 하지 않는다.
			else if ( emSLOT == SLOT_VEHICLE &&  !m_sVehicle.IsActiveValue() )
			{
				continue;
			}

			//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release(emSLOT);
			m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

			//	[모두에게] 자신의 착용 복장이 바뀜을 알림.
			GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutOn_Release_Brd;
			NetMsg_PutOn_Release_Brd.dwGaeaID = m_dwGaeaID;
			NetMsg_PutOn_Release_Brd.emSlot = emSLOT;
			SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsg_PutOn_Release_Brd) );	


			// 인벤에 삽입
			CItemDrop cDropItem;
			cDropItem.sItemCustom = m_PutOnItems[i];

			if ( IsInsertToInvenEx ( &cDropItem ) )
			{
				InsertToInvenEx ( &cDropItem );
			}
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

			RELEASE_SLOT_ITEM ( emSLOT );			
		}
	}

	// 탈것 악세서리 조건 체크
	if ( m_sVehicle.IsActiveValue() )
	{
		for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
		{
			if ( m_sVehicle.m_PutOnItems[i].sNativeID == NATIVEID_NULL() ) continue;
			
            if ( !SIMPLE_CHECK_ITEM( m_sVehicle.m_PutOnItems[i].sNativeID ) )
			{
				
				// 인벤 삽입
				CItemDrop cDropItem;
				cDropItem.sItemCustom = m_sVehicle.m_PutOnItems[i];
				
				if ( IsInsertToInvenEx ( &cDropItem ) )
				{
					InsertToInvenEx ( &cDropItem );
				}
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

				m_sVehicle.m_PutOnItems[i].sNativeID = NATIVEID_NULL();
				
				// 탈것 릴리즈 
				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
				NetMsg.dwVehicleNum	= m_sVehicle.m_dwGUID;
				NetMsg.accetype = (VEHICLE_ACCESSORYTYPE)i;
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);

				// 주변에 알림.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.dwVehicleNum = m_sVehicle.m_dwGUID;
				NetMsgBrd.accetype = (VEHICLE_ACCESSORYTYPE)i;
				SendMsgViewAround ( reinterpret_cast<NET_MSG_GENERIC*>(&NetMsgBrd) );				
			}
		}
	}
}

void GLChar::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : 이전 액션 취소.
	//

	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_ATTACK:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_SKILL:
		m_fIdleTime = 0.0f;
		break;
	case GLAT_GATHERING:
		break;

	default:
		break;
	};

	//CONSOLEMSG_WRITE( "TurnAction [%d]", toAction );

	//	Note : 액션 초기화.
	//
	m_fIdleTime = 0.0f;
	m_Action = toAction;

    switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_MOVE:
		m_wHealingNum = 0;
		break;

	case GLAT_ATTACK:
		m_fattTIMER = 0.0f;
		break;

	case GLAT_SKILL:
		m_fattTIMER = 0.0f;
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		break;
	
	case GLAT_GATHERING:
		break;

	default:
		break;
	};
}

void GLChar::AddPlayHostile ( DWORD dwCHARID, BOOL bBAD, BOOL bClubBattle )
{
	bool bBRIGHTEVENT   = m_pGLGaeaServer->IsBRIGHTEVENT();
	bool bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
	bool bGuidBattleMap = m_pLandMan->m_bGuidBattleMap;
	bool bClubDeathMatch = m_pLandMan->m_bClubDeathMatchMap;

	PGLCHAR pCHAR = m_pGLGaeaServer->GetCharID ( dwCHARID );
	if ( !pCHAR )		return;

	if ( pCHAR->IsSTATE( EM_ACT_DIE ) ) return;
	if ( IsSTATE( EM_ACT_DIE ) ) return;
	
	if ( GetSchool() == pCHAR->GetSchool() )	bSCHOOL_FREEPK = false;

	// DESC : 유저가 많은 대만에서는 학원간 자유PK일 경우 적대자가 기하급수적으로 늘어나며
	//			불안한 성능을 가진 HASH_MAP을 사용하는 현 데이타 구조로서는 사용하기 불가능하다.			            
	if ( !(bBRIGHTEVENT||bSCHOOL_FREEPK||bGuidBattleMap||bClubDeathMatch) )
	{
		//	Note : 적대 행위자 등록. ( pk )
		ADD_PLAYHOSTILE ( dwCHARID, bBAD );
	}
	// 학원간 자유피케이 진행중이며 상대가 같은 학교일경우 적대행위 대상자로 구분하여 관리함.
	if ( GLSchoolFreePK::GetInstance().IsON() && !bSCHOOL_FREEPK )
	{
		ADD_PLAYHOSTILE ( dwCHARID, bBAD );
	}

	// 학원간 자유피케이 진행중 상대가 렙이 20보다 낮을때 적대행위 대상자로 구분하여 관리함.
	if ( bSCHOOL_FREEPK )
	{
		if ( pCHAR->GetLevel() < GLCONST_CHAR::nPK_LIMIT_LEVEL || GetLevel() < GLCONST_CHAR::nPK_LIMIT_LEVEL )
		{
			ADD_PLAYHOSTILE ( dwCHARID, bBAD );
		}
	}

	//	Note : 적대자 등록 알림.
	GLMSG::SNETPC_PLAYERKILLING_ADD NetMsgAdd;
	NetMsgAdd.dwCharID = dwCHARID;
	NetMsgAdd.wSchoolID = pCHAR->m_wSchool;
	NetMsgAdd.bBAD = bBAD;
	NetMsgAdd.bClubBattle = bClubBattle;
	NetMsgAdd.fTime = 0.0f;

	StringCchCopy ( NetMsgAdd.szName, CHAR_SZNAME, pCHAR->m_szName );

	m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgAdd);
}

// *****************************************************
// Desc: 대련중인 대상인지 Check
// *****************************************************
bool GLChar::IsConflictTarget ( GLACTOR* pTARGET )
{
	//GASSERT(pTARGET&&"GLChar::IsConflictTarget()");
	if ( !pTARGET )				return false;

	EMCROW emCROW = pTARGET->GetCrow();
	if ( emCROW!=CROW_PC )		return false;

	SCONFTING* pCONFTING = pTARGET->GETCONFTING();
	if ( !pCONFTING )			return false;

	if ( m_sCONFTING.IsFIGHTING() && pCONFTING->IsFIGHTING() )
	{
		if ( m_sCONFTING.emTYPE!=pCONFTING->emTYPE )	return false;

		switch ( pCONFTING->emTYPE )
		{
		case EMCONFT_ONE:
			return ( m_dwGaeaID==pCONFTING->dwTAR_ID );

		case EMCONFT_PARTY:
			return ( m_dwPartyID==pCONFTING->dwTAR_ID );

		case EMCONFT_GUILD:
			return ( m_dwGuild==pCONFTING->dwTAR_ID );
		};
	}

	return false;
}

// *****************************************************
// Desc: 액션을 취할 수 있는 대상인지 check
// *****************************************************
bool GLChar::IsReActionable ( GLACTOR* pTARGET, bool benermy )
{
	GASSERT(pTARGET&&"GLChar::IsReActionable()");
	if ( !pTARGET )					return false;
	if ( !m_pLandMan )				return false;

	// 성향이벤트 중인지
	bool bBRIGHTEVENT = m_pGLGaeaServer->IsBRIGHTEVENT();

	EMCROW emCROW = pTARGET->GetCrow();
	bool bPKZone = m_pLandMan->IsPKZone();
	bool bGuidBattleMap = m_pLandMan->m_bGuidBattleMap;
	bool bClubDeathMatch = m_pLandMan->m_bClubDeathMatchMap;
	bool bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
	bool bClubBattleZone = m_pLandMan->IsClubBattleZone();
	if ( GetSchool() == pTARGET->GetSchool() )	bSCHOOL_FREEPK = false;

	bool bREACTION(true);

	// 상대가 적인 경우
	if ( benermy )
	{
		switch ( emCROW )
		{
		case CROW_PC:
			// Absolute Non-PK State Check.
			if ( GLCONST_CHAR::bPKLESS ) return false;

			// Desc : 절대 안전한 시간 (10초)
			if ( !pTARGET->IsSafeTime() && !IsSafeTime() && !pTARGET->IsSafeZone() && !IsSafeZone() )
			{
				// 같은 파티의 멤버인지
				GLPartyFieldMan& sPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
				GLPARTY_FIELD* sMyParty = sPartyFieldMan.GetParty ( m_dwPartyID );
				if ( sMyParty )
				{
					if ( sMyParty->ISMEMBER ( pTARGET->GetCtrlID() ) )
					{
						return false;
					}
				}

				// 서로 대련중인지
				bREACTION = IsConflictTarget ( pTARGET );
				if ( !bREACTION )
				{
					SCONFTING* pCONFTING = pTARGET->GETCONFTING();
					if ( m_sCONFTING.IsCONFRONTING() || pCONFTING->IsCONFRONTING() )
					{
						return false;
					}
				}
				else
				{
					return true;
				}

				// 적대관계인지 체크
				DWORD dwCHARID = pTARGET->GetCharID();
				bREACTION = IS_PLAYHOSTILE ( dwCHARID );
				if ( bREACTION ) return true;
	
				// 20 Lev 이상일때
				if ( pTARGET->GetLevel() >= GLCONST_CHAR::nPK_LIMIT_LEVEL && GLCONST_CHAR::bPK_MODE && bPKZone)
				{
					// PK 가능한 구역 ( 학교간 Free PK )
					if ( bSCHOOL_FREEPK )											return true;
					
					// 성향 이벤트 중이면
					if ( bBRIGHTEVENT && ( m_dwPartyID != pTARGET->GetPartyID() ) )	return true;
											
					// 그냥 강제공격
					bREACTION = true;		
				}

				PGLCHAR pChar = m_pGLGaeaServer->GetChar( pTARGET->GetCtrlID() );
				GLClubMan &cClubMan = m_pGLGaeaServer->GetClubMan();
				GLCLUB *pMyClub = cClubMan.GetClub ( m_dwGuild );
				GLCLUB *pTarClub = NULL;
				if ( pChar ) pTarClub = cClubMan.GetClub ( pChar->m_dwGuild );

				// 선도전에서 같은 동맹 클럽이면 공격 불가능
				if ( GLCONST_CHAR::bPK_MODE )
				{
					if ( bGuidBattleMap )
					{
						bREACTION = true;

						if ( pMyClub && pTarClub )
						{
							BOOL bAlliance = pMyClub->IsAllianceGuild ( pTarClub->m_dwID );
							if ( bAlliance )	bREACTION = false;
						} // end of if
					}
					else if ( bClubDeathMatch )
					{
						if ( m_dwGuild == pChar->m_dwGuild ) bREACTION = false;
						else bREACTION = true;
					}

					if ( bPKZone && bClubBattleZone )
					{
						if ( pMyClub && pTarClub )
						{
							if ( pMyClub->IsBattle( pTarClub->m_dwID ) ) return true;
							if ( pMyClub->IsBattleAlliance( pTarClub->m_dwAlliance ) ) return true;
						}
					}
				}

			} // end of if
			else
				bREACTION = false;
			break;
		
		case CROW_SUMMON:
		case CROW_MOB:
			if ( m_sCONFTING.IsCONFRONTING() )	bREACTION = false;
			else if ( IsSafeZone() ) 	bREACTION = false;
			else bREACTION = true;
			break;

		case CROW_NPC:
			bREACTION = false;
			break;

		case CROW_MATERIAL:
			bREACTION = false;
			break;

		//case CROW_PET:	break;	// PetData

		default:
			bREACTION = false;
			break;
		};
	}
	// 상대가 적이 아닌 경우
	else
	{
		switch ( emCROW )
		{
		case CROW_PC:
			{
				//	pc 에게 비공격 마법 같은 것은 대련자가 아닐 때만 가능.
				bREACTION = !IsConflictTarget ( pTARGET );

				//	상대방이 대련중일 때는 자기가 대련중이고 같은 편일때 가능.
				SCONFTING* pTAR_CONFT = pTARGET->GETCONFTING();
				if ( pTAR_CONFT->IsCONFRONTING() )
				{
					bREACTION = m_sCONFTING.IsSAMENESS(pTAR_CONFT->emTYPE,pTAR_CONFT->dwTAR_ID);
				}

				bool bHOSTILE = IS_PLAYHOSTILE ( pTARGET->GetCharID() );
				if ( bHOSTILE )	bREACTION = false;
			}
			break;
		
		case CROW_SUMMON:
		case CROW_MOB:
			bREACTION = false;
			break;

		case CROW_NPC:
			bREACTION = m_sCONFTING.IsCONFRONTING() ? false : true;
			break;

		case CROW_MATERIAL:
			bREACTION = true;
			break;

		//case CROW_PET:	break;	// PetData

		default:
			bREACTION = false;
			break;
		};
	}

	return bREACTION;
}


inline DWORD GLChar::ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const BOOL bShock )
{
	GLACTOR* pACTOR_TAR = m_pGLGaeaServer->GetTarget ( m_pLandMan, sTargetID );
	if ( !pACTOR_TAR )	return 0;

	//	Note : PC에게 damage를 주려면 대련자여야 가능.
	//		대련 종료 직후 damage 유입 차단을 위해서.
	//
	if ( !IsReActionable(pACTOR_TAR) )	return 0;

	return pACTOR_TAR->ReceiveDamage ( GETCROW(), m_dwGaeaID, nDAMAGE, bShock );
}

void GLChar::PreStrikeProc ( BOOL bSkill, BOOL bLowSP )
{
	SANIATTACK sAniAttack;
	STARGETID sTargetID;

	BOOL bCheckHit = FALSE;

	if ( !bSkill )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand] );

		GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
		VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[AN_ATTACK][emANISUBTYPE];
		if ( vecAniAttack.empty() )	return; // FALSE;

		//	Note : 특정 공격 에니메이션 설정 선택.
		//
		sAniAttack = vecAniAttack[0];

		sTargetID = m_TargetID;
		bCheckHit = TRUE;
	}
	else
	{
		//	Note : 스킬 정보 가져옴.
		//
		PGLSKILL pSkill = NULL;
		pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );

		GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
		VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[pSkill->m_sEXT_DATA.emANIMTYPE][pSkill->m_sEXT_DATA.emANISTYPE];
		if ( vecAniAttack.empty() )	return; // FALSE;

		//	Note : 특정 공격 에니메이션 설정 선택.
		//
		sAniAttack = vecAniAttack[0];

		if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
		{
			//	Note : 스킬 타겟을 하나만 유효하게 확인을 하고 있음.
			//	NEED : 다중 타겟일 경우 다시 확인해야함.
			sTargetID = STARGETID(static_cast<EMCROW>(m_sTARIDS[0].wCrow),static_cast<DWORD>(m_sTARIDS[0].wID));
			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.dwID = EMTARGET_NULL;
		}
	}

	//	큐 초기화
	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = true;
		if ( bCheckHit && (sTargetID.dwID!=EMTARGET_NULL) )
		{
			if ( !CHECKHIT(sTargetID,m_pLandMan,bLowSP) )		bhit = false;
		}

		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit,cast_bool(bLowSP)) );
	}
}

void GLChar::AvoidProc ( const SNATIVEID skill_id, const BOOL bLowSP )
{
	//CONSOLEMSG_WRITE( "AvoidProc Start [%u]", skill_id.dwID );

	if ( skill_id==NATIVEID_NULL() )
	{
		if ( !bLowSP )
		{
			WORD wDisSP = m_wSUM_DisSP + GLCONST_CHAR::wBASIC_DIS_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		GLACTOR *pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, m_TargetID );
		if ( pACTOR )	pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, FALSE);
	}
	else
	{
		//	Note : SP가 부족하지 않을때에는 SP 소모시킴.
		//
		if ( !bLowSP )
		{
			//	Note : 캐릭터가 배운 스킬 정보 가져옴.
			SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
			if( learniter==m_ExpSkills.end() )								return;	// 배운 스킬이 아닐 경우.
			SCHARSKILL &sSkill = (*learniter).second;

			//	스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
			if ( !pSkill )													return;
			SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];

			WORD wDisSP = m_wSUM_DisSP + sSKILL_DATA.wUSE_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			STARGETID sTargetID ( (EMCROW)m_sTARIDS[i].wCrow, (WORD)m_sTARIDS[i].wID );

			GLACTOR *pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, sTargetID );
			if ( pACTOR )	pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, FALSE);
		}
	}

	//	Note : 자신에게.
	GLMSG::SNETPC_ATTACK_AVOID NetMsg;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.dwID;

	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_AVOID_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID	= m_dwGaeaID;
	NetMsgBrd.emTarCrow	= m_TargetID.emCrow;
	NetMsgBrd.dwTarID	= m_TargetID.dwID;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
}

void GLChar::DamageProc ( int nDAMAGE, const DWORD dwDamageFlag, const BOOL bLowSP )
{
	//	Note : 데미지 산출.
	//				( sp 부족시 데미지 감산. )
	float	fDAMAGE_RATE(1.0f);
	if ( bLowSP )						fDAMAGE_RATE *= (1-GLCONST_CHAR::fLOWSP_DAMAGE);

	nDAMAGE = int(nDAMAGE*fDAMAGE_RATE);
	if ( nDAMAGE < 1 )	nDAMAGE = 1;

	// 대련 종료후 무적타임이면 공격 무시
	PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( m_TargetID.dwID );
	if ( pCHAR )
	{
		if ( pCHAR->m_sCONFTING.IsPOWERFULTIME () )
			return;
	}

	//	Note : SP 소모.
	//
	if ( !bLowSP )
	{
		WORD wDisSP = m_wSUM_DisSP + GLCONST_CHAR::wBASIC_DIS_SP;
		m_sSP.DECREASE ( wDisSP );

		GLMSG::SNETPC_UPDATE_SP NetMsg;
		NetMsg.wNowSP = m_sSP.wNow;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
	}

	if ( m_TargetID.emCrow==CROW_PC )
	{
		nDAMAGE = int(nDAMAGE*GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if ( nDAMAGE==0 )	nDAMAGE = 1;
	}

	//	Note : 대상자에게 대미지값 적용.
	//
	bool bShock = ( dwDamageFlag & DAMAGE_TYPE_SHOCK );
	ToDamage ( m_TargetID, nDAMAGE, bShock );

	//	Note : 자신에게.
	GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= m_TargetID.emCrow;
	NetMsg.dwTarID			= m_TargetID.dwID;
	NetMsg.nDamage			= nDAMAGE;
	NetMsg.dwDamageFlag		= dwDamageFlag;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_DAMAGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID		= m_dwGaeaID;
	NetMsgBrd.emTarCrow		= m_TargetID.emCrow;
	NetMsgBrd.dwTarID		= m_TargetID.dwID;
	NetMsgBrd.nDamage		= nDAMAGE;
	NetMsgBrd.dwDamageFlag	= dwDamageFlag;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	GLACTOR* pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, m_TargetID );
	if ( !pACTOR )	return;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	//	Note : 상태 이상 유발.
	//
	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		ITEM::SSATE_BLOW &sBlow = pRHandItem->sSuitOp.sBLOW;

		//	Note : 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBlow.emTYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, dwWeather, m_pLandMan->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBlow.emTYPE)) )
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBlow.fRATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );

		if ( bBLOW )
		{
			//	Note : 상태이상 발생.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = sBlow.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = sBlow.emTYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = sBlow.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = sBlow.fVAR2;

			pACTOR->STATEBLOW ( sSTATEBLOW );

			//	Note : 상태이상 발생 Msg.
			//
			GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
			NetMsgState.emCrow = m_TargetID.emCrow;
			NetMsgState.dwID = m_TargetID.dwID;
			NetMsgState.emBLOW = sBlow.emTYPE;
			NetMsgState.fAGE = fLIFE;
			NetMsgState.fSTATE_VAR1 = sBlow.fVAR1;
			NetMsgState.fSTATE_VAR2 = sBlow.fVAR2;

			//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

			//	Note : 스킬 대상에게 메세지 전송.
			if ( m_TargetID.emCrow==CROW_PC )	m_pGLGaeaServer->SENDTOCLIENT ( pACTOR->GETCLIENTID (), &NetMsgState );
		}
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		ITEM::SSATE_BLOW &sBlow = pLHandItem->sSuitOp.sBLOW;

		//	Note : 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBlow.emTYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, dwWeather, m_pLandMan->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBlow.emTYPE)) )
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBlow.fRATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );

		if ( bBLOW )
		{
			//	Note : 상태이상 발생.
			//	 
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = sBlow.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = sBlow.emTYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = sBlow.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = sBlow.fVAR2;

			pACTOR->STATEBLOW ( sSTATEBLOW );

			//	Note : 상태이상 발생 Msg.
			//
			GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
			NetMsgState.emCrow = m_TargetID.emCrow;
			NetMsgState.dwID = m_TargetID.dwID;
			NetMsgState.emBLOW = sBlow.emTYPE;
			NetMsgState.fAGE = fLIFE;
			NetMsgState.fSTATE_VAR1 = sBlow.fVAR1;
			NetMsgState.fSTATE_VAR2 = sBlow.fVAR2;

			//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			pACTOR->SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgState );

			//	Note : 스킬 대상에게 메세지 전송.
			if ( m_TargetID.emCrow==CROW_PC )	m_pGLGaeaServer->SENDTOCLIENT ( pACTOR->GETCLIENTID (), &NetMsgState );
		}
	}

	// 강한 타격
	if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) 
	{
		STARGETID dwActorID = STARGETID(CROW_PC,m_dwGaeaID);
		m_pLandMan->PushPullAct( dwActorID, m_TargetID, GLCONST_CHAR::fCRUSH_BLOW_RANGE );
	}
}

void GLChar::DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR )
{
	if ( sACTOR.emCrow==CROW_PC )
	{
		nDAMAGE = int(nDAMAGE*GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if ( nDAMAGE==0 )	nDAMAGE = 1;
	}

	ToDamage ( sACTOR, nDAMAGE, FALSE );

	//	Note : 자신에게.
	GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= sACTOR.emCrow;
	NetMsg.dwTarID			= sACTOR.dwID;
	NetMsg.nDamage			= nDAMAGE;
	NetMsg.dwDamageFlag		= DAMAGE_TYPE_NONE;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_DAMAGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID		= m_dwGaeaID;
	NetMsgBrd.emTarCrow		= sACTOR.emCrow;
	NetMsgBrd.dwTarID		= sACTOR.dwID;
	NetMsgBrd.nDamage		= nDAMAGE;
	NetMsgBrd.dwDamageFlag	= DAMAGE_TYPE_NONE;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
}

void GLChar::DefenseSkill( SNATIVEID sNativeID, WORD wLevel, STARGETID sACTOR )
{
	if ( sNativeID == NATIVEID_NULL() ) return;

	GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE NetMsg;
	NetMsg.emTarCrow = sACTOR.emCrow;
	NetMsg.dwTarID = sACTOR.dwID;
	NetMsg.sNativeID = sNativeID;
	NetMsg.wLevel = wLevel;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );

	m_bDefenseSkill	= true;

    return;
}

BOOL GLChar::AttackProcess ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime*GLCHARLOGIC::GETATTVELO();
	m_fattTIMER += GLCHARLOGIC::GETATT_ITEM();

	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand] );
	VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[AN_ATTACK][emANISUBTYPE];
	if ( vecAniAttack.empty() )	return FALSE;

	//	Note : 공격 모션 종류가 변경된 것으로 보임. 공격 취소. ( 장비 찰탁. )
	//
	if ( vecAniAttack.size() <= m_dwANISUBSELECT )
	{
		TurnAction ( GLAT_IDLE );

		//GLMSG::SNET_ACTION_BRD NetMsgBrd;
		//NetMsgBrd.emCrow = CROW_PC;
		//NetMsgBrd.dwID = m_dwGaeaID;
		//NetMsgBrd.emAction = GLAT_IDLE;
		//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

		//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

		return FALSE;
	}

	if ( !ISLONGRANGE_ARMS() )
	{
		GLACTOR *pTARGET = m_pGLGaeaServer->GetTarget ( m_pLandMan, m_TargetID );
		if ( !pTARGET )
		{
			TurnAction ( GLAT_IDLE );

			//	Note : 자신에게.
			GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
			NetMsg.emTarCrow		= m_TargetID.emCrow;
			NetMsg.dwTarID			= m_TargetID.dwID;
			NetMsg.nDamage			= 0;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );
			SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsg );

			////	Note : 모션 변경.
			//GLMSG::SNET_ACTION_BRD NetMsgBrd;
			//NetMsgBrd.emCrow = CROW_PC;
			//NetMsgBrd.dwID = m_dwGaeaID;
			//NetMsgBrd.emAction = GLAT_IDLE;
			//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

			////	Note : 모션 변경 (모두에게).
			//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

			return FALSE;
		}

		D3DXVECTOR3 vDist = m_vPos - pTARGET->GetPosition();
		float fTarLength = D3DXVec3Length(&vDist);

		WORD wAttackRange = pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
		if ( wAttackRange*GLCONST_CHAR::fREACT_VALID_SCALE < (WORD)(fTarLength) )
		{
			TurnAction ( GLAT_IDLE );

			//	Note : 자신에게.
			GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
			NetMsg.emTarCrow		= m_TargetID.emCrow;
			NetMsg.dwTarID			= m_TargetID.dwID;
			NetMsg.nDamage			= 0;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );
			SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsg );

			////	Note : 모션 변경.
			//GLMSG::SNET_ACTION_BRD NetMsgBrd;
			//NetMsgBrd.emCrow = CROW_PC;
			//NetMsgBrd.dwID = m_dwGaeaID;
			//NetMsgBrd.emAction = GLAT_IDLE;
			//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

			////	Note : 모션 변경 (모두에게).
			//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

			return FALSE;
		}
	}


	//	Note : 특정 공격 에니메이션 설정 선택.
	//
	const SANIATTACK &sAniAttack = vecAniAttack[m_dwANISUBSELECT];

	DWORD dwThisKey = sAniAttack.m_dwSTime + DWORD(m_fattTIMER*UNITANIKEY_PERSEC);
	
	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( dwThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
			{
				int nDamage = 0;
				DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
				dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, m_pLandMan );

				DamageProc ( nDamage, dwDamageFlag, sStrike.bLowSP );
			}
			else
			{
				AvoidProc ( NATIVEID_NULL(), sStrike.bLowSP );
			}

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	if ( dwThisKey >= sAniAttack.m_dwETime )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();
				
				if ( sStrike.bHit )
				{
					int nDamage = 0;
					DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
					dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, m_pLandMan );
					DamageProc ( nDamage, dwDamageFlag, sStrike.bLowSP );
				}
				else
				{
					AvoidProc ( NATIVEID_NULL(), sStrike.bLowSP );
				}

				//	큐 데이타가 처리된 경우 삭제한다.
				m_sHITARRAY.pop_front ();
			}
		}

		return FALSE;
	}

	//	Note : 아직 공격이 종료 되지 않았으나 hit는 완료됫고 에니키가 일정 수량 이하일 경우
	//		공격이 종료된걸로 간주. ( 클라이언트와 서버간에 딜레이 감안. )
	if ( m_sHITARRAY.empty() )
	{
		//DWORD dwDISKEY = sAniAttack.m_dwETime - dwThisKey;
		//if ( dwDISKEY <= 5 )		return FALSE;

		return FALSE;
	}

	return TRUE;
}

// *****************************************************
// Desc: 부활스킬 시동
// *****************************************************
WORD GLChar::DoRevive2Skill ( SNATIVEID skill_id, WORD wSKILL_LVL )
{
	if ( IsValidBody() )					return 0;
	if ( SKILL::MAX_LEVEL <= wSKILL_LVL )	return 0;
	if ( !IsSTATE(EM_ACT_DIE) )				return 0;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	const SKILL::SSPEC &sSPEC = pSkill->m_sAPPLY.sSPEC[wSKILL_LVL];

    //	Note : 부활 map과 부활 gate 지정, 위치는 의미 없는 값.
	BOOL bOk = m_pGLGaeaServer->RequestReBirth ( m_dwGaeaID, m_pLandMan->GetMapID(), UINT_MAX, m_vPos );
	if ( !bOk )
	{
		DEBUGMSG_WRITE ( "캐릭터 부활 시도중에 오류가 발생! m_pGLGaeaServer->RequestReBirth ()" );
		return 0;
	}

	//	부활 완료 체크.
	ReSetSTATE(EM_ACT_DIE);

	//	부활시 채력 회복.
	m_fGenAge = 0.0f;
	GLCHARLOGIC::INIT_RECOVER(int(sSPEC.fVAR1));
	TurnAction ( GLAT_IDLE );

	//	Note : 부활시 경험치 감소.
	ReBirthDecExp ();

	// PET
	// 맵이동시 Pet 및 Vehicle 삭제
	m_pGLGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );
	m_pGLGaeaServer->ReserveDropOutSummon ( SDROPOUTSUMMONINFO(m_dwSummonGUID,true) );
	m_pGLGaeaServer->SaveVehicle( m_dwClientID, m_dwGaeaID, true );

	//	부활 확인 메시지.
	GLMSG::SNETPC_REQ_REBIRTH_FB NetMsg;
	NetMsg.sMapID = m_pLandMan->GetMapID();
	NetMsg.vPos = GetPosition();
	NetMsg.wNowHP = m_sHP.wNow;
	NetMsg.wNowMP = m_sMP.wNow;
	NetMsg.wNowSP = m_sSP.wNow;

	m_pGLGaeaServer->SENDTOAGENT(m_dwClientID,&NetMsg);
	return m_sHP.wNow;
}

// *****************************************************
// Desc: 스킬 영향을 케릭터에 적용시킨다.
// *****************************************************
void GLChar::SkillProc ( SNATIVEID skill_id, BOOL bLowSP )
{
	//CONSOLEMSG_WRITE( "SkillProc Start [%u]", skill_id.dwID );

	//	Note : 캐릭터가 배운 스킬 정보 가져옴.
	//
	SCHARSKILL sSkill;
	WORD wSKILL_LVL = 0;

	if ( IsDefenseSkill() )
	{
		if ( skill_id != m_sDefenseSkill.m_dwSkillID ) return;
		sSkill.sNativeID = skill_id;
		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
		wSKILL_LVL = m_sDefenseSkill.m_wLevel;;

	}
	else
	{
		SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
		if ( learniter==m_ExpSkills.end() )	return;
		sSkill = (*learniter).second;
		wSKILL_LVL = sSkill.wLevel;
	}

	//	Note : 에니메이션 정보.
	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[m_SKILLMTYPE][m_SKILLSTYPE];
	if ( vecAniAttack.empty() )	return;

	//	Note : 특정 공격 에니메이션 설정 선택. (수정요)
	//
	const SANIATTACK &sAniAttack = vecAniAttack[0];

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL];

	//	Note : SP가 부족하지 않을때에는 SP 소모시킴.
	//
	if ( !bLowSP )
	{
		WORD wDisSP = m_wSUM_DisSP + sSKILL_DATA.wUSE_SP;
		m_sSP.DECREASE ( wDisSP );
	}

	int nEXP_NUM = 0;
	int nALL_LVL = 0;
	int nALL_VARHP = 0, nALL_FULLHP = 0;
	int nALL_VARMP = 0, nALL_FULLMP = 0;
	int nALL_VARSP = 0, nALL_FULLSP = 0;

	WORD wMAX_TAR = 0;
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )			wMAX_TAR = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1;
	else if ( pSkill->m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )	wMAX_TAR = sSKILL_DATA.wTARNUM * ( sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1 );
	else															wMAX_TAR = sSKILL_DATA.wTARNUM;

	if ( m_wTARNUM > wMAX_TAR )		m_wTARNUM = wMAX_TAR;

	//	Note : 스킬이 적용되는 타겟 검사.
	//
	STARGETID sTARID;
	for ( WORD i=0; i<m_wTARNUM; ++i )
	{
		const STARID &_sTARID = m_sTARIDS[i];

		sTARID.emCrow = _sTARID.GETCROW();
		sTARID.dwID = _sTARID.GETID();
		GLACTOR* pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, sTARID );
		if ( !pACTOR )									continue;

		bool bCrushBlowOne = false;

		for ( WORD j=0; j<sSKILL_DATA.wAPPLYNUM; ++j )
		{
			if ( pACTOR->GetNowHP()==0 )				continue;

			//	Note : 물리 데미지 발생.
			//
			DWORD dwDamageFlag = DAMAGE_TYPE_NONE;

			int nVAR_HP(0), nVAR_MP(0), nVAR_SP(0);

			//	저항치.
			short nRESIST = pACTOR->GETRESIST().GetElement(pSkill->m_sAPPLY.emELEMENT);
			if ( nRESIST>99 )	nRESIST = 99;

			//	SKILL 기본 적용.
			//
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_HP:
				//	대상에게 피해를 주는 스킬.
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
				{
//					bCRITICAL = CALCDAMAGE ( nVAR_HP, bShock, sTARID, m_pLandMan, pSkill, wSKILL_LVL, GLPeriod::GetInstance().GetWeather(), sAniAttack.m_wDivCount );
					DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
					dwDamageFlag = CALCDAMAGE ( nVAR_HP, m_dwGaeaID, sTARID, m_pLandMan, pSkill, wSKILL_LVL, dwWeather, sAniAttack.m_wDivCount );
					nVAR_HP = - nVAR_HP;
				}
				//	대상을 회복시켜 주는 스킬.
				else
				{
					int nDX = pACTOR->GetMaxHP() - pACTOR->GetNowHP();
					nVAR_HP += (int) min(nDX,sSKILL_DATA.fBASIC_VAR);
				}
				break;

			case SKILL::EMFOR_MP:
				//	대상에게 피해를 주는 스킬.
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_MP -= (int) ( nVAR - (nVAR*nRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );
				}
				//	대상을 회복시켜 주는 스킬.
				else
				{
					int nDX = pACTOR->GetMaxMP() - pACTOR->GetNowMP();
					nVAR_MP += (int) min(nDX,sSKILL_DATA.fBASIC_VAR);
				}
				break;

			case SKILL::EMFOR_SP:
				//	대상에게 피해를 주는 스킬.
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_SP -= (int) ( nVAR - (nVAR*nRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );
				}
				//	대상을 회복시켜 주는 스킬.
				else
				{
					int nDX = pACTOR->GetMaxSP() - pACTOR->GetNowSP();
					nVAR_SP += (int) min(nDX,sSKILL_DATA.fBASIC_VAR);
				}
				break;
			};

			if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) bCrushBlowOne = true;

			//	Note : SP 부족시에는 변화 값을 반으로 조정.
			//
			if ( bLowSP )
			{
				nVAR_HP /= 2;
				nVAR_MP /= 2;
				nVAR_SP /= 2;
			}

			//	Note : 스킬 특수 기능.
			//
			int nGATHER_HP(0), nGATHER_MP(0), nGATHER_SP(0);
			const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.sSPEC[wSKILL_LVL];

			switch ( pSkill->m_sAPPLY.emSPEC )
			{
			case EMSPECA_HP_GATHER:
				nGATHER_HP = int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
				break;

			case EMSPECA_MP_GATHER:
				nGATHER_MP = int ( abs(nVAR_MP) * sSKILL_SPEC.fVAR1 );
				break;

			case EMSPECA_SP_GATHER:
				nGATHER_SP = int ( abs(nVAR_SP) * sSKILL_SPEC.fVAR1 );
				break;
			};

			//	Note : 자기 자신에게 쓴 스킬은 제외하고 EXP 자료를 수집.
			//
			if ( sTARID!=STARGETID(CROW_PC,m_dwGaeaID) )
			{
				++nEXP_NUM;
				nALL_LVL += pACTOR->GetLevel ();

				nALL_VARHP += nVAR_HP;
				nALL_VARMP += nVAR_MP;
				nALL_VARSP += nVAR_SP;

				nALL_FULLHP += pACTOR->GetMaxHP();
				nALL_FULLMP += pACTOR->GetMaxMP();
				nALL_FULLSP += pACTOR->GetMaxSP();
			}

			//	Note : 스킬 기본 영향.
			//
			SSKILLACT sSKILLACT;
			sSKILLACT.sID = STARGETID(CROW_PC,m_dwGaeaID);
			sSKILLACT.sID_TAR = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
			sSKILLACT.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
			sSKILLACT.emAPPLY = pSkill->m_sBASIC.emAPPLY;
			sSKILLACT.dwDamageFlag = dwDamageFlag;			

			//	Note : 체력 변화 저장.
			//
			sSKILLACT.nVAR_HP = nVAR_HP;
			sSKILLACT.nVAR_MP = nVAR_MP;
			sSKILLACT.nVAR_SP = nVAR_SP;

			//	Note : 체력 흡수 저장.
			//
			sSKILLACT.nGATHER_HP = nGATHER_HP;
			sSKILLACT.nGATHER_MP = nGATHER_MP;
			sSKILLACT.nGATHER_SP = nGATHER_SP;

			if ( sSKILLACT.VALID() )
			{
				m_pLandMan->RegSkillAct ( sSKILLACT );
			}
		}

		//	Note : 스킬 보조 영향.
		//
		SSKILLACTEX sSKILLACTEX;
		sSKILLACTEX.sID = STARGETID(CROW_PC,m_dwGaeaID);
		sSKILLACTEX.sID_TAR = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
		sSKILLACTEX.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;

		sSKILLACTEX.idSKILL = skill_id;
		sSKILLACTEX.wSKILL_LVL = wSKILL_LVL;

		//  강한 타격 확률
		if ( bCrushBlowOne )
		{
			sSKILLACTEX.bCrushBlow = bCrushBlowOne;
			sSKILLACTEX.fPUSH_PULL = GLCONST_CHAR::fCRUSH_BLOW_RANGE;
		}

		//	Note : 상태 이상 유발.
		//
		ITEM::SSATE_BLOW sBLOW;
		sBLOW.emTYPE = EMBLOW_NONE;

		if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
		{
			SITEM *pITEM = GET_ELMT_ITEM ();
			if ( pITEM )	sBLOW = pITEM->sSuitOp.sBLOW;
		}
		else
		{
			const SKILL::SSTATE_BLOW &sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSKILL_LVL];

			sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
			sBLOW.fRATE = sSKILL_BLOW.fRATE;
			sBLOW.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL].fLIFE;
			sBLOW.fVAR1 = sSKILL_BLOW.fVAR1;
			sBLOW.fVAR2 = sSKILL_BLOW.fVAR2;
		}
        
		if ( sBLOW.emTYPE!=EMBLOW_NONE )
		{
			//	Note : 발생 확율 계산.
			//
			short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) );
			if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
			DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( m_pLandMan->GetMapID().wMainID, m_pLandMan->GetMapID().wSubID );
			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, dwWeather, m_pLandMan->IsWeatherActive() );

			BOOL bBLOW(FALSE);
			if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBLOW.emTYPE)) )
			{
				bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBLOW.fRATE * fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );
			}

			if ( bBLOW )
			{
				//	Note : 상태이상 발생.
				//	
				SSTATEBLOW sSTATEBLOW;
				float fLIFE = sBLOW.fLIFE * fPOWER;
				fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );

				sSTATEBLOW.emBLOW = sBLOW.emTYPE;
				sSTATEBLOW.fAGE = fLIFE;
				sSTATEBLOW.fSTATE_VAR1 = sBLOW.fVAR1;
				sSTATEBLOW.fSTATE_VAR2 = sBLOW.fVAR2;

				//	Note : 상태 이상 발생 저장.
				//
				sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
			}
		}

		//	Note : 스킬 특수 기능.
		//
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.sSPEC[wSKILL_LVL];
		switch ( pSkill->m_sAPPLY.emSPEC )
		{
		case EMSPECA_PUSHPULL:
			if ( !pACTOR->IsValidBody() )				break;
			if ( RANDOM_POS > sSKILL_SPEC.fVAR2 )		break;	//	Note : 발생확율 적용.
			if ( bCrushBlowOne )	break;
			sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR1;
			break;
		case EMSPECA_CRUSHING_BLOW:
			if ( !bCrushBlowOne )	break;
			sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR1;
			break;
		case EMSPECA_HP_DIV:
		case EMSPECA_MP_DIV:
		case EMSPECA_SP_DIV:
			break;

		case EMSPECA_RECBLOW:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.sSPEC[wSKILL_LVL].dwFLAG;
			break;

		case EMSPECA_BUFF_REMOVE:
			sSKILLACTEX.dwRemoveFlag = EMSPECA_BUFF_REMOVE;
			break;
		case EMSPECA_DEBUFF_REMOVE:
			sSKILLACTEX.dwRemoveFlag = EMSPECA_DEBUFF_REMOVE;
			break;

		case EMSPECA_REBIRTH:
			if ( sTARID.emCrow==CROW_PC )
			{
				PGLCHAR pPC = m_pGLGaeaServer->GetChar(sTARID.dwID);
				if ( pPC )
				{
					WORD wRECOVE = pPC->DoRevive2Skill(skill_id,wSKILL_LVL);
					if ( 0<wRECOVE)
					{
						++nEXP_NUM;
						nALL_LVL += pPC->GetLevel ();
						nALL_VARHP += wRECOVE;
						nALL_FULLHP += pPC->GetMaxHP();
					}
				}
			}
			break;
		};

		switch (pSkill->m_sAPPLY.emBASIC_TYPE)
		{
		case SKILL::EMFOR_CURE:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.dwCUREFLAG;
			break;
		};

		if ( sSKILLACTEX.VALID() )
		{
			m_pLandMan->RegSkillActEx ( sSKILLACTEX );
			nALL_VARHP;
		}

		//	Note : 회복에 기여했을 때만 경험치 발생.	( 공격시는 제외 )
		//
		int nSUM_EXP = 0;
		bool bRECOVE = (nALL_VARHP>=0&&nALL_VARMP>=0&&nALL_VARSP&&0) && (nALL_VARHP+nALL_VARMP+nALL_VARSP)>0;

		if ( bRECOVE )
		{
			//	실제 변화량이 있을 때만 경험치를 산출.
			if ( nALL_VARHP>0 )
				nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARHP,nALL_FULLHP,pACTOR->GetBonusExp());

			if ( nALL_VARMP>0 )
				nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARMP,nALL_FULLMP,pACTOR->GetBonusExp());

			if ( nALL_VARSP>0 )
				nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARSP,nALL_FULLSP,pACTOR->GetBonusExp());

			nSUM_EXP += int (nSUM_EXP*GLCONST_CHAR::fREC_EXP_RATE);
		}

		//	Note : 경험치 획득량 적용.
		//
		if ( nSUM_EXP )
		{
			// 아이템 에디트에서 입력한 경험치 배율 적용 (준혁)
			nSUM_EXP = int( (float)nSUM_EXP * GetExpMultipleRateFromSlotItem() );

			m_sExperience.lnNow += nSUM_EXP;
			m_sExperience.LIMIT();
		}
	}


	//	Note : 스킬 사용에 따른 소모량 FB MSG.
	//
	GLMSG::SNETPC_SKILLCONSUME_FB NetMsgFB;
	EMSLOT emLHand = GetCurLHand();
	SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
	if ( pItem && (pItem->sDrugOp.bInstance) )		NetMsgFB.wTurnNum = m_PutOnItems[emLHand].wTurnNum;
	NetMsgFB.wNowHP = m_sHP.wNow;
	NetMsgFB.wNowMP = m_sMP.wNow;
	NetMsgFB.wNowSP = m_sSP.wNow;

	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );

	//	Note : 소모성 아이탬 유효성 검사후, 사라질때 메시지 발생.
	//
	CheckInstanceItem ();
}


// *****************************************************
// Desc: 시동중인 스킬관련 정보 갱신
// *****************************************************
BOOL GLChar::SkillProcess ( float fElapsedTime )
{
	//CONSOLEMSG_WRITE( "SkillProcess Start [%u]", m_idACTIVESKILL.dwID );

	//	Note : 캐릭터가 배운 스킬 정보 가져옴.
	//
	WORD wSKILL_LVL;
	SCHARSKILL sSkill;

	if ( IsDefenseSkill() )
	{
		if ( m_idACTIVESKILL != m_sDefenseSkill.m_dwSkillID ) return FALSE;
		sSkill.sNativeID = m_idACTIVESKILL;
		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
		wSKILL_LVL = sSkill.wLevel;
	}
	else
	{
        SKILL_MAP_ITER learniter = m_ExpSkills.find ( m_idACTIVESKILL.dwID );
		if ( learniter==m_ExpSkills.end() )		return FALSE;
		sSkill = (*learniter).second;
		wSKILL_LVL = sSkill.wLevel;
	}

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL );

	//	자신의 동체반경 + 스킬타겟영역 + 스킬적용영역 + 20(여유값)
	float fSkillRange = (float) ( GETBODYRADIUS() + GETSKILLRANGE_TAR(*pSkill) +
		GETSKILLRANGE_APPLY(*pSkill,wSKILL_LVL) + 20 );

	// 대련 종료후 무적타임이면 공격 무시
	if ( m_sCONFTING.IsPOWERFULTIME () ) return FALSE;

	//	Note : 스킬이 적용되는 타겟 검사.
	//
	DWORD dwVALIDNUM(0); // 유효 타겟
	STARGETID sTARID;
	for ( WORD i=0; i<m_wTARNUM; ++i )
	{
		sTARID.emCrow = m_sTARIDS[i].GETCROW();
		sTARID.dwID = m_sTARIDS[i].GETID();
		GLACTOR* pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, sTARID );
		if ( !pACTOR )					continue;

		// 대련 종료후 무적타임이면 공격 무시
		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( sTARID.dwID );
		if ( pChar )
		{
			if ( pChar->m_sCONFTING.IsPOWERFULTIME () )
				continue;
		}

		// 타겟반경 + 스킬 유효반경
		float fReActionRange = (float) ( pACTOR->GetBodyRadius() + fSkillRange );

		// 스킬 발동자와 타겟과의 거리
		D3DXVECTOR3 vDist = m_vPos - pACTOR->GetPosition();  
		float fDist = D3DXVec3Length(&vDist);

		if ( fReActionRange*GLCONST_CHAR::fREACT_VALID_SCALE < fDist )
		{
			m_sTARIDS[i].wID = USHRT_MAX;
		}
		else
		{
			dwVALIDNUM++;
		}
	}

	// 스킬 유효반경 내에 타겟이 없을 경우
	if ( dwVALIDNUM==0 )
	{
		TurnAction ( GLAT_IDLE );

		//	Note : 자신에게.
		GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
		NetMsg.emTarCrow		= m_TargetID.emCrow;
		NetMsg.dwTarID			= m_TargetID.dwID;
		NetMsg.nDamage			= 0;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );
	}

	m_fattTIMER += fElapsedTime*GLCHARLOGIC::GETATTVELO();
	m_fattTIMER += GLCHARLOGIC::GETATT_ITEM();

	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK &vecAniAttack = ConstCharClass.m_ANIMATION[m_SKILLMTYPE][m_SKILLSTYPE];
	if ( vecAniAttack.empty() )	return FALSE;

	//	Note : 특정 공격 에니메이션 설정 선택. (수정요)
	//
	const SANIATTACK &sAniAttack = vecAniAttack[0];

	//
	//
	int nTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	DWORD dwThisKey = sAniAttack.m_dwSTime + DWORD(m_fattTIMER*UNITANIKEY_PERSEC);
	
	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( dwThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
			{
				//CONSOLEMSG_WRITE( "SkillProcess m_sHITARRAY1 SkillProc[%u]", m_idACTIVESKILL.dwID );
				SkillProc ( m_idACTIVESKILL, sStrike.bLowSP );
				//TurnAction ( GLAT_IDLE );
			}
			else
			{
				//CONSOLEMSG_WRITE( "SkillProcess m_sHITARRAY1 AvoidProc[%u]", m_idACTIVESKILL.dwID );
				AvoidProc ( m_idACTIVESKILL, sStrike.bLowSP );
				//TurnAction ( GLAT_IDLE );
			}

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	//
	if ( dwThisKey >= sAniAttack.m_dwETime )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();

				if ( sStrike.bHit )		
				{
					//CONSOLEMSG_WRITE( "SkillProcess m_sHITARRAY2 SkillProc[%u]", m_idACTIVESKILL.dwID );
					SkillProc ( m_idACTIVESKILL, sStrike.bLowSP );
					//TurnAction ( GLAT_IDLE );
				}
				else
				{
					//CONSOLEMSG_WRITE( "SkillProcess m_sHITARRAY2 AvoidProc[%u]", m_idACTIVESKILL.dwID );
					AvoidProc ( m_idACTIVESKILL, sStrike.bLowSP );
					//TurnAction ( GLAT_IDLE );
				}

				//	큐 데이타가 처리된 경우 삭제한다.
				m_sHITARRAY.pop_front ();
			}
		}

		if ( !sAniAttack.IsLOOP() )	return FALSE;

		//	Note : 스킬 반복.
		//
		EMSKILLCHECK emCHECK = GLCHARLOGIC::CHECHSKILL ( m_idACTIVESKILL, 1, IsDefenseSkill() );
		if ( emCHECK != EMSKILL_OK && emCHECK != EMSKILL_NOTSP )
		{
			//	Note : 스킬 구동 실패 FB 메시지.
			GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
			NetMsgFB.emSKILL_FB = emCHECK;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgFB );
			return FALSE;
		}

		//	Note : SKILL 구동.
		//
		SETACTIVESKILL ( m_idACTIVESKILL );

		BOOL bLowSP =  (emCHECK==EMSKILL_NOTSP) ? TRUE : FALSE;
		PreStrikeProc ( TRUE, bLowSP );

		//	Note : 스킬 사용시 소모값들 소모시킴.
		//
		GLCHARLOGIC::ACCOUNTSKILL ( m_idACTIVESKILL, 1, true );

		m_fattTIMER = 0.0f;
	}

	return TRUE;
}

inline HRESULT GLChar::UpdateClientState ( float fElapsedTime )
{
	//	Note : 증가수치를 클라이언트가 예측을 하게 되면? 메세지 발생을 안해도 된다.
	//		단 약간의 오차가 발생할 소지가 있음.
	//
	m_fSTATE_TIMER += fElapsedTime;
	if ( m_fSTATE_TIMER>1.6f )
	{
		m_fSTATE_TIMER = 0.0f;

		//	Note : 자신, 대련자에게 hp,mp 값 갱신.
		//
		//	파티원에게는 hp,mp 갱신하지 않는다.
		MsgSendUpdateState ( false );
	}

	m_fMBR_TIMER += fElapsedTime;

	if ( m_fMBR_TIMER > 1.6f )
	{
		m_fMBR_TIMER = 0.0f;

		GLPartyFieldMan& sPartyFieldMan = m_pGLGaeaServer->GetPartyMan();
		if ( sPartyFieldMan.GetParty ( m_dwPartyID ) )
		{
			GLMSG::SNET_PARTY_MBR_POS NetMsg;
			NetMsg.dwGaeaID = m_dwGaeaID;
			NetMsg.vPos = m_actorMove.Position();
			NetMsg.sHP = m_sHP;
			NetMsg.sMP = m_sMP;

			sPartyFieldMan.SendMsgToMember ( m_dwPartyID, (NET_MSG_GENERIC*) &NetMsg );
		}
	}

	return S_OK;
}

VOID GLChar::UpdateLandEffect()
{
	std::set<int> setOldLandEffectNum = m_setLandEffectNum;
	std::set<int>::iterator iter_EffNum;
	m_setLandEffectNum.clear();

	SLEVEL_ETC_FUNC *pLevelEtcFunc = m_pLandMan->GetLevelEtcFunc();
	if( !pLevelEtcFunc ) return;
	if( !pLevelEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] ) return;

	int i;
	for( i = 0; i < (int)pLevelEtcFunc->m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT landEffect = pLevelEtcFunc->m_vecLandEffect[i];
		if( m_vPos.x > landEffect.vMinPos.x &&
			m_vPos.z > landEffect.vMinPos.y &&
			m_vPos.x < landEffect.vMaxPos.x &&
			m_vPos.z < landEffect.vMaxPos.y )
		{
			m_setLandEffectNum.insert(i);
		}
	}

	BOOL bUpdateLandEff = FALSE;
	if( setOldLandEffectNum.size() != m_setLandEffectNum.size() )
	{
		bUpdateLandEff = TRUE;
	}else{
		iter_EffNum = m_setLandEffectNum.begin();
		for( ; iter_EffNum != m_setLandEffectNum.end(); ++iter_EffNum )
		{
			if( setOldLandEffectNum.find( *iter_EffNum ) == setOldLandEffectNum.end() )
			{
				bUpdateLandEff = TRUE; 
				break;
			}
		}
	}

	if( bUpdateLandEff )
	{
		DISABLEALLLANDEFF();

		GLMSG::SNETPC_LANDEFFECT NetMsg;
		int iNum = 0;
		iter_EffNum = m_setLandEffectNum.begin();
		for( ; iter_EffNum != m_setLandEffectNum.end(); ++iter_EffNum )
		{

			SLANDEFFECT landEffect = pLevelEtcFunc->FindLandEffect(*iter_EffNum);
			if( !landEffect.IsUse() ) continue;
			ADDLANDEFF( landEffect, iNum );
			NetMsg.sLandEffect[iNum] = landEffect;
			iNum++;
		}

		NetMsg.dwGaeaID		= m_dwGaeaID;	
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsg );
		SendMsgViewAround( (NET_MSG_GENERIC*) &NetMsg ); 	

		
	}


	// 모든 버프를 취소해야할 경우의 처리
	for( int i = 0; i < EMLANDEFFECT_MULTI; i++ )
	{
		SLANDEFFECT landEffect = m_sLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			DisableSkillFact();
			break;
		}
	}


}

BOOL GLChar::ShockProcess ( float fElapsedTime )
{
	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK &vecShock = ConstCharClass.m_ANIMATION[AN_SHOCK][AN_SUB_NONE];
	if ( vecShock.empty() )	return FALSE;

	m_fIdleTime += fElapsedTime * ( GETMOVEVELO() + GETMOVE_ITEM() );
			
	SANIATTACK &sAniShock = vecShock[0];

	int wTotalKeys = int(sAniShock.m_dwETime) - int(sAniShock.m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);
	
	if ( wThisKey >= wTotalKeys )	return FALSE;

	return TRUE;
}

float GLChar::GetMoveVelo ()
{
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = fDefaultVelo * ( GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM() ) ;
	return fMoveVelo;
}

void GLChar::ResetConfront ( EMCONFRONT_END emEND )
{
	//	Note : 체력 환원.
	//
	DoConftStateRestore();

	//	Note : 대련 종료를 (자신) 에이전트에 알림.
	GLMSG::SNETPC_CONFRONT_END2_AGT NetMsgMyAgt;
	NetMsgMyAgt.emEND = emEND;
	m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgMyAgt );

	//	Note : (자신) 클라이언트에.
	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgMyClt;
	NetMsgMyClt.emEND = emEND;
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgMyClt );

	//	파티에 있는 '대련' 참가정보를 리셋.
	GLMSG::SNETPC_CONFRONT_END2_CLT_MBR NetMsgTarMbr;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			GLPARTY_FIELD *pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
			if ( pParty )
			{
				NetMsgTarMbr.dwID = m_dwGaeaID;

				//	대련 리스트에서 제거.
				pParty->MBR_CONFRONT_LEAVE(m_dwGaeaID);

				//	파티원에게 대련에서 빠져 나감을 알림.
				m_pGLGaeaServer->SENDTOPARTYCLIENT ( m_dwPartyID, &NetMsgTarMbr );

				//	대련 상대방 파티원에게.
				GLPARTY_FIELD *pPartyConft = m_pGLGaeaServer->GetParty ( pParty->m_dwconftPARTYID );
				if ( pPartyConft )
				{
					//	파티원에게 대련에서 빠져 나감을 알림.
					m_pGLGaeaServer->SENDTOPARTYCLIENT ( pPartyConft->m_dwPARTYID, &NetMsgTarMbr );
				}
			}
		}
		break;

	case EMCONFT_GUILD:
		{
			GLClubMan &cClubMan = m_pGLGaeaServer->GetClubMan();
			GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
			if ( pCLUB )
			{
				NetMsgTarMbr.dwID = m_dwCharID;

				//	Note : 자신의 클럽 정보에서 대련참가 멤버에서 제거.
				pCLUB->DELCONFT_MEMBER ( m_dwCharID );

				//	Note : 자신의 클럽원에게.
				m_pGLGaeaServer->SENDTOCLUBCLIENT ( m_dwGuild, &NetMsgTarMbr );

				//	Note : 상대방 클럽원에게.
				GLCLUB *pTAR_CLUB = cClubMan.GetClub ( m_sCONFTING.dwTAR_ID );
				if ( pTAR_CLUB )
				{
					m_pGLGaeaServer->SENDTOCLUBCLIENT ( m_sCONFTING.dwTAR_ID, &NetMsgTarMbr );
				}
			}
		}
		break;
	};

	//	Note : 주변 유닛들에 대련종료 통보.
	//
	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd;
	NetMsgTarBrd.dwGaeaID = m_dwGaeaID;
	NetMsgTarBrd.emEND = emEND;
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgTarBrd );

	m_sCONFTING.RESET ();
	m_sCONFTING.SETPOWERFULTIME ( 10.0f );
}

void GLChar::VietnamGainCalculate()
{

	if ( IsSTATE(EM_ACT_WAITING) )		return; 
	if ( m_dwVietnamGainType == GAINTYPE_EMPTY )		return;
	// 중국 누적 시간 별 수익 변경
	CTimeSpan gameTime( 0, (int)m_sVietnamSystem.gameTime / 60, (int)m_sVietnamSystem.gameTime % 60, 0 );
	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan, crtGame15Span;
	CTime	  loginTime   = m_sVietnamSystem.loginTime;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;


	LONGLONG totalHours	  = crtGameSpan.GetTotalHours();


	// 5시간 이상이면 계산하지 않고 넘어간다.
	// 수익 등급이 바뀌면 메시지를 보냄
	if( totalHours >= 5 && m_dwVietnamGainType != GAINTYPE_EMPTY )
	{
		m_dwVietnamGainType = GAINTYPE_EMPTY;	

		GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
		NetMsg.dwGainType = (BYTE)GAINTYPE_EMPTY;
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
		// 수익 50%
	}else if( totalHours >= 3 && m_dwVietnamGainType != GAINTYPE_HALF )
	{	
		m_dwVietnamGainType = GAINTYPE_HALF;	

		GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
		NetMsg.dwGainType = (BYTE)GAINTYPE_HALF;
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
	}
	m_sVietnamSystem.currentGameTime = totalHours;
}

void GLChar::ChinaGainCalculate()
{
	if ( IsSTATE(EM_ACT_WAITING) )		return; 
	// 18세 이상이면 리턴한다.
	if( m_sChinaTime.userAge == 1 ) return;

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	// 중국 누적 시간 별 수익 변경
	CTimeSpan gameTime( 0, (int)m_sChinaTime.gameTime / 60, (int)m_sChinaTime.gameTime % 60, 0 );
	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan, crtGame15Span;
	CTime	  loginTime   = m_sChinaTime.loginTime;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

	// 수익 0%
	//if( m_ChinaGainType == 2 )
	//{
	//	crtGame15Span = crtTime - m_China15Time;
	//	// 15분이 지나면 메시지를 보냄
	//	if( crtGame15Span.GetTotalMinutes() > 15 )
	//	{
	//		m_China15Time	= CTime::GetCurrentTime();

	//		GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
	//		NetMsg.dwGainType = 3;
	//		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
	//	}
	//}else
	if( m_ChinaGainType != 2 )
	{
		LONGLONG totalHours	  = crtGameSpan.GetTotalHours();
//		LONGLONG totalMinutes = crtGameSpan.GetTotalMinutes();
//		LONGLONG totalSecond  = crtGameSpan.GetTotalSeconds();

		// 수익 등급이 바뀌면 메시지를 보냄
		if( totalHours >= 5 && m_ChinaGainType != GAINTYPE_EMPTY )
			//if( totalSecond >= 60 && m_ChinaGainType != 2 )
		{
			m_China15Time	= CTime::GetCurrentTime();
			m_ChinaGainType = GAINTYPE_EMPTY;	

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = GAINTYPE_EMPTY;
			m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
			// 수익 50%
		}else if( totalHours >= 3 && m_ChinaGainType != GAINTYPE_HALF  )
			//}else if( totalSecond >= 50 && m_ChinaGainType != 2 )
		{	
			m_ChinaGainType = GAINTYPE_HALF;

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = GAINTYPE_HALF;
			m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
		}else if( totalHours >= 1 && m_ChinaGainType == 255 )
			//}else if( totalSecond >= 40 && m_ChinaGainType != 2 )
		{
			m_ChinaGainType = 0;

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = 4;
			m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
		}
	}
	m_sChinaTime.currentGameTime = crtGameSpan.GetTotalMinutes();
}

void GLChar::EventCalculate()
{
	if ( IsSTATE(EM_ACT_WAITING) )		return; 

	SEventState sEventState = m_pGLGaeaServer->m_sEventState;

	if( sEventState.bEventStart == FALSE ) return;

	// 이벤트 적용 레벨이 아니면 리턴
	if( m_bEventApply == FALSE ) return;

	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan;
	CTime	  loginTime			 = m_sEventTime.loginTime;
	crtGameSpan					 = ( crtTime - loginTime );
	m_sEventTime.currentGameTime = crtGameSpan.GetTimeSpan();

	// 이벤트가 새로 시작 됨
	if( m_bEventStart == FALSE )
	{
		// 테스트 시엔 Second로 한다.
		if( crtGameSpan.GetTotalSeconds() >= sEventState.EventPlayTime )
		{
			m_bEventStart = TRUE;

			//	Note : 자신의 클라이언트에.
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START NetMsg;
			NetMsg.loginTime = m_sEventTime.loginTime;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*)&NetMsg );

		}
	}

	if( m_bEventStart == TRUE )
	{
		if( crtGameSpan.GetTotalSeconds() >= sEventState.EventPlayTime + sEventState.EventBusterTime )
		{
			m_bEventStart = FALSE;
			m_sEventTime.loginTime = crtTime.GetTime();

			// 새로 갱신된 이벤트 시간
			GLMSG::SNET_GM_LIMIT_EVENT_RESTART NetMsg;
			NetMsg.restartTime = crtTime.GetTime();

			m_pGLGaeaServer->SENDTOAGENT(m_dwClientID,&NetMsg);

			//	Note : 자신의 클라이언트에.
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END NetMsg2;
			NetMsg2.loginTime = m_sEventTime.loginTime;
			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*)&NetMsg2 );
		}
	}




}

HRESULT GLChar::FrameMove ( float fTime, float fElapsedTime )
{
	GASSERT(m_pLandMan);

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

#if defined(VN_PARAM) //vietnamtest%%%
	// 베트남 탐닉 방지 계산
	VietnamGainCalculate();
#endif

#ifdef CH_PARAM_USEGAIN
	// 중국 탐닉 방지 계산
	ChinaGainCalculate();
#else
	// 리미트 이벤트 계산
	EventCalculate();
#endif

	if ( !IsSTATE(EM_ACT_WAITING) && m_bEntryFailed )
	{
		//  진입조건이 맞지않아 다른 곳으로 이동 해야하는 경우 메시지를 보낸다.
		GLMSG::SNETENTRY_FAILED NetMsg;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );

		m_bEntryFailed = FALSE;
	}

	

	// 탈것 배터리 체크
	UpdateVehicle( fElapsedTime );


	if ( !IsSTATE(EM_GETVA_AFTER) && !(m_pGLGaeaServer->IsReserveServerStop ()) )	return S_FALSE;

	

	m_fAge += fElapsedTime;
	m_fGenAge += fElapsedTime;

	m_fSkillDelay += fElapsedTime;

	//	트레이드 상태 갱신.
	if ( m_sTrade.Valid() )
	{
		m_sTrade.FrameMove ( fTime, fElapsedTime );

		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( m_sTrade.GetTarget() );
		if ( !pChar )
		{
			GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
			NetMsg.emCancel = EMTRADE_CANCEL_NORMAL;

			m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );

			m_sTrade.Reset ();
		}
	}

	//	DB에 정보 저장할 시간이 되었는지 검사후에 저장 수행.
	//
	m_fSAVEDB_TIMER += fElapsedTime;
	if ( m_fSAVEDB_TIMER > 1800.0f )
	{
		m_fSAVEDB_TIMER = 0.0f;
		m_pGLGaeaServer->SaveCharDB ( m_dwGaeaID );
	}

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		//	Note : 대련 유효성 검사.
		//
		bool bvalid(false);
		PGLCHAR pTAR_ONE(NULL);
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				pTAR_ONE = m_pGLGaeaServer->GetChar ( m_sCONFTING.dwTAR_ID );
				if ( pTAR_ONE )		bvalid = true;
			}
			break;

		case EMCONFT_PARTY:
			{
				bool bpartyA(false), bpartyB(false);
				if ( PARTY_NULL!=m_dwPartyID && m_pGLGaeaServer->GetParty(m_dwPartyID) )						bpartyA = true;
				if ( PARTY_NULL!=m_sCONFTING.dwTAR_ID && m_pGLGaeaServer->GetParty(m_sCONFTING.dwTAR_ID) )	bpartyB = true;

				if ( bpartyA && bpartyB )	bvalid = true;
			}
			break;

		case EMCONFT_GUILD:
			{
				GLClubMan &sClubMan = m_pGLGaeaServer->GetClubMan();

				bool bclubA(false), bclubB(false);
				if ( CLUB_NULL!=m_dwGuild && sClubMan.GetClub(m_dwGuild) )							bclubA = true;
				if ( CLUB_NULL!=m_sCONFTING.dwTAR_ID && sClubMan.GetClub(m_sCONFTING.dwTAR_ID) )	bclubB = true;

				if ( bclubA && bclubB )	bvalid = true;
			}
			break;
		};
		

		if ( !bvalid )
		{
			ResetConfront ( EMCONFRONT_END_FAIL );
		}
		else
		{
			bool bFIGHT = m_sCONFTING.UPDATE(fElapsedTime);
			if ( bFIGHT )
			{
				//	Note : 대련 FIGHT 알림.
				//
				GLMSG::SNETPC_CONFRONT_FIGHT2_CLT NetMsg;
				m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );
			}

			switch ( m_sCONFTING.emTYPE )
			{
			case EMCONFT_ONE:
				{
					if ( m_sCONFTING.sOption.bBOUND )
					{
						D3DXVECTOR3 vDistance = m_vPos - m_sCONFTING.vPosition;
						float fDistance = D3DXVec3Length ( &vDistance );

						if ( GLCONST_CHAR::fCONFRONT_ONE_DIST < fDistance )
						{
							ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );

							ResetConfront ( EMCONFRONT_END_DISLOSS );
							pTAR_ONE->ResetConfront ( EMCONFRONT_END_DISWIN );
						}
					}

					//	대련이 '조건'에 의해서 취소됫을 경우.
					if ( !m_sCONFTING.IsCONFRONTING() )		break;

					//	Note : 서로 같은 파티인지 검사.
					//
					if ( GetPartyID()!=PARTY_NULL && pTAR_ONE->GetPartyID()==GetPartyID() )
					{
						ResetConfront ( EMCONFRONT_END_PARTY );
					}
				}
				break;

			case EMCONFT_PARTY:
				{
					if ( m_sCONFTING.sOption.bBOUND )
					{
						D3DXVECTOR3 vDistance = m_vPos - m_sCONFTING.vPosition;
						float fDistance = D3DXVec3Length ( &vDistance );

						if ( GLCONST_CHAR::fCONFRONT_PY_DIST < fDistance )
						{
							ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );

							ResetConfront ( EMCONFRONT_END_DISLOSS );
						}
					}
				}
				break;

			case EMCONFT_GUILD:
				{
					if ( m_sCONFTING.sOption.bBOUND )
					{
						D3DXVECTOR3 vDistance = m_vPos - m_sCONFTING.vPosition;
						float fDistance = D3DXVec3Length ( &vDistance );

						if ( GLCONST_CHAR::fCONFRONT_CLB_DIST < fDistance )
						{
							ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP );

							ResetConfront ( EMCONFRONT_END_DISLOSS );
						}
					}
				}
				break;
			};
		}
	}
	// 대련종료후 딜레이 시간 갱신
	else if ( m_sCONFTING.IsPOWERFULTIME () )
	{
		m_sCONFTING.UPDATEPWRFULTIME ( fElapsedTime );
	}

	//	Note : 적대 행위자의 타이머 갱신 및 관리.
	{
		GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;		
		SPLAYHOSTILE* pHOSTILE = NULL;
		for ( MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin() ; pos!=m_mapPlayHostile.end() ; )
		{
			pHOSTILE = pos->second;
			if( !pHOSTILE )
			{
				//	Note : 적대자 삭제 알림.				
				NetMsgDel.dwCharID = pos->first;
				m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgDel );

				CDebugSet::ToLogFile( "GLChar::FrameMove, pos->second = NULL" );
				m_mapPlayHostile.erase( pos++ );
				continue;
			}

			pHOSTILE->fTIME -= fElapsedTime;
			if ( pHOSTILE->fTIME < 0 )
			{
				//	Note : 적대자 삭제 알림.				
				NetMsgDel.dwCharID = pos->first;
				m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgDel );

				SAFE_DELETE( pHOSTILE );
				m_mapPlayHostile.erase( pos++ );
				continue;
			}
			++pos;
		}

		if ( IsValidBody() && m_nBright < 0 )
		{
			m_fBRIGHT_TIMER += fElapsedTime;
			if ( m_fBRIGHT_TIMER >= GLCONST_CHAR::fPK_POINT_DEC_RATE )
			{
				m_fBRIGHT_TIMER = 0.0f;

				//	Note : 속성수치 변화.
				m_nBright += 1;

				GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
				NetMsg.nBright = m_nBright;
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg);

				GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.nBright = m_nBright;
				SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
			}
		}
	}

	//	시간 갱신.
	//
	m_fPERIOD_TIMER += fElapsedTime;
	if ( m_fPERIOD_TIMER > 360.0f )
	{
		m_fPERIOD_TIMER = 0.0f;
		MsgSendPeriod ();
	}

	m_fITEM_TIMER += fElapsedTime;
	if ( m_fITEM_TIMER > 1800.0f )
	{
		m_fITEM_TIMER = 0.0f;
		RESET_TIMELMT_ITEM ();
		RESET_CHECK_ITEM ();
	}

	m_fMoveDelay += fElapsedTime;

	if ( m_sQITEMFACT.IsACTIVE() )
	{
		bool bOK = UPDATE_QITEMFACT(fElapsedTime);
		if ( !bOK )
		{
			GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;

			//	Note : 종료되었을 경우 자신에게 알림.
			m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgBrd );

			//	Note : 종료되었을 경우 주변 사람에게 알림.
			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
		}
	}

	if ( m_lnLastSendExp != m_sExperience.lnNow )
	{
		m_lnLastSendExp = m_sExperience.lnNow;

		//	Note : 경험치 변화 클라이언트에 알려줌.
		//
		GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
		NetMsgExp.lnNowExp = m_sExperience.lnNow;
		m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgExp );

		if ( m_sExperience.ISOVER() )
		{
			//	Note : 랩업 수행. ( 로컬 메시지 발생. )
			//
			GLMSG::SNETPC_REQ_LEVELUP NetMsg;
			MsgReqLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

			m_lnLastSendExp = m_sExperience.lnNow;


		}else{
			m_bVietnamLevelUp = FALSE; 
		}
	}

	if ( m_lnLastSendVNExp != m_lVNGainSysExp )
	{
		m_lnLastSendVNExp = m_lVNGainSysExp;

		//	Note : 경험치 변화 클라이언트에 알려줌.
		//
		GLMSG::SNETPC_VIETNAM_GAIN_EXP NetMsgExp;
		NetMsgExp.gainExp = m_lVNGainSysExp;
		m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgExp );

	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(HK_PARAM)
	if( m_lnTempMoney != m_lnMoney )
	{
		if( m_bMoneyUpdate )
		{
			m_bMoneyUpdate = FALSE;
		}else{
			// 해킹으로 로그를 남김
			HACKINGLOG_WRITE( "Different Update Money!!, Account[%s], ID[%s], Money %I64d, TempMoney %I64d, Money Gap %I64d", 
								m_szUID, m_szName, m_lnMoney, m_lnTempMoney, m_lnMoney - m_lnTempMoney );	
		}
		m_lnTempMoney = m_lnMoney;
	}else{
		m_bMoneyUpdate = FALSE;
	}

	if( m_lnTempStorageMoney != m_lnStorageMoney )
	{
		if( m_bStorageMoneyUpdate )
		{
			m_bStorageMoneyUpdate = FALSE;
		}else{
			// 해킹으로 로그를 남김
			HACKINGLOG_WRITE( "Different Update Storage Money!!, Account[%s], ID[%s], Storage Money %I64d, TempStorage Money %I64d, Storage Money Gap %I64d", 
							   m_szUID, m_szName, m_lnStorageMoney, m_lnTempStorageMoney, m_lnStorageMoney - m_lnTempStorageMoney );	
		}
		m_lnTempStorageMoney = m_lnStorageMoney;
	}else{
		m_bStorageMoneyUpdate = FALSE;
	}
#endif

	//	Note : 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	if ( m_TargetID.dwID != EMTARGET_NULL )
	{
		if ( !m_pGLGaeaServer->ValidCheckTarget(m_pLandMan,m_TargetID) )
			m_TargetID.dwID = EMTARGET_NULL;
	}
	
	if ( m_TargetID.dwID == EMTARGET_NULL && IsACTION(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );

	if ( IsValidBody () )
	{
		DoQuestCheckLimitTime ( fTime, fElapsedTime );

		if ( m_sHP.wNow>0 )
		{
			float fCONFT_POINT_RATE(1.0f);
			if ( m_sCONFTING.IsCONFRONTING() )		fCONFT_POINT_RATE = m_sCONFTING.sOption.fHP_RATE;


			// 지형 이상효과 업데이트
			UpdateLandEffect();

			
			//	Note : 기초 LOGIC 정보를 업대이트.
			//
			GLCHARLOGIC::UPDATE_DATA ( fTime, fElapsedTime, FALSE, fCONFT_POINT_RATE );

			UpdateClientState ( fElapsedTime );

			m_ARoundSlot.Update ( m_pLandMan );

			DoQuestReachZone();
		}
		else
		{
			//	Note : 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 )
			DoFalling ();

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			if ( m_bTracingUser )
			{
				NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = GetUserID();
				StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );

				CString strTemp;
				strTemp.Format( "Dead!!, [%s][%s], Money:[%I64d]", m_szUID, m_szName, m_lnMoney );
				StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

				m_pGLGaeaServer->SENDTOAGENT( m_dwClientID, &TracingMsg );
			}
#endif
		}
	}

	// 맵의 위치가 바꼈을때 퀘스트 실패 여부를 판단한다.
	if( m_sMapID != m_sOldMapID )
	{
		DoQuestCheckLeaveMap();

		m_sOldMapID = m_sMapID;
	}

	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		{
			//	Note : 케릭의 이동 업데이트.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo () );
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE );
			}

			//	Note : 케릭의 현제 위치 업데이트.
			//
			m_vPos = m_actorMove.Position();

			//	Note : 케릭의 현제 방향 업데이트.
			//
			D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
			if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection = vMovement - m_vPos;
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					m_vDir = vDirection;
				}
			}
		}
		break;

	case GLAT_ATTACK:
		{
			if ( !AttackProcess ( fElapsedTime ) )
			{
				TurnAction ( GLAT_IDLE );
			}

			GLACTOR* pACTOR = m_pGLGaeaServer->GetTarget ( m_pLandMan, m_TargetID );
			if ( pACTOR )
			{
				D3DXVECTOR3 vDirection = pACTOR->GetPosition() - m_vPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			if( !SkillProcess ( fElapsedTime ) )		
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_SHOCK:
		{
			if( !ShockProcess ( fElapsedTime ) )		TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_PUSHPULL:
		{
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE );
			}

			//	Note : Mob의 현제 위치 업데이트.
			//
			m_vPos = m_actorMove.Position();
		}
		break;

	case GLAT_FALLING:
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob의 현제 위치 업데이트.
	//
	m_vPos = m_actorMove.Position();

	return S_OK;
}

void GLChar::STATEBLOW ( const SSTATEBLOW &sStateBlow )
{
	int nIndex = 0;

	if ( sStateBlow.emBLOW <= EMBLOW_SINGLE )	nIndex = 0;
	else										nIndex = sStateBlow.emBLOW-EMBLOW_SINGLE;

	m_sSTATEBLOWS[nIndex] = sStateBlow;

	//	상태이상 ( 기절 ) 을 받을때 현제 행위 중단.
	if ( sStateBlow.emBLOW == EMBLOW_STUN )
	{
		//	Note : 쓰러짐을 클라이언트 들에게 알림.
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_IDLE;

		//	Note : 주변의 Char 에게.
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

		//	Note : 자신의 클라이언트에.
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );

		TurnAction ( GLAT_IDLE );
	}
}

void GLChar::CURE_STATEBLOW ( DWORD dwCUREFLAG )
{
	BOOL bChanged = FALSE;

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		if ( m_sSTATEBLOWS[i].emBLOW==EMBLOW_NONE )		continue;

		bChanged = TRUE;
		if ( STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW)&dwCUREFLAG )
		{
			m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
		}
	}

	if ( !bChanged )	return;

	//	Note : 상태 이상 변화 MsgFB.
	//
	GLMSG::SNETPC_CURESTATEBLOW_BRD NetMsgBRD;
	NetMsgBRD.dwID = m_dwGaeaID;
	NetMsgBRD.emCrow = CROW_PC;
	NetMsgBRD.dwCUREFLAG = dwCUREFLAG;

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBRD );

	//	Note : 대상에게 메세지 전송.
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBRD );
}

void GLChar::BUFF_REMOVE( DWORD dwBuffFlag )
{
	GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;

	if ( dwBuffFlag == 0 ) return;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		if ( dwBuffFlag == EMSPECA_BUFF_REMOVE )
		{
			if ( pSkill->m_sBASIC.emIMPACT_SIDE == SIDE_ENERMY )	continue;
		}
		else if ( dwBuffFlag == EMSPECA_DEBUFF_REMOVE )
		{
			if ( pSkill->m_sBASIC.emIMPACT_SIDE == SIDE_OUR )	continue;
		}

		DISABLESKEFF( i );

		NetMsgSkillBrd.bRESET[i] = true;
	}
	
	NetMsgSkillBrd.dwID = m_dwGaeaID;
	NetMsgSkillBrd.emCrow = CROW_PC;

	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgSkillBrd );
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgSkillBrd );
}

void GLChar::VAR_BODY_POINT ( const EMCROW emACrow, const DWORD dwAID, const BOOL bPartySkill, int nvar_hp, int nvar_mp, int nvar_sp )
{
	if ( m_sHP.wNow == 0 )	return;

	WORD wDxHP = m_sHP.VARIATION ( nvar_hp );
	m_sMP.VARIATION ( nvar_mp );
	m_sSP.VARIATION ( nvar_sp );

	m_sAssault.emCrow = emACrow;
	m_sAssault.dwID = dwAID;

	if ( emACrow == CROW_PC && nvar_hp<0 )
	{
		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwAID );
		if ( pChar )
		{
			AddDamageLog ( m_cDamageLog, dwAID, pChar->m_dwUserID, wDxHP );
		}
	}

	// 소환수가 때렸을 경우 소환수 주인이 때린걸로 등록
	if ( emACrow == CROW_SUMMON && nvar_hp<0 )
	{
		PGLSUMMONFIELD pSummon = GLGaeaServer::GetInstance().GetSummon ( dwAID );
		if( pSummon )
		{
			AddDamageLog ( m_cDamageLog, dwAID, pSummon->m_pOwner->m_dwUserID, wDxHP );
		}
	}

	//	[파티원들에게]
	if ( m_dwPartyID!=PARTY_NULL )
	{
		GLMSG::SNET_PARTY_MBR_POINT NetMsg;
		NetMsg.dwGaeaID = m_dwGaeaID;
		NetMsg.sHP = m_sHP;
		NetMsg.sMP = m_sMP;
		m_pGLGaeaServer->SENDTOPARTYCLIENT ( m_dwPartyID, (NET_MSG_GENERIC*) &NetMsg );
	}

	if ( nvar_hp<0 && IsACTION(GLAT_GATHERING) )
	{
		GLMSG::SNET_ACTION_BRD NetMsgBrd;
		NetMsgBrd.emCrow	= CROW_PC;
		NetMsgBrd.dwID		= m_dwGaeaID;
		NetMsgBrd.emAction	= GLAT_IDLE;

		//	Note : 주변의 Char 에게.
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );		
		TurnAction ( GLAT_IDLE );
	}
}

DWORD GLChar::SELECT_SKILLSLOT ( SNATIVEID skill_id )
{
	SKILLREALFACT_SIZE;
	DWORD dwSELECT = UINT_MAX;

	//	Note : 스킬이 들어갈 기본 슬롯 선택.
	//
	for ( DWORD i=0; i<SKILLREALFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID==skill_id )
		{
			return dwSELECT = i;
		}
	}

	float fAGE = FLT_MAX;
	for ( DWORD i=0; i<SKILLREALFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID==SNATIVEID(false) )
		{
			return dwSELECT = i;
		}

		if ( m_sSKILLFACT[i].fAGE < fAGE )
		{
			fAGE = m_sSKILLFACT[i].fAGE;
			dwSELECT = i;
		}
	}

	return dwSELECT;
}

BOOL GLChar::RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, DWORD &dwSELECT )
{
	dwSELECT = SKILLFACT_SIZE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )				return FALSE;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];

	BOOL bHOLD = FALSE;
	SSKILLFACT sSKILLEF;
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
		bHOLD = TRUE;
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	if ( pSkill->m_sAPPLY.emADDON != EMIMPACTA_NONE )
	{
		bHOLD = TRUE;
		sSKILLEF.emADDON = pSkill->m_sAPPLY.emADDON;
		sSKILLEF.fADDON_VAR = pSkill->m_sAPPLY.fADDON_VAR[wlevel];
	}

	switch ( pSkill->m_sAPPLY.emSPEC )
	{
	case EMSPECA_REFDAMAGE:
	case EMSPECA_NONBLOW:
	case EMSPECA_PIERCE:
	case EMSPECA_TARRANGE:
	case EMSPECA_MOVEVELO:
	case EMSPECA_ATTACKVELO:
	case EMSPECA_SKILLDELAY:
	case EMSPECA_PSY_DAMAGE_REDUCE:
	case EMSPECA_MAGIC_DAMAGE_REDUCE:
	case EMSPECA_PSY_DAMAGE_REFLECTION:
	case EMSPECA_MAGIC_DAMAGE_REFLECTION:
	case EMSPECA_DEFENSE_SKILL_ACTIVE:
		bHOLD = TRUE;
		sSKILLEF.emSPEC = pSkill->m_sAPPLY.emSPEC;
		sSKILLEF.fSPECVAR1 = pSkill->m_sAPPLY.sSPEC[wlevel].fVAR1;
		sSKILLEF.fSPECVAR2 = pSkill->m_sAPPLY.sSPEC[wlevel].fVAR2;
		sSKILLEF.dwSPECFLAG = pSkill->m_sAPPLY.sSPEC[wlevel].dwFLAG;
		sSKILLEF.dwNativeID = pSkill->m_sAPPLY.sSPEC[wlevel].dwNativeID;
		break;
	};

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//
	if ( bHOLD )
	{
		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.fAGE		= sSKILL_DATA.fLIFE;

		dwSELECT = SELECT_SKILLSLOT ( skill_id );
		m_sSKILLFACT[dwSELECT] = sSKILLEF;
	}

	return TRUE;
}

BOOL GLChar::RECEIVE_DRUGFACT ( EMITEM_DRUG emDRUG, WORD wCURE, BOOL bRATIO )
{
	//SKILL::EMTYPES emTYPE[3] = { SKILL::EMFOR_VARHP, SKILL::EMFOR_VARMP, SKILL::EMFOR_VARSP };
	//SNATIVEID nidNO[3] = { SNATIVEID(35,USHRT_MAX), SNATIVEID(35,USHRT_MAX), SNATIVEID(35,USHRT_MAX) };

	//if ( bRATIO )
	//{
	//	wCURE = m_sHP.wMax * wCURE / 100;
	//}

	//switch ( emDRUG )
	//{
	//case ITEM_DRUG_HP:
	//	nidNO[0].wSubID = 0;
	//	break;

	//case ITEM_DRUG_MP:
	//	nidNO[1].wSubID = 1;
	//	break;

	//case ITEM_DRUG_SP:
	//	nidNO[2].wSubID = 2;
	//	break;

	//case ITEM_DRUG_HP_MP:
	//	nidNO[0].wSubID = 0;
	//	nidNO[1].wSubID = 1;
	//	break;

	//case ITEM_DRUG_MP_SP:
	//	nidNO[1].wSubID = 1;
	//	nidNO[2].wSubID = 2;
	//	break;

	//case ITEM_DRUG_HP_MP_SP:
	//	nidNO[0].wSubID = 0;
	//	nidNO[1].wSubID = 1;
	//	nidNO[2].wSubID = 2;
	//	break;
	//};

	//for ( int i=0; i<3; ++i )
	//{
	//	const SNATIVEID &skill = nidNO[i];

	//	if ( skill.wSubID == USHRT_MAX )		continue;

	//	SSKILLFACT sSKILLEF;
	//	sSKILLEF.emTYPE = emTYPE[i];
	//	
	//	//	스킬 정보 가져옴.
	//	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill );
	//	if ( !pSkill )				return FALSE;
	//	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[0];

	//	float fRATE = wCURE/float(m_sHP.wMax);
	//	if ( fRATE>1.0f )	fRATE = 1.0f;

	//	float fLIFE = sSKILL_DATA.fLIFE;
	//	if ( fLIFE==0.0f )	fLIFE = 0.01f;

	//	sSKILLEF.sNATIVEID	= skill;
	//	sSKILLEF.wLEVEL		= 0;
	//	sSKILLEF.fAGE		= fLIFE;
	//	sSKILLEF.fMVAR		= fRATE/fLIFE;

	//	WORD wSELECT = SKILLREALFACT_SIZE + i;
	//	m_sSKILLFACT[wSELECT] = sSKILLEF;

	//	GLMSG::SNETPC_REQ_SKILLHOLDEX_BRD NetMsgBRD;
	//	NetMsgBRD.emCrow = GetCrow();
	//	NetMsgBRD.dwID = GetCtrlID();
	//	NetMsgBRD.wSLOT = wSELECT;
	//	NetMsgBRD.sSKILLEF = sSKILLEF;

	//	//	Note : '스킬대상'의 주변 클라이언트들에게 메세지 전송.
	//	//
	//	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

	//	//	Note : 스킬 대상에게 메세지 전송.
	//	m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID(), &NetMsgBRD );
	//}

	return TRUE;
}

void GLChar::ReBirthDecExp ()
{
	if ( m_nDECEXP==0 )		return;

	m_lnReExp = GetReExp();

	if ( (m_sExperience.lnNow-m_nDECEXP) < 0 )	m_sExperience.lnNow = 0;
	else	m_sExperience.lnNow -= m_nDECEXP;

	//	Note : 경험치 변화 클라이언트에 알려줌.
	//
	GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
	NetMsgExp.lnNowExp = m_sExperience.lnNow;
	m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgExp );
}

LONGLONG GLChar::GetReExp ()
{
	LONGLONG nDecExp = 0;
	LONGLONG nReExp = 0;

	nDecExp = m_nDECEXP;

	if ( nDecExp > m_sExperience.lnNow ) nDecExp = m_sExperience.lnNow;
	if ( nDecExp <= 0 )	return 0;


	float fRExpRecovery = GLOGICEX::GLDIE_RECOVERYEXP( m_wLevel );
	nReExp = (LONGLONG)(nDecExp * fRExpRecovery);

	if ( nReExp < 0  ) nReExp = 0;	
	if ( nDecExp < nReExp ) nReExp = nDecExp;

	return nReExp;

}
LONGLONG GLChar::GetReExpMoney ( LONGLONG nReExp )
{
	LONGLONG nDecMoney = 0;

	if ( nReExp <= 0 ) return 0;

	float fRMoney = GLOGICEX::GLDIE_EXPMONEY( m_wLevel );
	nDecMoney = (LONGLONG)(nReExp * fRMoney);

	return nDecMoney;
}

void GLChar::DROP_PUTONITEM ( DWORD _dwNUM, float _fRATE )
{
//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;
	GLITEMLMT & glItemmt = GLITEMLMT::GetInstance();

	for ( DWORD i=0; i<_dwNUM; ++i )
	{
		if ( RANDOM_GEN(_fRATE) )
		{
			DWORD dwSLOT_NUM(0);

			DWORD dwSLOT_LIST[SLOT_NSIZE_S_2];
			for ( int j=0; j<SLOT_NSIZE_S_2; ++j )
			{
				EMSLOT emSLOT = EMSLOT(j);

				if ( VALID_SLOT_ITEM(emSLOT) && !GET_SLOT_ITEM(emSLOT).IsGM_GENITEM() )
				{
					SITEM* pITEM = GET_SLOT_ITEMDATA(emSLOT);
					
					//	거래옵션
					if ( pITEM && pITEM->sBasicOp.IsTHROW() )
						dwSLOT_LIST[dwSLOT_NUM++] = j;
				}
			}

			if ( dwSLOT_NUM==0 )					break;

			DWORD dwSLOT = (DWORD) ( rand()%dwSLOT_NUM );
			
			if ( dwSLOT >= SLOT_NSIZE_S_2 )				break;

			dwSLOT = dwSLOT_LIST[dwSLOT];
			if ( !VALID_SLOT_ITEM(EMSLOT(dwSLOT)) )	break;

			// 보드는 드랍되지 않는다.
			if ( dwSLOT == SLOT_VEHICLE )			break;

			SITEMCUSTOM sITEM_CUSTOM = m_PutOnItems[dwSLOT];
			
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPos) + D3DXVECTOR3(RANDOM_NUM*4.0f,0.0f,RANDOM_NUM*4.0f);
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = m_pLandMan->IsCollisionNavi
			(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
			);

			if ( bCollision )
			{
				//	Note :아이템의 소유 이전 경로 기록.
				//
				glItemmt.ReqItemRoute ( sITEM_CUSTOM, ID_CHAR, m_dwCharID, ID_CHAR, 0, EMITEM_ROUTE_GROUND, sITEM_CUSTOM.wTurnNum );


				PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
				if ( pMyPet && pMyPet->IsValid () ) 
				{
					int nPetMID = pMyPet->m_sActiveSkillID.wMainID;
					int nPetSID = pMyPet->m_sActiveSkillID.wSubID;
                    // Note : 현재 펫 스킬 확인 로그 
					glItemmt.ReqAction ( m_dwCharID,		// 당사자.
										EMLOGACT_PET_SKILL, // 행위.
										ID_CHAR, 0,  0,				
										nPetMID,			// Pet Skill MID
										nPetSID, 			// Pet Skill SID
										0 );		
				}


				//	아이템 제거.
				GLCHARLOGIC::RELEASE_SLOT_ITEM ( (EMSLOT) dwSLOT );

				//	[자신에게] SLOT에 있었던 아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release((EMSLOT)dwSLOT);
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsg_PutOn_Release);

				//	아이템 발생.
				m_pLandMan->DropItem ( vGenPos, &sITEM_CUSTOM );
			}
		}
	}
}

void GLChar::GenerateReward ( bool bCONFT /*=false*/, bool bCONFT_SCHOOL /*=false*/ )
{
	//	Note : 경험치 손상 처리.
	//
	bool bBRIGHTEVENT = m_pGLGaeaServer->IsBRIGHTEVENT();

	m_nDECEXP = 0;
	// 범죄자 이거나 몹한테 죽었을경우
	if ( ISOFFENDER() || m_sAssault.emCrow!=CROW_PC )
	{
		m_nDECEXP = __int64 ( GLOGICEX::GLDIE_DECEXP(GETLEVEL()) * 0.01f * GET_LEVELUP_EXP() );
		
		if ( bCONFT )
		{
			//m_nDECEXP /= 10;	//	대련시 경험치 손상은 반감시킨다.
			m_nDECEXP = 0;		//	대련시 경험치 손상은 없음.
		}
		else
		{
			m_nDECEXP = __int64 ( m_nDECEXP * GET_PK_DECEXP_RATE()/100.0f );
		}
	}

	//	Note : 경험치 발생.
	//
	if ( m_nDECEXP > 0 )
	{
		//STARGETID mytarid(GETCROW(),m_dwGaeaID);

		//if ( m_sAssault.emCrow == CROW_PC && m_sAssault.dwID!=GAEAID_NULL )
		//{
		//	PGLCHAR pChar = m_pGLGaeaServer->GetChar ( m_sAssault.dwID );
		//	if ( pChar )
		//	{
		//		pChar->ReceiveKillExp ( mytarid, bCONFT_SCHOOL );
		//	}
		//}

		//if ( !m_cDamageLog.empty() )
		//{
		//	DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		//	DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		//	for ( ; iter!=iter_end; ++iter )
		//	{
		//		SDAMAGELOG &sDamageLog = (*iter).second;

		//		PGLCHAR pChar = m_pGLGaeaServer->GetChar ( sDamageLog.dwGaeaID );
		//		if ( !pChar )	continue;
		//		if ( sDamageLog.dwUserID != pChar->m_dwUserID )		continue;

		//		pChar->ReceiveAttackExp ( mytarid, sDamageLog.dwDamage, FALSE, bCONFT_SCHOOL );
		//	}
		//}
	}

	m_cDamageLog.clear ();

	// 성향 이벤트 중에는 경험치 감소하지 않고 아이템도 드랍되지 않음
	if ( bBRIGHTEVENT ) 
	{
		m_nDECEXP = 0; 
		return;
	}

	// 클럽 배틀중이면 경험치 손실 및 아이템 드랍 없음
	if ( m_sAssault.emCrow==CROW_PC && m_pLandMan->IsClubBattleZone() )
	{
		GLClubMan &cClubMan = GLGaeaServer::GetInstance().GetClubMan();
		GLCLUB *pCLUB = cClubMan.GetClub ( m_dwGuild );
		if ( pCLUB )
		{
			GLChar* pChar = GLGaeaServer::GetInstance().GetChar( m_sAssault.dwID );
			if ( pChar )
			{
				if ( pCLUB->IsBattle( pChar->m_dwGuild ) )
				{
					m_nDECEXP = 0;
					return;
				}
				
				GLCLUB *pCLUB_Tar = cClubMan.GetClub ( pChar->m_dwGuild );
				if ( pCLUB_Tar && pCLUB->IsBattleAlliance( pCLUB_Tar->m_dwAlliance ) )
				{
					m_nDECEXP = 0;
					return;
				}
			}
		}
	}

	// 경험치 NonDrop 맵일경우
	if ( !m_pLandMan->IsDecreaseExpMap () ) m_nDECEXP = 0;

	//	Note : 대련이 아니고 드롭맵일 경우 아이템 드롭. && m_bProtectPutOnItem(팻의 아이템 보호스킬)
	//
	if ( m_pLandMan && m_pLandMan->IsItemDrop() && !bCONFT && !m_bProtectPutOnItem )
	{
		//	범죄자일 경우
		if ( ISOFFENDER() )
		{
			DWORD dwDROP_NUM = GET_PK_ITEMDROP_NUM();
			float fDROP_RATE = GET_PK_ITEMDROP_RATE();
			DROP_PUTONITEM ( dwDROP_NUM, fDROP_RATE );
		}
		//	일반적으로 죽을경우.
		else if ( GLCONST_CHAR::fNONPK_ITEM_DROP!=0 )
		{
			DROP_PUTONITEM ( 1, GLCONST_CHAR::fNONPK_ITEM_DROP );
		}
	}
}

void GLChar::DelPlayHostile ()
{
//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;
	PGLCHAR pCHAR = NULL;
	for ( MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin(); pos != m_mapPlayHostile.end(); ++pos )
	{
		//	Note : 적대자 삭제 알림.		
		NetMsgDel.dwCharID = pos->first;
		m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgDel );

		pCHAR = m_pGLGaeaServer->GetCharID ( pos->first );
		if( pCHAR )
			pCHAR->DelPlayHostile ( m_dwCharID );

		if( !pos->second )
			CDebugSet::ToLogFile( "GLChar::DelPlayHostile, pos->second = NULL" );
	}

	DEL_PLAYHOSTILE_ALL();
}

void GLChar::DelPlayHostile ( DWORD dwCharID )
{
	bool bDEL = DEL_PLAYHOSTILE ( dwCharID );
	if ( bDEL )
	{
		//	Note : 적대자 삭제 알림.
		GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;
		NetMsgDel.dwCharID = dwCharID;
		m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgDel );
	}
}

//	특정 클럽의 적대자를 모두 지운다.
void GLChar::DelPlayHostileClub( DWORD dwClubID )
{
	PGLCHAR pCHAR = NULL;

	GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;
	
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin();
	MAPPLAYHOSTILE_ITER pos_end = m_mapPlayHostile.end();
	MAPPLAYHOSTILE_ITER del;

	GLClubMan &sClubMan = m_pGLGaeaServer->GetClubMan();
	GLCLUB *pClub = sClubMan.GetClub(m_dwGuild);
	if ( !pClub )	return;
	
	for ( ; pos != pos_end;  )
	{
		del = pos++;
		//	Note : 적대자 삭제 알림.	
		pCHAR = m_pGLGaeaServer->GetCharID ( del->first );
		if ( !pCHAR ) continue;

		GLCLUB *pClub_S = sClubMan.GetClub(pCHAR->m_dwGuild);
		if ( !pClub_S )	return;

		if ( pClub->IsBattle( pCHAR->m_dwGuild ) || pClub->IsBattleAlliance( pClub_S->m_dwAlliance ) )
		{
			NetMsgDel.dwCharID = del->first;
			m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgDel );
			
			pCHAR->DelPlayHostile ( m_dwCharID );
		}

		m_mapPlayHostile.erase( del );
	}
}


void GLChar::DoFalling ()
{
//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				STARGETID sCONFTID(CROW_PC,m_sCONFTING.dwTAR_ID);

				//	Note : 대련도중 대련자가 아닌 캐릭터나 몹에게 죽을 경우.
				//
				if( m_sAssault != sCONFTID )
				{
					//	Note : 상대방 대련자 ID 백업. Reset 시에 무효화되기 때문에 미리 백업.
					//
					const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;

					//	Note : (자신) 대련 종료.
					//
					ResetConfront(EMCONFRONT_END_FAIL);

					//	Note : 대련 상대방 찾음.
					//
					PGLCHAR pCONFT = m_pGLGaeaServer->GetChar(dwTAR_ID);
					if ( pCONFT )
					{
						//	Note : 대련 상대방이 영향을 준 경험치 로그 제거시킴.
						//
						DAMAGELOG_ITER iter;
						iter = m_cDamageLog.find ( pCONFT->m_dwUserID );
						if ( iter!=m_cDamageLog.end() )		m_cDamageLog.erase ( iter );

						//	Note : (상대방) 대련 종료.
						//
						pCONFT->ResetConfront ( EMCONFRONT_END_FAIL );
					}

					goto _DIE_ACTION;
				}

				//	Note : 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 등 등 )
				//
				GenerateReward ( true, m_sCONFTING.sOption.bSCHOOL );

				//	Note : 상대방 대련자 ID 백업. Reset 시에 무효화되기 때문에 미리 백업.
				//
				const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;
				const SCONFT_OPTION sOption = m_sCONFTING.sOption;

				//	Note : (자신) 대련 종료.
				//
				ResetConfront(EMCONFRONT_END_LOSS);

				//	Note : 대련 상대방 찾음.
				//
				PGLCHAR pCONFT = m_pGLGaeaServer->GetChar(dwTAR_ID);
				if ( pCONFT==NULL )			return;

				//	Note : (상대방) 대련 종료.
				//
				EMCONFRONT_END emCONFT_END = (pCONFT->GETHP()>0) ? EMCONFRONT_END_WIN : EMCONFRONT_END_LOSS;
				pCONFT->ResetConfront ( emCONFT_END );

				//	Note : 자신 생활점수 변화.
				//
				if ( GLCONST_CHAR::nCONFRONT_LOSS_LP!=0 && GLCONST_CHAR::nCONFRONT_WIN_LP!=0 )
				{
					float fLIFE_P(0);

					fLIFE_P = GLCONST_CHAR::nCONFRONT_LOSS_LP - (GETLEVEL()-pCONFT->GETLEVEL())/2.0f
						+ (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
					LIMIT ( fLIFE_P, 0.0f, -20.0f );

					ReceiveLivingPoint ( (int)fLIFE_P );

					//	Note : 상대방 생활점수 변화.
					//
					fLIFE_P = GLCONST_CHAR::nCONFRONT_WIN_LP + (GETLEVEL()-pCONFT->GETLEVEL())/2.0f
						+ (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
					LIMIT ( fLIFE_P, 20.0f, 0.0f );

					pCONFT->ReceiveLivingPoint ( (int)fLIFE_P );
				}
			}
			break;

		case EMCONFT_PARTY:
			{
				bool breward(false);
				if ( m_sAssault.emCrow==CROW_PC )
				{
					GLPARTY_FIELD *pConftParty = m_pGLGaeaServer->GetParty(m_sCONFTING.dwTAR_ID);
					if ( pConftParty )
					{
						const GLPARTY_FNET *pPARTY = pConftParty->GETMEMBER(m_sAssault.dwID);
						if ( pPARTY )	breward = pPARTY->m_bConfront;
					}
				}

				if ( !breward )
				{
					m_cDamageLog.clear();

					//	Note : (자신) 대련 종료.
					//
					ResetConfront(EMCONFRONT_END_NOTWIN);

					goto _DIE_ACTION;
				}

				//	Note : 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 등 등 )
				//
				GenerateReward ( true, m_sCONFTING.sOption.bSCHOOL );

				//	Note : (자신) 대련 종료.
				//
				ResetConfront(EMCONFRONT_END_NOTWIN);
			}
			break;

		case EMCONFT_GUILD:
			{
				bool breward(false);
				if ( m_sAssault.emCrow==CROW_PC )
				{
					GLClubMan &sClubMan = m_pGLGaeaServer->GetClubMan();
					PGLCHAR pCHAR = m_pGLGaeaServer->GetChar(m_sAssault.dwID);

					GLCLUB *pConftClub = sClubMan.GetClub(m_sCONFTING.dwTAR_ID);
					if ( pConftClub && pCHAR )
					{
						breward = pConftClub->ISCONFT_MEMBER ( pCHAR->m_dwCharID );
					}
				}

				if ( !breward )
				{
					m_cDamageLog.clear();

					//	Note : (자신) 대련 종료.
					//
					ResetConfront(EMCONFRONT_END_NOTWIN);

					goto _DIE_ACTION;
				}

				//	Note : 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 등 등 )
				//
				GenerateReward ( true );

				//	Note : (자신) 대련 종료.
				//
				ResetConfront(EMCONFRONT_END_NOTWIN);
			}
			break;
		}

		//	Memo :	대련중이라면 아래코드는 실행하지 않는다.
		return;
	}

	if ( m_sQITEMFACT.IsACTIVE() )
	{
		m_sQITEMFACT.RESET();

		GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;

		//	Note : 종료되었을 경우 자신에게 알림.
		m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID (), &NetMsgBrd );

		//	Note : 종료되었을 경우 주변 사람에게 알림.
		SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );
	}

_DIE_ACTION:

	//	Note : player 가 죽였을 경우.
	DWORD dwKILL_ID(0);
	if ( m_sAssault.emCrow==CROW_PC )
	{
		PGLCHAR pCHAR = m_pGLGaeaServer->GetChar(m_sAssault.dwID);
		if ( pCHAR )
		{
			dwKILL_ID = pCHAR->m_dwCharID;
			int nBRIGHT(0), nLIFE(0);

			bool bClubBattle = false;
			bool bAllianceBattle = false;
			GLClubMan &cClubMan = m_pGLGaeaServer->GetClubMan();
			GLCLUB *pMyClub = cClubMan.GetClub ( m_dwGuild );
			GLCLUB *pTarClub = cClubMan.GetClub ( pCHAR->m_dwGuild );
			
			if ( pMyClub && pTarClub )
			{
				bClubBattle =  pMyClub->IsBattle( pCHAR->m_dwGuild );
				bAllianceBattle = pMyClub->IsBattleAlliance ( pTarClub->m_dwAlliance );
			}

			bool bClubDeathMatch = false;

			if ( m_pLandMan->m_bClubDeathMatchMap )
			{
				GLClubDeathMatch* pCDM = GLClubDeathMatchFieldMan::GetInstance().Find( m_pLandMan->m_dwClubMapID );
				if( pCDM && pCDM->IsBattle() ) bClubDeathMatch = true;
			}

			// 클럽배틀시 죽었을 경우 처리
			if ( ( bClubBattle || bAllianceBattle ) && m_pLandMan->IsClubBattleZone() )
			{
				ClubBattleSetPoint( m_dwGuild, pCHAR->m_dwGuild, bAllianceBattle );

				DWORD dwClubID_P, dwClubID_S;
				if ( bAllianceBattle ) 
				{
					dwClubID_P = pMyClub->m_dwAlliance;
					dwClubID_S = pTarClub->m_dwAlliance;
				}
				else
				{
					dwClubID_P = m_dwGuild;
					dwClubID_S = pCHAR->m_dwGuild;
				}

				GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE NetMsg;

				NetMsg.dwBattleClubID = dwClubID_S;
				NetMsg.bKillPoint = false;
				
				m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );

				NetMsg.dwBattleClubID = dwClubID_P;
				NetMsg.bKillPoint = true;
													
				m_pGLGaeaServer->SENDTOCLIENT ( pCHAR->m_dwClientID, &NetMsg );
			
			}
			//	CDM 상황
			else if ( bClubDeathMatch )
			{
				GLClubDeathMatch* pCDM = GLClubDeathMatchFieldMan::GetInstance().Find( m_pLandMan->m_dwClubMapID );	
				if ( pCDM )
				{
					pCDM->AddCDMScore( pCHAR->m_dwGuild, m_dwGuild );

					//	POINT UPDATE 나랑, 상대방
					GLMSG::SNET_CLUB_DEATHMATCH_POINT_UPDATE NetMsg;
					NetMsg.bKillPoint = false;					
					m_pGLGaeaServer->SENDTOCLUBCLIENT_ONMAP ( m_pLandMan->GetMapID().dwID, m_dwGuild, &NetMsg );

					NetMsg.bKillPoint = true;														
					m_pGLGaeaServer->SENDTOCLUBCLIENT_ONMAP ( pCHAR->m_pLandMan->GetMapID().dwID, pCHAR->m_dwGuild, &NetMsg );
	
				}

			}
			//	Note : 서로 적대 행위자일 경우.
			else if ( !IS_HOSTILE_ACTOR(pCHAR->m_dwCharID) && pCHAR->IS_HOSTILE_ACTOR(m_dwCharID) )
			{
				//	Note : 범죄자 등급인 경우 상대방에게 생활점수 줌.
				//

				if ( ISOFFENDER() )
				{
					DWORD dwMY_PK_LEVEL = GET_PK_LEVEL();
					if ( dwMY_PK_LEVEL != UINT_MAX && dwMY_PK_LEVEL>0 )
					{
						//	Note : 생활점수 변화.
						nLIFE = 7-(GLCONST_CHAR::EMPK_STATE_LEVEL-dwMY_PK_LEVEL);
						pCHAR->ReceiveLivingPoint ( nLIFE );
					}
				}
				//	자신의 등급이 일반학생일 경우, 상대방 속성, 생점 조절.
				else
				{
					bool bGuidBattleMap = m_pLandMan->m_bGuidBattleMap;
					bool bFreePKMap = m_pLandMan->IsFreePK();
					bool bSCHOOL_FREEPK = GLSchoolFreePK::GetInstance().IsON();
					bool bBRIGHTEVENT = m_pGLGaeaServer->IsBRIGHTEVENT(); // 성향 이벤트용				
					
					if ( GetSchool() == pCHAR->GetSchool() )	bSCHOOL_FREEPK = false;

					if ( !(bGuidBattleMap||bSCHOOL_FREEPK||bFreePKMap) && !bBRIGHTEVENT )
					{
						nBRIGHT = GLCONST_CHAR::nPK_KILL_BRIGHT_POINT;
						nLIFE = GLCONST_CHAR::nPK_KILL_LIVING_POINT;

						//	Note : 속성수치 변화.
						pCHAR->m_nBright += nBRIGHT;

						GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
						NetMsg.nBright = pCHAR->m_nBright;
						m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsg);

						GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
						NetMsgBrd.dwGaeaID = pCHAR->m_dwGaeaID;
						NetMsgBrd.nBright = pCHAR->m_nBright;
						pCHAR->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

						//	Note : 생활점수 변화.
						pCHAR->m_nLiving += nLIFE;

						GLMSG::SNETPC_UPDATE_LP NetMsgLp;
						NetMsgLp.nLP = pCHAR->m_nLiving;
						m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgLp);
					}
				}
			}

			//	Note : 죽였을 경우 로그 기록.
			GLITEMLMT::GetInstance().ReqAction
			(
				dwKILL_ID,					//	당사자.
				EMLOGACT_KILL,				//	행위.
				ID_CHAR, m_dwCharID,		//	상대방.
				0,							//	exp
				nBRIGHT,					//	bright
				nLIFE,						//	life
				0							//	money
			);
		}
	}

	//	Note : 적대 행위자 삭제.
	DelPlayHostile();

	//	Note : 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 )
	GenerateReward ();

	// 시간제 귀혼주 착용시 무조껀 경험치를 0으로 만든다.
	if ( m_nDECEXP > 0 )
	{
		bool bItemRebirth = ISREVIVE() ? true : false;
		if ( bItemRebirth )
		{
			// 소모성 귀혼주이면 자동 귀혼주 사용
			SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_NECK);
			if ( !pITEM->ISINSTANCE() ) m_nDECEXP = 0;
		}
	}


	m_cDamageLog.clear();

	////	Note : 죽을때 로그 기록.
	// 부활하기 버튼이 나오지 않아 로그를 못남기는 경우가 있어서 죽으면 무조건 남긴다.
	{
		EMIDTYPE emKILL = ID_MOB;
		if ( m_sAssault.emCrow==CROW_PC )	emKILL = ID_CHAR;

		GLITEMLMT::GetInstance().ReqAction
		(
			m_dwCharID,					//	당사자.
			EMLOGACT_DIE,				//	행위.
			emKILL, dwKILL_ID,			//	상대방.
			m_nDECEXP,					// exp
			0,							// bright
			0,							// life
			0							// money
		);
	}

	//	Note : 퀘스트 진행 목록중 죽을시 실패 옵션 검사후 처리.
	DoQuestCheckLimitDie ();

	//	쓰러짐 설정.
	SetSTATE(EM_ACT_DIE);

	//	Note : 쓰러짐을 클라이언트들에게 알림.
	GLMSG::SNET_ACTION_BRD NetMsgBrd;
	NetMsgBrd.emCrow	= CROW_PC;
	NetMsgBrd.dwID		= m_dwGaeaID;
	NetMsgBrd.emAction	= GLAT_FALLING;

	//	Note : 주변의 Char 에게.
	SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

	//	Note : 자신의 클라이언트에.
	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgBrd );

	//	행위 변화.
	TurnAction ( GLAT_FALLING );

	// 배틀로얄 이벤트일때 쓰러지면 투명모드로 전환한다.
	if ( GLCONST_CHAR::bBATTLEROYAL )
	{
		SetSTATE(EM_REQ_VISIBLEOFF);

		//	Note : (에이젼트서버) 메세지 발생.
		//
		GLMSG::SNETPC_ACTSTATE NetMsgFld;
		NetMsgFld.dwActState = m_dwActState;
		m_pGLGaeaServer->SENDTOAGENT ( m_dwClientID, &NetMsgFld );
	}

	// 팻 Sad 액션
	PGLPETFIELD pMyPet = m_pGLGaeaServer->GetPET ( m_dwPetGUID );
	if ( pMyPet && pMyPet->IsValid () )
	{
		pMyPet->ReSetAllSTATE ();
		pMyPet->SetSTATE ( EM_PETACT_SAD );
		
		// 클라이언트에 알림
		GLMSG::SNETPET_SAD PetNetMsg;
		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &PetNetMsg );

		GLMSG::SNETPET_SAD_BRD PetNetMsgBrd;
		PetNetMsgBrd.dwGUID   = pMyPet->m_dwGUID;
		PetNetMsgBrd.dwOwner  = m_dwGaeaID;

		SendMsgViewAround ( (NET_MSG_GENERIC*) &PetNetMsgBrd );
	}

	// 죽었을 경우 소환수를 해제
	PGLSUMMONFIELD pMySummon = m_pGLGaeaServer->GetSummon( m_dwSummonGUID );
	if ( pMySummon && pMySummon->IsValid() )
	{
		m_pGLGaeaServer->DropOutSummon ( m_dwSummonGUID, true );
	}
		
}

HRESULT GLChar::TourchQItem ( DWORD dwGlobID )
{
	PITEMDROP pItemDrop = m_pLandMan->GetItem ( dwGlobID );
	if ( !pItemDrop )	return S_FALSE;

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;
	GLItemMan & glItemMan = GLItemMan::GetInstance();
	
	const SITEM *pItemData = glItemMan.GetItem(pItemDrop->sItemCustom.sNativeID);
	if ( !pItemData )	return S_FALSE;

	if ( !pItemDrop->IsTakeItem(m_dwPartyID,m_dwGaeaID) )	return S_FALSE;

	if ( pItemData->sBasicOp.emItemType!=ITEM_QITEM )		return S_FALSE;

	const ITEM::SQUESTIONITEM &sQUESTIONITEM = pItemData->sQuestionItem;

	//	Note : ?아이템의 영향. 파티일 경우 영향이 파티원에게 미친다.
	//
	switch ( sQUESTIONITEM.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_CRAZY:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_MADNESS:
	case QUESTION_ATTACK_UP_M:
		{
			SNATIVEID sNativeID;

			// 서비스 프로바이더가 해외인 경우
			if ( m_pGLGaeaServer->m_nServiceProvider != SP_KOREA )
			{
                if ( sQUESTIONITEM.emType == QUESTION_SPEED_UP_M ||
				     sQUESTIONITEM.emType == QUESTION_MADNESS    ||
				     sQUESTIONITEM.emType == QUESTION_ATTACK_UP_M )
				{
                    sNativeID = pItemData->sBasicOp.sNativeID;
				}
				else
				{
                    sNativeID = glItemMan.GetRandomQItemID ( pItemDrop->sItemCustom.sNativeID );
				}
			}
			else
			{
				sNativeID = glItemMan.GetRandomQItemID ( pItemDrop->sItemCustom.sNativeID );
			}
				
			GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);

			// 새롭게 추가한 ?아이템일 경우 종전과 같은 방법으로 처리
			if ( sNativeID == NATIVEID_NULL() )
			{
				if ( pParty )
				{
					GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
					GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
					for ( ; pos!=end; ++pos )
					{
						PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
						if ( !pCHAR )	continue;
						if ( pCHAR->m_sMapID!=m_sMapID )	continue;

						D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
						float fDist = D3DXVec3Length ( &vDist );
						if ( MAX_VIEWRANGE < fDist )		continue;

						pCHAR->RecieveQItemFact ( pItemDrop->sItemCustom.sNativeID );
					}
				}
				else
				{
					RecieveQItemFact ( pItemDrop->sItemCustom.sNativeID );
				}	
			}
			else
			{
				const SITEM *pItem = glItemMan.GetItem( sNativeID );
				if ( !pItem ) return S_FALSE;

				if ( pParty )
				{
					GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
					GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
					for ( ; pos!=end; ++pos )
					{
						PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
						if ( !pCHAR )	continue;
						if ( pCHAR->m_sMapID!=m_sMapID )	continue;

						D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
						float fDist = D3DXVec3Length ( &vDist );
						if ( MAX_VIEWRANGE < fDist )		continue;

						pCHAR->RecieveQItemFact ( sNativeID );
					}
				}
				else
				{
					RecieveQItemFact ( sNativeID );
				}
			}
		}
		break;

	case QUESTION_EXP_GET:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
			if ( pParty )
			{
				GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
				GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
					if ( !pCHAR )						continue;
					if ( pCHAR->m_sMapID!=m_sMapID )	continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
					float fDist = D3DXVec3Length ( &vDist );
					if ( MAX_VIEWRANGE < fDist )		continue;

					int nRecExp = int ( pCHAR->m_sExperience.lnMax * sQUESTIONITEM.fExp/100.0f );
					pCHAR->ReceiveExp ( nRecExp, false );
					m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgBrd);
				}
			}
			else
			{
				int nRecExp = int ( m_sExperience.lnMax * sQUESTIONITEM.fExp/100.0f );
				ReceiveExp ( nRecExp, false );
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
			}
		}
		break;

	case QUESTION_BOMB:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
			if ( pParty )
			{
				GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
				GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
					if ( !pCHAR )	continue;
					if ( pCHAR->m_sMapID!=m_sMapID )	continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
					float fDist = D3DXVec3Length ( &vDist );
					if ( MAX_VIEWRANGE < fDist )		continue;

					pCHAR->ReceiveDamage ( CROW_ITEM, 0, sQUESTIONITEM.wParam1, FALSE );
					m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgBrd);
				}
			}
			else
			{
				ReceiveDamage ( CROW_ITEM, 0, sQUESTIONITEM.wParam1, FALSE );
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
			}
		}
		break;

	case QUESTION_MOBGEN:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
			if ( pParty )
			{
				GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
				GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
					if ( !pCHAR )	continue;
					if ( pCHAR->m_sMapID!=m_sMapID )	continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
					float fDist = D3DXVec3Length ( &vDist );
					if ( MAX_VIEWRANGE < fDist )		continue;

					m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgBrd);
				}
			}
			else
			{
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
			}

			m_pLandMan->DropCrow ( SNATIVEID(sQUESTIONITEM.wParam1,sQUESTIONITEM.wParam2), pItemDrop->vPos.x, pItemDrop->vPos.z );
		}
		break;

	case QUESTION_HEAL:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPARTY_FIELD* pParty = m_pGLGaeaServer->GetParty(m_dwPartyID);
			if ( pParty )
			{
				GLPARTY_FIELD::MEMBER_ITER pos = pParty->m_cMEMBER.begin();
				GLPARTY_FIELD::MEMBER_ITER end = pParty->m_cMEMBER.end();
				for ( ; pos!=end; ++pos )
				{
					PGLCHAR pCHAR = m_pGLGaeaServer->GetChar ( (*pos).first );
					if ( !pCHAR )	continue;
					if ( pCHAR->m_sMapID!=m_sMapID )	continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPos - m_vPos;
					float fDist = D3DXVec3Length ( &vDist );
					if ( MAX_VIEWRANGE < fDist )		continue;

					m_pGLGaeaServer->SENDTOCLIENT(pCHAR->m_dwClientID,&NetMsgBrd);

					pCHAR->VAR_BODY_POINT ( CROW_PC, m_dwGaeaID, FALSE, USHRT_MAX, USHRT_MAX, USHRT_MAX );
				}
			}
			else
			{
				VAR_BODY_POINT ( CROW_PC, m_dwGaeaID, FALSE, USHRT_MAX, USHRT_MAX, USHRT_MAX );
				m_pGLGaeaServer->SENDTOCLIENT(m_dwClientID,&NetMsgBrd);
			}
		}
		break;
	};

	//	Note : 아이탬 Land에서 삭제.
	m_pLandMan->DropOutItem ( pItemDrop->dwGlobID );

	return S_OK;
}

// 아이템의 경험치 배율 구하기...아이템에 붙은 경험치 배율을 모두 더한다.
float GLChar::GetExpMultipleRateFromSlotItem()
{
	float fExpMultipleRateMax = 1.0f;

	GLItemMan & glItemMan = GLItemMan::GetInstance();

	for( int i=0; i<SLOT_TSIZE; ++i )
	{
		const SITEMCUSTOM &sItemCustoms = m_PutOnItems[i];

		if( sItemCustoms.sNativeID == SNATIVEID( false ) )
			continue;

		SITEM* pItem = glItemMan.GetItem( sItemCustoms.sNativeID );
		if( !pItem )
			continue;

		float fExpMultipleRate = pItem->GetExpMultiple();
		if(  1.0f < fExpMultipleRate )
			fExpMultipleRateMax += fExpMultipleRate - 1.0f;
	}

	return fExpMultipleRateMax < 1.0f ? 1.0f : fExpMultipleRateMax;
}

void GLChar::SENDBOXITEMINFO ()
{
	GLMSG::SNET_GET_CHARGEDITEM_FROMDB_FB NetMsgFB;

	NetMsgFB.emFB = EMREQ_CHARGEDITEM_FROMDB_FB_OK;

	//	아이템 샾에서 구입한 물품 목록 클라이언트에 전송

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	MAPSHOP::reverse_iterator pos = m_mapCharged.rbegin();
	MAPSHOP::reverse_iterator end = m_mapCharged.rend();

	for ( ; pos!=end; ++pos )
	{
		SHOPPURCHASE &sPURCHASE = (*pos).second;

		StringCchCopy ( NetMsgFB.szPurKey, PURKEY_LENGTH+1, sPURCHASE.strPurKey.GetString() );
		NetMsgFB.nidITEM = SNATIVEID(sPURCHASE.wItemMain,sPURCHASE.wItemSub);
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );
	}

	NetMsgFB.emFB = EMREQ_CHARGEDITEM_FROMDB_FB_END;
	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );
	return;
}

HRESULT GLChar::MGOddEvenOK( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;

	// 소유 금액 체크
	if( pNetMsg->uiBattingMoney > m_lnMoney )
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_MONEY_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

		return S_FALSE;
	}

	// 최대 배팅 가능 금액 체크
	if( pNetMsg->uiBattingMoney > MINIGAME_ODDEVEN::uiMaxBattingMoney )
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_MAXBATTING;
		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

		return S_FALSE;
	}

	MGOddEvenInit();

	m_ui64BattingMoney = pNetMsg->uiBattingMoney;

	// 배팅 완료 게임 시작
	NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_OK;
	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MGOddEvenCANCEL( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;
	NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FAIL;
	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MGOddEvenSelect( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	m_nOddEven = pNetMsg->emCase;

	return S_OK;
}

HRESULT GLChar::MGOddEvenAGAIN( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;

	if( pNetMsg->uiBattingMoney > m_lnMoney ) // 소유 금액 체크
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FAIL;
	}
	else
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_AGAIN_OK;
		NetMsgFB.wRound = m_wMGOddEvenCurRnd+1;

		m_ui64BattingMoney = m_ui64DividendMoney;
	}

	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MGOddEvenShuffle( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	std::vector<int> vecShuffleBuffer;
	vecShuffleBuffer.reserve( 100 );

	{ // 활률에 따른 랜덤 배열 생성
		int nSuccessCnt = (int)(MINIGAME_ODDEVEN::fSuccessRate[m_wMGOddEvenCurRnd]*100.0); // 성공 확률
		int i = 0;

		for( ; i<nSuccessCnt; ++i )
			vecShuffleBuffer.push_back( m_nOddEven );

		for( ; i<100; ++i)
			vecShuffleBuffer.push_back( EMMGAME_ODDEVEN_ODD - m_nOddEven );

		std::random_shuffle( vecShuffleBuffer.begin(), vecShuffleBuffer.end() );
	}

	srand ( (UINT)time( NULL ) );

	int nDiceNumber = rand() % 100;						// 0~99까지 숫자를 랜덤하게 선택한다.
	int nHabbyNumber = vecShuffleBuffer[nDiceNumber];	// 배열에서 선택

	GLITEMLMT::GetInstance().ReqMoneyExc(	ID_CHAR, m_dwCharID, 
											ID_CHAR, 0, 
											m_ui64BattingMoney, EMITEM_ROUTE_ODDEVEN );

	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;

	if( nHabbyNumber == m_nOddEven ) // 맞다면 배당금 지급
	{
		// 배당금 계산
		m_ui64DividendMoney = (LONGLONG)(m_ui64BattingMoney * MINIGAME_ODDEVEN::fReturnRate[m_wMGOddEvenCurRnd]);

		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_WIN;
		NetMsgFB.ui64DividendMoney = m_ui64DividendMoney;
		NetMsgFB.ui64ActualMoney = m_ui64DividendMoney - m_ui64BattingMoney; // 실수령 금액 = 배당금 - 배팅금
		
		CheckMoneyUpdate( m_lnMoney, NetMsgFB.ui64ActualMoney, TRUE, "Odd Event Shuffle." );
		m_bMoneyUpdate = TRUE;

		m_lnMoney += NetMsgFB.ui64ActualMoney;		
		m_wMGOddEvenCurRnd++;

		GLITEMLMT::GetInstance().ReqMoneyExc(	ID_CHAR, m_dwCharID, 
												ID_CHAR, m_wMGOddEvenCurRnd, 
												NetMsgFB.ui64ActualMoney, EMITEM_ROUTE_ODDEVEN );
	}
	else
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_LOSE;
		NetMsgFB.ui64DividendMoney = m_ui64BattingMoney;

		CheckMoneyUpdate( m_lnMoney, NetMsgFB.ui64ActualMoney, FALSE, "Odd Event Shuffle." );
		m_bMoneyUpdate = TRUE;

		m_lnMoney -= m_ui64BattingMoney;
		

		GLITEMLMT::GetInstance().ReqMoneyExc(	ID_CHAR, m_dwCharID, 
												ID_MOB, m_wMGOddEvenCurRnd, 
												m_ui64BattingMoney, EMITEM_ROUTE_ODDEVEN );
	}

	GLITEMLMT::GetInstance().ReqMoneyExc(	ID_CHAR, m_dwCharID, 
											ID_CHAR, 0, 
											m_lnMoney, EMITEM_ROUTE_ODDEVEN );

	if( nHabbyNumber )	NetMsgFB.emCase = EMMGAME_ODDEVEN_ODD;
	else				NetMsgFB.emCase = EMMGAME_ODDEVEN_EVEN;

	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MGOddEvenFinish( GLMSG::SNETPC_MGAME_ODDEVEN* pNetMsg )
{
	GLMSG::SNETPC_MGAME_ODDEVEN_FB NetMsgFB;

	// 반복 회수를 체크
	if( m_wMGOddEvenCurRnd >= MINIGAME_ODDEVEN::MAX_ROUND )
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_MAXROUND;
	}
	else
	{
		NetMsgFB.emResult = EMMGAME_ODDEVEN_FB_FINISH_OK;
		NetMsgFB.wRound = m_wMGOddEvenCurRnd;
		NetMsgFB.ui64DividendMoney = m_ui64DividendMoney;
	}

	m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::UpdateVehicle( float fElapsedTime )
{
	if ( !m_bVehicle || !m_sVehicle.IsActiveValue() ) return S_OK;
	m_fVehicleTimer += fElapsedTime;
	// 팻의 포만도 갱신
	if ( m_fVehicleTimer > 3600.0f/GLPeriod::REALTIME_TO_VBR )
	{
		m_fVehicleTimer = 0;
		m_sVehicle.m_nFull -= GLCONST_VEHICLE::nFullDecrVehicle[m_sVehicle.m_emTYPE];
		if ( m_sVehicle.m_nFull < 0 ) m_sVehicle.m_nFull = 0;

		// 클라이언트 포만도 갱신
		GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY NetMsg;
		NetMsg.nFull = m_sVehicle.m_nFull;

		m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsg );

		if ( m_sVehicle.IsNotEnoughFull() )	ActiveVehicle( false, false );
	}
	
	return S_OK;
}

BOOL GLChar::CheckMoneyUpdate( const LONGLONG lnCurrentMoney, const LONGLONG lnUpdateMoney, const bool bPlus, const char* msg )
{
	if( lnUpdateMoney < 100000000 && lnUpdateMoney >= 0 )
	{
		return FALSE;
	}

	char szTempChar[256] = {0,};
	string str;
	sprintf_s( szTempChar, "[%s][%s] %s", m_szUID, m_szName, msg );
	str = szTempChar;
	
	if( lnUpdateMoney < 0 )
	{
		str += " Not Correct Update Money Value!!";
	}

	if( bPlus )	sprintf_s( szTempChar, " Current Money %I64d Update Money %I64d Plus!!", lnCurrentMoney, lnUpdateMoney );
	else		sprintf_s( szTempChar, " Current Money %I64d Update Money %I64d Minus!!", lnCurrentMoney, lnUpdateMoney );
	str += szTempChar;

	BILLIONUPDATE_WRITE( str.c_str() );

	return TRUE;
}

void GLChar::ClubBattleSetPoint( DWORD dwClub_P, DWORD dwClub_S, bool bAlliancBattle )
{

	GLClubMan& cClubMan = GLGaeaServer::GetInstance().GetClubMan();
	GLCLUB* pClub_P = cClubMan.GetClub( dwClub_P );
	GLCLUB* pClub_S = cClubMan.GetClub( dwClub_S );

	if ( !pClub_P || !pClub_S ) return;

	if ( bAlliancBattle ) 
	{
		GLCLUB* pClub_M = cClubMan.GetClub( pClub_P->m_dwAlliance );
		if ( !pClub_M ) return;

		CLUB_ALLIANCE_ITER pos = pClub_M->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pClub_M->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;

			GLCLUB *pCLUB = cClubMan.GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;
			
			GLCLUBBATTLE* pClubBattle = pCLUB->GetClubBattle( pClub_S->m_dwAlliance );
			if ( !pClubBattle ) continue;

			pClubBattle->m_wDeathPointTemp++;
			pClubBattle->m_bKillUpdate = true;
		}

		pClub_M = cClubMan.GetClub( pClub_S->m_dwAlliance );
		if ( !pClub_M ) return;

		pos = pClub_M->m_setAlliance.begin();
		end = pClub_M->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = *pos;

			GLCLUB *pCLUB = cClubMan.GetClub ( sALLIANCE.m_dwID );
			if ( !pCLUB ) continue;
			
			GLCLUBBATTLE* pClubBattle = pCLUB->GetClubBattle( pClub_P->m_dwAlliance );
			if ( !pClubBattle ) continue;

			pClubBattle->m_wKillPointTemp++;
			pClubBattle->m_bKillUpdate = true;
		}
	}
	else
	{
		GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClub_S );
		if ( !pClubBattle ) return;

		pClubBattle->m_wDeathPointTemp++;
		pClubBattle->m_bKillUpdate = true;
		
		pClubBattle = pClub_S->GetClubBattle( dwClub_P );
		if ( !pClubBattle ) return;
				
		pClubBattle->m_wKillPointTemp++;
		pClubBattle->m_bKillUpdate = true;
	}
	
	return;
}

ITEM_COOLTIME*	GLChar::GetCoolTime( DWORD dwCoolID, EMCOOL_TYPE emCoolType )
{
	if ( emCoolType == EMCOOL_ITEMID )
	{
		COOLTIME_MAP_ITER pos = m_mapCoolTimeID.find(dwCoolID);
		if ( pos==m_mapCoolTimeID.end() )		return NULL;
		
		return &(*pos).second;
	}
	else if ( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_ITER pos = m_mapCoolTimeType.find(dwCoolID);
		if ( pos==m_mapCoolTimeType.end() )		return NULL;
		
		return &(*pos).second;
	}
	else	
		return NULL;
}

bool GLChar::IsCoolTime( SNATIVEID sNativeID )
{
	__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
	__time64_t tCoolTime = GetMaxCoolTime( sNativeID );

	if ( tCurTime < tCoolTime ) return true;

	return false;
}

bool GLChar::CheckCoolTime( SNATIVEID sNativeID )
{
	if ( IsCoolTime ( sNativeID ) )
	{
		GLMSG::SNET_ITEM_COOLTIME_ERROR NetMsg;
		NetMsg.sNativeID = sNativeID;

		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);
		return true;

	}
	return false;
}

__time64_t	GLChar::GetMaxCoolTime ( SNATIVEID sNativeID )
{
	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( !pItem )			return 0;

	ITEM_COOLTIME* pCoolTimeType = GetCoolTime( (DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE );
	ITEM_COOLTIME* pCoolTimeID = GetCoolTime( pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID );

	__time64_t tCoolType = 0;
	__time64_t tCoolID = 0;

	if ( pCoolTimeType )	tCoolType = pCoolTimeType->tCoolTime;
	if ( pCoolTimeID )		tCoolID = pCoolTimeID->tCoolTime;

	return (tCoolType > tCoolID) ? tCoolType : tCoolID;
}


bool GLChar::SetCoolTime( SNATIVEID sNativeID , EMCOOL_TYPE emCoolType )
{
	//	아이템 정보 가져옴.
	SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
	if ( !pItem )			return false;

	if ( !pItem->sBasicOp.IsCoolTime() ) return false;

	// 쿨타임 셋팅
	__time64_t  tCurTime = CTime::GetCurrentTime().GetTime();
	__time64_t  tCoolTime = tCurTime + pItem->sBasicOp.dwCoolTime;

	ITEM_COOLTIME* pCoolTime = GetCoolTime( sNativeID.dwID, emCoolType );
	if ( pCoolTime && tCoolTime < pCoolTime->tCoolTime ) return false;

	ITEM_COOLTIME sCoolTime;
	sCoolTime.dwID = pItem->sBasicOp.sNativeID.dwID;
	sCoolTime.tUseTime = tCurTime;
	sCoolTime.tCoolTime = tCoolTime;	

	if ( emCoolType == EMCOOL_ITEMID )
	{
		sCoolTime.dwCoolID = pItem->sBasicOp.sNativeID.dwID;
		m_mapCoolTimeID[sCoolTime.dwCoolID] = sCoolTime;
	}
	else if( emCoolType == EMCOOL_ITEMTYPE )
	{
		sCoolTime.dwCoolID = (DWORD)pItem->sBasicOp.emItemType;
		m_mapCoolTimeType[sCoolTime.dwCoolID] = sCoolTime;
	}

	GLMSG::SNETPC_ITEM_COOLTIME_UPDATE NetMsg;			
	NetMsg.dwID = sCoolTime.dwID;
	NetMsg.dwCoolID = sCoolTime.dwCoolID;
	NetMsg.tCoolTime = sCoolTime.tCoolTime;
	NetMsg.tUseTime = sCoolTime.tUseTime;
	NetMsg.emCoolType = emCoolType;

	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);

	return true;
}

bool GLChar::UpdateNpcCommission( LONGLONG lnCommission )
{
	if ( lnCommission < 0 ) return false;
	
	CheckMoneyUpdate( m_lnStorageMoney, lnCommission, TRUE, "Add Npc Commission");
	m_lnStorageMoney += lnCommission;
	m_bStorageMoneyUpdate = TRUE;
			
	GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_USER, m_dwUserID, lnCommission, EMITEM_ROUTE_NPCCOME );

	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = m_lnStorageMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT( m_dwClientID,&NetMsgStorage);

	return true;
}
