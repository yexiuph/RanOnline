#include "pch.h"
#include "LargeMapWindow.h"
#include "BasicScrollBarEx.h"
#include "GLMapAxisInfo.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"
#include "InnerInterface.h"
#include "d3dfont.h"
#include "UITextControl.h"

#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLargeMapWindow::CLargeMapWindow ()
	: m_pMousePosText( NULL )
	, m_pMousePosBack( NULL )
	, m_pScrollBarH( NULL )
	, m_pScrollBarV( NULL )
	, m_pMap( NULL )
	, m_pPlayerMark( NULL )
	, m_pLBThumb( NULL )
	, m_pRBThumb( NULL )
	, m_pTargetMark( NULL )
	, m_iTargetX( INT_MIN )
	, m_iTargetY( INT_MIN )
{
	SecureZeroMemory( m_pMark, sizeof(m_pMark) );
	Initialize();
}

CLargeMapWindow::~CLargeMapWindow ()
{
	m_vecGuild.clear();
}

void CLargeMapWindow::Initialize()
{
	m_fPercentH_BACK = 2.0f;
	m_fPercentV_BACK = 2.0f;
	m_nWORLD_START_X = 1;
	m_nWORLD_START_Y = 1;
	m_nWORLD_SIZE_X = 1;
	m_nWORLD_SIZE_Y = 1;
	m_nMAP_TEXSIZE_X = 1;
	m_nMAP_TEXSIZE_Y = 1;
	m_nVIEW_START_X = 1;
	m_nVIEW_START_Y = 1;
	m_nVIEW_SIZE_X = 1;
	m_nVIEW_SIZE_Y = 1;
	m_bFirstGap = FALSE;
	//m_CONTROL_NEWID = BASE_CONTROLID;
	m_bAUTO_MOVE = true;

	ConvIntDataFloatData();
}

void CLargeMapWindow::ConvIntDataFloatData()
{
	// 미니맵 마우스 좌클릭 계산은 FLOAT으로 하므로 미리 캐스팅 해둔다 ( 준혁 )

	m_vWORLD_START = D3DXVECTOR2( (float)m_nWORLD_START_X, (float)m_nWORLD_START_Y );
	m_vWORLD_SIZE = D3DXVECTOR2( (float)m_nWORLD_SIZE_X, (float)m_nWORLD_SIZE_Y );
	m_vMAP_TEXSIZE = D3DXVECTOR2( (float)m_nMAP_TEXSIZE_X, (float)m_nMAP_TEXSIZE_Y );
}

