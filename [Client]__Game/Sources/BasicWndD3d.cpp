#include "pch.h"

#include "SubPath.h"
#include "Dxviewport.h"
#include "dxincommand.h"
#include "DxInputDevice.h"
#include "DxEffectMan.h"

#include "DxSurfaceTex.h"
#include "DxGlowMan.h"
#include "DxPostProcess.h"

#include "GLPeriod.h"
#include "DxResponseMan.h"
#include "DxGlobalStage.h"
#include "s_NetClient.h"
#include "DxShadowMap.h"
#include "dxparamset.h"
#include "RANPARAM.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "NsSMeshSceneGraph.h"

#include "Basic.h"
#include "BasicWnd.h"

#include "D3DFont.h"
#include "BlockProg.h"

#include "CommonWeb.h"
#include "DxMeshTexMan.h"

#include "../[Lib]__RanClientUI/Sources/LoaderUI/LoadingThread.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/WaitServerDialogue.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/Cursor.h"

#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include ".\basicwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NPGAMEMON_CHECK_ERROR	2000
#define NPGAMEMON_TIMER			1
#define NPGAMEMON_AUTH_TIMER	2
#define NPGAMEMON_AUTH12_TIMER	3

//extern CBLOCK_PROG	g_BLOCK_PROG;
extern HWND g_hWnd;

typedef std::queue<UINT> UINT_QUEUE;
UINT_QUEUE g_AuthMsgBuffer;

#if defined(MY_PARAM) || defined(PH_PARAM) || defined(MYE_PARAM) //|| defined ( VN_PARAM ) || defined(TH_PARAM) || defined ( HK_PARAM ) || defined(TW_PARAM) // Nprotect 정의
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	TCHAR msg[128]={0};
	LPCTSTR lpszMsg( NULL );
	bool bAppExit( false );
	static DWORD dwCharIDBack = 0;

	switch( dwMsg )
	{
	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
		bAppExit = true;
		return false;
	case NPGAMEMON_INIT_ERROR:
		bAppExit = true; // Closing code
		_stprintf(msg, ID2GAMEEXTEXT( "NPGAMEMON_INIT_ERROR" ), dwArg);
		lpszMsg = msg;
		break;
	case NPGAMEMON_SPEEDHACK:
		bAppExit = true; // termination code
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_SPEEDHACK" );
		break;
	case NPGAMEMON_GAMEHACK_KILLED:
		bAppExit = true; // termination code, close or proceed continuously
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_GAMEHACK_KILLED" );
		break;
	case NPGAMEMON_GAMEHACK_DETECT:
		bAppExit = true; // termination code
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_GAMEHACK_DETECT" );
		break;
	case NPGAMEMON_GAMEHACK_DOUBT:
		bAppExit = true; // termination code
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_GAMEHACK_DOUBT" );
		break;
	case NPGAMEMON_CHECK_CSAUTH2:
		{

			
			// charid가 변경될때마다 ID 전송
			DWORD dwCHARID = GLGaeaClient::GetInstance().GetCharacter()->m_dwCharID;

			if ( dwCharIDBack != dwCHARID && dwCHARID != 0 )
			{
				CString strUserID;
				strUserID.Format( "CHAR_ID:%u", dwCHARID );
				npgl.Send( strUserID.GetString() );
			}	

			if( g_AuthMsgBuffer.empty() ) break;
			UINT AuthMsg = g_AuthMsgBuffer.front();
			g_AuthMsgBuffer.pop();

			if( AuthMsg == NET_MSG_GAMEGUARD_AUTH )
			{
				NET_GAMEGUARD_ANSWER NetMsg;
				memmove( &NetMsg.ggad, (GG_AUTH_DATA*)dwArg, sizeof(GG_AUTH_DATA) );

				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
				if ( pNetClient && pNetClient->IsOnline () )
				{
					const TCHAR* szEncrypt;
					szEncrypt = pNetClient->DoEncryptKey();					
					memcpy( NetMsg.szEnCrypt, szEncrypt, ENCRYPT_KEY+1);
				}

				DxGlobalStage::GetInstance().NetSend( (NET_MSG_GENERIC*)&NetMsg );
			}	
			else if( AuthMsg == NET_MSG_GAMEGUARD_AUTH_1 )
			{
				NET_GAMEGUARD_ANSWER_1 NetMsg;
				memmove( &NetMsg.ggad, (GG_AUTH_DATA*)dwArg, sizeof(GG_AUTH_DATA) );
				DxGlobalStage::GetInstance().NetSend( (NET_MSG_GENERIC*)&NetMsg );
			}
			else if( AuthMsg == NET_MSG_GAMEGUARD_AUTH_2 )
			{
				NET_GAMEGUARD_ANSWER_2 NetMsg;
				memmove( &NetMsg.ggad, (GG_AUTH_DATA*)dwArg, sizeof(GG_AUTH_DATA) );
				DxGlobalStage::GetInstance().NetSend( (NET_MSG_GENERIC*)&NetMsg );
			}
		}
		break;
	}

	if( bAppExit ) // 게임 종료시에만 false를 리턴
	{
		DWORD dwCHARID = GLGaeaClient::GetInstance().GetCharacter()->m_dwCharID;

		{ // Note : 란서버에 로그 기록
			GLMSG::SNET_BLOCK_DETECTED NetMsg;
			NetMsg.dwDETECTED = dwMsg; // NP 리턴 번호
			NetMsg.dwCHARID = dwCHARID;
			StringCchCopy( NetMsg.szINFO, GLMSG::SNET_BLOCK_DETECTED::MAX_INFO, npgl.GetInfo() );
			DxGlobalStage::GetInstance().NetSend( (NET_MSG_GENERIC*)&NetMsg );
		}

		{ // Note : NP서버에 로그 기록
			CString strUserID;
			strUserID.Format( "CHAR_ID:%u", dwCHARID );
			npgl.Send( strUserID.GetString() );
		}	

		// Note : 프로그램을 종료한다.
		//CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_OPEN(ID2GAMEINTEXT("WAITSERVER_HACK_MESSAGE"), WAITSERVER_CLOSEGAME);
		DxGlobalStage::GetInstance().CloseGame( lpszMsg );
		return false;
	}

	return true;
}
#endif

