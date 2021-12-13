#include "pch.h"

#include "./DxGameStage.h"
#include "./DxGlobalStage.h"
#include "./DxParamSet.h"

#include "./GLGaeaClient.h"
#include "./GLQUEST.h"
#include "./GLQUESTMAN.h"

#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/OuterUI/OuterInterface.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/WaitServerDialogue.h"
#include "../[Lib]__RanClientUI/Sources/ControlExUI/ModalWindow.h"

#include "../[Lib]__Engine/Sources/Common/DXInputString.h"

#include "../[Lib]__Engine/Sources/DxTools/DxInputDevice.h"
#include "../[Lib]__Engine/Sources/DxTools/DxWeatherMan.h"
#include "../[Lib]__Engine/Sources/DxTools/DxEnvironment.h"
#include "../[Lib]__Engine/Sources/DxTools/DxGlowMan.h"
#include "../[Lib]__Engine/Sources/DxTools/DxPostProcess.h"
#include "../[Lib]__Engine/Sources/DxTools/DxCubeMap.h"
#include "../[Lib]__Engine/Sources/DxTools/DxSkyMan.h"
#include "../[Lib]__Engine/Sources/DxTools/DxDynamicVB.h"	// ���� �ؾ��Ѵ�.
#include "../[Lib]__Engine/Sources/DxTools/DxSurfaceTex.h"

#include "../[Lib]__Engine/Sources/DxEffect/DxEffectMan.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffProj.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"

#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinAniControl.h"

#include "../[Lib]__MfcEx/Sources/RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxCharJoinData::ResetData ()
{
	m_bCharJoin = false;
	m_dwCharID = 0;

	m_dwClientID = 0;
	m_dwGaeaID = 0;
	m_sMapID = SNATIVEID(0,0);
	m_vPos = D3DXVECTOR3(0,0,0);

	m_dwPutOnItems = 0;
	m_dwNumInvenItem = 0;
	m_dwNumVNInvenItem = 0;
	m_dwNumSkill = 0;
	m_dwNumQuestProc = 0;
	m_dwNumQuestEnd = 0;
	m_dwChargeItem = 0;
	m_dwClubMember = 0;
	m_dwClubAlliance = 0;
	m_dwClubBattle = 0;
	m_dwItemCoolTime = 0;

	m_dwReceiveInvenItem = 0;
	m_dwReceiveVNInvenItem = 0;
	m_dwReceiveSkill = 0;
	m_dwReceiveQuestProc = 0;
	m_dwReceiveQuestEnd = 0;
	m_dwReceiveChargeItem = 0;
	m_dwReceiveClubMember = 0;
	m_dwReceiveClubAlliance = 0;
	m_dwReceiveClubBattle = 0;
	m_bReceiveClubInfo = false;
	m_dwReceiveItemCoolTime = 0;

	m_cCLUB.RESET();
	m_CharData2.Assign ( SCHARDATA2() );
}

DxGameStage::DxGameStage(void)
	: m_bUseInterface(TRUE)
{
}

DxGameStage::~DxGameStage(void)
{
}

HRESULT DxGameStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	//	Note : �������̽� �ʱ�ȭ.
	//
	CInnerInterface::GetInstance().SetResolution (MAKELONG(wHeight,wWidth));

	//CInnerInterface::GetInstance().InvalidateDeviceObjects ();
	//CInnerInterface::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	
	InvalidateDeviceObjects();
	RestoreDeviceObjects();

	return S_OK;
}

void DxGameStage::OnInitLogin ()
{
	m_CharJoinData.ResetData ();
}

void DxGameStage::SetSelectChar ( SCHARINFO_LOBBY *pCharInfo )
{
	m_CharJoinData.m_dwCharID = pCharInfo->m_dwCharID;
}

void DxGameStage::ResetCharJoinData ()
{
	m_CharJoinData.ResetData();
}

HRESULT DxGameStage::OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )
{
	GASSERT(hWnd);

	StringCchCopy ( m_szAppPath, MAX_PATH, szAppPath );
	m_hWnd = hWnd;

	GLGaeaClient::GetInstance().OneTimeSceneInit ( m_szAppPath, hWnd );

	return S_OK;
}