void CLargeMapWindow::CreateSubControl ()
{
	{	//	가로 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR_H", UI_FLAG_BOTTOM | UI_FLAG_XSIZE, LARGEMAP_SCROLLBAR_H );
		pScrollBar->CreateBaseScrollBar ( "LARGEMAP_SCROLLBAR_H", false );
		RegisterControl ( pScrollBar );
		m_pScrollBarH = pScrollBar;
		m_pScrollBarH->SetVisibleSingle ( FALSE );
	}

	{	//	세로 스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, LARGEMAP_SCROLLBAR_V );
		pScrollBar->CreateBaseScrollBar ( "LARGEMAP_SCROLLBAR_V" );
		RegisterControl ( pScrollBar );
		m_pScrollBarV = pScrollBar;
		m_pScrollBarV->SetVisibleSingle ( FALSE );
	}

	{
		CUIControl*	pMapImage = new CUIControl;
		pMapImage->CreateSub ( this, "LARGEMAP_IMAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE, LARGEMAP_IMAGE );
		RegisterControl ( pMapImage );
		m_pMap = pMapImage;
	}

	{
		CUIControl* pPlayerMark = new CUIControl;
		pPlayerMark->CreateSub ( this, "LARGEMAP_MARK", UI_FLAG_DEFAULT, PLAYER_MARK );		
		RegisterControl ( pPlayerMark );
		m_pPlayerMark = pPlayerMark;
	}

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	{
		CUIControl* pMousePosBack = new CUIControl;
		pMousePosBack->CreateSub ( this, "LARGEMAP_MOUSEPOS_BACK", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
		RegisterControl ( pMousePosBack );		
		m_pMousePosBack = pMousePosBack;

		CBasicTextBox*	pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "LARGEMAP_MOUSEPOS_TEXT", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
		pTextBox->SetFont ( pFont );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pTextBox );
		m_pMousePosText = pTextBox;
	}

	{
		CUIControl* pDirection = new CUIControl;
		pDirection->CreateSub ( this, "LARGEMAP_DIRECTION", UI_FLAG_BOTTOM, LARGEMAP_DIRECTION );		
		RegisterControl ( pDirection );
	}

	{
		CBasicButton* pRBThumb = new CBasicButton;
		pRBThumb->CreateSub ( this, "LARGEMAP_RB_THUMB", UI_FLAG_RIGHT | UI_FLAG_BOTTOM, LARGEMAP_RB_THUMB );
		pRBThumb->CreateFlip ( "LARGEMAP_RB_THUMB_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( pRBThumb );
		m_pRBThumb = pRBThumb;

		CBasicButton* pLBThumb = new CBasicButton;
		pLBThumb->CreateSub ( this, "LARGEMAP_LB_THUMB", UI_FLAG_BOTTOM, LARGEMAP_LB_THUMB );
		pLBThumb->CreateFlip ( "LARGEMAP_LB_THUMB_F", CBasicButton::MOUSEIN_FLIP );
		RegisterControl ( pLBThumb );
		m_pLBThumb = pLBThumb;
	}

	CString strMark[MAXPARTY] =
	{
		"TEST_MARK0", 
		"TEST_MARK1", 
		"TEST_MARK2", 
		"TEST_MARK3",
		"TEST_MARK4", 
		"TEST_MARK5", 
		"TEST_MARK6", 
		"TEST_MARK7"
	};

	for ( int i = 0; i < MAXPARTY; ++i )
	{
		CUIControl* pMark = new CUIControl;
		pMark->CreateSub( this, strMark[i].GetString() );
		//pMark->SetUseRender( TRUE );
		pMark->SetVisibleSingle( FALSE );
		RegisterControl( pMark );
		m_pMark[i] = pMark;
	}

	// 미니맵 마우스 좌클릭 목표지점 표시 ( 준혁 )
	{
		m_pTargetMark = new CUIControl;
		m_pTargetMark->CreateSub( this, "TARGET_MARK" );
		m_pTargetMark->SetVisibleSingle( FALSE );
		RegisterControl( m_pTargetMark );
	}
}

HRESULT CLargeMapWindow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	m_pd3dDevice = pd3dDevice;

	HRESULT hr = S_OK;
	hr = CUIWindowEx::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

//************************************
// Method:    SetDefaultPosition
// FullName:  CLargeMapWindow::SetDefaultPosition
// Access:    public 
// Returns:   void
// Qualifier: kdhong
//************************************
void CLargeMapWindow::SetDefaultPosition()
{
	const UIRECT& rcGlobalPos = GetGlobalPos ();
	UIRECT rcTemp;

	WORD wAlignFlag = GetAlignFlag ();			
	SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	{
		AlignSubControl( rcGlobalPos, rcTemp );

		long lResolution = CUIMan::GetResolution ();
		rcTemp.left = HIWORD(lResolution) - rcTemp.sizeX;

		SetGlobalPos( rcTemp );
	}
	SetAlignFlag( wAlignFlag );
}

void CLargeMapWindow::SetMapAxisInfo ( GLMapAxisInfo& sInfo, const CString& strMapName )
{
	Initialize();

	m_pMap->UnLoadTexture ();
	m_pMap->SetTextureName ( NULL );
	m_pMap->SetUseRender ( FALSE );

	// 맵 이동시 미니맵의 크기와 위치를 초기화 시킨다.
	// 이유는 미니맵의 사이즈가 1024와 512 두가지가 존재하기 때문에
	// 큰 맵에서 작은 맵으로의 이동시 문제가 생긴다.
	SetDefaultPosition();

	const D3DXVECTOR4 vMapTex = sInfo.GetMiniMapTexPos();	 
	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

	{	//	맵 텍스쳐 크기
		const D3DXVECTOR2 vMinimapTexSize = sInfo.GetMiniMapTexSize();
		m_nMAP_TEXSIZE_X = static_cast<int>(vMinimapTexSize.x);
		m_nMAP_TEXSIZE_Y = static_cast<int>(vMinimapTexSize.y);
	}

	{	//	가로
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		pThumbFrame->SetState ( m_nMAP_TEXSIZE_X, static_cast<int>(rcGlobalPos.sizeX) );
		pThumbFrame->SetPercent ( 0.0f );
	}

	{	//	세로
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		pThumbFrame->SetState ( m_nMAP_TEXSIZE_Y, static_cast<int>(rcGlobalPos.sizeY) );
		pThumbFrame->SetPercent ( 0.0f );
	}

	{
		m_nWORLD_START_X = sInfo.GetMapStartX ();
		m_nWORLD_START_Y = sInfo.GetMapStartY ();
		m_nWORLD_SIZE_X = sInfo.GetMapSizeX ();
		m_nWORLD_SIZE_Y = sInfo.GetMapSizeY ();
	}

	//	Exception
	{
		m_nMAP_TEXSIZE_X = (m_nMAP_TEXSIZE_X) ? m_nMAP_TEXSIZE_X : 1;
		m_nMAP_TEXSIZE_Y = (m_nMAP_TEXSIZE_Y) ? m_nMAP_TEXSIZE_Y : 1;

		m_nWORLD_SIZE_X = (m_nWORLD_SIZE_X) ? m_nWORLD_SIZE_X : 1;
		m_nWORLD_SIZE_Y = (m_nWORLD_SIZE_Y) ? m_nWORLD_SIZE_Y : 1;
	}

	m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
	m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

	m_vRenderSize.x *= 10000;
	m_vRenderSize.x = floor(m_vRenderSize.x);
	m_vRenderSize.x *= 0.0001f;

	m_vRenderSize.y *= 10000;
	m_vRenderSize.y = floor(m_vRenderSize.y);
	m_vRenderSize.y *= 0.0001f;

	// 맵 텍스쳐 체인지
	UIRECT rcTexture( 0.0f, 0.0f, m_vRenderSize.x, m_vRenderSize.y );
	m_pMap->SetTextureName ( sInfo.GetMinMapTex() );
	m_pMap->SetTexturePos ( rcTexture );

	// 텍스쳐 만들기
	m_pMap->LoadTexture ( m_pd3dDevice );
	m_pMap->SetUseRender ( TRUE );

	m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
	m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;
	
	m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
	m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

	ConvIntDataFloatData();
}

void CLargeMapWindow::Update ( int x ,int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_PosX = x;
	m_PosY = y;

	nScroll = 0;
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		if ( m_pMap->IsExclusiveSelfControl() ) //	마우스로 드래깅시 이미지 움직이기
		{	
			D3DXVECTOR2 vMOVE ( m_vMOUSE_BACK.x - x, m_vMOUSE_BACK.y - y );

			if ( 0.0f != vMOVE.x )
			{	
				const float fMovePercent = vMOVE.x / m_vScrollGap.x;
				CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
				const float& fPercent = pThumbFrame->GetPercent ();
				float fNewPercent = fPercent + fMovePercent;
				fNewPercent = max ( 0.0f, fNewPercent );
				fNewPercent = min ( 1.0f, fNewPercent );
				pThumbFrame->SetPercent ( fNewPercent );
			}

			if ( 0.0f != vMOVE.y )
			{
				const float fMovePercent = vMOVE.y / m_vScrollGap.y;
				CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
				const float& fPercent = pThumbFrame->GetPercent ();
				float fNewPercent = fPercent + fMovePercent;
				fNewPercent = max ( 0.0f, fNewPercent );
				fNewPercent = min ( 1.0f, fNewPercent );
				pThumbFrame->SetPercent ( fNewPercent );
			}

			SetAutoMove ( false );

			m_vMOUSE_BACK.x = (float)x;
			m_vMOUSE_BACK.y = (float)y;
		}

		{	//	세로 스크롤
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();		
			const float fPercentV = pThumbFrame->GetPercent ();		
			if ( fPercentV != m_fPercentV_BACK )
			{
				const float fMOVE = m_vScrollGap.y * fPercentV;
				m_vRenderPos.y = fMOVE / m_nMAP_TEXSIZE_Y;

				m_vRenderPos.y *= 10000.0f;
				m_vRenderPos.y = floor(m_vRenderPos.y);
				m_vRenderPos.y *= 0.0001f;

				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );				

				m_fPercentV_BACK = fPercentV;

				m_nVIEW_START_Y = static_cast<int>(fMOVE);

				if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;
				if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
			}
		}

		{	//	가로 스크롤
			CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
			const float fPercentH = pThumbFrame->GetPercent ();
			if ( fPercentH != m_fPercentH_BACK )
			{
				const float fMOVE = m_vScrollGap.x * fPercentH;
				m_vRenderPos.x = fMOVE / m_nMAP_TEXSIZE_X;

				m_vRenderPos.x *= 10000.0f;
				m_vRenderPos.x = floor(m_vRenderPos.x);
				m_vRenderPos.x *= 0.0001f;

				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );

				m_fPercentH_BACK = fPercentH;

				m_nVIEW_START_X = static_cast<int>(fMOVE);
				if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
				if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
			}
		}

		if ( m_pRBThumb->IsExclusiveSelfControl() )  // 우측 미니맵 늘이기
		{
			const UIRECT& rcGlobalPos = GetGlobalPos ();
			D3DXVECTOR2 vDELTA;

			vDELTA.x = static_cast<float>(x) - rcGlobalPos.left;			
			vDELTA.y = static_cast<float>(y) - rcGlobalPos.top;

			const UIRECT& rcGlobalMapPos = m_pMap->GetGlobalPos ();
			float fGAP_SIZEX = (rcGlobalPos.sizeX - rcGlobalMapPos.sizeX);
			float fGAP_SIZEY = (rcGlobalPos.sizeY - rcGlobalMapPos.sizeY);

			long lResolution = CUIMan::GetResolution ();
			WORD X_RES = HIWORD(lResolution);
			WORD Y_RES = LOWORD(lResolution);

			{				
				float fMAP_DELTAX = m_nMAP_TEXSIZE_X + fGAP_SIZEX;
				if ( fMAP_DELTAX <= vDELTA.x )
				{
					vDELTA.x = fMAP_DELTAX;
				}
				if ( vDELTA.x >= X_RES )
				{
					vDELTA.x = X_RES;
				}
			}

			{
				float fMAP_DELTAY = (float)m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
				if ( fMAP_DELTAY <= vDELTA.y )
				{
					vDELTA.y = fMAP_DELTAY;
				}

				if ( vDELTA.y >= Y_RES )
				{
					vDELTA.y = Y_RES;
				}
			}

			//
			//float fMAP_DELTAY = m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
			//if ( fMAP_DELTAX <= vDELTA.x )
			//{
			//	vDELTA.x = fMAP_DELTAX;
			//}
			//if ( fMAP_DELTAY <= vDELTA.y )
			//{
			//	vDELTA.y = fMAP_DELTAY;
			//}

			WORD wAlignFlag = GetAlignFlag ();			
			SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			{
				UIRECT rcNewPos( rcGlobalPos.left, rcGlobalPos.top, vDELTA.x, vDELTA.y );
				AlignSubControl( rcGlobalPos, rcNewPos );
				SetGlobalPos( rcNewPos );
			}
			SetAlignFlag ( wAlignFlag );

			{
				const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

				m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
				m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

				m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
				m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

				if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
				if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;

				if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;
				if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;

				m_vRenderPos.x = static_cast<float>(m_nVIEW_START_X) / static_cast<float>(m_nMAP_TEXSIZE_X);
				m_vRenderPos.y = static_cast<float>(m_nVIEW_START_Y) / static_cast<float>(m_nMAP_TEXSIZE_Y);

				m_vRenderSize.x *= 10000;
				m_vRenderSize.x = floor(m_vRenderSize.x);
				m_vRenderSize.x *= 0.0001f;

				m_vRenderSize.y *= 10000;
				m_vRenderSize.y = floor(m_vRenderSize.y);
				m_vRenderSize.y *= 0.0001f;

				// 맵 텍스쳐 체인지
				UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
				m_pMap->SetTexturePos ( rcTexture );

				m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
				m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;

				m_pScrollBarH->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_X, m_nVIEW_SIZE_X );
				m_pScrollBarV->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_Y, m_nVIEW_SIZE_Y );
			}
		}

		if ( m_pLBThumb->IsExclusiveSelfControl() )  // 좌측 미니맵 늘이기
		{
			const UIRECT& rcGlobalPos = GetGlobalPos ();
			D3DXVECTOR2 vORIGINPOS( rcGlobalPos.right, rcGlobalPos.top );
			D3DXVECTOR2 vDELTA;
			D3DXVECTOR2 vSTART;

			vDELTA.x = fabs(static_cast<float>(x) - rcGlobalPos.right);
			vSTART.x = rcGlobalPos.right - vDELTA.x;
			vSTART.y = rcGlobalPos.top;
//			vDELTA.x = static_cast<float>(x) - rcGlobalPos.left;			
			vDELTA.y = static_cast<float>(y) - rcGlobalPos.top;

			const UIRECT& rcGlobalMapPos = m_pMap->GetGlobalPos ();

			//bool bNO_RESIZE = false;
			//if ( rcGlobalMapPos.sizeX >= m_nMAP_TEXSIZE_X && fabs(vDELTA.x) >= rcGlobalPos.sizeX )
			//{
			//	bNO_RESIZE = true;
			//}

   //         D3DXVECTOR2 vProtectSize = GetProtectSize();
			//if ( vProtectSize.x >= rcGlobalPos.sizeX )
			//{
			//	if ( fabs(vDELTA.x) <= rcGlobalPos.sizeX )
			//		bNO_RESIZE = true;
			//}

			//if ( !bNO_RESIZE )
			{
				float fGAP_SIZEX = (rcGlobalPos.sizeX - rcGlobalMapPos.sizeX);
				float fGAP_SIZEY = (rcGlobalPos.sizeY - rcGlobalMapPos.sizeY);

			long lResolution = CUIMan::GetResolution ();
			WORD X_RES = HIWORD(lResolution);
			WORD Y_RES = LOWORD(lResolution);			

			{
				float fMAP_DELTAX = (float)m_nMAP_TEXSIZE_X + fGAP_SIZEX;
				if ( fMAP_DELTAX <= vDELTA.x )
				{
					vDELTA.x = fMAP_DELTAX;
				}

				if ( vDELTA.x >= X_RES )
				{
					vDELTA.x = X_RES;
				}
			}

			{
				float fMAP_DELTAY = (float)m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
				if ( fMAP_DELTAY <= vDELTA.y )
				{
					vDELTA.y = fMAP_DELTAY;
				}

				if ( vDELTA.y >= Y_RES )
				{
					vDELTA.y = Y_RES;
				}
			}
				//float fMAP_DELTAX = m_nMAP_TEXSIZE_X + fGAP_SIZEX;
				//float fMAP_DELTAY = m_nMAP_TEXSIZE_Y + fGAP_SIZEY;
				//if ( fMAP_DELTAX <= vDELTA.x )
				//{
				//	vDELTA.x = fMAP_DELTAX;
				//}
				//if ( fMAP_DELTAY <= vDELTA.y )
				//{
				//	vDELTA.y = fMAP_DELTAY;
				//}

				WORD wAlignFlag = GetAlignFlag ();			
				SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
				{
					UIRECT rcNewPos( vSTART.x, vSTART.y, vDELTA.x, vDELTA.y );
					AlignSubControl ( rcGlobalPos, rcNewPos );
					SetGlobalPos ( rcNewPos );
				}
				SetAlignFlag ( wAlignFlag );

				SetGlobalPos ( D3DXVECTOR2(vORIGINPOS.x-GetGlobalPos().sizeX,vORIGINPOS.y ) );

				{
					const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();

					m_vRenderSize.x = rcGlobalPos.sizeX / m_nMAP_TEXSIZE_X;
					m_vRenderSize.y = rcGlobalPos.sizeY / m_nMAP_TEXSIZE_Y;

					m_nVIEW_SIZE_X = static_cast<int>(rcGlobalPos.sizeX);
					m_nVIEW_SIZE_Y = static_cast<int>(rcGlobalPos.sizeY);

					if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
					if ( m_nVIEW_START_X + m_nVIEW_SIZE_X > m_nMAP_TEXSIZE_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;

					if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;
					if ( m_nVIEW_START_Y + m_nVIEW_SIZE_Y > m_nMAP_TEXSIZE_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;

					m_vRenderPos.x = static_cast<float>(m_nVIEW_START_X) / static_cast<float>(m_nMAP_TEXSIZE_X);
					m_vRenderPos.y = static_cast<float>(m_nVIEW_START_Y) / static_cast<float>(m_nMAP_TEXSIZE_Y);

					m_vRenderSize.x *= 10000;
					m_vRenderSize.x = floor(m_vRenderSize.x);
					m_vRenderSize.x *= 0.0001f;

					m_vRenderSize.y *= 10000;
					m_vRenderSize.y = floor(m_vRenderSize.y);
					m_vRenderSize.y *= 0.0001f;

					// 맵 텍스쳐 체인지
					UIRECT rcTexture( m_vRenderPos.x, m_vRenderPos.y, m_vRenderSize.x, m_vRenderSize.y );
					m_pMap->SetTexturePos ( rcTexture );

					m_vScrollGap.x = m_nMAP_TEXSIZE_X - rcGlobalPos.sizeX;
					m_vScrollGap.y = m_nMAP_TEXSIZE_Y - rcGlobalPos.sizeY;

					m_pScrollBarH->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_X, m_nVIEW_SIZE_X );
					m_pScrollBarV->GetThumbFrame()->SetState ( m_nMAP_TEXSIZE_Y, m_nVIEW_SIZE_Y );
				}
			}
		}

		{
			const D3DXVECTOR3 &vPlayerPos = GLGaeaClient::GetInstance().GetCharacterPos ();		

			m_pPlayerMark->SetVisibleSingle( FALSE );
			UPDATE_CHAR_POS( m_pPlayerMark, vPlayerPos );

			D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
			D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
			D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;

			UPDATE_CHAR_ROTATE ( vCameraDir );

			UPDATE_PARTY ();
			UPDATE_GUILD ();
			UPDATE_MOUSE_POS ( x, y );
		}
	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();
		pThumbFrame->SetPercent ( fPercent );
	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();
		pThumbFrame->SetPercent ( fPercent );
	}

	if ( m_bAUTO_MOVE )
	{
		//	자기 위치 표시
		const D3DXVECTOR3 &vPlayerPos = GLGaeaClient::GetInstance().GetCharacterPos ();

		int nMAP_X(0), nMAP_Y(0);
		CONVERT_WORLD2MAP ( vPlayerPos, nMAP_X, nMAP_Y );
		DoCHARPOS_OnCENTER_InVIEW ( nMAP_X, nMAP_Y );
	}

	// 미니맵 마우스 좌클릭 목표지점 갱신 ( 준혁 )
	{
		UPDATE_TARGET_POS( m_iTargetX, m_iTargetY );
	}
}

void CLargeMapWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( LARGEMAP_WINDOW );
			}
		}
		break;
	case LARGEMAP_IMAGE:	TranslateMeg( cID, dwMsg, m_pMap );				break;
	case LARGEMAP_LB_THUMB:	TranslateMegThumb( cID, dwMsg, m_pLBThumb );	break;
	case LARGEMAP_RB_THUMB:	TranslateMegThumb( cID, dwMsg, m_pRBThumb );	break;
	}
}

void CLargeMapWindow::TranslateMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
	{
		if ( CHECK_RB_DOWN_LIKE ( dwMsg ) || CHECK_MB_DOWN_LIKE ( dwMsg ) )
		{
			pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )						//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = pUIControl->GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;

				if( cID == LARGEMAP_IMAGE )
				{
					m_vMOUSE_BACK.x = (float)m_PosX;
					m_vMOUSE_BACK.y = (float)m_PosY;
				}

				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_MB_UP_LIKE ( dwMsg ) )
		{
			pUIControl->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
		}

		if( CHECK_LB_UP_LIKE( dwMsg ) )				// 미니맵 좌클릭해서 캐릭터 이동 ( 준혁 )
			OnLButtonUp();
	}
	else if ( CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_MB_UP_LIKE ( dwMsg ) )
	{												//	컨트롤 영역밖에서 버튼을 떼는 경우 발생하더라도
		pUIControl->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;						//	최초 포지션갭을 해제한다.
	}
}

