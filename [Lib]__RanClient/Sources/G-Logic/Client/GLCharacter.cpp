#include "pch.h"
#include "shlobj.h"
#include "SUBPATH.h"

#include "../[Lib]__EngineUI/Sources/Cursor.h"
//#include "DxCursor.h"
#include "DxInputDevice.h"
#include "editmeshs.h"
#include "DxMethods.h"
#include "DxViewPort.h"
#include "DxEffectMan.h"
#include "DxEnvironment.h"
#include "DxShadowMap.h"
#include "EditMeshs.h"
#include "GLogicData.h"
#include "GLItemMan.h"
#include "DxEffcharData.h"
#include "DxEffProj.h"

#include "tlhelp32.h"
#include "winbase.h"

#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )

#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "stl_Func.h"
#include "DxEffGroupPlayer.h"
#include "GLStrikeM.h"
#include "GLCrowData.h"
#include "GLPartyClient.h"
#include "GLQUEST.h"
#include "GLQUESTMAN.h"
#include "GLFriendClient.h"
#include "GLFactEffect.h"

#include "RANPARAM.h"
#include "DxRenderStates.h"
#include "GLCharacter.h"
#include "DXInputString.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include "GLTaxiStation.h"
#include "GLItemMixMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace COMMENT
{
	std::string szEMREACTION[REACT_SIZE+1] =
	{
		"REACT_MOB",
		"REACT_NPC",
		"REACT_P2P",
		"REACT_PVP",

		"REACT_ITEM",
		"REACT_SKILL",

		"REACT_NULL",
	};
};

namespace
{
	class CFINDER
	{
	public:
		enum { BUFFER = 36, };
	protected:
		std::vector<STARGETID*>		m_vecTAR;

	public:
		CFINDER()
		{
			m_vecTAR.reserve(BUFFER);
		}

	public:
		void CLEAR ()
		{
			if ( !m_vecTAR.empty() )
			{
				m_vecTAR.erase ( m_vecTAR.begin(), m_vecTAR.end() );
			}
		}

	public:
		STARGETID& FindClosedCrow ( const DETECTMAP_RANGE &mapRange, const D3DXVECTOR3 vFromPt )
		{
			GASSERT(mapRange.first!=mapRange.second);

			CLEAR();
			std_afunc::CCompareTargetDist sCOMP(vFromPt);

			for ( DETECTMAP_ITER pos=mapRange.first; pos!=mapRange.second; ++pos )
			{
				m_vecTAR.push_back ( &(*pos) );
			}

			std::vector<STARGETID*>::iterator found = std::min_element ( m_vecTAR.begin(), m_vecTAR.end(), sCOMP );

			return *(*found);
		}

		static CFINDER& GetInstance()
		{
			static CFINDER Instance;
			return Instance;
		}
	};

	STARGETID* FindCrow ( const DETECTMAP_RANGE &mapRange, const STARGETID &_starid )
	{
		static STARGETID sTARID;

		GASSERT(mapRange.first!=mapRange.second);

		DETECTMAP_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; iter++ )
		{
			sTARID = (*iter);

			if ( sTARID.emCrow==_starid.emCrow && sTARID.dwID==_starid.dwID )	return &sTARID;
		}

		return NULL;
	}

	STARGETID* FindCrow ( const DETECTMAP_RANGE &mapRange, SCONFTING_CLT::CONFT_MEM &setconftMember )
	{
		static STARGETID sTARID;

		GASSERT(mapRange.first!=mapRange.second);

		DETECTMAP_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; iter++ )
		{
			sTARID = (*iter);

			if ( sTARID.emCrow==CROW_PC )
			{
				SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find ( sTARID.dwID );
				if ( iter!=setconftMember.end() )	return &sTARID;
			}
		}

		return NULL;
	}

	STARGETID* FindCrowCID ( const DETECTMAP_RANGE &mapRange, SCONFTING_CLT::CONFT_MEM &setconftMember )
	{
		static STARGETID sTARID;

		GASSERT(mapRange.first!=mapRange.second);

		DETECTMAP_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; iter++ )
		{
			sTARID = (*iter);

			if ( sTARID.emCrow==CROW_PC )
			{
				PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( sTARID.dwID );

				if ( pCHAR )
				{
					SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find ( pCHAR->GetCharData().dwCharID );
					if ( iter!=setconftMember.end() )	return &sTARID;
				}
			}
		}

		return NULL;
	}
};

const float GLCharacter::m_fELAPS_MOVE = 0.2f;

GLCharacter::GLCharacter () :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),

	m_vServerPos(0,0,0),

	m_bPASSIVITY_ACTION(FALSE),
	m_fPASSIVITY_ACTION_ELAPS(0.0f),
	m_fPASSIVITY_ACTION_TOTAL(0.0f),

	m_bPASSIVITY_ACTION_CHECK(FALSE),
	m_fPASSIVITY_ACTION_CHECK(0.0f),

	m_fKeyDownCheckTime(0.0f),
	m_bOneHourNotInputKey(FALSE),

	m_Action(GLAT_IDLE),
	m_dwActState(NULL),

	m_wTARNUM(0),
	m_vTARPOS(0,0,0),

	m_fLastMsgMoveSend(0.0f),

	m_fIdleTime(0.0f),
	m_fattTIMER(0.0f),
	m_nattSTEP(0),

	m_dwWAIT(0),

	m_sRunSkill(NATIVEID_NULL()),
	m_sActiveSkill(NATIVEID_NULL()),
	m_dwANISUBCOUNT(0),
	m_dwANISUBSELECT(0),
	m_dwANISUBGESTURE(0),

	m_emOldQuestionType(QUESTION_NONE),

	m_dwGaeaID(0),

	m_wInvenPosX1(0),
	m_wInvenPosY1(0),
	m_wInvenPosX2(0),
	m_wInvenPosY2(0),

	m_bCLUB_CERTIFY(FALSE),
	m_fCLUB_CERTIFY_TIMER(0),

	m_wPMPosX(USHRT_MAX),
	m_wPMPosY(USHRT_MAX),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),
	m_dwNPCID(0),

	m_dwSummonGUID(GAEAID_NULL),

	m_bEnableHairSytle(FALSE),
	m_bEnableHairColor(FALSE),
	m_bEnableFaceStyle(FALSE),

	m_bAttackable(TRUE),
	m_llPetCardGenNum(0),
	m_sPetCardNativeID(NATIVEID_NULL()),
	m_cPetCardGenType(-1),
	m_bRecievedPetCardInfo(FALSE),
	m_bIsPetActive(FALSE),
	m_bMiniGameOpen(FALSE),
//	m_bWeaponSwitching(FALSE),	
	m_hCheckStrDLL( NULL ),
	m_pCheckString( NULL ),
	m_bReqVehicle ( FALSE ),
	m_bIsVehicleActive ( FALSE ),
	m_bRecivedVehicleItemInfo ( FALSE ),
	m_bGarbageOpen ( false ),
	m_bItemShopOpen ( false ),
	m_bReqAttendList( false ),
	m_dwComboAttend(0),
	m_tAttendLogin(0),
	m_dwAttendTime(0),

	m_bRecordChat(FALSE),
	m_strRecordChat("")
{
	D3DXMatrixIdentity ( &m_matTrans );
	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )		m_dwNumStorageItem[i] = UINT_MAX;

	for( int i = 0; i < 8; i++ )
	{
		m_bOldVisibleTracingUI[i] = FALSE;
		m_bNewVisibleTracingUI[i] = FALSE;
	}

	m_fPrintProcessInfoMsgGap = -10.0f;

	m_vecTAR_TEMP.reserve(220);

	SetSTATE(EM_ACT_RUN);

	m_mapPETCardInfo.clear();
	m_mapPETCardInfoTemp.clear();

	m_mapPETReviveInfo.clear();

	m_mapVEHICLEItemInfo.clear();
	m_mapVEHICLEItemInfoTemp.clear();

	m_vecAttend.clear();

	m_vecMarketClick.clear();

#ifdef TH_PARAM
	HMODULE m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

	if ( m_hCheckStrDLL )
	{
		m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
	}
#endif 

}

GLCharacter::~GLCharacter ()
{

	SAFE_DELETE(m_pSkinChar);

#ifdef TH_PARAM
	if( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif
}

float GLCharacter::GetBuyRate ()
{
	float fBUY_RATE = GET_PK_SHOP2BUY();

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLand )
	{
		fBUY_RATE += pLand->m_fCommission;
	}

	return fBUY_RATE;
}

float GLCharacter::GetSaleRate ()
{
	float fSALE_RATE = GET_PK_SHOP2SALE ();
	
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLand )
	{
		fSALE_RATE -= pLand->m_fCommission;
	}

	return fSALE_RATE;
}

void GLCharacter::ResetData ()
{
	int i = 0;

	GLCHARLOGIC::RESET_DATA();

	m_vDir = D3DXVECTOR3(0,0,-1);
	m_vDirOrig = D3DXVECTOR3(0,0,-1);
	m_vPos = D3DXVECTOR3(0,0,0);
	D3DXMatrixIdentity ( &m_matTrans );

	m_Action = GLAT_IDLE;
	m_dwActState = NULL;
	SetSTATE(EM_ACT_RUN);

	m_sTargetID = STARGETID(CROW_MOB,EMTARGET_NULL,D3DXVECTOR3( 0, 0, 0 ));

	m_wTARNUM = 0;
	m_vTARPOS = D3DXVECTOR3(0,0,0);
	for ( i=0; i<EMTARGET_NET; ++i )
		m_sTARIDS[i] = STARID(CROW_MOB,EMTARGET_NULL);

	m_sRunSkill = SNATIVEID(false);
	m_sActiveSkill = SNATIVEID(false);
	m_dwANISUBCOUNT = 0;
	m_dwANISUBSELECT = 0;
	m_dwANISUBGESTURE = 0;

	m_fLastMsgMoveSend = 0.0f;
	m_sLastMsgMove = GLMSG::SNETPC_GOTO();
	m_sLastMsgMoveSend = GLMSG::SNETPC_GOTO();

	m_sREACTION.RESET();

	m_fIdleTime = 0.0f;
	m_fattTIMER = 0.0f;
	m_nattSTEP = 0;
	m_dwWAIT = 0;

	m_bRecordChat = FALSE;

	m_strRecordChat = "";

	m_dwGaeaID = 0;

	for ( i=0; i<EMSTORAGE_CHANNEL; ++i )		m_dwNumStorageItem[i] = UINT_MAX;
	m_sCONFTING.RESET();

	m_sPMarket.DoMarketClose();

	m_bCLUB_CERTIFY = FALSE;
	m_fCLUB_CERTIFY_TIMER = 0;

	m_sCLUB.RESET();

	m_vMaxOrg = D3DXVECTOR3(6,20,6);
	m_vMinOrg = D3DXVECTOR3(-6,0,-6);

	m_vMax = D3DXVECTOR3(6,20,6);
	m_vMin = D3DXVECTOR3(-6,0,-6);

	m_dwNPCID = 0;

	m_bAttackable = TRUE;

	m_mapPETCardInfo.clear();
	m_mapPETCardInfoTemp.clear();
	m_bRecievedPetCardInfo = FALSE;
	m_bIsPetActive		   = FALSE;

	m_mapPETReviveInfo.clear();

	m_mapVEHICLEItemInfo.clear();
	m_mapVEHICLEItemInfoTemp.clear();

	m_bReqVehicle			  = FALSE;
	m_bIsVehicleActive		  = FALSE;
	m_bRecivedVehicleItemInfo = FALSE;

	for( i = 0; i < 8; i++ )
	{
		m_bOldVisibleTracingUI[i] = FALSE;
		m_bNewVisibleTracingUI[i] = FALSE;
	}

	m_fPrintProcessInfoMsgGap = -10.0f;

	m_sVehicle.RESET();

	m_bGarbageOpen = false;
	m_bItemShopOpen = false;
	m_bReqAttendList = false;
	m_vecAttend.clear();
	m_dwComboAttend = 0;
	m_tAttendLogin = 0;
	m_dwAttendTime = 0;

	m_dwTransfromSkill = SNATIVEID::ID_NULL;

	m_vecMarketClick.clear();
}

HRESULT GLCharacter::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[emIndex], pd3dDevice, TRUE );
	if ( !pSkinChar )	return E_FAIL;


	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, TRUE );

	
	UpdateSuit( TRUE );





	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 에니메이션 초기화.
	//
	if ( m_pSkinChar )
	{
		m_pSkinChar->SELECTANI ( AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );

		if ( !m_pSkinChar->GETCURANIMNODE() )
		{
			CDebugSet::ToLogFile ( "ERORR : current animation node null point error [ %s ] [ M %d S %d ]", m_pSkinChar->m_szFileName, AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );
			m_pSkinChar->DEFAULTANI ();
		}
	}

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS, m_matTrans, m_vDir );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &m_vDir );
	}

	return S_OK;
}

HRESULT GLCharacter::SkinLoadForTool( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFile )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( szFile, pd3dDevice, TRUE );
	if ( !pSkinChar )	return E_FAIL;


	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, TRUE );


//	UpdateSuit( TRUE );





	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 에니메이션 초기화.
	//
	if ( m_pSkinChar )
	{
		m_pSkinChar->SELECTANI ( AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );

		if ( !m_pSkinChar->GETCURANIMNODE() )
		{
			CDebugSet::ToLogFile ( "ERORR : current animation node null point error [ %s ] [ M %d S %d ]", m_pSkinChar->m_szFileName, AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );
			m_pSkinChar->DEFAULTANI ();
		}
	}

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS, m_matTrans, m_vDir );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &m_vDir );
	}

	return S_OK;
}


HRESULT GLCharacter::Create ( NavigationMesh* pNavi, SCHARDATA2 *pCharData2, D3DXVECTOR3* pvPos, DWORD dwGaeaID, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;
	m_dwGaeaID = dwGaeaID;

	m_dwTransfromSkill = SNATIVEID::ID_NULL;

	//	Note : 케릭터의 외형을 설정.
	if ( pCharData2 )										SCHARDATA2::Assign ( *pCharData2 );
	else if ( GLCONST_CHAR::VAID_CHAR_DATA2(0,GLCI_ARMS_M) )	SCHARDATA2::Assign ( GLCONST_CHAR::GET_CHAR_DATA2(0,GLCI_ARMS_M) );

    m_cInventory.SetAddLine ( GetOnINVENLINE(), true );
	CInnerInterface::GetInstance().SetInventorySlotViewSize ( EM_INVEN_DEF_SIZE_Y + GetOnINVENLINE() );

	// 베트남 인벤토리는 세로가 10칸이므로 6칸을 더 추가한다.
#if defined(VN_PARAM) //vietnamtest%%%
	m_cVietnamInventory.SetAddLine ( 6, true );
#endif	

	GLCHARLOGIC::INIT_DATA ( FALSE, TRUE );
	m_sCONFTING.RESET();

	//	초기 런 스킬 지정.
	if ( m_wSKILLQUICK_ACT < EMSKILLQUICK_SIZE )
	{
		m_sRunSkill = m_sSKILLQUICK[m_wSKILLQUICK_ACT];
	}

	hr = SkinLoad ( pd3dDevice );
	if ( FAILED(hr) )	return hr;


	//	Note : 이동 제어 초기화.
	if ( pvPos )	m_vPos = *pvPos;
	else			m_vPos = m_vStartPos;
	
	//	Note : 형상 위치 초기화.
	D3DXMatrixTranslation ( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z );

	m_actorMove.SetMaxSpeed ( GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO );
	if ( pNavi )
	{
		m_actorMove.Create ( pNavi, m_vPos, -1 );
	}

	DoActWait ();

	GLQuestPlay::MAPQUEST& mapQuest = m_cQuestPlay.GetQuestProc ();
	{
		bool bKNOCK(false);

		GLQuestPlay::MAPQUEST_ITER iter = mapQuest.begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = mapQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG *pPROG = (*iter).second;
			if ( !pPROG->IsReqREADING () )	continue;

			bKNOCK = true;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( pQUEST )
				CInnerInterface::GetInstance().PrintConsoleText ( ID2GAMEINTEXT("QUEST_KNOCK"), pQUEST->GetTITLE() );
			else
				CInnerInterface::GetInstance().PrintConsoleText ( ID2GAMEINTEXT("QUEST_NOINFO"), pPROG->m_sNID.dwID );
		}

		if ( bKNOCK )
		{
			CInnerInterface::GetInstance().PrintConsoleText ( ID2GAMEINTEXT("QUEST_KNOCK_END") );
		}
	}

	GLFriendClient::GetInstance().Clear();


	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ NetMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;
	NETSEND ( &NetMsg );

#if defined(VN_PARAM) //vietnamtest%%%
	// 이동 할 때마다 베트남 탐닉 시스템 시간을 요청한다.
	GLMSG::SNETPC_VIETNAM_TIME_REQ NetVietnamMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;	
	NETSENDTOFIELD ( &NetVietnamMsg );
#endif


	PLANDMANCLIENT pLandMClient = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLandMClient )								return E_FAIL;

	SNATIVEID mapID = pLandMClient->GetMapID();
	if( pLandMClient->IsInstantMap() ) mapID.wSubID = 0;

	SMAPNODE *pMapNode = GLGaeaClient::GetInstance().FindMapNode ( mapID );
	if ( pMapNode )
	{
		GLLevelFile cLevelFile;
		if( cLevelFile.LoadFile ( pMapNode->strFile.c_str(), TRUE, NULL ) )
		{
			SLEVEL_ETC_FUNC* pEtcFunc = cLevelFile.GetLevelEtcFunc();
			if( !pEtcFunc )	return E_FAIL;
			if( pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] )
			{
				D3DXVECTOR3 vFromPt, vLookAtPt, vUpPt;
				DxViewPort::GetInstance().GetViewTrans( vFromPt, vLookAtPt, vUpPt );
				vFromPt = pEtcFunc->m_sControlCam.vCamPos + vLookAtPt;

				DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookAtPt, vUpPt );
				DxViewPort::GetInstance().SetCameraControl( TRUE, pEtcFunc->m_sControlCam.vCamPos, pEtcFunc->m_sControlCam.fUp, 
																	pEtcFunc->m_sControlCam.fDown, pEtcFunc->m_sControlCam.fLeft, 
																	pEtcFunc->m_sControlCam.fRight, pEtcFunc->m_sControlCam.fFar, 
																	pEtcFunc->m_sControlCam.fNear );
			}else{
				DxViewPort::GetInstance().SetCameraControl( FALSE );
			}
		}

	}

	return S_OK;
}

