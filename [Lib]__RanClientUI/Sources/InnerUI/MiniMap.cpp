#include "pch.h"
#include "minimap.h"
#include "../[Lib]__Engine/Sources/DxTools/TextureManager.h"
#include "StringFile.h"
#include "STRINGUTILS.h"
#include "DXLandMan.h"
#include "GLGaeaClient.h"
#include "DxRenderStates.h"
#include "DxViewPort.h"
#include "../[Lib]__EngineUI/Sources/InterfaceCfg.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GLMapAxisInfo.h"
#include "GameTextControl.h"
#include "d3dfont.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#include "RANPARAM.h"
#include "DxGlobalStage.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GLPeriod.h"
#include "GLItemMan.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "DxClubMan.h"
#include "InnerInterface.h"

#include "MiniMapInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CMiniMap::nTIME_MINUTE = 60;

CMiniMap::CMiniMap()
	: m_nMap_X(1)
	, m_nMap_Y(1)
	, m_nMapSize(1)
	, m_nMin_X(1)
	, m_nMin_Y(1)

	, m_bTEST( true )
	, m_wAmmoCount( 0 )

	, m_pMimiMapInfo(NULL)
	, m_pMiniMapInfoDummy(NULL)
	, m_pMiniMapInfoLeftDummy(NULL)
	, m_pMap(NULL)
	, m_pUserMark(NULL)
	, m_pXPos(NULL)
	, m_pYPos(NULL)
	, m_pMiniMapName(NULL)
	, m_pZoomOut(NULL)
	, m_pZoomIn(NULL)
	, m_pTime(NULL)
	, m_pTimeText(NULL)
	, m_pServerTimeText(NULL)
	, m_pDirection(NULL)
	, m_pArrowCountBack(NULL)
	, m_pArrowCountText(NULL)
	, m_pPK_ATTACK(NULL)
	, m_pPK_DEFENSE(NULL)
	, m_pFullScreenButton(NULL)
	, m_pFullScreenButtonDummy(NULL)
	, m_pAmmoText(NULL)
	, m_pCenterPoint(NULL)
	, m_pClubTimeText(NULL)
{
}

CMiniMap::~CMiniMap(void)
{
}

