#include "pch.h"
#include "minimapinfo.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMiniMapInfo::CMiniMapInfo()
	: m_nMiniPosX(0)
	, m_nMiniPosY(0)
	, m_pChannelTextBox(NULL)
	, m_pMinimapTextBox(NULL)
	, m_pClubMark(NULL)
{
}

CMiniMapInfo::~CMiniMapInfo(void)
{
}

void CMiniMapInfo::UpdatePostion ()
{
	CString strText;

	if ( CInnerInterface::GetInstance().IsCHANNEL () )
	{
		strText.Format( "[%d]", DxGlobalStage::GetInstance().GetChannel() );
		m_pChannelTextBox->SetOneLineText( strText, NS_UITEXTCOLOR::WHITE );

		strText.Format ( "%s %d/%d", m_strMapName, m_nMiniPosX, m_nMiniPosY );
		m_pMinimapTextBox->SetOneLineText ( strText, NS_UITEXTCOLOR::WHITE );
	}
	else
	{
		strText.Format ( "%s %d/%d", m_strMapName, m_nMiniPosX, m_nMiniPosY );
		m_pMinimapTextBox->SetOneLineText ( strText, NS_UITEXTCOLOR::WHITE );
	}
}

void CMiniMapInfo::SetMapPos ( float fCurX, float fCurY, int nMiniX, int nMiniY )
{
	if ( m_nMiniPosX != nMiniX || m_nMiniPosY != nMiniY )
	{
		m_nMiniPosX = nMiniX;
		m_nMiniPosY = nMiniY;

        UpdatePostion ();
	}
}

void CMiniMapInfo::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MINIMAP_CLUBMARK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{				
				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( m_strClubName, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;
	}
}

void CMiniMapInfo::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	// Player Position Update 
	static D3DXVECTOR3 vPlayerPosBACK;
	const D3DXVECTOR3 &vPlayerPos = GLGaeaClient::GetInstance().GetCharacterPos ();
	if ( vPlayerPos != vPlayerPosBACK )
	{
		vPlayerPosBACK = vPlayerPos;

		int nPosX(0), nPosY(0);
		PLANDMANCLIENT pLandClient = GLGaeaClient::GetInstance().GetActiveMap();
		if ( pLandClient )
		{
			GLMapAxisInfo &sMapAxisInfo = pLandClient->GetMapAxisInfo();
			sMapAxisInfo.Convert2MapPos ( vPlayerPos.x, vPlayerPos.z, nPosX, nPosY );

			SetMapPos ( vPlayerPos.x, vPlayerPos.z, nPosX, nPosY );
		}
	}

	const CBasicTextBox::STEXTPART & sTEXTPART = m_pMinimapTextBox->GetTextPart ( 0 );
	INT nChannel = DxGlobalStage::GetInstance().GetChannel();
	D3DXVECTOR2 vPos;

	//	NOTE
	//		클럽
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap ();
	if ( pLand && pLand->m_dwGuidClub!=CLUB_NULL )
	{
		m_dwVer = pLand->m_dwGuidClubMarkVer;
		m_nID = pLand->m_dwGuidClub;
		m_nServer = static_cast<int>(GLGaeaClient::GetInstance().GetCharacter()->m_dwServerID);
		m_strClubName = pLand->m_szGuidClubName;
		
		const UIRECT& rcGlobalPosClubMark = m_pClubMark->GetGlobalPos();
		vPos.x = sTEXTPART.m_rcPart.left - (rcGlobalPosClubMark.sizeX+3.0f);

#if defined( VN_PARAM ) 
		vPos.y = sTEXTPART.m_rcPart.top + 2.0f;
#else
		vPos.y = sTEXTPART.m_rcPart.top + 1.0f;
#endif

		m_pClubMark->SetGlobalPos ( vPos );
		m_pClubMark->SetVisibleSingle ( TRUE );

		if( nChannel != -1) // 채널
		{
			const CBasicTextBox::STEXTPART & sCTEXTPART = m_pChannelTextBox->GetTextPart ( 0 );

			vPos.x = vPos.x - ( sCTEXTPART.m_rcPart.sizeX + 3.0f );
			vPos.y = sCTEXTPART.m_rcPart.top;

			m_pChannelTextBox->SetGlobalPos( vPos );
			m_pChannelTextBox->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pChannelTextBox->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pClubMark->SetVisibleSingle ( FALSE );

		if( nChannel != -1) // 채널
		{
			const CBasicTextBox::STEXTPART & sCTEXTPART = m_pChannelTextBox->GetTextPart ( 0 );

			vPos.x = sTEXTPART.m_rcPart.left - ( sCTEXTPART.m_rcPart.sizeX + 3.0f );
			vPos.y = sTEXTPART.m_rcPart.top;

			m_pChannelTextBox->SetGlobalPos( vPos );
			m_pChannelTextBox->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pChannelTextBox->SetVisibleSingle( FALSE );
		}
	}
}

HRESULT CMiniMapInfo::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr = S_OK;

	BOOL bClubMarkVisible = m_pClubMark->IsVisible ();
	BOOL bChannelVisible = m_pChannelTextBox->IsVisible();

	m_pClubMark->SetVisibleSingle ( FALSE ); // MEMO
	m_pChannelTextBox->SetVisibleSingle( FALSE );
	{
		hr = CUIGroup::Render ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}
	m_pClubMark->SetVisibleSingle ( bClubMarkVisible );
	m_pChannelTextBox->SetVisibleSingle( bChannelVisible );
	
	if ( bClubMarkVisible )
	{
		const DxClubMan::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData ( pd3dDevice, m_nServer, m_nID, m_dwVer );

		m_pClubMark->SetTexturePos ( 0, sMarkData.vTex_1_LU );
		m_pClubMark->SetTexturePos ( 1, sMarkData.vTex_2_RU );
		m_pClubMark->SetTexturePos ( 3, sMarkData.vTex_3_LD );
		m_pClubMark->SetTexturePos ( 2, sMarkData.vTex_4_RD );
		
		m_pClubMark->SetTexture ( sMarkData.pddsTexture );
		hr = m_pClubMark->Render ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	if( bChannelVisible )
	{
		hr = m_pChannelTextBox->Render ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

void CMiniMapInfo::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pChannelTextBox = new CBasicTextBox;
	pChannelTextBox->CreateSub( this, "MINIMAP_CHANNEL" );
	pChannelTextBox->SetFont( pFont );
	pChannelTextBox->SetTextAlign( TEXT_ALIGN_BOTH_X );
	pChannelTextBox->SetUseRender( TRUE );
	pChannelTextBox->SetDiffuseAlpha( 0 );
	pChannelTextBox->SetVisibleSingle( FALSE );
	RegisterControl( pChannelTextBox );
	m_pChannelTextBox = pChannelTextBox;

	CUIControl* pClubMark = new CUIControl;
	pClubMark->CreateSub( this, "MINIMAP_CLUBMARK", UI_FLAG_DEFAULT, MINIMAP_CLUBMARK );
	pClubMark->SetUseRender( TRUE );
	pClubMark->SetVisibleSingle( FALSE );
	RegisterControl( pClubMark );
	m_pClubMark = pClubMark;

	CBasicTextBox* pMinimapTextBox = new CBasicTextBox;
	pMinimapTextBox->CreateSub ( this, "MINIMAP_POSITION" );
	pMinimapTextBox->SetFont ( pFont );
	pMinimapTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pMinimapTextBox );
	m_pMinimapTextBox = pMinimapTextBox;
}