BOOL GLCharacter::IsVALID_STORAGE ( DWORD dwChannel )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )	return FALSE;
	return m_bStorage[dwChannel];
}

const SITEMCUSTOM& GLCharacter::GET_PRETRADE_ITEM ()
{
	static SITEMCUSTOM sItemCustom;
	sItemCustom.sNativeID = NATIVEID_NULL();

	SINVEN_POS sPreTradeItem = GLTradeClient::GetInstance().GetPreItem();

	if ( !sPreTradeItem.VALID() )		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem ( sPreTradeItem.wPosX, sPreTradeItem.wPosY );
	if ( !pResistItem )					return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

void GLCharacter::ReSelectAnimation ()
{
	// 제스처중이면 애니메이션 리셋 안함.
	if ( !IsACTION ( GLAT_TALK ) && !IsACTION( GLAT_GATHERING ) )
	{
		//	Note : 에니메이션 초기화.
		//
		if ( m_bVehicle )
		{
			int emType = m_sVehicle.m_emTYPE;
			m_emANISUBTYPE = (EMANI_SUBTYPE) ( AN_SUB_HOVERBOARD + emType );
		}

		if ( m_pSkinChar->GETCURSTYPE() !=GLCHARLOGIC::m_emANISUBTYPE )
			m_pSkinChar->SELECTANI ( m_pSkinChar->GETCURMTYPE(), GLCHARLOGIC::m_emANISUBTYPE );
	}
}

HRESULT GLCharacter::DoActWait ()
{
	m_dwWAIT = 0;
	SetSTATE(EM_ACT_WAITING);

	return S_OK;
}

HRESULT GLCharacter::ReBirth ( WORD wHP, WORD wMP, WORD wSP, D3DXVECTOR3 vPos, bool bWait )
{
	FACTEFF::DeleteEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

	//	부활시 채력치 회복값.
	INIT_RECOVER ();

	m_sHP.wNow = wHP;
	m_sMP.wNow = wMP;
	m_sSP.wNow = wSP;

	//	이동된 멥에 적응.
	MoveActiveMap ( GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh(), vPos, bWait );

	TurnAction ( GLAT_IDLE );

	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, m_vPos.x, m_vPos.y, m_vPos.z );

	STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup
	(
		GLCONST_CHAR::strREBIRTH_EFFECT.c_str(),
		matEffect,
		&sTargetID
	);

	ReSetSTATE(EM_ACT_DIE);

	return S_OK;
}

HRESULT GLCharacter::MoveActiveMap ( NavigationMesh* pNavi, const D3DXVECTOR3 &vPos, bool bWait )
{
	if ( bWait ) DoActWait ();

	//	Note : 이동 제어 초기화.
	m_vPos = vPos;

	m_actorMove.Stop ();
	if ( pNavi )
	{
		m_actorMove.Create ( pNavi, m_vPos, -1 );
		SetPosition ( m_vPos );
	}

	ReSetSTATE(EM_REQ_GATEOUT);

	// 이동 할 때마다 이벤트 시간을 요청한다.
	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ NetMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;
	NETSEND ( &NetMsg );

#if defined(VN_PARAM) //vietnamtest%%%
	// 이동 할 때마다 베트남 탐닉 시스템 시간을 요청한다.
	GLMSG::SNETPC_VIETNAM_TIME_REQ NetVietnamMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;	
	NETSENDTOFIELD ( &NetVietnamMsg );
#endif

	PLANDMANCLIENT pLandMClient = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLandMClient )								return E_FAIL;

	SNATIVEID mapID = pLandMClient->GetMapID();
	if( pLandMClient->IsInstantMap() ) mapID.wSubID = 0;

	SMAPNODE *pMapNode = GLGaeaClient::GetInstance().FindMapNode ( mapID );
	if ( pMapNode )
	{
		GLLevelFile cLevelFile;
		if( cLevelFile.LoadFile ( pMapNode->strFile.c_str(), TRUE, NULL ) )

		{
			SLEVEL_ETC_FUNC* pEtcFunc = cLevelFile.GetLevelEtcFunc();
			if( !pEtcFunc )	return E_FAIL;
			if( pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] )
			{
				D3DXVECTOR3 vFromPt, vLookAtPt, vUpPt;
				DxViewPort::GetInstance().GetViewTrans( vFromPt, vLookAtPt, vUpPt );
				vFromPt = pEtcFunc->m_sControlCam.vCamPos + vLookAtPt;

				DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookAtPt, vUpPt );
				DxViewPort::GetInstance().SetCameraControl( TRUE, pEtcFunc->m_sControlCam.vCamPos, pEtcFunc->m_sControlCam.fUp, 
																	pEtcFunc->m_sControlCam.fDown, pEtcFunc->m_sControlCam.fLeft, 
																	pEtcFunc->m_sControlCam.fRight, pEtcFunc->m_sControlCam.fFar, 
																	pEtcFunc->m_sControlCam.fNear );
			}else{
				DxViewPort::GetInstance().SetCameraControl( FALSE );
			}
		}

	}


	return S_OK;
}

// *****************************************************
// Desc: 장착한 아이템 업데이트
// *****************************************************
HRESULT GLCharacter::UpdateSuit( BOOL bChangeHair, BOOL bChangeFace )
{
	GASSERT(m_pd3dDevice);

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return E_FAIL;



	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	

	if ( bChangeFace )
	{
		// 얼굴
		if ( sCONST.dwHEADNUM > m_wFace )
		{
			std::string strHEAD_CPS = sCONST.strHEAD_CPS[m_wFace];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

			if ( pCharPart && strcmp(strHEAD_CPS.c_str(),pCharPart->m_szFileName) )
			{
				m_pSkinChar->SetPiece ( strHEAD_CPS.c_str(), m_pd3dDevice, 0x0, 0, TRUE );
			}
		}
	}
	// 헤어스타일
	if( bChangeHair )
	{
		if ( sCONST.dwHAIRNUM > m_wHair )
		{
			std::string strHAIR_CPS = sCONST.strHAIR_CPS[m_wHair];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

			if ( pCharPart && strcmp(strHAIR_CPS.c_str(),pCharPart->m_szFileName) )
			{
				m_pSkinChar->SetPiece ( strHAIR_CPS.c_str(), m_pd3dDevice, 0x0, 0, TRUE );
			}
		}
	}

	m_pSkinChar->SetHairColor( m_wHairColor );

	// 장착한 아이템 업데이트
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) ) continue;

		const SITEMCUSTOM &ItemCustom = m_PutOnItems[i];

		// 복장 아이템
		SNATIVEID nidITEM = ItemCustom.nidDISGUISE;
		if ( nidITEM==SNATIVEID(false) )	nidITEM = ItemCustom.sNativeID;
		
		if ( !m_bVehicle && i == SLOT_VEHICLE  ) nidITEM = SNATIVEID(false);
		else if (  m_bVehicle && i == SLOT_VEHICLE )
		{
			nidITEM = m_sVehicle.GetSkinID();
			if ( nidITEM == SNATIVEID(false) )
			{
				m_sVehicle.m_sVehicleID = ItemCustom.sNativeID;
				nidITEM = ItemCustom.sNativeID;
			}
		}

		// 유효하지 않으면
		if ( nidITEM == SNATIVEID(false) )
		{
			//	Note : 기본 스킨과 지금 장착된 스킨이 틀릴 경우.
			//	SLOT->PIECE.
			PDXSKINPIECE pSkinPiece = NULL;	//	기본 스킨.
			PDXCHARPART pCharPart = NULL;	//	현재 장착 스킨.

			EMPIECECHAR emPiece = SLOT_2_PIECE(EMSLOT(i));
			if ( emPiece!=PIECE_SIZE )
			{
				pSkinPiece = pSkinChar->GetPiece(emPiece);	//	기본 스킨.
				pCharPart = m_pSkinChar->GetPiece(emPiece);	//	현재 장착 스킨.
			}

			if ( pSkinPiece )
			{
				if ( pCharPart && strcmp(pSkinPiece->m_szFileName,pCharPart->m_szFileName) )
					m_pSkinChar->SetPiece ( pSkinPiece->m_szFileName, m_pd3dDevice, 0X0, 0, TRUE );
			}
			else
			{
				if ( pCharPart )	m_pSkinChar->ResetPiece(emPiece);
			}
		}
		// 유효하면
		else
		{
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( nidITEM );
			if ( pItem ) 
			{
				m_pSkinChar->SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), TRUE );
			}
		}
	}

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS, m_matTrans, m_vDir );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &m_vDir );
	}

	if( m_dwTransfromSkill != SNATIVEID::ID_NULL )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID.wSubID );
		if( pSkill ) 
		{
			for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
			{
				if( m_wSex == 1 )
				{				
					m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, TRUE );
				}else{
					m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, TRUE );
				}
			}
		}
	}

	return S_OK;
}

WORD GLCharacter::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

HRESULT GLCharacter::SetPosition ( D3DXVECTOR3 vPos )
{
	m_vPos = vPos;
	m_actorMove.SetPosition ( m_vPos, -1 );
	if ( m_actorMove.PathIsActive() )		m_actorMove.Stop();

	return S_OK;
}

EMCONFT_TYPE GLCharacter::GetConftType ( DWORD dwID )
{
	PGLCHARCLIENT pChar = GLGaeaClient::GetInstance().GetChar ( dwID );
	if ( !pChar )		return EMCONFT_NONE;

	if ( IsClubMaster() && pChar->IsClubMaster() )			return EMCONFT_GUILD;
	else if ( IsPartyMaster() && pChar->IsPartyMaster() )	return EMCONFT_PARTY;
	else													return EMCONFT_ONE;

	return EMCONFT_NONE;
}

void GLCharacter::DoPASSIVITY ( BOOL bPASSIVITY )
{
	if ( m_bPASSIVITY_ACTION_CHECK )	return;

	m_bPASSIVITY_ACTION = bPASSIVITY;
	m_fPASSIVITY_ACTION_ELAPS = 0.0f;

	if ( !bPASSIVITY )
	{
		ResetPASSIVITY ();
	}
}

enum
{
	EMPASSIVITY_ACTION_TOTAL		= 3600,
	EMPASSIVITY_ACTION_ELAPS		= 60,
	EMPASSIVITY_ACTION_CHECK		= 30,
	EMPASSIVITY_ACTION_CHECK_RESET	= 360,
};

bool GLCharacter::CheckPASSIVITY ( float fElaps )
{
	if ( m_bPASSIVITY_ACTION_CHECK )
	{
		m_fPASSIVITY_ACTION_CHECK += fElaps;
		return true;
	}

	m_fPASSIVITY_ACTION_ELAPS += fElaps;
	m_fPASSIVITY_ACTION_TOTAL += fElaps;

	if ( m_fPASSIVITY_ACTION_ELAPS > EMPASSIVITY_ACTION_CHECK_RESET )
	{
		ResetPASSIVITY ();
	}

//	CDebugSet::ToView ( 4, 1, "action total : %5.1f", m_fPASSIVITY_ACTION_TOTAL );
//	CDebugSet::ToView ( 4, 2, "action elaps : %5.1f", m_fPASSIVITY_ACTION_ELAPS );

	if ( m_fPASSIVITY_ACTION_TOTAL > EMPASSIVITY_ACTION_TOTAL &&
		m_fPASSIVITY_ACTION_ELAPS < EMPASSIVITY_ACTION_ELAPS )
	{
		m_bPASSIVITY_ACTION_CHECK = TRUE;
		CDebugSet::ToListView ( "[___CheckPASSIVITY___]" );
		return true;
	}

	return false;
}

bool GLCharacter::IsBlockPASSIVITY ()
{
	return ( m_bPASSIVITY_ACTION_CHECK && m_fPASSIVITY_ACTION_CHECK>EMPASSIVITY_ACTION_CHECK );
}

void GLCharacter::ResetPASSIVITY ()
{
	m_bPASSIVITY_ACTION = FALSE;
	m_fPASSIVITY_ACTION_ELAPS = 0.0f;
	m_fPASSIVITY_ACTION_TOTAL = 0.0f;
	m_bPASSIVITY_ACTION_CHECK = FALSE;
	m_fPASSIVITY_ACTION_CHECK = 0.0f;
}

BOOL GLCharacter::ActionMoveTo ( float fTime, D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt, BOOL bContinue, BOOL bREACT )
{
	// 상점이 개설된 상태에서 이동할 수 없다.
	if ( m_sPMarket.IsOpen() ) return FALSE;

	if ( m_bSTATE_STUN )	   return FALSE;

	BOOL bColl=FALSE;
	DWORD dwCollID;
	D3DXVECTOR3 vCollPos;
	if ( m_actorMove.GetParentMesh() )
	{
		m_actorMove.GetParentMesh()->IsCollision ( vFromPt, vTargetPt, vCollPos, dwCollID, bColl );
	}
	if ( !bColl )	return FALSE;
	
	//	이동하고자 하는 영역이 몹이 있는 영역과 겹칠 경우 이동 불가.
	bColl = GLGaeaClient::GetInstance().IsCollisionMobToPoint ( vCollPos, GETBODYRADIUS() );
	if ( bColl )	return FALSE;

	BOOL bGoto = m_actorMove.GotoLocation
	(
		vCollPos+D3DXVECTOR3(0,+10,0),
		vCollPos+D3DXVECTOR3(0,-10,0)
	);

	if ( bGoto )
	{
		TurnAction ( GLAT_MOVE );
		m_sTargetID.vPos = vCollPos;

		//	Note : 이동 메시지 정보.
		GLMSG::SNETPC_GOTO NetMsg;
		NetMsg.dwActState = m_dwActState;
		NetMsg.vCurPos = m_vPos;
		NetMsg.vTarPos = m_sTargetID.vPos;

		m_sLastMsgMove = NetMsg;

		//	Note : 메세지 발생. ( 지속적으로 발생하는 이동 콘트롤중 일부만 메시지 발생. )
		//
		if ( bContinue && (m_fLastMsgMoveSend+m_fELAPS_MOVE) > fTime )		return TRUE;

		DoPASSIVITY ( bREACT );

		if ( m_actorMove.PathIsActive() )
		{
			m_fLastMsgMoveSend = fTime;
			m_sLastMsgMoveSend = NetMsg;
			NETSENDTOFIELD ( &NetMsg );
		}
	}

	return TRUE;
}

BOOL GLCharacter::LargeMapMoveTo( D3DXVECTOR3* pPos )
{
	// 미니맵을 클릭해서 이동하는 경우 ( 준혁 )

	D3DXVECTOR3 vFrom = *pPos;
	vFrom.y += 1000.0f;

	D3DXVECTOR3 vTarget = *pPos;
	vTarget.y -= 1000.0f;

	return ActionMoveTo( 0.0f, vFrom, vTarget, FALSE, TRUE );
}

void GLCharacter::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : 앳 액션 리셋.
	//
	switch ( m_Action )
	{
	case GLAT_ATTACK:
		if ( toAction<=GLAT_TALK )
		{
			//	Note : 공격 종료 메세지 발생.
			GLMSG::SNETPC_ATTACK_CANCEL NetMsg;
			NETSENDTOFIELD ( &NetMsg );
		}
		break;

	case GLAT_SKILL:
		if ( toAction == GLAT_MOVE || toAction == GLAT_ATTACK || toAction == GLAT_SHOCK )
		{
			GLMSG::SNETPC_SKILL_CANCEL NetMsg;
			NETSENDTOFIELD ( &NetMsg );
		}

		m_idACTIVESKILL = NATIVEID_NULL();
		SetDefenseSkill( false );
		m_wTARNUM = 0;
		break;

	case GLAT_FALLING:
		if ( toAction==GLAT_FALLING )	return;
		break;
		
	case GLAT_DIE:
		if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )	return;
		break;

	case GLAT_CONFT_END:
		ReSetSTATE(EM_ACT_CONFT_WIN);
		break;
	case GLAT_GATHERING:
		ReqCancelGathering();
		break;
	};

	//	Note : 액션 초기화.
	//

	m_Action = toAction;



	//	Note : 액션 바뀜.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;
		}
		break;

	case GLAT_ATTACK:
		{
			StartAttackProc ();
		}
		break;

	case GLAT_SKILL:
		{
			m_pSkinChar->m_bResetSkillAni = TRUE;
			StartSkillProc ();
		}
		break;

	case GLAT_TALK:
		{
		}
		break;

	case GLAT_FALLING:
		m_sHP.wNow = 0;
	
		FACTEFF::DeleteEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );
		break;

	case GLAT_DIE:
		//	채력을 0로 초기화.
		//	( 클라이언트에서 회복량 보간으로 서버에서 채력이 0 일때 0가 아닐수 있음 )
		m_sHP.wNow = 0;
		
		FACTEFF::DeleteEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );
		break;

	case GLAT_CONFT_END:
		break;

	case GLAT_GATHERING:		
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};

	if ( m_actorMove.PathIsActive() )
	{
		if ( !IsACTION(GLAT_MOVE) && !IsACTION(GLAT_PUSHPULL) )		m_actorMove.Stop();
	}
}

BOOL GLCharacter::IsCtrlBlockBody ()
{
	if ( IsACTION(GLAT_SHOCK) )		return TRUE;
	if ( IsACTION(GLAT_PUSHPULL) )	return TRUE;
	if ( IsACTION(GLAT_CONFT_END) )	return TRUE;
	if ( IsACTION(GLAT_GATHERING) && ( m_dwANISUBGESTURE == 0 ||  m_dwANISUBGESTURE == 1 ) ) 
		return TRUE;

	return FALSE;
}

BOOL GLCharacter::IsValidBody ()
{
	if ( IsSTATE(EM_ACT_WAITING) )		return FALSE;
	if ( IsSTATE(EM_REQ_GATEOUT) )		return FALSE;
	if ( IsSTATE(EM_ACT_DIE) )			return FALSE;

	return ( m_Action < GLAT_FALLING );
}