void CLargeMapWindow::TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl )
{
	GASSERT( pUIControl );

	if ( CHECK_MOUSE_IN ( dwMsg ) )					//	컨트롤 내부에 마우스가 위치하고,
	{
		if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )			//	마우스를 좌클릭으로 눌렀을 때,
		{
			if( CHECK_MOUSE_IN( m_pMap->GetMessageEx() ) )
				return;

			pUIControl->SetExclusiveControl();		//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )						//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = pUIControl->GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;

				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	좌클릭했던것을 놓으면
		{
			pUIControl->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )			//	컨트롤 영역밖에서 버튼을 떼는 경우가
	{												//	발생하더라도
		pUIControl->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;						//	최초 포지션갭을 해제한다.
	}
}

HRESULT	CLargeMapWindow::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	if( !IsVisible() ) return S_OK;

	HRESULT hr = S_OK;

	BOOL bPlayerMarkVisible = m_pPlayerMark->IsVisible ();
	m_pPlayerMark->SetVisibleSingle ( FALSE );
	{
		hr = CUIWindowEx::Render ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}
	m_pPlayerMark->SetVisibleSingle ( bPlayerMarkVisible );	

	//	NOTE
	//		RENDER STATES BEGIN
	DWORD dwMinFilter(0), dwMagFilter(0), dwMipFilter(0);
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &dwMinFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &dwMagFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER, &dwMipFilter );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	hr = m_pPlayerMark->Render ( pd3dDevice );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, dwMagFilter );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, dwMipFilter );

	return hr;
}

void CLargeMapWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	UPDATE_GUILD_INFO( bVisible );

	if ( bVisible )
	{
		m_pPlayerMark->SetVisibleSingle ( TRUE );

		//	자기 위치 표시
		const D3DXVECTOR3 &vPlayerPos = GLGaeaClient::GetInstance().GetCharacterPos ();

		int nMAP_X(0), nMAP_Y(0);
		CONVERT_WORLD2MAP ( vPlayerPos, nMAP_X, nMAP_Y );
		DoCHARPOS_OnCENTER_InVIEW ( nMAP_X, nMAP_Y );

		UPDATE_CHAR_POS ( m_pPlayerMark, vPlayerPos );
		UPDATE_CHAR_ROTATE ( CInnerInterface::GetInstance().GetCharDir () );

		//	NOTE
		//		맵 자동 이동
		SetAutoMove ( true );		
	}
}

void CLargeMapWindow::OnLButtonUp()
{
	if( CInnerInterface::GetInstance().IsOpenWindowToMoveBlock() )
		return;

	INT iMapX( 0 ), iMapY( 0 );
	CONVERT_MOUSE2MAP( m_PosX, m_PosY, iMapX, iMapY );

	D3DXVECTOR3 vWorldPos( 0.0f, 0.0f, 0.0f );
	CONVERT_MAP2WORLD( iMapX, iMapY, vWorldPos );

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	if( !pCharacter )
		return;

	if( !pCharacter->LargeMapMoveTo( &vWorldPos ) )
		return;

	m_iTargetX = iMapX;
	m_iTargetY = iMapY;

	UPDATE_TARGET_POS( m_iTargetX, m_iTargetY );
}