void CMiniMap::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pTimeText = new CBasicTextBox;
	pTimeText->CreateSub ( this, "MINIMAP_TIME_TEXT" );
	pTimeText->SetFont ( pFont );
	pTimeText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pTimeText );
	m_pTimeText = pTimeText;
	
	
	CBasicTextBox* pServerTimeText = new CBasicTextBox;
	pServerTimeText->CreateSub ( this, "MINIMAP_SERVER_TIME_TEXT" );
	pServerTimeText->SetFont ( pFont );
	pServerTimeText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	//{
	//	pServerTimeText->SetUseRender( TRUE );
	//	pServerTimeText->SetDiffuse( NS_UIDEBUGSET::PINK );
	//}
	RegisterControl ( pServerTimeText );
	m_pServerTimeText = pServerTimeText;
	

	CBasicTextBox* pAmmoText = new CBasicTextBox;
	pAmmoText->CreateSub ( this, "MINIMAP_AMMO_TEXT" );
	pAmmoText->SetFont ( pFont );
	pAmmoText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pAmmoText );
	m_pAmmoText = pAmmoText;

	CBasicTextBox* pClubTimeText = new CBasicTextBox;
	pClubTimeText->CreateSub ( this, "MINIMAP_CLUB_TIME_TEXT" );
	pClubTimeText->SetFont ( pFont );
	pClubTimeText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pClubTimeText );
	m_pClubTimeText = pClubTimeText;

	CUIControl* pBack = new CUIControl;
	pBack->CreateSub ( this, "MINIMAP_BACK", UI_FLAG_RIGHT );
	RegisterControl ( pBack );		

	CUIControl* pDirection = new CUIControl;
	pDirection->CreateSub ( this, "MINIMAP_DIRECTION", UI_FLAG_RIGHT );
	RegisterControl ( pDirection );
	m_pDirection = pDirection;

	CUIControl* pCenterPoint = new CUIControl;
	pCenterPoint->CreateSub ( this, "MINIMAP_CENTER_POINT", UI_FLAG_RIGHT );
	RegisterControl ( pCenterPoint );
	m_pCenterPoint = pCenterPoint;

	CUIControl* pFullScreen = new CUIControl;
	pFullScreen->CreateSub ( this, "MINIMAP_FULLSCREEN", UI_FLAG_RIGHT, MINIMAP_FULLSCREEN );
	RegisterControl ( pFullScreen );
	m_pFullScreenButton = pFullScreen;

	CUIControl* pFullScreenDummy = new CUIControl;
	pFullScreenDummy->CreateSub ( this, "MINIMAP_FULLSCREEN", UI_FLAG_RIGHT );
	pFullScreenDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pFullScreenDummy );
	m_pFullScreenButtonDummy = pFullScreenDummy;

	/////////////// 맴정보 컨트롤 생성 ////////////////
	CMiniMapInfo* pMiniMapInfo = new CMiniMapInfo;
	pMiniMapInfo->CreateSub( this, "MINIMAP_INFO" );
	pMiniMapInfo->CreateSubControl();
	pMiniMapInfo->SetVisibleSingle( TRUE );
	RegisterControl( pMiniMapInfo );
	m_pMimiMapInfo = pMiniMapInfo;

	CMiniMapInfo* pMiniMapInfoDummy = new CMiniMapInfo;
	pMiniMapInfoDummy->CreateSub ( this, "MINIMAP_INFO" );
	pMiniMapInfoDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pMiniMapInfoDummy );
	m_pMiniMapInfoDummy = pMiniMapInfoDummy;

	CMiniMapInfo* pMiniMapInfoLeftDummy = new CMiniMapInfo;
	pMiniMapInfoLeftDummy->CreateSub ( this, "MINIMAP_INFO_LEFT" );
	pMiniMapInfoLeftDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pMiniMapInfoLeftDummy );
	m_pMiniMapInfoLeftDummy = pMiniMapInfoLeftDummy;
	///////////////////////////////////////////////////
}

void CMiniMap::SetMapAxisInfo ( GLMapAxisInfo &sInfo, const CString & strMapName )
{
	if( NULL == m_pMimiMapInfo ) return;

	if ( m_pMimiMapInfo->GetMapName() != strMapName )
	{
		m_pMimiMapInfo->SetMapName( strMapName );
		m_pMimiMapInfo->UpdatePostion ();
	}
}

void CMiniMap::RotateMap()
{
	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;

	static	D3DXVECTOR3 vLookatPtBACK;
	static	D3DXVECTOR3 vFromPtBACK;
	static	D3DXVECTOR3 vCameraDirBACK;

	if ( vLookatPtBACK == vLookatPt &&
		vFromPtBACK == vFromPt &&
		vCameraDirBACK == vCameraDir )
	{
		return ;
	}

	//	Note : 현제의 회전값.
	//
	float LengthXZ;
	float thetaY;

	LengthXZ = (float) sqrt ( vCameraDir.x*vCameraDir.x + vCameraDir.z*vCameraDir.z );
	
	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//
	if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	thetaY = (float) acos ( vCameraDir.x / LengthXZ );
	if ( vCameraDir.z>0 )	thetaY = -thetaY;

	thetaY += D3DX_PI / 2.f;

	{
		UIRECT	rcGlobalPosDir = m_pDirection->GetGlobalPos();
		D3DXVECTOR2	vCenterPos = D3DXVECTOR2( rcGlobalPosDir.left + (rcGlobalPosDir.right - rcGlobalPosDir.left)/2.f, rcGlobalPosDir.top + (rcGlobalPosDir.bottom - rcGlobalPosDir.top)/2.f );
		
		D3DXVECTOR2 vGlobalPosDir[4];
		vGlobalPosDir[0].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[0].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[1].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[1].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[2].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[2].y = rcGlobalPosDir.bottom - vCenterPos.y;
		vGlobalPosDir[3].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[3].y = rcGlobalPosDir.bottom - vCenterPos.y;

		D3DXVECTOR2 vOutTexturePos[4];
		for ( int i = 0; i < 4; i++ )
		{
			vOutTexturePos[i].x = (float) (cos(-thetaY)* vGlobalPosDir[i].x + sin(-thetaY)* vGlobalPosDir[i].y  ) + vCenterPos.x;
			vOutTexturePos[i].y = (float) (-sin(-thetaY)* vGlobalPosDir[i].x + cos(-thetaY)* vGlobalPosDir[i].y)  + vCenterPos.y;
			m_pDirection->SetGlobalPos(i , vOutTexturePos[i]);		
		}		
	}
}