HRESULT CBasicWnd::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicWnd::Resize3DEnvironment()
{
	HRESULT hr=S_OK;
	hr = CD3DApplication::Resize3DEnvironment();
	if ( FAILED(hr) ) return hr;

	DxGlobalStage::GetInstance().ReSizeWindow ( m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight );

	return S_OK;
}

void CBasicWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CRect rectWnd;
	GetClientRect ( &rectWnd );

	WORD wWidth = rectWnd.Width();
	WORD wHeight = rectWnd.Height();

	//	Note : 현재해상도 받아오기.
	//
	if ( !RANPARAM::bScrWindowed && GetD3dDevice() )
	{
		LPDIRECT3DSURFACEQ pBackBuffer=NULL;
		GetD3dDevice()->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

		if ( pBackBuffer )
		{
			pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
			pBackBuffer->Release();

			wWidth = static_cast<WORD> ( m_d3dsdBackBuffer.Width );
			wHeight = static_cast<WORD> ( m_d3dsdBackBuffer.Height );
		}
	}

	// TODO: Add your message handler code here
	if ( m_bCreated )	ReSizeWindow ( wWidth, wHeight );
}

HRESULT CBasicWnd::OneTimeSceneInit()
{
	HRESULT hr(S_OK);
	CBasicApp *pFrame = (CBasicApp *) AfxGetApp();

	//	Note : 각종 장치들.
	//
	hr = DxResponseMan::GetInstance().OneTimeSceneInit ( pFrame->m_szAppPath, this,
		RANPARAM::strFontType, RANPARAM::dwLangSet, RANPARAM::strGDIFont );

	if ( FAILED(hr) )
	{
		return  E_FAIL;
	}

	dxincommand::bDISP_CONSOLE = RANPARAM::bDISP_CONSOLE;
	dxincommand::bDISP_FPS = RANPARAM::bDISP_FPS;

	// 이 함수 불러주면 HelpWindow에 WebBrowser 설치 준비함 ( 준혁 )
	if( m_bEnableGUI )
		CCommonWeb::Get()->Create( (CWnd*)this, &m_bVisibleGUI, &m_rtBoundGUI );

	return S_OK;
}