void CLargeMapWindow::UPDATE_CHAR_POS ( CUIControl * pControl, const D3DXVECTOR3& vPlayerPos )
{
	GASSERT( pControl );

	//CUIControl* pMark = FindControl ( cID ); // MEMO : FindControl은 NULL을 넘긴다.
	//if ( !pMark ) return ;

	int nMapPosX(0), nMapPosY(0);
	CONVERT_WORLD2MAP ( vPlayerPos, nMapPosX, nMapPosY );

	int nPOINT_X = nMapPosX - m_nVIEW_START_X;
	int nPOINT_Y = nMapPosY - m_nVIEW_START_Y;

	//pMark->SetVisibleSingle ( FALSE );

    if ( nPOINT_X < 0 || nPOINT_Y < 0 ) return ;
	if ( m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y ) return ;

	const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos ();
	const UIRECT& rcMarkPos = pControl->GetGlobalPos ();
	D3DXVECTOR2 vNewMarkPos ( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), nPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
	pControl->SetGlobalPos ( vNewMarkPos );
	pControl->SetVisibleSingle ( TRUE );
}

void CLargeMapWindow::UPDATE_CHAR_ROTATE ( const D3DXVECTOR3& vCharDir )
{
	//	Note : 현제의 회전값.
	//
	float LengthXZ;
	float thetaY;

	LengthXZ = (float) sqrt ( vCharDir.x*vCharDir.x + vCharDir.z*vCharDir.z );			
	
	//	Note : 수식의 특성상 LengthXZ가... 0가되면... 문제가 발생한다.
	//	
	if ( LengthXZ == 0 )	LengthXZ = 0.001f;

	thetaY = (float) acos ( vCharDir.x / LengthXZ );
	if ( vCharDir.z>0 )	thetaY = -thetaY;

	thetaY += D3DX_PI / 2.f;

	{
		UIRECT	rcGlobalPosDir = m_pPlayerMark->GetGlobalPos();
		D3DXVECTOR2	vCenterPos( floor(rcGlobalPosDir.left + (rcGlobalPosDir.sizeX)/2.f), floor(rcGlobalPosDir.top + (rcGlobalPosDir.sizeY)/2.f) );
		
		D3DXVECTOR2 vGlobalPosDir[4];
		vGlobalPosDir[0].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[0].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[1].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[1].y = rcGlobalPosDir.top - vCenterPos.y;
		vGlobalPosDir[2].x = rcGlobalPosDir.right - vCenterPos.x;
		vGlobalPosDir[2].y = rcGlobalPosDir.bottom - vCenterPos.y;
		vGlobalPosDir[3].x = rcGlobalPosDir.left - vCenterPos.x;
		vGlobalPosDir[3].y = rcGlobalPosDir.bottom - vCenterPos.y;
		
		for ( int i = 0; i < 4; ++i )
		{
			D3DXVECTOR2 vOutPos;
			vOutPos.x = (float) (cos(-thetaY) * vGlobalPosDir[i].x + sin(-thetaY)* vGlobalPosDir[i].y) + vCenterPos.x;
			vOutPos.y = (float) (-sin(-thetaY) * vGlobalPosDir[i].x + cos(-thetaY)* vGlobalPosDir[i].y)  + vCenterPos.y;
			m_pPlayerMark->SetGlobalPos(i , vOutPos);		
		}		
	}
}