void CMiniMap::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MINIMAP_FULLSCREEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_FULLSCREENBUTTON );

				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

					const UIRECT& rcPos = m_pFullScreenButtonDummy->GetGlobalPos ();				
					m_pFullScreenButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{					
					//	함수 호출
					DxGlobalStage::GetInstance().ChangeWndMode ();
				}
			}
		}
		break;
	}
}

void CMiniMap::UpdateTime ( float fElapsedTime )
{
	const float fTextureSize = 512.0f;		//	텍스처 크기
	const float fINTERPOLIATION = 0.5f;		//	12시간
	const float fViewSize = 102.0f;			//	한번에 보여지는 사이즈
	static const float fHalfUVRatio = (fViewSize / fTextureSize)/2;
	static const float fOneDaySec = GLPeriod::DAY_HOUR * GLPeriod::TIME_SEC;	

	const float fCurTime = (( GLPeriod::GetInstance().GetHour () * GLPeriod::TIME_SEC ) + 
		GLPeriod::GetInstance().GetSecond()) / fOneDaySec;

	UIRECT TextureRect = m_pTime->GetTexturePos();

	TextureRect.left = fCurTime - fHalfUVRatio + fINTERPOLIATION;
	TextureRect.right= fCurTime + fHalfUVRatio + fINTERPOLIATION;
	TextureRect.sizeX = TextureRect.right - TextureRect.left;

	m_pTime->SetTexturePos ( TextureRect );
}

void CMiniMap::UpdateClubTime( float fCLUBBATTLETime )
{
	CString strCombine;
	strCombine.Format ( "%s %02d:%02d", ID2GAMEWORD( "CLUB_BATTLE_TIME", 0 ), (DWORD)fCLUBBATTLETime/60, (DWORD)fCLUBBATTLETime%60 );
	m_pClubTimeText->SetOneLineText( strCombine, NS_UITEXTCOLOR::RED );
}

void CMiniMap::StartClubTime()
{
	if( m_pClubTimeText ) m_pClubTimeText->SetVisibleSingle( TRUE );
}

void CMiniMap::EndClubTime()
{
	if( m_pClubTimeText ) m_pClubTimeText->SetVisibleSingle( FALSE );
}