BOOL GLCharacter::IsDie ()
{
	if ( !IsSTATE(EM_ACT_DIE) )
	{
		if ( IsACTION(GLAT_DIE) )		return TRUE;
	}

	return FALSE;
}

BOOL GLCharacter::IsPartyMaster ()
{
	GLPARTY_CLIENT* pParty = GLPartyClient::GetInstance().GetMaster();
	if ( pParty==NULL )		return FALSE;
	
	return ( pParty->m_dwGaeaID==m_dwGaeaID );
}

BOOL GLCharacter::IsPartyMem ()
{
	return GLPartyClient::GetInstance().GetMaster()!=NULL;
}

BOOL GLCharacter::IsClubMaster ()
{
	return m_sCLUB.m_dwMasterID==m_dwCharID;
}

BOOL GLCharacter::IsCDCertify ()
{
	return m_sCLUB.IsMemberFlgCDCertify(m_dwCharID);
}

void GLCharacter::P2PReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{
	if ( m_bSTATE_STUN )				return;
	
	if ( dwML & (DXKEY_UP|DXKEY_DUP) )
	{
		//	Note : 리엑션 취소.
		SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
		m_sREACTION.RESET();

		PLANDMANCLIENT pLandMan = GLGaeaClient::GetInstance().GetActiveMap();
		
		PGLCHARCLIENT pCharClient = pLandMan->GetChar ( sTargetID.dwID );
		if ( !pCharClient )		return;


		const D3DXVECTOR3 &vTarPos = pCharClient->GetPosition();

		float fMoveDist = FLT_MAX;
		float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
		if ( IsACTION(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sTargetID.vPos-vTarPos) );

		WORD wP2PRange = pCharClient->GetBodyRadius() + GETBODYRADIUS() + 25;
		WORD wP2PAbleDis = wP2PRange + 2;

		if ( fDist>wP2PAbleDis )
		{
			if ( fMoveDist > wP2PAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vDir = vTarPos - m_vPos;
				D3DXVec3Normalize ( &vDir, &vDir );

				bMove = TRUE;
				vMoveTo = m_vPos + vDir*wP2PRange;
				vMoveTo.y = vTarPos.y;

				//	Note : 리엑션 등록.
				//
				m_sREACTION.emREACTION = REACT_P2P;
				m_sREACTION.sTARID = sTargetID;
				m_sREACTION.vMARK_POS = vMoveTo;
			}
			else
			{
				m_sREACTION = sREC_TEMP;
			}

			return;
		}

		//	Note : PtoP 메뉴.
		//
		//bool bVALID_CLUB = m_sCLUB.m_dwID!=CLUB_NULL;
		//bool bMASTER = m_sCLUB.m_dwMasterID==m_dwCharID;

		if( !GLCONST_CHAR::bBATTLEROYAL )
		{
			CInnerInterface::GetInstance().SetPtoPWindowOpen( pCharClient, sTargetID.dwID, m_sCLUB );
		}
	}
}

void GLCharacter::PvPReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{

	if ( m_bSTATE_STUN )	return;	

	// 공격애니메이션이 종료유무
	if ( !m_bAttackable ) return;

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLand && pLand->IsPeaceZone() )	return;

	//	Note : 리엑션 취소.
	SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	m_sREACTION.RESET();

	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
	if ( IsACTION(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sTargetID.vPos-vTarPos) );

	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
	if ( !pTarget )		return;

	//	Note : 장거리 무기일 경우만 공격가능거리에 사정거리 옵션 값을 더해줌.
	//
	WORD wAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
	if ( ISLONGRANGE_ARMS() )	 wAttackRange += (WORD) GETSUM_TARRANGE();
	WORD wAttackAbleDis = wAttackRange + 2;

	if ( fDist>wAttackAbleDis )
	{
		if ( fMoveDist > wAttackAbleDis )
		{
			//	Note : 이동지점 지정.
			//
			D3DXVECTOR3 vPos = vTarPos - m_vPos;
			D3DXVec3Normalize ( &vPos, &vPos );

			bMove = TRUE;
			vMoveTo = vTarPos - vPos*wAttackRange;
			vMoveTo.y = vTarPos.y;

			//	Note : 리엑션 등록.
			//
			m_sREACTION.emREACTION = REACT_PVP;
			m_sREACTION.bCONTINUE = bcontinue;
			m_sREACTION.sTARID = sTargetID;
			m_sREACTION.vMARK_POS = vMoveTo;
		}
		else
		{
			m_sREACTION = sREC_TEMP;
		}

		return;
	}

	//	지형과 충돌하는지 검사.
	//	
	D3DXVECTOR3 vPos1 = GetPosition(); vPos1.y += 15.0f;
	D3DXVECTOR3 vPos2 = vTarPos; vPos2.y += 15.0f;
	
	BOOL bCollision;
	D3DXVECTOR3 vCollision;
	LPDXFRAME pDxFrame;

	DxLandMan *pLandMan = GLGaeaClient::GetInstance().GetActiveMap()->GetLandMan();
	pLandMan->IsCollision ( vPos1, vPos2, vCollision, bCollision, pDxFrame, FALSE );
	if ( bCollision )	return;

	PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( AN_ATTACK, GLCHARLOGIC::m_emANISUBTYPE );
	if ( !pAnicont )	return;

	WORD wStrikeNum = pAnicont->pAnimCont->m_wStrikeCount;

	//	Note : 공격수행 가능 검사.
	//    
	EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);
	if ( emBeginFB==EMBEGINA_OK || emBeginFB==EMBEGINA_SP )
	{
		m_sTargetID.emCrow = sTargetID.emCrow;
		m_sTargetID.dwID = sTargetID.dwID;
		TurnAction ( GLAT_ATTACK );
	}

	if ( bcontinue )
	{
		m_sREACTION.emREACTION = REACT_PVP;
		m_sREACTION.bCONTINUE = true;
		m_sREACTION.sTARID = sTargetID;
		m_sREACTION.vMARK_POS = m_vPos;
	}

	return;
}

void GLCharacter::MobReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{
	if ( m_bSTATE_STUN )	return;

	// 공격애니메이션이 종료유무
	if ( !m_bAttackable ) return;

	m_sOLD_TARMOB = sTargetID;

	//	Note : 리엑션 취소.
	SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	m_sREACTION.RESET();

	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
	if ( IsACTION(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sTargetID.vPos-vTarPos) );

	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
	if ( !pTarget )		return;

	//	Note : 장거리 무기일 경우만 공격가능거리에 사정거리 옵션 값을 더해줌.
	//
	WORD wAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
	if ( ISLONGRANGE_ARMS() )	 wAttackRange += (WORD) GETSUM_TARRANGE();
	WORD wAttackAbleDis = wAttackRange + 2;

	if ( fDist>wAttackAbleDis )
	{
		if ( fMoveDist > wAttackAbleDis )
		{
			//	Note : 이동지점 지정.
			//
			D3DXVECTOR3 vPos = vTarPos - m_vPos;
			D3DXVec3Normalize ( &vPos, &vPos );

			bMove = TRUE;
			vMoveTo = vTarPos - vPos*wAttackRange;
			vMoveTo.y = vTarPos.y;

			//	Note : 리엑션 등록.
			//
			m_sREACTION.emREACTION = REACT_MOB;
			m_sREACTION.bCONTINUE = bcontinue;
			m_sREACTION.sTARID = sTargetID;
			m_sREACTION.vMARK_POS = vMoveTo;
		}
		else
		{
			m_sREACTION = sREC_TEMP;
		}

		return;
	}

	//	지형과 충돌하는지 검사.
	//	
	D3DXVECTOR3 vPos1 = GetPosition(); vPos1.y += 15.0f;
	D3DXVECTOR3 vPos2 = vTarPos; vPos2.y += 15.0f;
	
	BOOL bCollision;
	D3DXVECTOR3 vCollision;
	LPDXFRAME pDxFrame;

	DxLandMan *pLandMan = GLGaeaClient::GetInstance().GetActiveMap()->GetLandMan();
	pLandMan->IsCollision ( vPos1, vPos2, vCollision, bCollision, pDxFrame, FALSE );
	if ( bCollision )	return;

	PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( AN_ATTACK, GLCHARLOGIC::m_emANISUBTYPE );
	if ( !pAnicont )	return;

	WORD wStrikeNum = pAnicont->pAnimCont->m_wStrikeCount;

	//	Note : 공격수행 가능 검사.
	//
	EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);
	if ( emBeginFB==EMBEGINA_OK || emBeginFB==EMBEGINA_SP )
	{
		m_sTargetID.emCrow = sTargetID.emCrow;
		m_sTargetID.dwID = sTargetID.dwID;
		TurnAction ( GLAT_ATTACK );
	}

	if ( bcontinue )
	{
		m_sREACTION.bCONTINUE = true;
		m_sREACTION.emREACTION = REACT_MOB;
		m_sREACTION.sTARID = sTargetID;
		m_sREACTION.vMARK_POS = m_vPos;
	}

	return;
}

// *****************************************************
// Desc: NPC에 관련된 예약된 액션을 처리한다.
// *****************************************************
void GLCharacter::NpcReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{
	if ( m_bSTATE_STUN )	return;

	//	Note : 리엑션 취소.
	SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	m_sREACTION.RESET();

	// 선택된 NPC 초기화
	m_dwNPCID = 0;

	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	// 필요없는 코드
	D3DXVECTOR3 vDistance = m_vPos - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	// NPC와의 거리
	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
	if ( IsACTION(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sTargetID.vPos-vTarPos) );

	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
	if ( !pTarget )	return;

	// 대화가 가능한 범위
	WORD wTakeRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + 30;
	WORD wTakeAbleDis = wTakeRange + 2;

	// 지금 위치가 대화가 불가능 하면
	if ( fDist>wTakeAbleDis )
	{
		// 이동한 목적지가 대화가 불가능 하면 가능한 위치로 이동하는 예약된 액션을 등록
		if ( fMoveDist > wTakeAbleDis )
		{
			//	Note : 이동지점 지정.
			//
			D3DXVECTOR3 vPos = vTarPos - m_vPos;
			D3DXVec3Normalize ( &vPos, &vPos );

			bMove = TRUE; 
			vMoveTo = vTarPos - vPos*wTakeRange;
			vMoveTo.y = vTarPos.y;

			//	Note : 리엑션 등록.
			//
			m_sREACTION.emREACTION = REACT_NPC;
			m_sREACTION.sTARID = sTargetID;
			m_sREACTION.vMARK_POS = vMoveTo;
		}
		// 이동한 목적지가 대화가 가능하면 그냥 예전 예약액션을 그래도 사용
		else
		{
			m_sREACTION = sREC_TEMP;
		}

		return;
	}

	// 만약 예약된 NPC가 있을경우 NPC의 위치 확인
	if ( dwML&(DXKEY_UP|DXKEY_DUP) )
	{
		PGLCROWCLIENT pCrow = GLGaeaClient::GetInstance().GetActiveMap()->GetCrow ( sTargetID.dwID );
		if ( !pCrow )		return;
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCrow->m_sNativeID );
		if ( !pCrowData )	return;

		m_dwNPCID   = sTargetID.dwID;

		CInnerInterface::GetInstance().SetDialogueWindowOpen ( sTargetID, this );
	}
}

// *****************************************************
// Desc: 채집NPC에 관련된 예약된 액션을 처리한다.
// *****************************************************
void GLCharacter::GatheringReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{
	if ( m_bSTATE_STUN )	return;

	//	Note : 리엑션 취소.
	SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	m_sREACTION.RESET();

	// 선택된 NPC 초기화
	m_dwNPCID = 0;

	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	// 필요없는 코드
	D3DXVECTOR3 vDistance = m_vPos - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	// NPC와의 거리
	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(m_vPos-vTarPos) );
	if ( IsACTION(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sTargetID.vPos-vTarPos) );

	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID );
	if ( !pTarget )	return;

	// 대화가 가능한 범위
	WORD wCollectionRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + 2;
	WORD wCollectionAbleDis = wCollectionRange + 10;

	// 지금 위치가 대화가 불가능 하면
	if ( fDist>wCollectionAbleDis )
	{
		// 이동한 목적지가 대화가 불가능 하면 가능한 위치로 이동하는 예약된 액션을 등록
		if ( fMoveDist > wCollectionAbleDis )
		{
			//	Note : 이동지점 지정.
			//
			D3DXVECTOR3 vPos = vTarPos - m_vPos;
			D3DXVec3Normalize ( &vPos, &vPos );

			bMove = TRUE; 
			vMoveTo = vTarPos - vPos*wCollectionRange;
			vMoveTo.y = vTarPos.y;

			//	Note : 리엑션 등록.
			//
			m_sREACTION.emREACTION = REACT_GATHERING;
			m_sREACTION.sTARID = sTargetID;
			m_sREACTION.vMARK_POS = vMoveTo;
		}
		// 이동한 목적지가 대화가 가능하면 그냥 예전 예약액션을 그래도 사용
		else
		{
			m_sREACTION = sREC_TEMP;
		}

		return;
	}

	// 만약 예약된 NPC가 있을경우 NPC의 위치 확인
	if ( dwML&(DXKEY_UP|DXKEY_DUP) )
	{
		PGLMATERIALCLIENT pMaterial = GLGaeaClient::GetInstance().GetActiveMap()->GetMaterial ( sTargetID.dwID );
		if ( !pMaterial )		return;
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pMaterial->m_sNativeID );
		if ( !pCrowData )	return;
		if ( pCrowData->m_emCrow != CROW_MATERIAL ) return;

		// 채집 시작
		ReqGathering( sTargetID );
	}
}

void GLCharacter::ItemReaction ( const STARGETID &sTargetID, const DWORD dwML, const bool bcontinue, BOOL &bMove, D3DXVECTOR3 &vMoveTo )
{
	if ( m_bSTATE_STUN )	return;

	//	Note : 리엑션 취소.
	SREACTION sREC_TEMP = ( m_sREACTION.sTARID == sTargetID ) ? m_sREACTION : SREACTION();
	m_sREACTION.RESET();

	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

	D3DXVECTOR3 vDistance = m_vPos - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	WORD wTarBodyRadius = 4;
	WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
	WORD wTakeAbleDis = wTakeRange + 12;

	if ( fDistance>wTakeAbleDis )
	{
		//	Note : 이동지점 지정.
		//
		D3DXVECTOR3 vPos = vTarPos - m_vPos;
		D3DXVec3Normalize ( &vPos, &vPos );

		bMove = TRUE; 
		vMoveTo = vTarPos - vPos*wTakeRange;

		//	Note : 리엑션 등록.
		//
		m_sREACTION.emREACTION = REACT_ITEM;
		m_sREACTION.sTARID = sTargetID;
		m_sREACTION.vMARK_POS = vMoveTo;

		return;
	}
	
	if ( dwML&(DXKEY_UP|DXKEY_DUP) )
	{
		//	줍기 시도.
		ReqFieldTo ( sTargetID );
	}
}

void GLCharacter::ReservedAction ( float fTime )
{
    if ( !m_sREACTION.ISVALID() )									return;
	if ( m_actorMove.PathIsActive() || !IsACTION(GLAT_IDLE) )		return;
	if ( !GLGaeaClient::GetInstance().GetActiveMap() )				return;
	if ( m_bVehicle )												return;
	
	if ( m_bSTATE_STUN )
	{
		m_sREACTION.RESET();
		return;
	}


	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();

	//	Note : 목표 위치가 정확한지 검사.
	//
	D3DXVECTOR3 vCURPOS = m_actorMove.Position();
	D3DXVECTOR3 vDIR =  m_sREACTION.vMARK_POS - vCURPOS;
	float fLength = D3DXVec3Length ( &vDIR );
	if ( fLength > 10.0f )
	{
		m_sREACTION.RESET();
		return;
	}

	//	Note : 피행위자가 정확한지 검사.
	//
	BOOL bTARID(TRUE);
	STARGETID sTARID = m_sREACTION.sTARID;
	switch ( m_sREACTION.emREACTION )
	{
	case REACT_MOB:
	case REACT_NPC:
	case REACT_P2P:
	case REACT_PVP:
	case REACT_GATHERING:
		{
			GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTARID );
 			if ( !pTarget || pTarget->IsACTION(GLAT_FALLING) || pTarget->IsACTION(GLAT_DIE) )	bTARID = FALSE;
		}
		break;

	case REACT_ITEM:
		switch ( sTARID.emCrow )
		{
		case CROW_ITEM:
			if ( !pLand->GetItem(sTARID.dwID) )										bTARID = FALSE;
			break;
		case CROW_MONEY:
			if ( !pLand->GetMoney(sTARID.dwID) )									bTARID = FALSE;
			break;
		//case CROW_PET:	break;	// PetData
		default:
			bTARID = FALSE;
			break;
		};
		break;
	};

	if ( !bTARID )
	{
		m_sREACTION.RESET();
		return;
	}

	//	피행위자의 현제 위치.
	m_sREACTION.sTARID.vPos = GLGaeaClient::GetInstance().GetTargetPos ( sTARID );

	BOOL bMove(FALSE);
	D3DXVECTOR3 vMoveTo(0,0,0);
	bool bcontinue = m_sREACTION.bCONTINUE;
	switch ( m_sREACTION.emREACTION )
	{
	case REACT_P2P:			P2PReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;
	case REACT_PVP:			PvPReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;

	case REACT_MOB:			MobReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;
	case REACT_NPC:			NpcReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;
	case REACT_ITEM:		ItemReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;
	case REACT_SKILL:		SkillReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);		break;
	case REACT_GATHERING:	GatheringReaction(m_sREACTION.sTARID,DXKEY_UP,bcontinue,bMove,vMoveTo);	break;
	};

	//	Note : Reaction 에서 이동을 요청한 경우.
	//
	if ( bMove )
	{
		ActionMoveTo ( fTime, vMoveTo+D3DXVECTOR3(0,+5,0), vMoveTo+D3DXVECTOR3(0,-5,0), FALSE, TRUE );
	}
}