HRESULT CBasicWnd::CreateObjects()
{
	{ // Note : 로딩시 텍스트를 출력하기 위해서 폰트 부분을 초기화.
		
		//	Note : 폰트 메니져 초기화.
		DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
		CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

		//	Note	:	디버그셋 경로 설정 및 초기화
		CDebugSet::InitDeviceObjects( pD3dFont9 );
	}

	HRESULT hr = S_OK;

	hr = NLOADINGTHREAD::StartThreadLOAD ( &m_pd3dDevice, m_hWnd, ((CBasicApp*)AfxGetApp())->m_szAppPath, CString("loading_001.tga"), true );
	if( FAILED(hr) )	return E_FAIL;
	
	//	텍스쳐 로드 완료시까지 대기
	NLOADINGTHREAD::WaitThread ();

	hr = InitDeviceObjects ();
	if( FAILED(hr) )
	{
		NLOADINGTHREAD::EndThread ();
		return hr;
	}

	//	Note	:	로더 스레드 종료
	NLOADINGTHREAD::EndThread ();	
	
	hr = RestoreDeviceObjects();	
	if( FAILED(hr) )	return hr;

#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) //|| defined ( VN_PARAM ) || defined(TH_PARAM) || defined ( HK_PARAM ) || defined(TW_PARAM) // Nprotect 정의
	npgl.SetHwnd( m_hWnd );
	m_nGGTimer = SetTimer(NPGAMEMON_TIMER, 10000, 0);
	m_nGGATimer = SetTimer(NPGAMEMON_AUTH_TIMER, 60000, 0);
	m_nGGA12Timer = SetTimer(NPGAMEMON_AUTH12_TIMER, 33, 0);
#endif

	return hr;
}

HRESULT CBasicWnd::InitDeviceObjects()
{	
	HRESULT hr = S_OK;
	CBasicApp *pFrame = (CBasicApp *) AfxGetApp();

	//	Note : 뷰포트.
	//
	DxViewPort::GetInstance().InitDeviceObjects ( m_pd3dDevice, m_hWnd );

	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().InitDeviceObjects ( m_pd3dDevice, TRUE );	

	DXLIGHT sDirectional;
	sDirectional.SetDefault();
	sDirectional.m_Light.Diffuse = D3DXCOLOR(0,0,0,1);
	sDirectional.m_Light.Ambient = D3DXCOLOR(0,0,0,1);
	DxLightMan::SetDefDirect ( sDirectional );

	//	Note : 게임 스테이지.
	//
	WORD wWidth = m_wndSizeX;
	WORD wHeight = m_wndSizeY;

	DxGlobalStage::GetInstance().SetD3DApp(this);
	hr = DxGlobalStage::GetInstance().OneTimeSceneInit ( pFrame->m_szAppPath, m_hWndApp, FALSE, wWidth, wHeight );
	if ( FAILED(hr) )	return hr;

	//	Note : 게임 스테이지.
	//
	hr = DxGlobalStage::GetInstance().InitDeviceObjects ( m_pd3dDevice );	
	if ( FAILED(hr) )	return hr;

	// Note : DynamicLoad를 사용할 것을 명한다.
	NSOCTREE::EnableDynamicLoad();

	LOADINGSTEP::SETSTEP ( 11 );

	//	Note : 커서 초기화.
	//
	char szFullPath[MAX_PATH] = {0};
	StringCchCopy ( szFullPath, MAX_PATH, pFrame->m_szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_EDIT );

	CCursor::GetInstance().SetGameCursor ( RANPARAM::bGameCursor );
	CCursor::GetInstance().OneTimeSceneInit(szFullPath,m_hWnd);
	CCursor::GetInstance().InitDeviceObjects ();

	return S_OK;
}

HRESULT CBasicWnd::RestoreDeviceObjects()
{
	HRESULT hr=S_OK;

	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().RestoreDeviceObjects ();

	//	Note : 게임 스테이지.
	//
	DxGlobalStage::GetInstance().RestoreDeviceObjects ();

	//	Note : 글로우 처리를 하기위해 사용 한다.
	DxGlowMan::GetInstance().SetProjectActiveON();
	DxPostProcess::GetInstance().SetProjectActiveON();

	DXPARAMSET::INIT ();
	
	//	DEBUG : 프로파일링 초기화.
	PROFILE_INIT();
	PROFILE_INIT2();

	return S_OK;
}