void CLargeMapWindow::UPDATE_TARGET_POS( int iX, int iY )
{
	if( !m_pTargetMark )
		return;

	// 일단 숨긴다
	m_pTargetMark->SetVisibleSingle( FALSE );

	// 목표 지점이 초기화 되었다
	if( m_iTargetX == INT_MIN || m_iTargetY == INT_MIN )
		return;

	int nPOINT_X = iX - m_nVIEW_START_X;
	int nPOINT_Y = iY - m_nVIEW_START_Y;

	// 화면을 벗어난 위치에 있다
    if( nPOINT_X < 0 || nPOINT_Y < 0 )
		return;

	// 화면을 벗어난 위치에 있다
	if( m_nVIEW_SIZE_X < nPOINT_X || m_nVIEW_SIZE_Y < nPOINT_Y )
		return;

	// 목표 지점 도달
	{
		const D3DXVECTOR3& vPlayerPos = GLGaeaClient::GetInstance().GetCharacterPos();

		int nMAP_X( 0 ), nMAP_Y( 0 );
		CONVERT_WORLD2MAP( vPlayerPos, nMAP_X, nMAP_Y );

		// 18 18 ㅋㅋ
		if( abs( nMAP_X - m_iTargetX ) < 18 && abs( nMAP_Y - m_iTargetY ) < 18 )
		{
            VISIBLE_TARGET_POS();
			return;
		}
	}

	// 위치 갱신
	{
		const UIRECT& rcGlobalPos = m_pMap->GetGlobalPos();
		const UIRECT& rcMarkPos = m_pTargetMark->GetGlobalPos();
		D3DXVECTOR2 vNewMarkPos( nPOINT_X + rcGlobalPos.left - (rcMarkPos.sizeX/2.f), nPOINT_Y + rcGlobalPos.top - (rcMarkPos.sizeY/2.f) );
		m_pTargetMark->SetGlobalPos( vNewMarkPos );
	}

	// 화면에 보여준다
	m_pTargetMark->SetVisibleSingle( TRUE );
}

void CLargeMapWindow::VISIBLE_TARGET_POS( BOOL bVisible )
{
	if( !m_pTargetMark )
		return;

	if( !bVisible )
	{
		m_iTargetX = INT_MIN;
		m_iTargetY = INT_MIN;
	}

	m_pTargetMark->SetVisibleSingle( bVisible );
}