STARGETID* GLCharacter::GetCONFT_TAR ( DETECTMAP_RANGE &pair )
{
	if ( !m_sCONFTING.IsFIGHTING() )	return NULL;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		{
			//	Note : 대련자가 걸렸을 경우 이를 우선시.
			STARGETID sCONFT_TARID(CROW_PC,m_sCONFTING.dwTAR_ID);
			STARGETID *pTARID = FindCrow ( pair, sCONFT_TARID );
			if ( pTARID )	return pTARID;
		}
		break;

	case EMCONFT_PARTY:
		{
			STARGETID *pTARID = FindCrow ( pair, m_sCONFTING.setConftMember );
			if ( pTARID )	return pTARID;
		}
		break;

	case EMCONFT_GUILD:
		{
			STARGETID *pTARID = FindCrowCID ( pair, m_sCONFTING.setConftMember );
			if ( pTARID )	return pTARID;
		}
		break;
	};

	return NULL;
}

bool GLCharacter::IsPK_TAR ( PGLCHARCLIENT pCHAR, bool bFORCED )
{
	//	Note : PK 의 경우.
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap ();
	if ( !pLAND )				return false;
	if ( !pLAND->IsPKZone() )	return false;

	SDROP_CHAR &sCHARDATA_TAR = pCHAR->GetCharData();

	//	파티일 경우는 어떤 경우든 적대행위가 되지 않음.
	GLPARTY_CLIENT* pPARTY_CLT = GLPartyClient::GetInstance().FindMember(sCHARDATA_TAR.dwGaeaID);
	if ( pPARTY_CLT )			return false;

	//	적대행위자.
	bool bHOSTILE = IS_PLAYHOSTILE ( sCHARDATA_TAR.dwCharID );
	if ( bHOSTILE )																		return true;

	//	선도클럽전 타겟.
	if ( pLAND->m_bClubBattle )
	{
		if ( m_sCLUB.IsAllianceGuild(sCHARDATA_TAR.dwGuild) ) return false;
		else												  return true;
	}

	if ( pLAND->m_bClubDeathMatch && GLGaeaClient::GetInstance().IsClubDeathMatch() )
	{
		if ( m_sCLUB.m_dwID != sCHARDATA_TAR.dwGuild ) 
			return true;
	}

	//	강제 PK.
	if ( bFORCED )																	
	{
		if ( sCHARDATA_TAR.wLevel <= GLCONST_CHAR::nPK_LIMIT_LEVEL )
			return false;
		else
			return true;
	}

	//	학교간 프리 PK.
	if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_Z_FORCED ) 
	{
		bool bSCHOOL_FREEPK = GLGaeaClient::GetInstance().IsSchoolFreePk ();
		if ( bSCHOOL_FREEPK && m_wSchool!=sCHARDATA_TAR.wSchool )					return true;
	}

	// 성향 이벤트일경우.
	if ( GLGaeaClient::GetInstance().IsBRIGHTEVENT() )								return true;

	// 클럽 배틀중일경우
	if ( ( m_sCLUB.IsBattle(pCHAR->GETCLUBID()) || m_sCLUB.IsBattleAlliance(pCHAR->GETALLIANCEID()) ) 
		&& pLAND->IsClubBattleZone() )		
		return true;

	return false;
}

bool GLCharacter::IsClubBattle_TAR ( GLCharClient* pCHAR )
{
	//	Note : PK 의 경우.
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap ();
	if ( !pLAND )				return false;

	// 클럽 배틀중일경우
	if ( ( m_sCLUB.IsBattle(pCHAR->GETCLUBID()) || m_sCLUB.IsBattleAlliance( pCHAR->GETALLIANCEID()) )
		&& pLAND->IsClubBattleZone() )
		return true;

	return false;
}

int GLCharacter::GetClubColorIndex( GLCharClient* pCHAR )
{
	int nIndex = 0;
	
	if ( !pCHAR ) return nIndex;

	CLUB_BATTLE_ITER pos = m_sCLUB.m_mapBattle.begin();
	CLUB_BATTLE_ITER pos_end = m_sCLUB.m_mapBattle.end();

	for ( ; pos != pos_end; ++pos )
	{
		if ( pCHAR->GETCLUBID() == (*pos).first ) break;
		if ( pCHAR->GETALLIANCEID() == (*pos).first ) break;
		nIndex++;
	}

	nIndex = nIndex % NS_UITEXTCOLOR::MAX_CLUB_BATTLE_COLOR;

	return nIndex;
}

SACTIONTAR GLCharacter::FindActionTarget ( const D3DXVECTOR3 &vTargetPt, const D3DXVECTOR3 &vFromPt, const EMFIND_TAR emFINDTAR )
{
	SACTIONTAR sACTAR;
	if ( IsSTATE(EM_ACT_CONTINUEMOVE) )		return sACTAR;

	//	Note : 사용자 입력값.
	//
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();
	
	DWORD dwKeyR = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_RUN] );
	DWORD dwKeyLAlt = dxInputDev.GetKeyState ( DIK_LMENU );
	DWORD dwKeyLCtrl = dxInputDev.GetKeyState ( DIK_LCONTROL );
	DWORD dwKeyLShift = dxInputDev.GetKeyState ( DIK_LSHIFT );
	DWORD dwKeySPACE = dxInputDev.GetKeyState ( DIK_SPACE );

	DWORD dwKeyZ = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_PKMODE] );

	bool bONLY_ITEM = (dwKeyLAlt&DXKEY_DOWNED)!=NULL;
	bool bODER_P2P = (dwKeyLShift&DXKEY_DOWNED)!=NULL;

	bool bANY(false), bENEMY(false), bOUR(false), bDIE(false);
	switch ( emFINDTAR )
	{
	case ENFIND_TAR_NULL:
		bANY = true;
		bENEMY = true;
		bOUR = true;
		break;
	case EMFIND_TAR_ENEMY:
		bENEMY = true;
		break;
	case EMFIND_TAR_ANY:
		bANY = true;
		break;
	case EMFIND_TAR_OUR:
		bOUR = true;
		break;

	case EMFIND_TAR_ANY_DIE:
		bANY = true;
		bDIE = true;
		break;
	case EMFIND_TAR_OUR_DIE:
		bOUR = true;
		bDIE = true;
		break;
	};

	STARGETID sTID_FOUND;
	DETECTMAP_RANGE pairRange;

	//	Crow 검색 ( 단, 연속 이동일 때는 검색하지 않는다. )
	DETECTMAP *pDetectMap = NULL;

	if ( bDIE )
	{
		pDetectMap = GLGaeaClient::GetInstance().DetectCrowDie ( vFromPt, vTargetPt );
		if ( pDetectMap )
		{
			//	Note : ALT 키를 누른 상태에서는 검출 제외 ==> 아이템, 돈에 우선권을 줌.
			//
			if ( !bONLY_ITEM )
			{
				if ( bENEMY )
				{
					//	MOB.
					pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
					if ( pairRange.first != pairRange.second )
					{
						sACTAR.emACTAR = EMACTAR_MOB;
						sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
						return sACTAR;
					}
				}
				else
				{
					//	PC ANY.
					pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
					if ( pairRange.first != pairRange.second )
					{
						sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );

						sACTAR.emACTAR = EMACTAR_PC_ANY;
						sACTAR.sTARID = sTID_FOUND;
						return sACTAR;
					}
				}
			}
		}

		return sACTAR;
	}

	DWORD emCrow = ( CROW_EX_PC | CROW_EX_NPC | CROW_EX_MOB | CROW_EX_ITEM | 
					 CROW_EX_MONEY | CROW_EX_SUMMON | CROW_EX_MATERIAL );
	pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, vFromPt, vTargetPt );

	if ( !pDetectMap )
	{
		//	Note : 스킬의 '모두에게' 혹은 '우리편에게' 속성일 경우 선택된 케릭이 없을때,
		//			자신이 선택되는지 점검.
		if ( emFINDTAR==EMFIND_TAR_ANY || emFINDTAR==EMFIND_TAR_ENEMY )
		{
			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, m_vMax, m_vMin );
			if ( bCol )
			{
				//	우리편이면 OUR 아니면 ANY
				EMACTIONTAR		emACTAR = EMACTAR_PC_ANY;
				if ( bOUR )		emACTAR = EMACTAR_PC_OUR;

				sACTAR.emACTAR = emACTAR;
				sACTAR.sTARID = STARGETID(CROW_PC,m_dwGaeaID,m_vPos);
				return sACTAR;
			}
		}

		return sACTAR;
	}

	//	Note : ALT 키를 누른 상태에서는 검출 제외 ==> 아이템, 돈에 우선권을 줌.
	//
	if ( !bONLY_ITEM )
	{
		//	PVP.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			//	Note : P2P 용도.
			//
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			if ( bODER_P2P )
			{
				sACTAR.emACTAR = EMACTAR_PC_P2P;
				sACTAR.sTARID = sTID_FOUND;
				return sACTAR;
			}
			else if ( bENEMY )
			{
				STARGETID *pPVP(NULL);

				//	Note : 대련자가 있는지 검사.	( 대련자 우선 )
				//
				pPVP = GetCONFT_TAR(pairRange);
				if ( pPVP )
				{
					sACTAR.emACTAR = EMACTAR_PC_PVP;
					sACTAR.sTARID = *pPVP;
					return sACTAR;
				}

				STARGETID &sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
				PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( sTID_FOUND.dwID );
				if ( pCHAR )
				{
					// 강제 공격키를 누르지 않아도 되는 경우( 성향 이벤트 중일경우 )
					bool bFORCED_PK = (dwKeyZ&DXKEY_DOWNED)!=NULL;
					bFORCED_PK = bFORCED_PK && !CInnerInterface::GetInstance().IsCHAT_BEGIN();
					bool bBRIGHTEVENT = GLGaeaClient::GetInstance().IsBRIGHTEVENT();

					bFORCED_PK = bFORCED_PK || bBRIGHTEVENT;
					if ( IsPK_TAR(pCHAR,bFORCED_PK) )
					{
						sACTAR.emACTAR = EMACTAR_PC_PVP;
						sACTAR.sTARID = sTID_FOUND;
						return sACTAR;
					}
				}
				
			}
		}

		

		if ( bENEMY )
		{
			//	MOB.
			pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
			if ( pairRange.first != pairRange.second )
			{
				STARGETID *pTARID = FindCrow ( pairRange, m_sOLD_TARMOB );
				if ( pTARID )
				{
					sACTAR.emACTAR = EMACTAR_MOB;
					sACTAR.sTARID = *pTARID;
					return sACTAR;
				}

				sACTAR.emACTAR = EMACTAR_MOB;
				sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
				return sACTAR;
			}
		}


		//	NPC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_NPC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sACTAR.emACTAR = EMACTAR_NPC;
			sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			return sACTAR;
		}

		//	METERIAL
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MATERIAL, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sACTAR.emACTAR = EMACTAR_MATERIAL;
			sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			return sACTAR;
		}

		//	PC, OUR, ANY.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			GLPARTY_CLIENT* pPARTY_CLT = GLPartyClient::GetInstance().FindMember(sTID_FOUND.dwID);

			//	우리편이면 OUR 아니면 ANY
			if ( pPARTY_CLT && bOUR )
			{
				sACTAR.emACTAR = EMACTAR_PC_OUR;
				sACTAR.sTARID = sTID_FOUND;
				return sACTAR;
			}
			else
			{
				sACTAR.emACTAR = EMACTAR_PC_ANY;
				sACTAR.sTARID = sTID_FOUND;
				return sACTAR;
			}
		}

		//	Note : 스킬의 '모두에게' 혹은 '우리편에게' 속성일 경우 선택된 케릭이 없을때,
		//			자신이 선택되는지 점검.
		if ( emFINDTAR==EMFIND_TAR_ANY || emFINDTAR==EMFIND_TAR_ENEMY )
		{
			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, m_vMax, m_vMin );
			if ( bCol )
			{
				//	우리편이면 OUR 아니면 ANY
				EMACTIONTAR		emACTAR = EMACTAR_PC_ANY;
				if ( bOUR )		emACTAR = EMACTAR_PC_OUR;

				sACTAR.emACTAR = emACTAR;
				sACTAR.sTARID = STARGETID(CROW_PC,m_dwGaeaID,m_vPos);
				return sACTAR;
			}
		}
	}

	// SUMMON ATTACK
	bool bFORCED_PK = (dwKeyZ&DXKEY_DOWNED)!=NULL;
	if ( bFORCED_PK )
	{
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			if ( bENEMY )
			{
				PGLANYSUMMON pSummon = GLGaeaClient::GetInstance().GetSummon ( sTID_FOUND.dwID );
				if ( pSummon )
				{
					// 강제 공격키를 누르지 않아도 되는 경우( 성향 이벤트 중일경우 )
					bool bFORCED_PK = (dwKeyZ&DXKEY_DOWNED)!=NULL;
					if ( bFORCED_PK )
					{
						GLPARTY_CLIENT* pPARTY_CLT = GLPartyClient::GetInstance().FindMember(pSummon->m_dwOwner);
						if ( !pPARTY_CLT || !bOUR ) 
						{
							sACTAR.emACTAR = EMACTAR_SUMMON_ATTACK;
							sACTAR.sTARID = sTID_FOUND;
						}
						return sACTAR;
					}
				}
			}
		}
	}

	//	ITEM.
	pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
	if ( pairRange.first != pairRange.second )
	{
		sACTAR.emACTAR = EMACTAR_ITEM;
		sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
		return sACTAR;
	}

	//	MONEY.
	pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
	if ( pairRange.first != pairRange.second )
	{
		sACTAR.emACTAR = EMACTAR_ITEM;
		sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
		return sACTAR;
	}

	return sACTAR;
}

SACTIONTAR GLCharacter::FindNearItem ()
{
	SACTIONTAR sACTAR_ITEM;
	SACTIONTAR sACTAR_MONEY;

	//	Crow 검색 ( 단, 연속 이동일 때는 검색하지 않는다. )
	DETECTMAP *pDetectMap = NULL;
	DETECTMAP_RANGE pairRange;
	STARGETID sTID_FOUND;

	DWORD emCrow = ( CROW_EX_ITEM | CROW_EX_MONEY );
	pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, m_vPos, MAX_VIEWRANGE/3.0f );
	if ( pDetectMap )
	{
		//	Item.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, m_vPos );

			sACTAR_ITEM.emACTAR = EMACTAR_ITEM;
			sACTAR_ITEM.sTARID = sTID_FOUND;
		}

		//	Money.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, m_vPos );

			sACTAR_MONEY.emACTAR = EMACTAR_ITEM;
			sACTAR_MONEY.sTARID = sTID_FOUND;
		}
	}

	if ( sACTAR_MONEY.emACTAR!=EMACTAR_NULL && sACTAR_ITEM.emACTAR!=EMACTAR_NULL )
	{
		std_afunc::CCompareTargetDist sDist(m_vPos);

		bool bMore = sDist.operator () ( sACTAR_MONEY.sTARID, sACTAR_ITEM.sTARID );

		if ( bMore )	return sACTAR_MONEY;
		else			return sACTAR_ITEM;
	}
	else if ( sACTAR_MONEY.emACTAR!=EMACTAR_NULL )
	{
		return sACTAR_MONEY;
	}
	else if ( sACTAR_ITEM.emACTAR!=EMACTAR_NULL )
	{
		return sACTAR_ITEM;
	}

	SACTIONTAR sACTAR;
	return sACTAR;
}

HRESULT GLCharacter::PlayerUpdate ( float fTime, float fElapsedTime )
{
	//	Note : 사용자 입력값 적용.
	//
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	DWORD dwKeyR(NULL);
	DWORD dwKeyX(NULL);
	DWORD dwKeyLAlt(NULL);
	DWORD dwKeyLCtrl(NULL);
	DWORD dwKeyLShift(NULL);
	DWORD dwkeySPACE(NULL);

	DWORD dwML(NULL), dwMM(NULL), dwMR(NULL);


	//	Note : 채팅윈도우 활성화시 키보드 가져 오지 않음.
	//
	if ( !DXInputString::GetInstance().IsOn() )
	{
		dwKeyR = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_RUN] );
		dwKeyX = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_ATTACKMODE] );
		dwkeySPACE = dxInputDev.GetKeyState ( DIK_SPACE );
	}

#if defined( TW_PARAM ) || defined( HK_PARAM ) 
	if ( !m_sPMarket.IsOpen() ) 
	{
		if( m_bOneHourNotInputKey == FALSE )
		{
			if( dxInputDev.IsUpdatInputState() )
			{
				m_fKeyDownCheckTime = 0.0f;
			}else{
				m_fKeyDownCheckTime += fElapsedTime;

				if( m_fKeyDownCheckTime >= 3600 )
				{
					m_bOneHourNotInputKey = TRUE;
					CInnerInterface::GetInstance().SetBlockProgramFound(true);
				}
			}
		}
	}else{
		m_fKeyDownCheckTime = 0.0f;
	}

	CDebugSet::ToView ( 1, "KeyDownCheckTime %f", m_fKeyDownCheckTime );
#endif

	dwKeyLAlt = dxInputDev.GetKeyState ( DIK_LMENU );
	dwKeyLCtrl = dxInputDev.GetKeyState ( DIK_LCONTROL );
	dwKeyLShift = dxInputDev.GetKeyState ( DIK_LSHIFT );

	dwML = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
	dwMM = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );
	dwMR = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );

#ifdef _RELEASED
	// 메시지 해킹 테스트
	DWORD dwKeyA = dxInputDev.GetKeyState ( DIK_A );
	DWORD dwKeyQ = dxInputDev.GetKeyState ( DIK_Q );

	if ( (dwKeyA&DXKEY_DOWNED) && (dwKeyLShift&DXKEY_DOWNED) )
	{
        SendHackingMSG ();
	}

	if ( (dwKeyQ&DXKEY_DOWNED) && (dwKeyLShift&DXKEY_DOWNED) )
	{
		SendHackingMSG1 ();
	}