HRESULT DxGameStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	HRESULT hr=S_OK;
	m_pd3dDevice = pd3dDevice;

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if ( FAILED(hr) )	return hr;

	WORD wWidth = 800;
	WORD wHeight = 600;

	//	Note : �����ػ� �޾ƿ���.
	//
	LPDIRECT3DSURFACEQ pBackBuffer=NULL;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	if ( pBackBuffer )
	{
		pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
		pBackBuffer->Release();

		wWidth = static_cast<WORD> ( m_d3dsdBackBuffer.Width );
		wHeight = static_cast<WORD> ( m_d3dsdBackBuffer.Height );
	}

	RANPARAM::SETCHARID(m_CharJoinData.m_CharData2.m_szName);
	RANPARAM::LOAD_GAMEOPT ();
	DXPARAMSET::LOADDATA();

	//	Note : �������̽� �ʱ�ȭ.
	//
	CInnerInterface::GetInstance().SetResolution(MAKELONG(wHeight,wWidth));
	CInnerInterface::GetInstance().OneTimeSceneInit();

	//	Note : ����Ʈ.
	//
	DxViewPort::GetInstance().SetGameCamera ();
	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 70.0f, -70.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
	DxViewPort::GetInstance().SetActiveMap ( &GLGaeaClient::GetInstance() );

	hr = CInnerInterface::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	GASSERT(!FAILED(hr)&&"CInnerInterface::GetInstance().InitDeviceObjects");
	if ( FAILED(hr) )	return hr;


	//	Note : ���̾�Ŭ���̾�Ʈ �ʱ�ȭ.
	//
	hr = GLGaeaClient::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	GASSERT(!FAILED(hr)&&"GLGaeaClient::GetInstance().InitDeviceObjects");
	if ( FAILED(hr) )	return hr;

	//	Note : ĳ���Ͱ� ������ �� ����.
	//
	SNATIVEID sMapID = m_CharJoinData.m_sMapID;
	hr = GLGaeaClient::GetInstance().SetActiveMap ( sMapID, m_CharJoinData.m_vPos );
	GASSERT(!FAILED(hr)&&"GLGaeaClient::GetInstance().SetActiveMap");
	if ( FAILED(hr) )	return hr;

	//	Note : ĳ���͸� �ʻ� ��ġ��Ŵ.
	//
	NavigationMesh* pNavi = GLGaeaClient::GetInstance().GetActiveMap()->GetNaviMesh();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	
	pCharacter->Create ( pNavi, &m_CharJoinData.m_CharData2, &m_CharJoinData.m_vPos, m_CharJoinData.m_dwGaeaID, m_pd3dDevice );
	pCharacter->m_sCLUB = m_CharJoinData.m_cCLUB;

	CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();

#if defined( TH_PARAM ) 
	CInnerInterface::GetInstance().SetThaiCCafeClass( pCharacter->m_dwThaiCCafeClass );
#endif
	
	// �����̽þ� PC�� �̺�Ʈ
#if defined( MYE_PARAM ) || defined(MY_PARAM)
	CInnerInterface::GetInstance().SetThaiCCafeClass( pCharacter->m_nMyCCafeClass );
#endif

	//	Note : ĳ���� ��ġ�� ī�޶� ����.
	D3DXVECTOR3 vCamera = GLGaeaClient::GetInstance().GetCharacter()->GetPosition();
	vCamera.y += 10.0f;
	DxViewPort::GetInstance().CameraJump ( vCamera );

	CString strMiniMap;
	PLANDMANCLIENT pLandClient = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLandClient )
	{
		CString strText = GLGaeaClient::GetInstance().GetMapName ( pLandClient->GetMapID() );
		CInnerInterface::GetInstance().SetMiniMapInfo ( pLandClient->GetMapAxisInfo(), strText );
	}

	DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( "Q.egp" );

	//	Note : InputString Off.
	//
	DXInputString::GetInstance().OffInput();

	return S_OK;
}