void CMiniMap::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( m_pFullScreenButton->IsVisible () )
	{
		const UIRECT& rcGlobalPos = m_pFullScreenButtonDummy->GetGlobalPos ();
		m_pFullScreenButton->SetGlobalPos ( D3DXVECTOR2(rcGlobalPos.left,rcGlobalPos.top) );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	RotateMap ();

	if ( m_pTimeText )
	{
		CString strCombine;
		const int nMonth = (int) GLPeriod::GetInstance().GetMonth ();
		const int nDay = (int) GLPeriod::GetInstance().GetMonthToday ();
		const int nHour = (int) GLPeriod::GetInstance().GetHour ();
		const int nMinute = (int) (nTIME_MINUTE * GLPeriod::GetInstance().GetSecond() / GLPeriod::TIME_SEC);
		static int nMinuteBACK;
		if ( nMinute != nMinuteBACK )
		{
			nMinuteBACK = nMinute;

			strCombine.Format ( "%02d/%02d %02d:%02d", nMonth, nDay, nHour, nMinute );
			m_pTimeText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
	
	
	if ( m_pServerTimeText ) // Monster7j
	{
		CString strCombine;
		const int nServerMonth = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetMonth ();
		const int nServerDay = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetDay ();
		const int nServerHour = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetHour ();
		const int nServerMinute = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetMinute();
		static int nServerMinuteBACK;
		if ( nServerMinute != nServerMinuteBACK )
		{
			nServerMinuteBACK = nServerMinute;

			strCombine.Format ( "%02d/%02d %02d:%02d", nServerMonth, nServerDay, nServerHour, nServerMinute );
			m_pServerTimeText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}
	
	m_pAmmoText->SetVisibleSingle ( FALSE );

	//	화살, 부적 업데이트
	EMSLOT emLHand = GLGaeaClient::GetInstance().GetCharacter()->GetCurLHand();
	if ( GLGaeaClient::GetInstance().GetCharacter()->VALID_SLOT_ITEM(emLHand) )
	{
		SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_SLOT_ITEM(emLHand);
		SITEM* pItemData = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );

		EMITEM_TYPE emItemType = pItemData->sBasicOp.emItemType;
		if ( emItemType == ITEM_ARROW || emItemType == ITEM_CHARM )
		{
			int nType = (emItemType == ITEM_ARROW)?0:1;

			if ( sItemCustom.wTurnNum != m_wAmmoCount )
			{
				CString strCombine;
				strCombine.Format ( "%s : %d", ID2GAMEWORD("ARROW_TYPE",nType), sItemCustom.wTurnNum );

                DWORD ArrowColor = NS_UITEXTCOLOR::WHITE;
				if ( sItemCustom.wTurnNum < 10 ) ArrowColor = NS_UITEXTCOLOR::RED;
				else if( sItemCustom.wTurnNum < 50 ) ArrowColor = NS_UITEXTCOLOR::YELLOW;

				m_pAmmoText->SetText ( strCombine.GetString(), ArrowColor );

				m_wAmmoCount = sItemCustom.wTurnNum;
			}

			m_pAmmoText->SetVisibleSingle ( TRUE );
		}
	}

	bool bFULL_SCREEN_ON_WINDOW = RANPARAM::bScrWindowed && RANPARAM::bScrWndHalfSize;
	if ( bFULL_SCREEN_ON_WINDOW )
	{
		const UIRECT& rcGlobalPosDummy = m_pMiniMapInfoLeftDummy->GetGlobalPos ();
		const UIRECT& rcGlobalPos = m_pMimiMapInfo->GetGlobalPos ();
		if ( rcGlobalPosDummy.left != rcGlobalPos.left || rcGlobalPosDummy.top != rcGlobalPos.top )
		{
			m_pMimiMapInfo->SetGlobalPos ( rcGlobalPosDummy );
		}

		m_pFullScreenButton->SetVisibleSingle ( TRUE );
	}
	else
	{
		const UIRECT& rcGlobalPosDummy = m_pMiniMapInfoDummy->GetGlobalPos ();
		const UIRECT& rcGlobalPos = m_pMimiMapInfo->GetGlobalPos ();
		if ( rcGlobalPosDummy.left != rcGlobalPos.left || rcGlobalPosDummy.top != rcGlobalPos.top )
		{
			m_pMimiMapInfo->SetGlobalPos ( rcGlobalPosDummy );
		}

		m_pFullScreenButton->SetVisibleSingle ( FALSE );
	}
}

HRESULT CMiniMap::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_pCenterPoint->SetVisibleSingle ( FALSE );
	m_pDirection->SetVisibleSingle ( FALSE );

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pDirection->SetVisibleSingle ( TRUE );
	m_pCenterPoint->SetVisibleSingle ( TRUE );

	{
		//	NOTE
		//		RENDER STATES BEGIN
		DWORD dwMinFilter, dwMagFilter, dwMipFilter;
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
		pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER, &dwMipFilter );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		hr = m_pDirection->Render ( pd3dDevice );

		//	NOTE
		//		RENDER STATES END
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, dwMipFilter );

		if ( FAILED ( hr ) )	return hr;
	}     

	return m_pCenterPoint->Render ( pd3dDevice );
}