#endif

	//	Note : 트레이드 등록 예비 아이템 리셋.
	//
	if ( dwMR & DXKEY_DOWN )
	{
		GLTradeClient::GetInstance().ReSetPreItem();
	}

	//	Note : 것기 뛰기 변경.
	if ( dwKeyR&DXKEY_UP )
	{
		ReqToggleRun ();
	}

	//	Note : 평화 모드 모션 변경.
	if ( dwKeyX&DXKEY_UP )
	{
		ReqTogglePeaceMode ();
	}

	//	Note : 마켓, 창고, 거래창, 모달박스가 활성화시 케릭터 제어 잠김.
	//
	BOOL bCharMoveBlock = CInnerInterface::GetInstance().IsCharMoveBlock ();
	if ( bCharMoveBlock )
	{
		//DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);
		CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
		DxViewPort::GetInstance().SetHandling ( false );
		return S_FALSE;
	}

	//	Note : 카메라 키보드로 제어 on/off
	//
	{
		DxViewPort::GetInstance().SetHandling ( true );

		bool bKeyHandling(true);
		if ( DXInputString::GetInstance().IsOn() )	bKeyHandling = false;
		DxViewPort::GetInstance().SetKeyboard ( bKeyHandling );
	}

	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt ();
	BOOL bOk = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );
	if ( !bOk )	return S_FALSE;

	//	손에 든 아이탬이 있을때. 바닥을 찍는다면.
	bool bCLICK_LEFT = NULL != (dwML&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP));
	bool bCLICK_RIGHT = NULL != (dwMR&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP));

	if ( VALID_HOLD_ITEM() && (bCLICK_LEFT||bCLICK_RIGHT) )
	{
		if ( dwML&DXKEY_UP || dwMR&DXKEY_UP )
		{
			D3DXVECTOR3 vCollisionPos;
			DWORD dwCollisionID;
			BOOL bCollision;
			GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh()->IsCollision
			(
				vFromPt,
				vTargetPt,
				vCollisionPos,
				dwCollisionID,
				bCollision
			);

			if ( bCollision )
			{
				D3DXVECTOR3 vDir = vCollisionPos - GetPosition();
				vDir.y = 0.0f;

				D3DXVec3Normalize ( &vDir, &vDir );
				D3DXVECTOR3 vDropPos = GetPosition() + vDir*float(GLCONST_CHAR::wBODYRADIUS+12);

				GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh()->IsCollision
				(
					vDropPos + D3DXVECTOR3(0,+5,0),
					vDropPos + D3DXVECTOR3(0,-5,0),
					vCollisionPos,
					dwCollisionID,
					bCollision
				);

				vDropPos = GetPosition();
				if ( bCollision )		vDropPos = vCollisionPos;

				if ( bCLICK_LEFT )		ReqHoldToField ( vDropPos );
				else					ReqHoldToFieldFireCracker ( vDropPos );

				return S_FALSE;
			}
		}
		else	return S_FALSE;
	}

	//	Note : 스킬 행동 우선일 경우 스킬 정보 가져옴.
	//
	PGLSKILL pRunSkill(NULL);
	if ( dwMR&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )
	{
		m_sActiveSkill = m_sRunSkill;

		SKILL_MAP_ITER learniter = m_ExpSkills.find ( m_sActiveSkill.dwID );
		if ( learniter!=m_ExpSkills.end() )
		{
			SCHARSKILL &sSkill = (*learniter).second;
			pRunSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkill.wMainID, m_sActiveSkill.wSubID );
		}
	}

	//	Note : 피행위자 찾기 속성.
	EMFIND_TAR emFINDTAR(ENFIND_TAR_NULL);
	if ( pRunSkill )
	{
		if ( pRunSkill->m_sAPPLY.emSPEC==EMSPECA_REBIRTH )
		{
			switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
			{
			default:				break;
			case SIDE_ENERMY:		break;
			case SIDE_OUR:			emFINDTAR = EMFIND_TAR_OUR_DIE;		break;
			case SIDE_ANYBODY:		emFINDTAR = EMFIND_TAR_ANY_DIE;		break;
			};
		}
		else
		{
			switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
			{
			default:				break;
			case SIDE_ENERMY:		emFINDTAR = EMFIND_TAR_ENEMY;	break;
			case SIDE_OUR:			emFINDTAR = EMFIND_TAR_OUR;		break;
			case SIDE_ANYBODY:		emFINDTAR = EMFIND_TAR_ANY;		break;
			};
		}
	}

	//	Note : 행동과 관련된 피행위자 찾기.
	//
	GLCOPY* pACTTAR(NULL);
	m_sACTAR = FindActionTarget ( vTargetPt, vFromPt, emFINDTAR );

	EMCROW emCROW = m_sACTAR.sTARID.emCrow;

	//EMACTAR_PC_ANY;


	bool bBRIGHTEVENT = GLGaeaClient::GetInstance().IsBRIGHTEVENT();

	if ( (emCROW==CROW_PC&&m_sACTAR.emACTAR==EMACTAR_PC_PVP) || m_sACTAR.emACTAR==EMACTAR_MOB || ( bBRIGHTEVENT&&m_sACTAR.emACTAR!=EMACTAR_NPC ) ||
		(emCROW==CROW_SUMMON&&m_sACTAR.emACTAR==EMACTAR_SUMMON_ATTACK) )
	{
		pACTTAR = GLGaeaClient::GetInstance().GetCopyActor ( m_sACTAR.sTARID );
	}

	if ( pACTTAR )
	{
//#ifdef CH_PARAM
		// 중국 : 오른쪽 버튼으로 타켓 지정시 카메라 제어 잠김.
		// DxViewPort::GetInstance().SetHandling( false );
//#endif

		D3DXVECTOR3 vCenterTarget = m_sACTAR.sTARID.vPos;
		vCenterTarget.y += 10.f;

		D3DXVECTOR3 vFromTEMP = vFromPt;
		if( !GLGaeaClient::GetInstance().IsMapCollsion( vCenterTarget, vFromTEMP ) )
		{
			CInnerInterface::GetInstance().SetTargetInfo ( m_sACTAR.sTARID );
		}
	}
	else
	{
//#ifdef CH_PARAM
		// 중국 : 오른쪽 버튼으로 타켓 지정시 카메라 제어 풀림.
		// DxViewPort::GetInstance().SetHandling( true );
//#endif
	}

	if ( m_sCONFTING.bFIGHT && m_sCONFTING.sOption.bSCHOOL )
	{
		//	Note : 학교간 대련 마크 표시 Update.
		CInnerInterface::GetInstance().SetAcademyUpdateNumber
		(
			GLPartyClient::GetInstance().GetConfrontNum (),
			(int) m_sCONFTING.setConftMember.size()
		);
	}

	////	Note : 마우스 모양 선택.
	////
	//switch ( m_sACTAR.emACTAR )
	//{
	//case EMACTAR_NULL:		DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);		break;
	//case EMACTAR_PC_PVP:	DxCursor::GetInstance().SetCursorType(EMCS_ATTACK);		break;
	//case EMACTAR_MOB:		DxCursor::GetInstance().SetCursorType(EMCS_ATTACK);		break;
	//case EMACTAR_NPC:
	//	{
	//		bool bTALK(false);
	//		PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	//		if ( pLAND )
	//		{
	//			PGLCROWCLIENT pCROW = pLAND->GetCrow(m_sACTAR.sTARID.dwID);
	//			if ( pCROW )
	//			{
	//				if ( strlen(pCROW->m_pCrowData->GetTalkFile()) )		bTALK = true;
	//			}
	//		}

	//		if ( bTALK )	DxCursor::GetInstance().SetCursorType(EMCS_TALK);
	//		else			DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);
	//	}
	//	break;

	//case EMACTAR_PC_OUR:	DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);		break;
	//case EMACTAR_PC_P2P:	DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);		break;
	//case EMACTAR_PC_ANY:	DxCursor::GetInstance().SetCursorType(EMCS_NORMAL);		break;
	//case EMACTAR_ITEM:		DxCursor::GetInstance().SetCursorType(EMCS_SELECT);		break;
	//};
	//	Note : 마우스 모양 선택.
	//
	switch ( m_sACTAR.emACTAR )
	{
	case EMACTAR_NULL:			CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
	case EMACTAR_SUMMON_ATTACK: CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);		break;
	case EMACTAR_PC_PVP:		CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);		break;
	case EMACTAR_MOB:			CCursor::GetInstance().SetCursorType(CURSOR_ATTACK);		break;
	case EMACTAR_NPC:
		{
			bool bTALK(false);
			PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
			if ( pLAND )
			{
				PGLCROWCLIENT pCROW = pLAND->GetCrow(m_sACTAR.sTARID.dwID);
				if ( pCROW )
				{
					if ( _tcslen(pCROW->m_pCrowData->GetTalkFile()) )		bTALK = true;
				}
			}

			if ( bTALK )	CCursor::GetInstance().SetCursorType(CURSOR_TALK);
			else			CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);
		}
		break;

	case EMACTAR_PC_OUR:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
	case EMACTAR_PC_P2P:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
	case EMACTAR_PC_ANY:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
	case EMACTAR_ITEM:		CCursor::GetInstance().SetCursorType(CURSOR_HAND);			break;
	case EMACTAR_MATERIAL:	CCursor::GetInstance().SetCursorType(CURSOR_NORMAL);		break;
	};

	//	Note : 연속 강제 행동 모드 인지 체크.
	bool bcontinue = (dwKeyLCtrl&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP)) != NULL;
	if ( !bcontinue ) bcontinue = RANPARAM::bFORCED_ATTACK!=FALSE;

	BOOL bMove(FALSE);
	D3DXVECTOR3	vMoveTo(0,0,0);
	if ( !IsACTION(GLAT_ATTACK) && !IsACTION(GLAT_SKILL) && !m_bVehicle && !m_bReqVehicle )
	{
		STARGETID sTARID = m_sACTAR.sTARID;

		if ( pRunSkill )
		{			
			SetDefenseSkill( false );

			//	Note : 스킬 발동 시도.
			//
			EMIMPACT_TAR emTAR = pRunSkill->m_sBASIC.emIMPACT_TAR;
			bool bSKILL_ACT = ( emTAR==TAR_SELF || emTAR==TAR_ZONE );

			//	Note : 스킬 속성 검사.11
			bool bSK2ENERMY(false), bSK2OUR(false), bSK2ANYBODY(false);
			switch ( pRunSkill->m_sBASIC.emIMPACT_SIDE )
			{
			default:				break;
			case SIDE_ENERMY:
				bSK2ENERMY = true;
				bcontinue = false;	// 스킬 강제공격 옵션 off
				break;
			
			case SIDE_OUR:
				bSK2OUR = true;
				bcontinue = false;	//	연속 강제 행동 off ( 적에게 수행하는 것만 가능하게. )
				break;
			
			case SIDE_ANYBODY:
				bSK2ANYBODY = true;
				bcontinue = false;	//	연속 강제 행동 off ( 적에게 수행하는 것만 가능하게. )
				break;
			};

			if ( bSKILL_ACT && m_sACTAR.emACTAR==EMACTAR_NULL )
			{
				D3DXVECTOR3 vCollisionPos;
				DWORD dwCollisionID;
				BOOL bCollision;
				GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh()->IsCollision
				(
					vFromPt, vTargetPt,
					vCollisionPos, dwCollisionID, bCollision
				);

				if ( bCollision )
				{
					D3DXVECTOR3 vDir = vCollisionPos - GetPosition();
					sTARID.vPos = vCollisionPos;
				}
				else
				{
					if ( emTAR==TAR_ZONE )	bSKILL_ACT = false;
				}
			}

			switch ( m_sACTAR.emACTAR )
			{
			case EMACTAR_NULL:			if(bSKILL_ACT)	SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_SUMMON_ATTACK: if(bSK2ENERMY)	SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_PC_PVP:		if(bSK2ENERMY)	SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_MOB:			if(bSK2ENERMY)	SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_NPC:			break;
			case EMACTAR_PC_OUR:		if(bSK2OUR)		SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_PC_P2P:		break;
			case EMACTAR_PC_ANY:		if(bSK2ANYBODY)	SkillReaction(sTARID,dwMR,bcontinue,bMove,vMoveTo);		break;
			case EMACTAR_ITEM:			break;
			case EMACTAR_MATERIAL:		break;
			};
		}
		else if ( dwML&(DXKEY_DOWNED|DXKEY_UP|DXKEY_DUP) )
		{
			//	Note : 상대에 따른 반응.
			//
			switch ( m_sACTAR.emACTAR )
			{
			case EMACTAR_NULL:			break;
			case EMACTAR_SUMMON_ATTACK: PvPReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_PC_PVP:		PvPReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_MOB:			MobReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_NPC:			NpcReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_PC_OUR:		break;
			case EMACTAR_PC_P2P:		P2PReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_PC_ANY:		break;
			case EMACTAR_ITEM:			ItemReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			case EMACTAR_MATERIAL:		GatheringReaction(sTARID,dwML,bcontinue,bMove,vMoveTo);	break;
			};
		}
		else if ( dwkeySPACE & DXKEY_DOWN )
		{
			m_sACTAR = FindNearItem ();
			STARGETID sTARID = m_sACTAR.sTARID;

			if ( m_sACTAR.emACTAR==EMACTAR_ITEM )
				ItemReaction(sTARID,DXKEY_DOWNED|DXKEY_UP,bcontinue,bMove,vMoveTo);
		}
	}

	//	Note : Reaction 에서 이동을 요청한 경우.
	//
	if ( bMove )
	{
		ActionMoveTo ( fTime, vMoveTo+D3DXVECTOR3(0,+5,0), vMoveTo+D3DXVECTOR3(0,-5,0), FALSE, TRUE );
		return S_OK;
	}

	//	Note : 일반 이동 처리.
	//
	BOOL bMOVEABLE = (m_sACTAR.emACTAR==EMACTAR_NULL);
	//				&& !IsACTION(GLAT_ATTACK) && !IsACTION(GLAT_SKILL);
	//				반응성 증가와 연속공격 취소를 원활하게 하기 위해서 액션 중간에도 취소가 가능하게 고침.
	if ( bMOVEABLE )
	{
		if ( dwML&DXKEY_UP )
		{
			CInnerInterface::GetInstance().DisableMinimapTarget();

			m_sREACTION.RESET();

			ReSetSTATE(EM_ACT_CONTINUEMOVE);
			ActionMoveTo ( fTime, vFromPt, vTargetPt );
		}
		else if ( dwML&DXKEY_DOWNED )
		{
			CInnerInterface::GetInstance().DisableMinimapTarget();

 			m_sREACTION.RESET();

			SetSTATE(EM_ACT_CONTINUEMOVE);
			ActionMoveTo ( fTime, vFromPt, vTargetPt, IsSTATE(EM_ACT_CONTINUEMOVE) );
		}
	}

	if ( !(dwML&DXKEY_DOWNED) && IsSTATE(EM_ACT_CONTINUEMOVE) )		ReSetSTATE(EM_ACT_CONTINUEMOVE);

	return S_OK;
}