HRESULT CBasicWnd::FrameMove3DEnvironment ()
{
	HRESULT hr=S_OK;

	if ( m_bCreated )
	{
		hr = CD3DApplication::FrameMove3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	return S_OK;
}

HRESULT CBasicWnd::Render3DEnvironment ()
{
	HRESULT hr=S_OK;
	
	if ( m_bCreated )
	{
		hr = CD3DApplication::Render3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT CBasicWnd::FrameMove()
{
	PROFILE_BLOCKSTART();

	PROFILE_BEGIN("FrameMove");

	//if ( g_BLOCK_PROG.IsDETECT() )
	//{
	//	if ( DxGlobalStage::GetInstance().GetActiveStage() == DxGlobalStage::GetInstance().GetGameStage() )
	//	{
	//		DWORD dwCHARID = GLGaeaClient::GetInstance().GetCharacter()->m_dwCharID;

	//		CBLOCK_PROG::SDETECT sDETECT;
	//		do
	//		{
	//			sDETECT = g_BLOCK_PROG.NextDETECT();
	//			if ( sDETECT.dwID==0 )	break;

	//			GLMSG::SNET_BLOCK_DETECTED NetMsg;
	//			NetMsg.dwDETECTED = sDETECT.dwID;
	//			NetMsg.dwCHARID = dwCHARID;
	//			StringCchCopy(NetMsg.szINFO,GLMSG::SNET_BLOCK_DETECTED::MAX_INFO,sDETECT.szINFO);
	//			DxGlobalStage::GetInstance().NetSend ( (NET_MSG_GENERIC*)&NetMsg );

	//			//CDebugSet::ToListView ( "detect process : %d, %s", sDETECT.dwID, sDETECT.szINFO );

	//			// 프로그램을 종료한다.
	//			CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_OPEN(ID2GAMEINTEXT("WAITSERVER_HACK_MESSAGE"), WAITSERVER_CLOSEGAME);
	//		}
	//		while ( sDETECT.dwID!=0 );
	//	}
	//}

	//	Note : 게임 스테이지.
	//
	DxGlobalStage::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );

	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().FrameMove ( m_fTime, m_fElapsedTime, m_bDefWin, TRUE );

	//	Note : 뷰포트
	//
	DxViewPort::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );

	PROFILE_END("FrameMove");

	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	//	Note : 풀스크린/창모드 전환.
	DWORD dwL_ALT = DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU );
	DWORD dwRETURN = DxInputDevice::GetInstance().GetKeyState ( DIK_RETURN );
	if ( (dwL_ALT&DXKEY_PRESSED) && (dwRETURN&DXKEY_UP) )	
	{
		if ( SUCCEEDED( ToggleFullscreen() ) )
		{
			RANPARAM::bScrWindowed = !RANPARAM::bScrWindowed;
			DXPARAMSET::GetInstance().m_bScrWindowed = !DXPARAMSET::GetInstance().m_bScrWindowed;
		}
	}

	return S_OK;
}

HRESULT CBasicWnd::Render()
{
	HRESULT hr(S_OK);
	if ( !m_pd3dDevice )	return S_FALSE;

	PROFILE_BLOCKSTART2();

	PROFILE_BEGIN2("Render");

	//	Fog 셋팅
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice );
	colorClear = DxFogMan::GetInstance().GetFogColor();

	// Clear the viewport
	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
						colorClear, 1.0f, 0L );

	// Begin the scene
	if( SUCCEEDED(hr=m_pd3dDevice->BeginScene()) )
	{
//		//	Note : Glow 
//		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
		//	Note : Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice );

		//	Note : 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

		//	Note : 포인트 라이트 VertexShaderConstant
		//
		D3DLIGHTQ	pLight;
		D3DXVECTOR4	vPointPos;
		D3DXVECTOR3	vPointDiff;
		for ( int i=0; i<7; i++ )
		{
			if ( DxLightMan::GetInstance()->GetClosedLight(i+1) )
			{
				pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
				vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
				vPointPos.x = pLight.Position.x;
				vPointPos.y = pLight.Position.y;
				vPointPos.z = pLight.Position.z;
				vPointPos.w = pLight.Range;
			}
			else
			{
				vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
				vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
		}

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );		// 강제 설정

		//	Note : 게임 스테이지.
		//
		PROFILE_BEGIN2("Render_DxGlobalStage");
		DxGlobalStage::GetInstance().Render ();
		PROFILE_END2("Render_DxGlobalStage");

		//// Cursor
		//DxCursor::GetInstance().Render( m_pd3dDevice );

		//NSCHARSG::Render( m_pd3dDevice );

		//	Note : 커서 그리기.
		//
		CCursor::GetInstance().Render ( m_pd3dDevice, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );

		PROFILE_END2("Render");
		PROFILE_BLOCKEND2();
		PROFILE_DUMPOUTPUT2();

		RenderText();

		// End the scene.
		m_pd3dDevice->EndScene();

		return S_OK;
	}
	else
	{
		//	BeginScene() 이 실패하는 경우는 EndScene() 이 호출되지 않아서 생기는 문제이다.
		//CDebugSet::ToLogFile ( "BeginScene() fail %d", hr );
		m_pd3dDevice->EndScene();
	}

	PROFILE_END2("Render");
	PROFILE_BLOCKEND2();
	PROFILE_DUMPOUTPUT2();

	return S_OK;
}