HRESULT DxGameStage::RestoreDeviceObjects ()
{
	HRESULT hr;
	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

	//	Note : ����Ʈ
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 1100.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;


	hr = GLGaeaClient::GetInstance().RestoreDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	hr = CInnerInterface::GetInstance().RestoreDeviceObjects(m_pd3dDevice);
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT DxGameStage::InvalidateDeviceObjects ()
{
	HRESULT hr;
	hr = GLGaeaClient::GetInstance().InvalidateDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	hr = CInnerInterface::GetInstance().InvalidateDeviceObjects();
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT DxGameStage::DeleteDeviceObjects ()
{
	HRESULT hr;
	RANPARAM::SAVE_GAMEOPT ();
	RANPARAM::SAVE ();
	
	hr = GLGaeaClient::GetInstance().DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	hr = CInnerInterface::GetInstance().DeleteDeviceObjects();
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT DxGameStage::FinalCleanup ()
{
	HRESULT hr;

	hr = CInnerInterface::GetInstance().FinalCleanup();
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT DxGameStage::FrameMove ( float m_fTime, float m_fElapsedTime )
{
	HRESULT hr;

	//if ( DxGlobalStage::GetInstance().GetBlockDetectState() )
	//{
	//	CInnerInterface::GetInstance().SetBlockProgramFound(true);

	//	static bool bCONSOLE_TEXT(false);
	//	if ( !bCONSOLE_TEXT )
	//	{
	//		bCONSOLE_TEXT = true;
	//		CInnerInterface::GetInstance().PrintConsoleText ( ID2GAMEINTEXT("BLOCK_PROGRAM_FOUND_CONSOLE") );
	//	}
	//}

	////	Note : ���� ��ȭ ���� �׽�Ʈ.
	////
	//if ( DxInputDevice::GetInstance().GetKeyState(DIK_F6)&DXKEY_UP )
	//	DxWeatherMan::GetInstance()->SetCheckAuto	( !DxWeatherMan::GetInstance()->GetCheckAuto() );

	////	Note : �׸��� ��ȭ ���� �׽�Ʈ.
	////
	//if ( DxInputDevice::GetInstance().GetKeyState(DIK_F5)&DXKEY_UP )
	//	DxShadowMap::GetInstance().SetShadowDetail	( (EMSHADOWDETAIL) ((DxShadowMap::GetInstance().GetShadowDetail()+1)%5) );

	// float ��Ȯ�� üũ
	CheckControlfp();

	if ( DxInputDevice::GetInstance().GetKeyState(DIK_F12)&DXKEY_DOWN )
	{
		m_bUseInterface = !m_bUseInterface;
	}

	//	Note : �������̽� ������Ʈ.
	//
	PROFILE_BEGIN("CInnerInterface::FrameMove");
	if( m_bUseInterface )
	{
		CInnerInterface::GetInstance().FrameMove(m_pd3dDevice, m_fElapsedTime);
	}
	PROFILE_END("CInnerInterface::FrameMove");

	hr = GLGaeaClient::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );
	if ( FAILED(hr) )	return hr;

	//	Note : �ɸ��� �̵��� ���� ī�޶� ����.
	//
	D3DXVECTOR3 vCamera = GLGaeaClient::GetInstance().GetCharacter()->GetPosition();
	vCamera.y += 10.0f;
	DxViewPort::GetInstance().CameraJump ( vCamera );

	// Note : ȭ�� ȿ��
	DxEffProjMan::GetInstance().FrameMove( m_fElapsedTime );

	//CDebugSet::ToView ( 0, 11, "ime composition : %d", DXInputString::GetInstance().CheckComposition() );

	return S_OK;
}

HRESULT DxGameStage::Render ()
{
	HRESULT hr;
	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	//	Note : ���� �޴���
	//
	PROFILE_BEGIN2("Weather Render_Prev");
	DxWeatherMan::GetInstance()->Render_Prev ( m_pd3dDevice );
	PROFILE_END2("Weather Render_Prev");

	PROFILE_BEGIN2("Environment Render");
	DxEnvironment::GetInstance().Render ( m_pd3dDevice, NULL, GLGaeaClient::GetInstance().GetActiveMap()->GetLandMan() );
	PROFILE_END2("Environment Render");

	//	Note : ���� ����.
	//
	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, TRUE );

	//	Note : ����, ĳ����, ĳ���� �׸���, ĳ���� �ݻ�.
	PROFILE_BEGIN2("GLGaeaClient::Render");
	hr = GLGaeaClient::GetInstance().Render ( CV );
	PROFILE_END2("GLGaeaClient::Render");
	if ( FAILED(hr) )	return hr;

	//	Note : ������ ���õ� ȿ�� ����, �ϴ� ����.
	PROFILE_BEGIN2("GLGaeaClient::RenderEff");
	GLGaeaClient::GetInstance().RenderEff ( CV );
	PROFILE_END2("GLGaeaClient::RenderEff");

	//	Note : ȿ��
	PROFILE_BEGIN2("DxEffGroupPlayer::Render");
	DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
	PROFILE_END2("DxEffGroupPlayer::Render");

	// Note : ����ȭ�� Eff
	OPTMManager::GetInstance().Render( m_pd3dDevice );

	PROFILE_BEGIN2("GLGaeaClient::RenderPickAlpha");
	GLGaeaClient::GetInstance().RenderPickAlpha();
	PROFILE_END2("GLGaeaClient::RenderPickAlpha");

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	//	Note : ���� �޴���
	//
	PROFILE_BEGIN2("Weather Render");
	DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
	PROFILE_END2("Weather Render");

	//	Note : ť�� �ؽ��� ������..!!
	//
	DxCubeMap::GetInstance().Render ( m_pd3dDevice );

	//	Note : �۷ο� ó���� �ϱ����� ��� �Ѵ�.
	//
	
	//	Note : Glow 
	DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
	DxGlowMan::GetInstance().Render ( m_pd3dDevice );
	DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );

	//	Note : Wave �Ǿ� �ϴ°��� ������ ���������� �Ѹ���.
	//
	PROFILE_BEGIN2("DxEnvironment::UserToRefraction()");
	DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
	PROFILE_END2("DxEnvironment::UserToRefraction()");
	
	PROFILE_BEGIN2("DxEnvironment::RenderWave()");
	DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
	PROFILE_END2("DxEnvironment::RenderWave()");

	// Note : ��ó��
	DxPostProcess::GetInstance().Render( m_pd3dDevice );

	// Note : ȭ�� ȿ��
	DxEffProjMan::GetInstance().Render( m_pd3dDevice );

	PROFILE_BEGIN2("CInnerInterface Render");
	if( m_bUseInterface )
	{
		CInnerInterface::GetInstance().Render ( m_pd3dDevice );
	}
	PROFILE_END2("CInnerInterface Render");

	return S_OK;
}

void DxGameStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_LOBBY_CHAR_JOIN:
		{
			GLMSG::SNETLOBBY_CHARJOIN *pCharJoin = (GLMSG::SNETLOBBY_CHARJOIN *) nmg;
			if ( m_CharJoinData.m_dwCharID != pCharJoin->Data.m_dwCharID )	return;

			m_CharJoinData.m_bCharJoin = true;
			m_CharJoinData.m_dwClientID = pCharJoin->dwClientID;
			m_CharJoinData.m_dwGaeaID = pCharJoin->dwGaeaID;
			m_CharJoinData.m_sMapID = pCharJoin->sMapID;
			m_CharJoinData.m_vPos = pCharJoin->vPos;

			m_CharJoinData.m_CharData2.GETCHARDATA() = pCharJoin->Data;

			StringCchCopy ( m_CharJoinData.m_CharData2.m_szUID , USR_ID_LENGTH+1, pCharJoin->szUserID );

			m_CharJoinData.m_CharData2.m_wSKILLQUICK_ACT = pCharJoin->wSKILLQUICK_ACT;
			memcpy ( m_CharJoinData.m_CharData2.m_sSKILLQUICK, pCharJoin->sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
			memcpy ( m_CharJoinData.m_CharData2.m_sACTIONQUICK, pCharJoin->sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

			m_CharJoinData.m_dwNumInvenItem = pCharJoin->dwNumInvenItems;
#if defined(VN_PARAM) //vietnamtest%%%
			m_CharJoinData.m_dwNumVNInvenItem = pCharJoin->dwNumVNInvenItems;
#endif

			m_CharJoinData.m_dwNumSkill = pCharJoin->dwNumSkill;
			m_CharJoinData.m_dwNumQuestProc = pCharJoin->dwNumQuestProc;
			m_CharJoinData.m_dwNumQuestEnd = pCharJoin->dwNumQuestEnd;
			m_CharJoinData.m_dwChargeItem = pCharJoin->dwChargeItem;
			m_CharJoinData.m_dwItemCoolTime = pCharJoin->dwItemCoolTime;

			m_CharJoinData.m_CharData2.m_sStartMapID = pCharJoin->sStartMapID;
			m_CharJoinData.m_CharData2.m_dwStartGate = pCharJoin->dwStartGate;
			m_CharJoinData.m_CharData2.m_sLastCallMapID = pCharJoin->sLastCallMapID;
			m_CharJoinData.m_CharData2.m_vLastCallPos = pCharJoin->vLastCallPos;
			m_CharJoinData.m_CharData2.m_dwThaiCCafeClass = pCharJoin->dwThaiCCafeClass;
			m_CharJoinData.m_CharData2.m_nMyCCafeClass	  = pCharJoin->nMyCCafeClass;	// �����̽þ� PC�� �̺�Ʈ

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			m_CharJoinData.m_CharData2.m_bTracingUser  = pCharJoin->bTracingChar;
#endif

			CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_JOIN" );

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}

			DxSkinAniControl::m_bBIGHEAD = pCharJoin->bBIGHEAD;			
		}
		break;

	case NET_MSG_LOBBY_CHAR_PUTON:
		{
			GLMSG::SNETLOBBY_CHARPUTON *pCharJoin = (GLMSG::SNETLOBBY_CHARPUTON *) nmg;

			m_CharJoinData.m_dwPutOnItems = SLOT_TSIZE;
			memcpy ( m_CharJoinData.m_CharData2.m_PutOnItems, pCharJoin->PutOnItems, sizeof(SITEMCUSTOM)*SLOT_TSIZE );
		}
		break;
	case NET_MSG_LOBBY_CHAR_PUTON_EX:
		{
			GLMSG::SNETLOBBY_CHARPUTON_EX *pCharJoin = (GLMSG::SNETLOBBY_CHARPUTON_EX *) nmg;

			m_CharJoinData.m_dwPutOnItems++;
			m_CharJoinData.m_CharData2.m_PutOnItems[pCharJoin->nSlot] = pCharJoin->PutOnItem;
		}	
		break;

	case NET_MSG_LOBBY_CHAR_ITEM:
		{
			GLMSG::SNETLOBBY_INVENITEM *pNetInvenItem = (GLMSG::SNETLOBBY_INVENITEM *) nmg;
			SINVENITEM &InvenItem = pNetInvenItem->Data;

			++m_CharJoinData.m_dwReceiveInvenItem;
			GLInventory &cInventory = m_CharJoinData.m_CharData2.m_cInventory;
			BOOL bOk = cInventory.InsertItem ( InvenItem.sItemCustom, InvenItem.wPosX, InvenItem.wPosY, true );

			CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_ITEM" );
			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_LOBBY_CHAR_VIETNAM_ITEM:
		{
			GLMSG::SNETLOBBY_VIETNAM_INVENITEM *pNetInvenItem = (GLMSG::SNETLOBBY_VIETNAM_INVENITEM *) nmg;
			SINVENITEM &InvenItem = pNetInvenItem->Data;

			++m_CharJoinData.m_dwReceiveVNInvenItem;
			GLInventory &cInventory = m_CharJoinData.m_CharData2.m_cVietnamInventory;
			BOOL bOk = cInventory.InsertItem ( InvenItem.sItemCustom, InvenItem.wPosX, InvenItem.wPosY, true );

			CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_VIETNAM_ITEM" );
			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;
#endif

	case NET_MSG_LOBBY_CHAR_SKILL:
		{
			GLMSG::SNETLOBBY_CHARSKILL *pNetMsg = (GLMSG::SNETLOBBY_CHARSKILL *) nmg;
			
			m_CharJoinData.m_dwReceiveSkill += pNetMsg->wNum;
			for ( WORD i=0; i<pNetMsg->wNum; ++i )
			{
				m_CharJoinData.m_CharData2.m_ExpSkills.insert ( std::make_pair ( pNetMsg->sSKILL[i].sNativeID.dwID, pNetMsg->sSKILL[i] ) );
			}

			CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_SKILL" );
			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_QUEST_END:
		{
			GLMSG::SNETLOBBY_QUEST_END *pNetMsg = (GLMSG::SNETLOBBY_QUEST_END *) nmg;

			//	Note : ���� ����.
			//
			CByteStream sByteStream;
			sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
			
			//	Note : ����Ʈ ����.
			//
			GLQUESTPROG sQUEST_PROG_END;
			sQUEST_PROG_END.GET_BYBUFFER ( sByteStream );

			//	Note : ����Ʈ�� ���.
			//
			
			GLQuestPlay::MAPQUEST& mapQUEST = m_CharJoinData.m_CharData2.m_cQuestPlay.GetQuestEnd();
			if ( m_CharJoinData.m_CharData2.m_cQuestPlay.InsertEnd ( sQUEST_PROG_END ) )
			{
				// ���� �Ҷ� ����
				++m_CharJoinData.m_dwReceiveQuestEnd;
			}

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_QUEST_PROG:
		{
			GLMSG::SNETLOBBY_QUEST_PROG *pNetMsg = (GLMSG::SNETLOBBY_QUEST_PROG *) nmg;
		
			CByteStream sByteStream;
			sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
			
			//	Note : ����Ʈ ����.
			//
			GLQUESTPROG sQUEST_PROG_NEW;
			sQUEST_PROG_NEW.GET_BYBUFFER ( sByteStream );

			//	Note : ����Ʈ�� ���.
			//
			
			GLQuestPlay::MAPQUEST& mapQUEST = m_CharJoinData.m_CharData2.m_cQuestPlay.GetQuestProc();
			if ( m_CharJoinData.m_CharData2.m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW ) )
			{
				++m_CharJoinData.m_dwReceiveQuestProc;
			}

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_CHARGE_ITEM:
		{
			GLMSG::SNETLOBBY_CHARGE_ITEM *pNetMsg = (GLMSG::SNETLOBBY_CHARGE_ITEM *) nmg;

			++m_CharJoinData.m_dwReceiveChargeItem;
			m_CharJoinData.m_CharData2.ADDSHOPPURCHASE ( pNetMsg->szPurKey, pNetMsg->nidITEM );

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_ITEM_COOLTIME:
		{
			GLMSG::SNETLOBBY_ITEM_COOLTIME *pNetMsg = (GLMSG::SNETLOBBY_ITEM_COOLTIME *) nmg;
			
			m_CharJoinData.m_dwReceiveItemCoolTime += pNetMsg->dwNum;
			for ( WORD i=0; i<pNetMsg->dwNum; ++i )
			{
				ITEM_COOLTIME& sCoolTime = pNetMsg->sItemCoolTime[i];
				if ( pNetMsg->emCoolType == EMCOOL_ITEMID )
				{
					m_CharJoinData.m_CharData2.m_mapCoolTimeID[sCoolTime.dwCoolID] = sCoolTime;
				}
				else if ( pNetMsg->emCoolType == EMCOOL_ITEMTYPE )
				{
					m_CharJoinData.m_CharData2.m_mapCoolTimeType[sCoolTime.dwCoolID] = sCoolTime;
				}
			}

			CDebugSet::ToListView ( "NET_MSG_LOBBY_ITEM_COOLTIME" );
			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_CLUB_INFO:
		{
			GLMSG::SNETLOBBY_CLUB_INFO *pNetMsg = (GLMSG::SNETLOBBY_CLUB_INFO *) nmg;
			m_CharJoinData.m_cCLUB.m_dwID = pNetMsg->dwClubID;
			StringCchCopy ( m_CharJoinData.m_cCLUB.m_szName, CHAR_SZNAME, pNetMsg->szClubName );

			m_CharJoinData.m_cCLUB.m_dwMasterID = pNetMsg->dwMasterID;
			StringCchCopy ( m_CharJoinData.m_cCLUB.m_szMasterName, CHAR_SZNAME, pNetMsg->szMasterName );
			m_CharJoinData.m_cCLUB.m_dwCDCertifior = pNetMsg->dwCDCertifior;

			m_CharJoinData.m_cCLUB.m_dwRank = pNetMsg->dwRank;
			m_CharJoinData.m_cCLUB.m_dwMarkVER = pNetMsg->dwMarkVER;

			m_CharJoinData.m_cCLUB.m_tOrganize = pNetMsg->tOrganize;
			m_CharJoinData.m_cCLUB.m_tDissolution = pNetMsg->tDissolution;

			StringCchCopy ( m_CharJoinData.m_cCLUB.m_szNotice, EMCLUB_NOTICE_LEN+1, pNetMsg->szNotice );

			m_CharJoinData.m_cCLUB.m_dwBattleWin = pNetMsg->dwBattleWin;
			m_CharJoinData.m_cCLUB.m_dwBattleLose = pNetMsg->dwBattleLose;
			m_CharJoinData.m_cCLUB.m_dwBattleDraw = pNetMsg->dwBattleDraw;

			m_CharJoinData.m_cCLUB.m_dwAllianceBattleWin = pNetMsg->dwAllianceBattleWin;
			m_CharJoinData.m_cCLUB.m_dwAllianceBattleLose = pNetMsg->dwAllianceBattleLose;
			m_CharJoinData.m_cCLUB.m_dwAllianceBattleDraw = pNetMsg->dwAllianceBattleDraw;

			//	Note : Ŭ�� ����� ����.
			m_CharJoinData.m_bReceiveClubInfo = true;
			m_CharJoinData.m_dwClubMember = pNetMsg->dwMEMBER_NUM;
			m_CharJoinData.m_dwClubAlliance = pNetMsg->dwALLIANCE_NUM;
			m_CharJoinData.m_dwClubBattle = pNetMsg->dwBATTLE_NUM;

			m_CharJoinData.m_cCLUB.m_dwAlliance = pNetMsg->dwAlliance;

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_CLUB_MEMBER:
		{
			GLMSG::SNETLOBBY_CLUB_MEMBER *pNetMsg = (GLMSG::SNETLOBBY_CLUB_MEMBER *) nmg;

			m_CharJoinData.m_dwReceiveClubMember += pNetMsg->dwMemberNum;
			for ( DWORD i=0; i<pNetMsg->dwMemberNum; ++i )
			{
				m_CharJoinData.m_cCLUB.AddMember ( pNetMsg->sMEMBER[i].dwID, pNetMsg->sMEMBER[i].szMember, pNetMsg->sMEMBER[i].dwFlag ); 
			}

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_CLUB_ALLIANCE:
		{
			GLMSG::SNETLOBY_CLUB_ANCE_2CLT *pNetMsg = (GLMSG::SNETLOBY_CLUB_ANCE_2CLT *) nmg;

			m_CharJoinData.m_dwReceiveClubAlliance += pNetMsg->dwAllianceNum;
			for ( DWORD i=0; i<pNetMsg->dwAllianceNum; ++i )
			{
				m_CharJoinData.m_cCLUB.AddAlliance ( pNetMsg->sALLIANCE[i].m_dwID, pNetMsg->sALLIANCE[i].m_szName ); 
			}

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_LOBBY_CLUB_BATTLE:
		{
			GLMSG::SNETLOBBY_CLUB_BATTLE *pNetMsg = (GLMSG::SNETLOBBY_CLUB_BATTLE *) nmg;

			m_CharJoinData.m_dwReceiveClubBattle += pNetMsg->dwBattleNum;
			GLCLUBBATTLE sBattle;

			for ( DWORD i=0; i<pNetMsg->dwBattleNum; ++i )
			{
				sBattle = pNetMsg->sBATTLE[i];
				m_CharJoinData.m_cCLUB.ADDBATTLECLUB ( sBattle ); 
			}

			if ( m_CharJoinData.CheckReceive() )
			{
				NET_MSG_GENERIC nmg = { sizeof(NET_MSG_GENERIC), NET_MSG_LOBBY_GAME_COMPLETE };
				COuterInterface::GetInstance().MsgProcess ( &nmg );
			}
		}
		break;

	case NET_MSG_CHAT_FB:				// ä�ø޽���
	case NET_MSG_CHAT_CTRL_FB:			// �����ڿ� ä�ø޽���
	case NET_MSG_SERVER_GENERALCHAT:
	case NET_MSG_CHAT_PRIVATE_FAIL:
	case NET_MSG_CHAT_BLOCK:
	case NET_MSG_APEX_ANSWER:			// APEX �޼���
	case NET_MSG_GCTRL_REQ_TAKE_FB:
	case NET_MSG_GCTRL_DROP_OUT_FORCED:
	case NET_MSG_GCTRL_PARTY_LURE_TAR:	//	��Ƽ ���� ��û ���� ���� �ʿ�.
	case NET_MSG_GCTRL_TRADE_TAR:
	case NET_MSG_GCTRL_CONFRONT_TAR:
	case NET_MSG_EVENT_LOTTERY_FB:		//	����
		if ( DxGlobalStage::GetInstance().GetActiveStage() == DxGlobalStage::GetInstance().GetGameStage() )
		{
			CInnerInterface::GetInstance().MsgProcess ( nmg );
		}
		break;

	default:
		CDebugSet::ToListView ( "DxGameStage::MsgProcess() �з����� ���� �޽��� ����. TYPE[%d]", nmg->nType );
		break;
	};
}