HRESULT GLCharacter::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	if( !m_pSkinChar )	return E_FAIL;
	
	BOOL bLowSP = ( float(m_sSP.wNow) / float(m_sSP.wMax) ) <= GLCONST_CHAR::fLOWSP_MOTION;
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();

	BOOL bPeaceZone = pLand ? pLand->IsPeaceZone() : FALSE;
	if ( !bPeaceZone && IsSTATE(EM_ACT_PEACEMODE) )		bPeaceZone = TRUE;

	BOOL bFreeze = FALSE;

	EMANI_MAINTYPE emMType;
	EMANI_SUBTYPE emSType;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		
		if ( bPeaceZone && m_pSkinChar->GETANI ( AN_PLACID, AN_SUB_NONE ) )
		{
			emMType = AN_PLACID;
			emSType = AN_SUB_NONE;
			if ( m_bVehicle )
			{
				int emType = m_sVehicle.m_emTYPE ;
				emSType = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;
			}
			else  emSType = AN_SUB_NONE;
		}
		else
		{
			BOOL bPANT(FALSE); // 헐떡거림.
			
			if ( m_pSkinChar->GETANI ( AN_GUARD_L, m_emANISUBTYPE ) )
			{
				bPANT = bLowSP || m_bSTATE_PANT;
				for ( int i=0; i<EMBLOW_MULTI; ++i )
				{
					if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_STUN || m_sSTATEBLOWS[i].emBLOW == EMBLOW_MAD )		bPANT = TRUE;
				}
			}

			if ( bPANT )
			{
				emMType = AN_GUARD_L;
				emSType = m_emANISUBTYPE;
			}
			else
			{
				emMType = AN_GUARD_N;
				emSType = m_emANISUBTYPE;
			}
		}

		m_pSkinChar->SELECTANI ( emMType, emSType );
		m_bAttackable = TRUE;
		break;

	case GLAT_MOVE:
		emMType = IsSTATE(EM_ACT_RUN) ? AN_RUN : AN_WALK;
		emSType = bPeaceZone ? AN_SUB_NONE : m_emANISUBTYPE;
		if ( m_bVehicle )
		{
			int emType = m_sVehicle.m_emTYPE;
			emSType = (EMANI_SUBTYPE) ( AN_SUB_HOVERBOARD + emType );
		}
		m_pSkinChar->SELECTANI ( emMType, emSType );
		break;

	case GLAT_ATTACK:
		{
			EMSLOT emRHand = GetCurRHand();
			EMSLOT emLHand = GetCurLHand();

			EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand] );
			m_pSkinChar->SELECTANI ( AN_ATTACK, emANISUBTYPE, NULL, m_dwANISUBSELECT );
			if ( m_pSkinChar->ISENDANIM () || m_bVehicle )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_SKILL:
		{
			m_pSkinChar->SELECTSKILLANI ( GLCHARLOGIC::m_emANIMAINSKILL, GLCHARLOGIC::m_emANISUBSKILL );

			PSANIMCONTAINER pANIMCON = m_pSkinChar->GETCURANIM();
			if ( ( !(pANIMCON->m_dwFlag&ACF_LOOP) && m_pSkinChar->ISENDANIM () ) || m_bVehicle)
			{

				TurnAction ( GLAT_IDLE );
//				m_pSkinChar->m_bResetSkillAni = TRUE;
			}	
		}
		break;

	case GLAT_TALK:
		{
			BOOL bOK = m_pSkinChar->SELECTANI ( AN_GESTURE, EMANI_SUBTYPE(m_dwANISUBGESTURE) );
			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );
				break;
			}

			PSANIMCONTAINER pANIMCON = m_pSkinChar->GETCURANIM();
			if ( !(pANIMCON->m_dwFlag&ACF_LOOP) && m_pSkinChar->ISENDANIM () )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_SHOCK:

		emSType = m_emANISUBTYPE;
		if ( m_bVehicle )
		{
			int emType = m_sVehicle.m_emTYPE;
			emSType = (EMANI_SUBTYPE) ( AN_SUB_HOVERBOARD + emType );
		}

		m_pSkinChar->SELECTANI ( AN_SHOCK, emSType );
		if ( m_pSkinChar->ISENDANIM () )	TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		m_pSkinChar->SELECTANI ( AN_SHOCK, GLCHARLOGIC::m_emANISUBTYPE );
		break;

	case GLAT_FALLING:

		emSType = AN_SUB_NONE;
		if ( m_bVehicle )
		{
			int emType = m_sVehicle.m_emTYPE;
			emSType = (EMANI_SUBTYPE) ( AN_SUB_HOVERBOARD + emType );
		}

		m_pSkinChar->SELECTANI ( AN_DIE, emSType, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM () )
		{
			TurnAction ( GLAT_DIE );
		}
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		m_pSkinChar->TOENDTIME();
		break;

	case GLAT_CONFT_END:
		{
			EMANI_MAINTYPE emMTYPE(AN_CONFT_LOSS);
			if ( IsSTATE(EM_ACT_CONFT_WIN) )	emMTYPE = AN_CONFT_WIN;

			m_pSkinChar->SELECTANI ( emMTYPE, AN_SUB_NONE );
			if ( m_pSkinChar->ISENDANIM () )	TurnAction ( GLAT_IDLE );
		}
		break;
	case GLAT_GATHERING:
		{
			BOOL bOK = m_pSkinChar->SELECTANI ( AN_GATHERING, EMANI_SUBTYPE(m_dwANISUBGESTURE) );
			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );
				break;
			}

			PSANIMCONTAINER pANIMCON = m_pSkinChar->GETCURANIM();
			if ( !(pANIMCON->m_dwFlag&ACF_LOOP) && m_pSkinChar->ISENDANIM () )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;
	};

	//	Note : 스킨 업데이트.
	//
	m_pSkinChar->SetPosition ( m_vPos );

	BOOL bContinue = IsACTION(GLAT_ATTACK) || IsACTION(GLAT_SKILL);

	//	Note : 상태 이상에 따라 모션 속도를 조정한다.
	//
	float fSkinAniElap = fElapsedTime;
	switch ( m_Action )
	{
	case GLAT_MOVE:
		fSkinAniElap *= ( GLCHARLOGIC::GETMOVE_ITEM() + GLCHARLOGIC::GETMOVEVELO() );
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GLCHARLOGIC::GETATTVELO();
		fSkinAniElap += GLCHARLOGIC::GETATT_ITEM(); // 절대치
		break;
	};

	if ( !IsSTATE(EM_REQ_VISIBLENONE) )	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone );
	
	//	Note : 비 전투 지역일때 표시 안되야 하는 아이템(단검,투척) 인지 검사후 랜더 끄기.
	//
	if ( bPeaceZone )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		SITEM *pItemR = GET_SLOT_ITEMDATA ( emRHand );
		SITEM *pItemL = GET_SLOT_ITEMDATA ( emLHand );

		if ( pItemR )
		{
			BOOL bRend = !( pItemR->sSuitOp.emAttack==ITEMATT_DAGGER || pItemR->sSuitOp.emAttack==ITEMATT_THROW );
			m_pSkinChar->SetPartRend ( PIECE_RHAND, bRend );
		}
		if ( pItemL )
		{
			BOOL bRend = !( pItemL->sSuitOp.emAttack==ITEMATT_DAGGER || pItemL->sSuitOp.emAttack==ITEMATT_THROW );
			m_pSkinChar->SetPartRend ( PIECE_LHAND, bRend );
		}
	}
	else
	{
		m_pSkinChar->SetPartRend ( PIECE_RHAND, TRUE );
		m_pSkinChar->SetPartRend ( PIECE_LHAND, TRUE );
	}

	return S_OK;
}

// *****************************************************
// Desc: 게이트 조사
// *****************************************************
DWORD GLCharacter::DetectGate ()
{
	if ( IsSTATE(EM_REQ_GATEOUT) )		return UINT_MAX;

	PLANDMANCLIENT pLandMClient = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLandMClient )								return UINT_MAX;

	DxLandGateMan *pLandGateMan = &pLandMClient->GetLandGateMan();
	if ( !pLandGateMan )								return UINT_MAX;
		
	if ( pLandMClient->m_fAge<3.0f )					return UINT_MAX;

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate ( GetPosition() );
	if ( !pLandGate )									return UINT_MAX;
		
	if ( pLandGate->GetFlags()&DxLandGate::GATE_OUT )	return pLandGate->GetGateID();

	return UINT_MAX;
}

// *****************************************************
// Desc: 게이트의 이름 조사
// *****************************************************
CString GLCharacter::DetectGateToMapName ()
{
	if ( IsSTATE(EM_REQ_GATEOUT) )						return "";

	PLANDMANCLIENT pLandMClient = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLandMClient )								return "";

	DxLandGateMan *pLandGateMan = &pLandMClient->GetLandGateMan();
	if ( !pLandGateMan )								return "";
		
	if ( pLandMClient->m_fAge<3.0f )					return "";

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate ( GetPosition() );
	if ( !pLandGate )									return "";
		
	if ( pLandGate->GetFlags()&DxLandGate::GATE_OUT )
	{
		SNATIVEID sToMapID = pLandGate->GetToMapID();

		SMAPNODE *pMapNode = GLGaeaClient::GetInstance().FindMapNode ( sToMapID );
		if ( pMapNode )		return pMapNode->strMapName.c_str();
	}

	return "";
}

float GLCharacter::GetMoveVelo ()
{
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = fDefaultVelo * ( GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM() );
	return fMoveVelo;
}

void GLCharacter::VietnamCalculate()
{
	const int nMax1 = 180;
	const int nMax2 = 300;
	/*const int nMax1 = 30;
	const int nMax2 = 50;*/
	CInnerInterface::GetInstance().SET_VNGAINTYPE_GAUGE( 0, nMax2 );
   	if( m_sVietnamSystem.loginTime == 0 ) return;
	if( m_dwVietnamGainType == GAINTYPE_EMPTY ) 
	{
		CInnerInterface::GetInstance().SET_VNGAINTYPE_GAUGE( nMax2, nMax2 );
		return;
	}

	CTimeSpan gameTime( 0, (int)m_sVietnamSystem.gameTime / 60, (int)m_sVietnamSystem.gameTime % 60, 0 );


	CTime loginTime   = m_sVietnamSystem.loginTime;
	CTime crtTime     = GLGaeaClient::GetInstance().GetCurrentTime();
	CTimeSpan crtGameSpan;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

	int totalMinute  = (int)crtGameSpan.GetTotalMinutes();
	int totalSecond  = (int)crtGameSpan.GetTotalSeconds();

	CDebugSet::ToView ( 1, "Vietnam total Time M %d S %d", totalMinute, totalSecond );
	CDebugSet::ToView ( 2, "GameTime H %d M %d S %d Value %d", gameTime.GetTotalHours(), gameTime.GetTotalMinutes(), gameTime.GetTotalSeconds(),
						m_sVietnamSystem.gameTime );
	CDebugSet::ToView ( 3, "CrtTime Year %d Mon %d D %d H %d M %d S %d LoginTime Year %d Mon %d D %d H %d M %d S %d", 
							crtTime.GetYear(), crtTime.GetMonth(), crtTime.GetDay(), crtTime.GetHour(), crtTime.GetMinute(), crtTime.GetSecond(), 
							loginTime.GetYear(), loginTime.GetMonth(), loginTime.GetDay(), loginTime.GetHour(), loginTime.GetMinute(), loginTime.GetSecond() );


	if( m_dwVietnamGainType == GAINTYPE_MAX )
	{
		if( totalMinute > nMax1 )
			totalMinute = nMax1;
	}else if( m_dwVietnamGainType == GAINTYPE_HALF )
	{
		if( totalMinute < nMax1 )
			totalMinute = nMax1;
		if( totalMinute > nMax2 )
			totalMinute = nMax2;
	}

	if( totalMinute < 0 )  totalMinute = 0;
	
	CInnerInterface::GetInstance().SET_VNGAINTYPE_GAUGE( totalMinute, nMax2 );
	
	
	//	m_sVietnamSystem.currentGameTime = totalHours;
	//	m_sVietnamSystem.currentGameTime = totalMinutes;
	// 테스트는 초로 저장..
}

void GLCharacter::EventCalculate()
{
	if( m_bEventStart == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Start FALSE" );	
		return;
	}
	if( m_bEventApply == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Apply FALSE" );	
		return;
	}

	CTime crtTime   = GLGaeaClient::GetInstance().GetCurrentTime();
	CTime startTime = m_tLoginTime;
	CTimeSpan crtGameSpan;
	crtGameSpan					 = ( crtTime - startTime );

	if( m_bEventBuster == FALSE )
	{
		m_RemainEventTime  = m_EventStartTime - (int)crtGameSpan.GetTotalSeconds();
		m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime )
		{
//			m_bEventBuster = TRUE;	
			m_RemainEventTime = 0;
		}
	}

	if( m_bEventBuster == TRUE )
	{
		m_RemainEventTime  = 0;
		m_RemainBusterTime = (m_EventStartTime + m_EventBusterTime) - (int)crtGameSpan.GetTotalSeconds();
		if( m_RemainBusterTime > m_EventBusterTime )
			m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime + m_EventBusterTime )
		{
//			m_bEventBuster = FALSE;
//			m_tLoginTime   = crtTime.GetTime();

			m_RemainEventTime  = m_EventStartTime;
			m_RemainBusterTime = m_EventBusterTime;
		}
	}

	

	
	/*CDebugSet::ToView ( 1, "gameSpan: %d ;; bEventBuster: %d ;; startTime: %d ;; busterTime: %d", 
					    (int)crtGameSpan.GetTotalSeconds(), m_bEventBuster, m_EventStartTime, m_EventBusterTime );

	CDebugSet::ToView ( 2, "RemainEventTime: %d RemainBusterTime: %d", m_RemainEventTime, m_RemainBusterTime );*/

	
}

void GLCharacter::UpdateSpecialSkill()
{
	// 변신 스킬 체크
	if( m_dwTransfromSkill != SNATIVEID::ID_NULL )
	{
		if( m_sSKILLFACT[m_dwTransfromSkill].IsSpecialSkill( SKILL::EMSSTYPE_TRANSFORM ) )
		{
			// 쓰러지는 모션이면 변신해제
			STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
			if( IsACTION( GLAT_FALLING ) )
			{
				FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID );
			}

			if( m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID != NATIVEID_NULL() ) 
			{			
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransfromSkill].sNATIVEID.wSubID );
				if( pSkill && pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
				{
					if( pSkill->m_sSPECIAL_SKILL.strEffectName.size() != 0 )
					{
						if( m_sSKILLFACT[m_dwTransfromSkill].fAGE <= pSkill->m_sSPECIAL_SKILL.dwRemainSecond && 
							!m_sSKILLFACT[m_dwTransfromSkill].bRanderSpecialEffect )
						{
							D3DXMATRIX matTrans;							
							D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
							DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sSPECIAL_SKILL.strEffectName.c_str(), matTrans, &sTargetID );
							m_sSKILLFACT[m_dwTransfromSkill].bRanderSpecialEffect = TRUE;
						}
					}
				}
			}else{
				DISABLESKEFF(m_dwTransfromSkill);
				UpdateSuit(TRUE);				
				m_dwTransfromSkill = SNATIVEID::ID_NULL;
			}

			
		}else{
			DISABLESKEFF(m_dwTransfromSkill);
			UpdateSuit(TRUE);
			m_dwTransfromSkill = SNATIVEID::ID_NULL;
		}
	}
}

void GLCharacter::SET_RECORD_CHAT()
{
	if( m_bRecordChat ) return;
	m_bRecordChat = TRUE;
	m_strRecordChat		= CInnerInterface::GetInstance().GET_RECORD_CHAT();
	m_recordStartTime	= GLGaeaClient::GetInstance().GetCurrentTime();

	CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_START") );
}

void GLCharacter::UPDATE_RECORD_CHAT( bool bCloseClient /* = FALSE */ )
{
	if( !m_bRecordChat ) return;
	CTime curTime = GLGaeaClient::GetInstance().GetCurrentTime();
	CTimeSpan timeSpan = curTime - m_recordStartTime;
	if( timeSpan.GetMinutes() >= 2 || bCloseClient ) 
	{
		m_strRecordChat += CInnerInterface::GetInstance().GET_RECORD_CHAT();

		{
			TCHAR szPROFILE[MAX_PATH] = {0};
			TCHAR szFullPathFileName[MAX_PATH] = {0};
			SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

			StringCchCopy( szFullPathFileName, MAX_PATH, szPROFILE );
			StringCchCat( szFullPathFileName, MAX_PATH, SUBPATH::SAVE_ROOT );
			CreateDirectory( szFullPathFileName, NULL );

			CHAR  szFileName[MAX_PATH] = {0};
			sprintf_s( szFileName, "chat_[%d%d%d%d%d].txt", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 
													      curTime.GetHour(), curTime.GetMinute() );

			StringCchCat ( szFullPathFileName, MAX_PATH, szFileName );

			m_bRecordChat   = FALSE;

			CFile file;
			if ( ! file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary ) )// 파일 열기
			{

				CDebugSet::ToListView ( "RECORD_CHAT_FAILED" );
				return;
			}

			CString strSaveChat;			
			for( int i = 0; i < m_strRecordChat.GetLength(); i++ )
			{
				char szTempChar = m_strRecordChat.GetAt(i) ^ 0x2139;
				strSaveChat += szTempChar;
			}

			int nLength = strSaveChat.GetLength();
			file.Write(&nLength, sizeof(int));
			file.Write(strSaveChat.GetString(), nLength);
			file.Close();


			m_strRecordChat = "";
		}

		if( !bCloseClient )
		{
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_END") );
		}

	}
}

void GLCharacter::UpdateLandEffect()
{
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if( !pLand ) return;

	SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
	if( !pLevelEtcFunc ) return;
	if( !pLevelEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] ) return;

	// 모든 버프를 취소해야할 경우의 처리
	for( int i = 0; i < EMLANDEFFECT_MULTI; i++ )
	{
		SLANDEFFECT landEffect = m_sLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			if( m_dwTransfromSkill != SNATIVEID::ID_NULL )
			{
				m_dwTransfromSkill = SNATIVEID::ID_NULL;
				UpdateSuit(TRUE);
			}
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );
				DISABLESKEFF(i);
			}			
			return;
		}
	}
}

HRESULT GLCharacter::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;


	UPDATE_RECORD_CHAT();

#if defined(VN_PARAM) //vietnamtest%%%
	VietnamCalculate();
#endif

#ifndef CH_PARAM_USEGAIN
	EventCalculate();