HRESULT CBasicWnd::RenderText()
{
	if ( !dxincommand::bDISP_CONSOLE && !dxincommand::bDISP_FPS )	return S_OK;

    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	// Output display stats
	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );
	if ( !pD3dFont )	return S_OK;

	if ( dxincommand::bDISP_FPS )
	{
		// Output display stats
		FLOAT fNextLine = 40.0f; 

		StringCchCopy ( szMsg, MAX_PATH, m_strDeviceStats );
		fNextLine -= 20.0f;
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

		StringCchCopy( szMsg, MAX_PATH, m_strFrameStats );
		fNextLine -= 20.0f;
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );
	}

	if ( dxincommand::bDISP_CONSOLE )
	{
		CDebugSet::Render ();
	}

    return S_OK;
}

HRESULT CBasicWnd::InvalidateDeviceObjects()
{
	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().InvalidateDeviceObjects ();

	//	Note : 게임 스테이지.
	//
	DxGlobalStage::GetInstance().InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT CBasicWnd::DeleteDeviceObjects()
{
	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().DeleteDeviceObjects ();

	//	Note : 게임 스테이지.
	//
	DxGlobalStage::GetInstance().DeleteDeviceObjects ();

	//	Note : 마우스 커서.
	//
	CCursor::GetInstance().DeleteDeviceObjects ();

	return S_OK;
}

HRESULT CBasicWnd::FinalCleanup()
{
	//	Note : 게임 스테이지.
	//
	DxGlobalStage::GetInstance().FinalCleanup ();

	//	Note : 각종 장치들.
	//
	DxResponseMan::GetInstance().FinalCleanup ();

	return S_OK;
}

void CBasicWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nID==SC_KEYMENU)	return;

	__super::OnSysCommand(nID, lParam);
}

BOOL CBasicWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;

	DxGlobalStage::GetInstance().MsgProc ( &msg );

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}


void CBasicWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// 윈도우 모드나 풀스크린 모드일때 최소 해상도(윈도우포함)를 800*600으로 고정
	// 따라서 윈도우 모드일때는 실제 클라이언트의 크기는 800*600보다 작을 수 있다.
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 600;

	__super::OnGetMinMaxInfo(lpMMI);
}

BOOL CBasicWnd::OnNcActivate(BOOL bActive)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DxInputDevice::GetInstance().OnActivate ( bActive );
	// 볼륨 복원을 위해
	DxBgmSound::GetInstance().SetActivate ( bActive );

	return __super::OnNcActivate(bActive);
}

void CBasicWnd::OnTimer(UINT nIDEvent)
{
#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) //|| defined ( VN_PARAM ) || defined(TH_PARAM) || defined ( HK_PARAM ) || defined(TW_PARAM) // Nprotect 정의
	switch( nIDEvent )
	{
	case NPGAMEMON_TIMER:
		{
			DWORD dwResult = npgl.Check();
			if( dwResult != NPGAMEMON_SUCCESS )
			{
				DWORD dwCHARID = GLGaeaClient::GetInstance().GetCharacter()->m_dwCharID;

				{ // Note : 란서버에 로그 기록
					GLMSG::SNET_BLOCK_DETECTED NetMsg;
					NetMsg.dwDETECTED = NPGAMEMON_CHECK_ERROR;
					NetMsg.dwCHARID = dwCHARID;
					StringCchCopy( NetMsg.szINFO, GLMSG::SNET_BLOCK_DETECTED::MAX_INFO, npgl.GetInfo() );
					DxGlobalStage::GetInstance().NetSend( (NET_MSG_GENERIC*)&NetMsg );
				}

				{ // Note : NP서버에 로그 기록
					CString strUserID;
					strUserID.Format( "CHAR_ID:%u", dwCHARID );
					npgl.Send( strUserID.GetString() );
				}

				TCHAR msg[128]={0};
				_stprintf( msg , "GameGuard check error." );

				// Note : 프로그램을 종료한다.
				DxGlobalStage::GetInstance().CloseGame( msg );
			}
		}
		break;

	case NPGAMEMON_AUTH_TIMER:
		{
			GG_AUTH_DATA gad;
			UINT AuthMsg = DxGlobalStage::GetInstance().GetGGAuthData( &gad );
			if( AuthMsg )
			{
				g_AuthMsgBuffer.push( AuthMsg );
				npgl.Auth2( &gad );
			}
		}
		break;

	case NPGAMEMON_AUTH12_TIMER:
		{
			GG_AUTH_DATA gad;
			UINT AuthMsg = DxGlobalStage::GetInstance().GetGGAuthData( &gad );
			if( AuthMsg )
			{
				g_AuthMsgBuffer.push( AuthMsg );
				npgl.Auth2( &gad );
			}
		}
		break;
	}
#endif

	__super::OnTimer(nIDEvent);
}

void CBasicWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 렌더링 제어
	m_pApp->SetActive ( !bMinimized );
}