void CLargeMapWindow::DoCHARPOS_OnCENTER_InVIEW ( int nMapPosX, int nMapPosY )
{
	//	NOTE
	//		뷰의 이동
	m_nVIEW_START_X = nMapPosX - static_cast<int>((static_cast<float>(m_nVIEW_SIZE_X)/2.0f));
	m_nVIEW_START_Y = nMapPosY - static_cast<int>((static_cast<float>(m_nVIEW_SIZE_Y)/2.0f));

	//	뷰 영역 체크
	if ( m_nVIEW_START_X < 0 ) m_nVIEW_START_X = 0;
	if ( m_nVIEW_START_Y < 0 ) m_nVIEW_START_Y = 0;
	if ( m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X < m_nVIEW_START_X ) m_nVIEW_START_X = m_nMAP_TEXSIZE_X - m_nVIEW_SIZE_X;
	if ( m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y < m_nVIEW_START_Y ) m_nVIEW_START_Y = m_nMAP_TEXSIZE_Y - m_nVIEW_SIZE_Y;
	
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarH->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();

		if ( m_vScrollGap.x == 0.0f )	pThumbFrame->SetPercent ( 0.0f );
		else							pThumbFrame->SetPercent ( m_nVIEW_START_X/m_vScrollGap.x );
	}

	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBarV->GetThumbFrame ();
		const float& fPercent = pThumbFrame->GetPercent ();

		if ( m_vScrollGap.y == 0.0f )	pThumbFrame->SetPercent ( 0.0f );
		else							pThumbFrame->SetPercent ( m_nVIEW_START_Y/m_vScrollGap.y );
	}
}

void CLargeMapWindow::CONVERT_WORLD2MAP ( const D3DXVECTOR3& vPos, int& nPosX, int& nPosY )
{
	//	NOTE
	//		월드 좌표의 기준을 재설정, Y값이 계산되는 것은 캐릭터가 진행하는 방향과
	//		맵이 진행하는 방향이 반대로 되어 있기 때문임.
	int nCur_X = static_cast<int>(vPos.x) - m_nWORLD_START_X;
	int nCur_Y = (m_nWORLD_SIZE_Y-static_cast<int>(vPos.z)) + m_nWORLD_START_Y;

	//	NOTE
	//		재정의된 월드 좌표를 맵 포지션으로 바꿈
	nPosX = static_cast<int>(m_nMAP_TEXSIZE_X * nCur_X / m_nWORLD_SIZE_X);
	nPosY = static_cast<int>(m_nMAP_TEXSIZE_Y * nCur_Y / m_nWORLD_SIZE_Y);
}

void CLargeMapWindow::CONVERT_MOUSE2MAP ( const int nMousePosX, const int nMousePosY, int& nPosX, int& nPosY )
{
	const UIRECT& rcMapGlobalPos = m_pMap->GetGlobalPos ();
	const int nMouseX_RELATIVE = nMousePosX - static_cast<int>(rcMapGlobalPos.left);
	const int nMouseY_RELATIVE = nMousePosY - static_cast<int>(rcMapGlobalPos.top);	

	nPosX = nMouseX_RELATIVE + m_nVIEW_START_X;
	nPosY = nMouseY_RELATIVE + m_nVIEW_START_Y;
}

void CLargeMapWindow::CONVERT_MAP2WORLD ( const int nPosX, const int nPosY, D3DXVECTOR3& vPos )
{
	float fPosX = m_vWORLD_SIZE.x / m_vMAP_TEXSIZE.x * (float)nPosX;
	float fPosY = m_vWORLD_SIZE.y / m_vMAP_TEXSIZE.y * (float)nPosY;

	vPos.x = fPosX + m_vWORLD_START.x;
	vPos.z = m_vWORLD_SIZE.y - ( fPosY - m_vWORLD_START.y );
}

void CLargeMapWindow::UPDATE_MOUSE_POS ( const int nMouseX, const int nMouseY )
{
	if ( !CHECK_MOUSE_IN ( m_pMap->GetMessageEx () ) )
	{
		m_pMousePosText->SetVisibleSingle ( FALSE );
		m_pMousePosBack->SetVisibleSingle ( FALSE );
		return ;
	}

	const UIRECT& rcMapGlobalPos = m_pMap->GetGlobalPos ();
	const int nMouseX_RELATIVE = nMouseX - static_cast<int>(rcMapGlobalPos.left);
	const int nMouseY_RELATIVE = nMouseY - static_cast<int>(rcMapGlobalPos.top);	

	const int nX = nMouseX_RELATIVE + m_nVIEW_START_X;
	const int nY = m_nMAP_TEXSIZE_Y - (nMouseY_RELATIVE + m_nVIEW_START_Y);

	int PosX = (int)(((m_nWORLD_SIZE_X * nX) / m_nMAP_TEXSIZE_X) / 50.f);
	int PosY = (int)(((m_nWORLD_SIZE_Y * nY) / m_nMAP_TEXSIZE_Y) / 50.f);	
    
	CString strMousePos;
	strMousePos.Format ( "%03d %03d", PosX, PosY );
	m_pMousePosText->SetOneLineText ( strMousePos, NS_UITEXTCOLOR::WHITE );
	m_pMousePosText->SetVisibleSingle ( TRUE );
	m_pMousePosBack->SetVisibleSingle ( TRUE );
}