#endif

	// 만약 기다리고 있다면
	if ( IsSTATE(EM_ACT_WAITING) )
	{
		if ( ++m_dwWAIT > EMWAIT_COUNT )
		{
			m_dwWAIT = 0;
			ReSetSTATE(EM_ACT_WAITING);

			//	Note : 맵에 진입함을 서버에 알려줌. ( 주위 환경 정보를 받기 위해서. )
			//
			GLMSG::SNETREQ_LANDIN NetMsgLandIn;
			NETSEND ( &NetMsgLandIn );
			CDebugSet::ToListView ( "GLMSG::SNETREQ_LANDIN" );

			//	Note : 서버에 준비되었음을 알림.
			//
			GLMSG::SNETREQ_READY NetMsgReady;
			NETSEND ( &NetMsgReady );
			CDebugSet::ToListView ( "GLMSG::SNETREQ_READY" );

			//	Note : 서버에 친구 리스트 전송 요청.
			GLFriendClient::GetInstance().ReqFriendList();

			// PET
			// 팻이 활동중에 맵을 이동했다면 다시 불러준다.
			if ( m_bIsPetActive )
			{
				 ReqReGenPet ();
				m_bIsPetActive = FALSE;
			}

			if ( m_bIsVehicleActive ) 
			{
				if ( ReqSetVehicle( true ) == S_OK ) m_bIsVehicleActive = FALSE;
			}

			// 탈것 장착하고 있다면 정보 요청 
			if ( m_PutOnItems[SLOT_VEHICLE].sNativeID != NATIVEID_NULL()  )
			{
				ReqVehicleUpdate();
			}

			// 인벤의 장착하고 있는 탈것 정보 요청
			if ( !m_bRecivedVehicleItemInfo ) 
			{
				ReqVehicleInvenUpdate();
				m_bRecivedVehicleItemInfo = true;
			}

			// 들고있는 팻카드 정보 요청
			if ( !m_bRecievedPetCardInfo )
			{
				// PET
				// 들고있는 팻카드의 정보를 요청한다.
				ReqPetCardInfo ();

#if defined KRT_PARAM || defined _RELEASED
				// PET
				// 시효만료로 사라진 팻카드의 팻정보 요청
				ReqPetReviveInfo ();
#endif
				m_bRecievedPetCardInfo = true;
			}


		}
	}

	//	Note : 살아있을 때만 갱신되는 것들.
	//
	if ( IsValidBody() )
	{
		bool bCHECK = CheckPASSIVITY ( fElapsedTime );
		if ( bCHECK )
		{
			//	Note : 수동적 행위 체크 박스 띄움.
			CInnerInterface::GetInstance().SetBlockProgramAlarm ( true );
		}

		if ( IsBlockPASSIVITY() && !DxGlobalStage::GetInstance().GetBlockDetectState() )
		{
			DxGlobalStage::GetInstance().SetDetectedReport ();

			GLMSG::SNET_BLOCK_DETECTED	NetMsg;
			NetMsg.dwDETECTED = UINT_MAX;
			NetMsg.dwCHARID = m_dwCharID;
			NETSEND ( &NetMsg );

			DxGlobalStage::GetInstance().CloseGame();
		}

		m_cQuestPlay.FrameMove ( fTime, fElapsedTime );

		//	Note : 플래이어 조작 처리.
		//
		if ( !IsCtrlBlockBody() )
		{
			PlayerUpdate ( fTime, fElapsedTime );

			//	Note : 예약행위 검사후 수행.
			//		PlayerUpdate() 안에서는 인터페이스 우선 순위 때문에 처리 할수 없음.
			//
			if ( m_sREACTION.ISVALID() )
			{
				ReservedAction ( fTime );
			}
		}

		//	Note : 체력 상태 갱신.
		//
		float fCONFT_POINT_RATE(1.0f);
		if ( m_sCONFTING.IsCONFRONTING() )		fCONFT_POINT_RATE = m_sCONFTING.sOption.fHP_RATE;

		// 지형 효과 업데이트
		UpdateLandEffect();
		GLCHARLOGIC::UPDATE_DATA ( fTime, fElapsedTime, TRUE, fCONFT_POINT_RATE );

		if ( m_bCLUB_CERTIFY )
		{
			m_fCLUB_CERTIFY_TIMER += fElapsedTime;

			float fRER = m_fCLUB_CERTIFY_TIMER / GLCONST_CHAR::fCDCERTIFY_TIME;
			if ( fRER > 1.0f )	fRER = 1.0f;

			CInnerInterface::GetInstance().SET_CONFT_CONFIRM_PERCENT ( fRER );
		}
	}

	if ( !IsSTATE(EM_ACT_CONTINUEMOVE) )
	{
		if ( memcmp(&m_sLastMsgMove,&m_sLastMsgMoveSend, sizeof(GLMSG::SNETPC_GOTO) ) )
		{
			m_fLastMsgMoveSend = fTime;
			m_sLastMsgMoveSend = m_sLastMsgMove;

			GLMSG::SNETPC_GOTO sMSG = m_sLastMsgMoveSend;
			NETSENDTOFIELD ( &sMSG );
		}
	}

	//	Note : 클럽마크 버전 확인.
	//
	ReqClubMarkInfo ( m_sCLUB.m_dwID, m_sCLUB.m_dwMarkVER );

	//	트레이드 상태 갱신.
	GLTradeClient::GetInstance().FrameMove ( fTime, fElapsedTime );

	//	Note : 타겟이 무효화 되는지 점검한다.
	//
	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( !pTarget )
	{
		m_sTargetID.dwID = EMTARGET_NULL;
		if ( IsACTION(GLAT_ATTACK) )	TurnAction ( GLAT_IDLE );
	}

	//	Note : Gate 검사.
	//
	DWORD dwDetectGate = DetectGate ();
	if ( dwDetectGate!=UINT_MAX )	CInnerInterface::GetInstance().SetGateOpen ( TRUE );

	// Note : 헤어를 Edit 해야 할 경우만 변경한다.
	if( m_bEnableHairSytle )	HairStyleUpdate();
	if( m_bEnableHairColor )	HairColorUpdate();
	if( m_bEnableFaceStyle )	FaceStyleUpdate();

	//	Note : 에니메이션, 스킨 업데이트.
	//
	UpateAnimation ( fTime, fElapsedTime );

	//	Note : 행위별 처리.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			if ( m_fIdleTime > 20.0f )
			{
				PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
				BOOL bPeaceZone = pLand ? pLand->IsPeaceZone() : FALSE;

				if ( !bPeaceZone && !IsSTATE(EM_ACT_PEACEMODE) )
				{
					ReqTogglePeaceMode();
				}
			}
		}
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		{
			//	Note : 케릭의 이동 업데이트.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo () );
			hr = m_actorMove.Update ( fElapsedTime );
			if ( FAILED(hr) )	return E_FAIL;

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
			AttackProc ( fElapsedTime );

			//	Note : 케릭의 현재 방향 업데이트.
			//
			GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - m_vPos;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
			if ( !pSkill )
			{
				TurnAction(GLAT_IDLE);
				break;
			}

			if ( !SkillProc ( fElapsedTime ) )	TurnAction(GLAT_IDLE);
		
			//	Note : 공격 방향으로 회전.
			//
			m_vDir = UpdateSkillDirection ( m_vPos, m_vDir, m_idACTIVESKILL, m_vTARPOS, m_sTARIDS );
		}
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		{
			m_actorMove.Update ( fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_FALLING:
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		if ( !m_sCONFTING.IsFIGHTING() )
		{
			DWORD dwCOUNT = m_sCONFTING.UPDATE(fElapsedTime);
			if ( EMCONFT_COUNT!=dwCOUNT )
			{
				CInnerInterface::GetInstance().SetFightBegin ( (int)(dwCOUNT+1) );
			}
		}
	}

	//	Note : 스킬 이펙트 업데이트.
	//
	FACTEFF::UpdateSkillEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );

	UpdateSpecialSkill();



	//	?Item 지속 화면 효과. on/off
	if ( m_emOldQuestionType!=m_sQITEMFACT.emType )
	{
		m_emOldQuestionType = m_sQITEMFACT.emType;

		switch ( m_sQITEMFACT.emType )
		{
		case QUESTION_NONE:
			{
				//	?Item 화면 효과.
				DxEffProjMan::GetInstance().EnableGetItemEFF ( FALSE );

				//	?Item BGM.
				PLANDMANCLIENT pLandClient = GLGaeaClient::GetInstance().GetActiveMap();
				if ( !pLandClient )		break;

				DxLandMan* pLandMan = pLandClient->GetLandMan();
				if ( !pLandMan )		break;

				const CString &strBgm = pLandMan->GetBgmFile();
				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm );
				DxBgmSound::GetInstance().Play();
			}
			break;

		case QUESTION_SPEED_UP:
		case QUESTION_CRAZY:
		case QUESTION_ATTACK_UP:
		case QUESTION_EXP_UP:
		case QUESTION_LUCKY:
		case QUESTION_SPEED_UP_M:
		case QUESTION_MADNESS:
		case QUESTION_ATTACK_UP_M:
			{
				if ( RANPARAM::bBuff )
					DxEffProjMan::GetInstance().EnableGetItemEFF ( TRUE );

				const std::string &strBgm = GLCONST_CHAR::strQITEM_BGM[m_sQITEMFACT.emType];
				if ( strBgm.empty() || strBgm=="null" )		break;

				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm.c_str() );
				DxBgmSound::GetInstance().Play();
			}
			break;
		};
	}

	//	Note : 케릭의 현제 위치 업데이트.
	//
	m_vPos = m_actorMove.Position();

	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	//D3DXMATRIX matScale;
	//D3DXMatrixScaling ( &matScale, 2, 2, 2 );

	m_matTrans = matYRot * matTrans;

	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

//	if( m_sQITEMFACT.IsACTIVE() ) m_sQITEMFACT.fTime -= fElapsedTime;

	//	Note : 현제 행동 상태 확인용.
	//
	CDebugSet::ToView ( 5, COMMENT::ANI_MAINTYPE[m_pSkinChar->GETCURMTYPE()].c_str() );
	CDebugSet::ToView ( 6, COMMENT::ANI_SUBTYPE[m_pSkinChar->GETCURSTYPE()].c_str() );
	CDebugSet::ToView ( 8, COMMENT::ATIONTYPE[m_Action].c_str() );

	CDebugSet::ToView ( 9, "char pos : %4.1f, %4.1f, %4.1f", m_vPos.x, m_vPos.y, m_vPos.z );

	// 추적 유저일 경우에 정보들을 남긴다.
#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined ( HK_PARAM ) // ***Tracing Log print
	if( m_bTracingUser )
	{
		PrintTracingUserWindowInfo();
		PrintTracingUserProcessInfo( fElapsedTime );
	}
#endif 

	return S_OK;
}

HRESULT GLCharacter::FrameMoveForTool( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;	

	//	Note : 에니메이션, 스킨 업데이트.
	//
//	UpateAnimation ( fTime, fElapsedTime );

	
	if ( !m_pSkinChar->ISENDANIM() )			
		m_pSkinChar->FrameMove ( fTime, fElapsedTime );

	//	Note : 스킬 이펙트 업데이트.
	//

	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	//D3DXMATRIX matScale;
	//D3DXMatrixScaling ( &matScale, 2, 2, 2 );

	m_matTrans = matYRot * matTrans;

	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;
	return S_OK;
}

void GLCharacter::PrintTracingUserProcessInfo( float fElapsedTime )
{


		// 5분마다 한번씩
	if( m_fPrintProcessInfoMsgGap == -10.0f || m_fPrintProcessInfoMsgGap > 600 )
	{
		m_fPrintProcessInfoMsgGap = 0.0f;

		// 프로세스 스냅샷 핸들을 얻는다.
		HANDLE h_snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

		// 스냅샷 되었을때 프로세스 주소내에 열거된 리스트를 저장하는 구조체
		PROCESSENTRY32 entry_data;
		entry_data.dwSize = sizeof(entry_data);

		// 첫번째 프로세스의 정보를 얻는다.
		char continue_flag = Process32First(h_snap_shot, &entry_data);         
		while(continue_flag)
		{
			char temp[256] = {0, };

			char name[MAX_PATH];
			unsigned int id;
			unsigned int thread_counts;
			unsigned int parent_id;
			unsigned int priority;
			PROCESS_MEMORY_COUNTERS pmc;            

			// 프로세스 아이디를 이용하여 해당 프로세스 핸들 값을 얻는다.
			HANDLE hwnd = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,entry_data.th32ProcessID);

			if(hwnd != NULL){
				// 정상적으로 프로세스를 열었다면..
				// 프로세스의 경로명을 얻는다.
				// 정상적으로 경로명을 얻었다면 해당 경로명을 그렇지 못했다면 파일명만을 출력한다.
				if(GetModuleFileNameEx(hwnd, NULL, temp, 256)) 	strcpy_s(name, temp);
				else strcpy_s(name, entry_data.szExeFile);				
				GetProcessMemoryInfo( hwnd, &pmc, sizeof(pmc));
				CloseHandle(hwnd);
			} else {
				// 정상적으로 프로세스를 열지 못했다면..
				strcpy_s(name, entry_data.szExeFile);

			}
			id = entry_data.th32ProcessID;
			thread_counts = entry_data.cntThreads;
			parent_id = entry_data.th32ParentProcessID;
			priority = entry_data.pcPriClassBase;
			

			NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );
			CString strTemp;
			int memory = pmc.WorkingSetSize / 1024;
			strTemp.Format( "ProcessInfo, ProcessName[%s], Memory[%dKB], Thread count[%d]",
							 name, memory, thread_counts );

			StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );
			NETSEND ( &TracingMsg );

	
			// 다음 프로세스의 정보를 얻는다.

			continue_flag = Process32Next(h_snap_shot, &entry_data);

		}

		CloseHandle(h_snap_shot);
	}else{
		m_fPrintProcessInfoMsgGap += fElapsedTime;
	}
}

void GLCharacter::PrintTracingUserWindowInfo()
{
	BYTE dwID[] = {INVENTORY_WINDOW, CHARACTER_WINDOW, SKILL_WINDOW, PARTY_WINDOW, QUEST_WINDOW, CLUB_WINDOW, FRIEND_WINDOW, LARGEMAP_WINDOW};
	BYTE i;
	for( i = 0; i < 8; i++ )
	{
		m_bNewVisibleTracingUI[i] = CInnerInterface::GetInstance().IsVisibleGroup( (UIGUID)dwID[i] );
		if( m_bOldVisibleTracingUI[i] != m_bNewVisibleTracingUI[i] )
		{
			m_bOldVisibleTracingUI[i] = m_bNewVisibleTracingUI[i];
			if( m_bNewVisibleTracingUI[i] == TRUE )
			{
				NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = GetUserID();
				StringCchCopy( TracingMsg.szAccount, USR_ID_LENGTH+1, m_szUID );
				CString strTemp;

				switch (i)
				{
				case 0:
					strTemp.Format( "Open Inventory Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 1:
					strTemp.Format( "Open Character Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 2:
					strTemp.Format( "Open Skill Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 3:
					strTemp.Format( "Open Party Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 4:
					strTemp.Format( "Open Quest Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 5:
					strTemp.Format( "Open Club Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 6:
					strTemp.Format( "Open Friend Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 7:
					strTemp.Format( "Open Mini Map, [%d][%s]", GetUserID(), m_szName );
					break;
				}
				StringCchCopy( TracingMsg.szLogMsg, TRACING_LOG_SIZE, strTemp.GetString() );

				NETSEND ( &TracingMsg );
			}
		}
	}
}

float GLCharacter::GetDirection ()
{
	return DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
}

HRESULT GLCharacter::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( IsSTATE(EM_REQ_VISIBLENONE) )	return S_FALSE;

	//	Note : m_bINVISIBLE 가 참일 경우 반투명하게 표현해야함.
	//
	bool bHALF_VISIBLE = m_bINVISIBLE || IsSTATE(EM_REQ_VISIBLEOFF);
	if ( bHALF_VISIBLE )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
	}
	else
	{
		DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
	}

	if ( m_pSkinChar )
	{
		m_pSkinChar->Render( pd3dDevice, m_matTrans );
	}

	#ifdef _SYNC_TEST
	{
		
		EDITMESHS::RENDERSPHERE( pd3dDevice, m_actorMove.GetTargetPosition(), 2.f );
		EDITMESHS::RENDERSPHERE( pd3dDevice, m_actorMove.Position(), 2.f );
		EDITMESHS::RENDERSPHERE( pd3dDevice, m_vPos, 2.f );
		EDITMESHS::RENDERSPHERE( pd3dDevice, m_vServerPos, 2.0f );
	}
	#endif

	//D3DXVECTOR3 vMax(m_vPos.x+MAX_VIEWRANGE, m_vPos.y+20, m_vPos.z+MAX_VIEWRANGE);
	//D3DXVECTOR3 vMin(m_vPos.x-MAX_VIEWRANGE, m_vPos.y-20, m_vPos.z-MAX_VIEWRANGE);
	//EDITMESHS::RENDERAABB ( pd3dDevice,  vMax, vMin );

	return S_OK;
}

HRESULT GLCharacter::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	
	if ( IsSTATE(EM_REQ_VISIBLENONE) )	return S_FALSE;
	if ( IsSTATE(EM_REQ_VISIBLEOFF) )	return S_FALSE;

	//	Note : 그림자 랜더링.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLCharacter::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( IsSTATE(EM_REQ_VISIBLENONE) )	return S_FALSE;
	if ( IsSTATE(EM_REQ_VISIBLEOFF) )	return S_FALSE;

	//	Note : 반사 랜더링.
	//
	if ( m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_pSkinChar, m_matTrans, pd3dDevice );
	}

	return S_OK;
}

HRESULT GLCharacter::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InitDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLCharacter::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLCharacter::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLCharacter::DeleteDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
	}

	return S_OK;
}

EMELEMENT GLCharacter::GET_ITEM_ELMT ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	EMELEMENT emELEMENT(EMELEMENT_SPIRIT);
	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pRHandItem->sSuitOp.sBLOW.emTYPE );
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pLHandItem->sSuitOp.sBLOW.emTYPE );
	}

	return emELEMENT;
}

void GLCharacter::StartAttackProc ()
{
	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLAND && pLAND->IsPeaceZone() )	return;


	if ( IsSTATE(EM_ACT_PEACEMODE) )
	{
		ReqTogglePeaceMode ();
	}

	DoPASSIVITY ( TRUE );

	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand] );
	DWORD dwAMOUNT = m_pSkinChar->GETANIAMOUNT ( AN_ATTACK, emANISUBTYPE );

	m_dwANISUBSELECT = m_dwANISUBCOUNT++;
	if ( m_dwANISUBCOUNT >= dwAMOUNT )		m_dwANISUBCOUNT = 0;

	BOOL bCONFT = ISCONFRONT_TAR ( m_sTargetID );

	//	Note : 공격 메시지 발생.
	//
	GLMSG::SNETPC_ATTACK NetMsg;
	NetMsg.emTarCrow = m_sTargetID.emCrow;
	NetMsg.dwTarID = m_sTargetID.dwID;
	NetMsg.dwAniSel = m_dwANISUBSELECT;
	NetMsg.dwFlags = NULL;

	NETSENDTOFIELD ( &NetMsg );

	m_bAttackable = FALSE;
}

void GLCharacter::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = GLGaeaClient::GetInstance().ValidCheckTarget ( m_sTargetID );
	if ( !bOk )	return;

	GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( m_sTargetID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	타겟의 위치.
	D3DXVECTOR3 vTARPOS = GLGaeaClient::GetInstance().GetTargetPos ( m_sTargetID );

	D3DXVECTOR3 vDir = vTARPOS - m_vPos;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - m_vPos;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = m_vPos + vTarDir*fLength * 10.0f;

	//	타격 위치를 알아냄.
	STRIKE::SSTRIKE sStrike;
	STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
	bOk = Selector.SELECT ( sStrikeEff, sStrike );
	if ( !bOk )	return;

	EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
	if( emSlot==SLOT_TSIZE )		return;
	if( emSlot==SLOT_RHAND )		emSlot = GetCurRHand();
	else if( emSlot==SLOT_LHAND )	emSlot = GetCurLHand();

	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );

	STARGETID sTargetID = m_sTargetID;
	sTargetID.vPos = vTARPOS;

	//	Note : 무기의 속성 살펴봄.
	//
	EMELEMENT emELEMENT = GET_ITEM_ELMT ();

	if ( VALID_SLOT_ITEM(emSlot) )
	{
		SITEM *pItem = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emSlot).sNativeID);
		if ( pItem )
		{
			if ( pItem->sSuitOp.emAttack <= ITEMATT_NEAR )
			{
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )	strEFFECT = GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT);

				DxEffGroupPlayer::GetInstance().NewEffGroup
				(
					strEFFECT.c_str(),
					matEffect,
					&sTargetID
				);

				DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
			}
			else
			{
				if ( pItem->sSuitOp.emAttack == ITEMATT_BOW )
				{
					EMSLOT emLHand = GetCurLHand();

					SITEM *pLHAND = NULL;
					if ( VALID_SLOT_ITEM(emLHand) )	pLHAND = GLItemMan::GetInstance().GetItem(GET_SLOT_ITEM(emLHand).sNativeID);
					if ( pLHAND && pLHAND->sBasicOp.emItemType == ITEM_ARROW )
					{
						DxEffSingleGroup* pEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup
						(
							pLHAND->GetTargetEffect(),
							matEffect,
							&sTargetID
						);
						
						if ( pEffSingleG )
						{
							pEffSingleG->AddEffAfter ( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT)  );
							pEffSingleG->AddEffAfter ( EFFABODY, std::string(pLHAND->GetTargBodyEffect()) );
							pEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
						}
					}
				}
			}
		}
	}
	else
	{
		//	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
			matEffect,
			&sTargetID
		);

		DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
	}
}

BOOL GLCharacter::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * m_fSTATE_MOVE;
	GASSERT(m_pSkinChar->GETCURANIMNODE());

	PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
	GASSERT(pAnicontNode);
	PSANIMCONTAINER pAnimCont = pAnicontNode->pAnimCont;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( GLGaeaClient::GetInstance().IsVisibleCV(m_sTargetID) )
			{
				//	Note : 공격시 이팩트 발휘..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}

	return TRUE;
}

// *****************************************************
// Desc: 엑션을 취할 수 있는 대상인지 check
// *****************************************************
BOOL GLCharacter::IsReActionable ( const STARGETID &sTargetID, BOOL bENEMY )
{
	GLCOPY *pTARGET = GLGaeaClient::GetInstance().GetCopyActor(sTargetID);

	if ( !pTARGET )		return false;

	EMCROW emCROW = pTARGET->GetCrow();

	PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( sTargetID.dwID );

	SDROP_CHAR &sCHARDATA_TAR = pCHAR->GetCharData();

	BOOL breaction(true);
	if ( bENEMY )
	{
		switch ( emCROW )
		{
		case CROW_PC:
			{
				if ( IsSafeZone() || pTARGET->IsSafeZone() ) return false;
				//	학교간 프리 PK.
				if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_Z_FORCED ) 
				{
					bool bSCHOOL_FREEPK = GLGaeaClient::GetInstance().IsSchoolFreePk ();

					if ( bSCHOOL_FREEPK && m_wSchool!=sCHARDATA_TAR.wSchool )
					{
						return true;
					}
				}

				{
					bool bBRIGHTEVENT = GLGaeaClient::GetInstance().IsBRIGHTEVENT();
					if( bBRIGHTEVENT )
					{
						return true;
					}
				}

				PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();

				if ( pLand && pLand->IsClubBattleZone() )
				{
					if ( m_sCLUB.IsBattle( pCHAR->GETCLUBID() ) ) return true;
					if ( m_sCLUB.IsBattleAlliance( pCHAR->GETALLIANCEID() ) ) return true;
				}


				//	대련시 공격 가능.
				breaction = ISCONFRONT_TAR ( sTargetID );
				
				//	pk 일 경우 공격 가능.
				if ( !breaction )	breaction = IS_PLAYHOSTILE ( pTARGET->GetCharID() );

				if ( !breaction )
				{
					// 선도전시 동맹클럽 맴버는 공격 불가.
					if ( pLand )
					{
						if ( pLand->m_bClubBattle )
						{
							PGLCHARCLIENT pCLIENT = GLGaeaClient::GetInstance().GetChar( sTargetID.dwID );
							BOOL bAlliance = m_sCLUB.IsAllianceGuild ( pCLIENT->GETCLUBID() );

							if ( bAlliance )
								breaction = false;
							else
								breaction = true;
						}
						else if ( pLand->m_bClubDeathMatch )
						{
							PGLCHARCLIENT pCLIENT = GLGaeaClient::GetInstance().GetChar( sTargetID.dwID );
							if ( m_sCLUB.m_dwID == pCLIENT->GETCLUBID() )	breaction = false;
							else breaction = true;
						}
					}
				}
			}

			break;

		case CROW_MOB:
			if ( m_sCONFTING.IsCONFRONTING() ) breaction = false;
			else if ( IsSafeZone() ) breaction = false;
			else breaction = true;
			break;

		case CROW_NPC:
			breaction = false;
			break;

		case CROW_MATERIAL:
			breaction = false;
			break;

		//case CROW_PET:	break;	// PetData

		default:
			breaction = false;
			break;
		};
	}
	else
	{
		switch ( emCROW )
		{
		case CROW_PC:
			{
				//	pc 에게 비공격 마법 같은 것은 대련자가 아닐 때만 가능. ( 일단 클라이언트는 검사 안함. )
				breaction = !IS_PLAYHOSTILE ( pTARGET->GetCharID() );

				PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
				if ( pLand && ( pLand->m_bClubBattle || pLand->m_bClubDeathMatch || pLand->IsFreePK() ) )
				{
					PGLCHARCLIENT pCLIENT = GLGaeaClient::GetInstance().GetChar(sTargetID.dwID);
					breaction = (pCLIENT->GETCLUBID()==m_dwGuild);
				}
			}
			break;
		
		case CROW_MOB:
			breaction = false;
			break;

		case CROW_NPC:
			breaction = m_sCONFTING.IsCONFRONTING() ? false : true;
			break;

		case CROW_MATERIAL:
			breaction = true;
			break;

		//case CROW_PET:	break;	// PetData

		default:
			breaction = false;
			break;
		};
	}

	return breaction;
}

BOOL GLCharacter::ISCONFRONT_TAR ( const STARGETID &sTargetID )
{
	if ( sTargetID.emCrow!=CROW_PC )	return FALSE;

	if ( m_sCONFTING.IsFIGHTING() )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			return ( sTargetID.dwID==m_sCONFTING.dwTAR_ID );
			break;

		case EMCONFT_PARTY:
			{
				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(sTargetID.dwID);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;

		case EMCONFT_GUILD:
			{
				PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( sTargetID.dwID );
				if ( !pCHAR )						return FALSE;

				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(pCHAR->GetCharData().dwCharID);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;
		};
	}

	return FALSE;
}

void GLCharacter::DELCONFRONT_MEMBER ( DWORD dwID )
{
	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		break;

	case EMCONFT_PARTY:
		{
			//	Note : 자기 파티원일 경우.
			GLPartyClient::GetInstance().ResetConfrontMember ( dwID );

			//	Note : 상대방 파티원일 경우.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;

	case EMCONFT_GUILD:
		{
			//	Note : 자기 클럽원일 경우.
			m_sCLUB.DELCONFT_MEMBER ( dwID );

			//	Note : 상대방 클럽원일 경우.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;
	};

	return;
}

void GLCharacter::NewConftBoundEffect ( const EMCONFT_TYPE emCONFT )
{
	//	Note : 이팩트 설정.
	//
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );

	D3DXMATRIX matTrans, matRotY;
	D3DXVECTOR3 vDist;
	D3DXMatrixIdentity ( &matTrans );

	for ( float fRotY=0.0f; fRotY<(D3DX_PI*2.0f); fRotY += (D3DX_PI*2.0f/24.0f) )
	{
		vDist = D3DXVECTOR3(1,0,0);
		D3DXMatrixRotationY(&matRotY,fRotY);
		D3DXVec3TransformCoord(&vDist,&vDist,&matRotY);

		float fCONFT_DIST(GLCONST_CHAR::fCONFRONT_ONE_DIST);
		switch ( emCONFT )
		{
		case EMCONFT_ONE:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_ONE_DIST;
			break;
		case EMCONFT_PARTY:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_PY_DIST;
			break;

		case EMCONFT_GUILD:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_CLB_DIST;
			break;
		};

		vDist = m_sCONFTING.vPosition + vDist * fCONFT_DIST;

		BOOL bCollision(FALSE);
		D3DXVECTOR3 vCollision;
		DWORD dwCollisionID;

		NavigationMesh* pNaviMesh = GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh();
		pNaviMesh->IsCollision(vDist+D3DXVECTOR3(0,10000.0f,0),vDist+D3DXVECTOR3(0,-10000.0f,0),vCollision,dwCollisionID,bCollision);

		matTrans._41 = vDist.x;
		matTrans._42 = vCollision.y;
		matTrans._43 = vDist.z;

		DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCONFRONT_BOUND.c_str(), matTrans );
	}
}

void GLCharacter::DelConftBoundEffect ()
{
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );
}

BOOL GLCharacter::IsVaildTradeInvenSpace ()
{
	GLInventory &sTradeBoxTar = GLTradeClient::GetInstance().GetTarTradeBox();

	GLInventory sInvenTemp;
	sInvenTemp.SetAddLine ( m_cInventory.GETAddLine(), true );
	sInvenTemp.Assign ( m_cInventory );

	//	Note : 자신의 교환 목록에 올라간 아이템을 인밴에서 제외.
	//
	GLInventory &sTradeBoxMy = GLTradeClient::GetInstance().GetMyTradeBox();
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

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
void GLCharacter::HairStyleInitData()
{
	m_bEnableHairSytle = TRUE;
	
	m_wHairStyleTEMP = m_wHair;

	// 컬러 변화 중일 경우 건들지 않는다.
	if( !m_bEnableHairColor )
	{
		m_wHairColorTEMP = m_wHairColor;
	}
}

void GLCharacter::HairColorInitData()
{
	m_bEnableHairColor = TRUE;

	m_wHairColorTEMP = m_wHairColor;

	// 스타일 변화 중일 경우 건들지 않는다.
	if( !m_bEnableHairSytle )
	{
		m_wHairStyleTEMP = m_wHair;
	}
}

void GLCharacter::HairStyleChange( WORD wStyle )
{
	m_wHairStyleTEMP = wStyle; 

	HairStyleUpdate();
}

void GLCharacter::HairColorChange( WORD wColor )
{ 
	m_wHairColorTEMP = wColor; 

	HairColorUpdate();
}

void GLCharacter::HairStyleUpdate()
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// 헤어스타일.
	if ( sCONST.dwHAIRNUM > m_wHairStyleTEMP )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[m_wHairStyleTEMP];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if( pCharPart && strcmp(strHAIR_CPS.c_str(),pCharPart->m_szFileName) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL, TRUE );
		}
	}

	// Note : 이것을 호출하면서.. 머리형이 원상태로 바뀌어 버린다.
	//		그래서 FALSE 호출로 머리모양을 안바뀌게 한다.
	UpdateSuit( FALSE, TRUE );
}

void GLCharacter::HairColorUpdate()
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	m_pSkinChar->SetHairColor( m_wHairColorTEMP );
}

void GLCharacter::FaceStyleInitData()
{
	m_bEnableFaceStyle = TRUE;
	
	m_wFaceStyleTEMP = m_wFace;
}

void GLCharacter::FaceStyleChange( WORD wStyle )
{
	m_wFaceStyleTEMP = wStyle; 

	FaceStyleUpdate();
}

void GLCharacter::FaceStyleUpdate()
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// 얼굴
	if ( sCONST.dwHEADNUM > m_wFaceStyleTEMP )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[m_wFaceStyleTEMP];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

		if ( pCharPart && strcmp(strHEAD_CPS.c_str(),pCharPart->m_szFileName) )
		{
			m_pSkinChar->SetPiece ( strHEAD_CPS.c_str(), m_pd3dDevice, 0x0, 0, TRUE );
		}
	}

	// Note : 이것을 호출하면서.. 머리형이 원상태로 바뀌어 버린다.
	//		그래서 FALSE 호출로 머리모양을 안바뀌게 한다.
	UpdateSuit( TRUE, FALSE );
}

BOOL GLCharacter::IsMarketClick( DWORD dwGaeaID )
{
	for( int i = 0; i < ( int ) m_vecMarketClick.size(); i++ )
	{
		if( m_vecMarketClick[ i ] == dwGaeaID )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void GLCharacter::InsertMarketClick( DWORD dwGaeaID )
{
	m_vecMarketClick.push_back( dwGaeaID );
}

void GLCharacter::InitAllSkillFact ()
{
	int i;
	for ( i=0; i<EMBLOW_MULTI; ++i )		DISABLEBLOW ( i );
	for ( i=0; i<SKILLFACT_SIZE; ++i )		DISABLESKEFF ( i );

	m_dwTransfromSkill = SNATIVEID::ID_NULL;

	FACTEFF::DeleteEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT, m_sSTATEBLOWS );
}

D3DXVECTOR3 GLCharacter::GetPosBodyHeight ()
{ 
	if ( m_bVehicle ) return D3DXVECTOR3( m_vPos.x, m_vPos.y+m_fHeight+10.0f, m_vPos.z ); 
	else return D3DXVECTOR3( m_vPos.x, m_vPos.y+m_fHeight, m_vPos.z ); 
}

LONGLONG GLCharacter::GetCalcTaxiCharge( int nSelectMap, int nSelectStop )
{
	GLTaxiStation& sTaxiStation = GLTaxiStation::GetInstance();

	STAXI_MAP* pTaxiMap = sTaxiStation.GetTaxiMap( nSelectMap );
	if ( !pTaxiMap )
	{
		GASSERT ( 0 && "pTaxiMap를 찾을수 없습니다." );
		return 0;
	}

	STAXI_STATION* pSTATION = pTaxiMap->GetStation( nSelectStop );
	if ( !pSTATION )
	{
		GASSERT ( 0 && "STATION을 찾을 수 없습니다." );
		return 0;
	}

	DWORD dwCurMapID = GLGaeaClient::GetInstance().GetActiveMapID().dwID;
	LONGLONG dwCharge = sTaxiStation.GetBasicCharge();

	if ( pSTATION->dwMAPID != dwCurMapID ) dwCharge += pSTATION->dwMapCharge;

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLand )
	{
		volatile float fSHOP_RATE = GetBuyRate();
		volatile float fSHOP_RATE_C = fSHOP_RATE * 0.01f;
		dwCharge = LONGLONG ( (float)dwCharge * fSHOP_RATE_C );
	}

	return dwCharge;
}

bool GLCharacter::IsCoolTime( SNATIVEID sNativeID )
{
	__time64_t tCurTime = GLGaeaClient::GetInstance().GetCurrentTime().GetTime();
	__time64_t tCoolTime = GetMaxCoolTime( sNativeID );

	if ( tCurTime < tCoolTime ) return true;

	return false;
}

bool GLCharacter::CheckCoolTime( SNATIVEID sNativeID )
{

	if ( IsCoolTime ( sNativeID ) )
	{
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sNativeID );
		if ( !pItem )	return true;

		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_COOLTIME"), pItem->GetName() );
		return true;
	}

	return false;
}

__time64_t	GLCharacter::GetMaxCoolTime ( SNATIVEID sNativeID )
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

ITEM_COOLTIME*	GLCharacter::GetCoolTime ( SNATIVEID sNativeID )
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

	return (tCoolType > tCoolID) ? pCoolTimeType : pCoolTimeID;
}



ITEM_COOLTIME*	GLCharacter::GetCoolTime( DWORD dwCoolID, EMCOOL_TYPE emCoolType )
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

bool GLCharacter::SetCoolTime ( ITEM_COOLTIME& sCoolTime, EMCOOL_TYPE emCoolType )
{
	if ( emCoolType == EMCOOL_ITEMID )
	{
		m_mapCoolTimeID[sCoolTime.dwCoolID] = sCoolTime;
	}
	else if( emCoolType == EMCOOL_ITEMTYPE )
	{
		m_mapCoolTimeType[sCoolTime.dwCoolID] = sCoolTime;
	}

	return true;
}

const SITEMCUSTOM GLCharacter::GET_ITEM_MIX( int nIndex )
{
	SITEMCUSTOM sItemCustom;
	sItemCustom.sNativeID = NATIVEID_NULL();

	if( !m_sItemMixPos[nIndex].VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sItemMixPos[nIndex].wPosX, m_sItemMixPos[nIndex].wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

void GLCharacter::SetItemMixMoveItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	if( m_sPreInventoryItem.VALID() )
	{
		for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			if ( m_sItemMixPos[i].wPosX == m_sPreInventoryItem.wPosX &&
				m_sItemMixPos[i].wPosY == m_sPreInventoryItem.wPosY )
			{
				m_sPreInventoryItem.RESET();
				return;
			}

		}

		m_sItemMixPos[nIndex].SET( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
		
		m_sPreInventoryItem.RESET();
	}
	
	return;
}

void GLCharacter::ReSetItemMixItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM || m_sPreInventoryItem.VALID() )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	m_sItemMixPos[nIndex].RESET();
}

void GLCharacter::ResetItemMix()
{
	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		m_sItemMixPos[i].RESET();
	}
	m_sPreInventoryItem.RESET();
}

bool GLCharacter::ValidItemMixOpen()
{
	if ( CInnerInterface::GetInstance().IsVisibleGroup( ITEM_MIX_INVEN_WINDOW ) 
		|| CInnerInterface::GetInstance().IsVisibleGroup( ITEM_MIX_WINDOW ) )
		return true;
	return false;
}

DWORD GLCharacter::GetItemMixMoney()
{
	ITEM_MIX sItemMix;

	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		sItemMix.sMeterialItem[i].sNID = GET_ITEM_MIX( i ).sNativeID;

		if( sItemMix.sMeterialItem[i].sNID != NATIVEID_NULL() )
			sItemMix.sMeterialItem[i].nNum = (BYTE)GET_ITEM_MIX( i ).wTurnNum;
	}

	GLItemMixMan::GetInstance().SortMeterialItem( sItemMix );

	const ITEM_MIX* pItemMix = GLItemMixMan::GetInstance().GetItemMix( sItemMix );
	if ( !pItemMix ) return UINT_MAX;

	return pItemMix->dwPrice;
}

bool GLCharacter::ValidWindowOpen()
{
	if ( ValidRebuildOpen() )					return true;	// ITEMREBUILD_MARK
	if ( ValidGarbageOpen() )					return true;	// 휴지통
	if ( ValidItemMixOpen() )					return true;	// 아이템 조합
	if ( IsOpenMiniGame() )						return true;	// 미니게임
	if ( GLTradeClient::GetInstance().Valid() )	return true;

	return false;
}
