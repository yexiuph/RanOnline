#include "pch.h"

#include "UIControl.h"
#include "DxInputDevice.h"
#include "TextureManager.h" //../[Lib]__Engine/Sources/DxCommon
#include "InterfaceCfg.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIControl::CUIControl()
	: m_WndID( 0 )
	, m_pTexture( NULL )
	, m_bVisible( TRUE )
	, m_fFadeElapsedTime( 0.f )
	, m_fFadeLimitTime( UI_FADETIME )
	, m_fVisibleRate ( 1.0f )
	, m_VisibleState( VISIBLE_MODE )
	, m_bTransparency( FALSE )
	, m_wAlignFlag( 0 )
	, m_bUseRender( TRUE )
	, m_pParent( NULL )
	, m_bCheckProtectSize( FALSE )
	, m_dwBeginAlpha( 255 )
	, m_bNO_UPDATE( FALSE )
	, m_dwMsg( 0 )
	, m_bExclusiveControl( FALSE )
	, m_bFocusControl( FALSE )
	, m_nFoldID( 0 )
{
}

CUIControl::~CUIControl()
{
	CUIControl::InvalidateDeviceObjects ();
	CUIControl::DeleteDeviceObjects ();
}


CUIControl*	CUIControl::GetTopParent ()
{	
	CUIControl* pCurNode = GetParent ();
	CUIControl* pParent = pCurNode;
	while ( pCurNode )
	{
		pParent = pCurNode;
		pCurNode = pCurNode->GetParent ();
	}

	return pParent;
}

void CUIControl::SetVisibleSingle(BOOL visible)
{
	m_bVisible = visible;
	m_fFadeElapsedTime = 0;

	if ( m_bVisible)
	{
		m_VisibleState = VISIBLE_MODE;
		m_fVisibleRate = 1.0f;
	}
	else
	{
		m_VisibleState = INVISIBLE_MODE;
		m_fVisibleRate = 0.0f;
	}
}

void CUIControl::SetVertexPos()
{
	m_UIVertex[0] = D3DXVECTOR2 ( m_rcGlobalPos.left,	m_rcGlobalPos.top );
	m_UIVertex[1] = D3DXVECTOR2 ( m_rcGlobalPos.right,	m_rcGlobalPos.top );
	m_UIVertex[2] = D3DXVECTOR2 ( m_rcGlobalPos.right,	m_rcGlobalPos.bottom );
	m_UIVertex[3] = D3DXVECTOR2 ( m_rcGlobalPos.left,	m_rcGlobalPos.bottom );
}

void CUIControl::SetVertexPos( float z )
{
	m_UIVertex[0].z = z;
	m_UIVertex[1].z = z;
	m_UIVertex[2].z = z;
	m_UIVertex[3].z = z;
}

void CUIControl::SetGlobalPos(const UIRECT& Pos)
{
	m_rcGlobalPos = Pos;
	CheckProtectSize();
	CheckBoundary();
	SetVertexPos();
}

void CUIControl::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	m_rcGlobalPos.left	=vPos.x;
	m_rcGlobalPos.right	=vPos.x + m_rcGlobalPos.sizeX; // MEMO : 여기도 -1이 들어가야 할거 같은데 변경 불가.
	m_rcGlobalPos.top	=vPos.y;
	m_rcGlobalPos.bottom=vPos.y + m_rcGlobalPos.sizeY;

    CheckBoundary (); // MEMO
	SetVertexPos();
}

void CUIControl::Create ( UIGUID WndID,	const char* szControlKeyword, WORD wAlignFlag )
{
	GASSERT( szControlKeyword );

	UIRECT rcPos;
	UIRECT rcTexPos;
	CString strTextureName;

	if( RANPARAM::bXML_USE == TRUE )
	{
		INTERFACE_CFG uiCfg;
		if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword, uiCfg ) ) return ;

		rcPos = uiCfg.rcControlPos;
		rcTexPos = uiCfg.rcTexturePos;
		strTextureName = uiCfg.strTextureName;
	}
	else
	{
		if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword ) ) return ;

		rcPos = CInterfaceCfg::GetInstance().GetPosition ();	
		rcTexPos = CInterfaceCfg::GetInstance().GetTexturePosition ();
		strTextureName = CInterfaceCfg::GetInstance().GetTextureName ();
	}

	SetWndID( WndID );
	SetAlignFlag( wAlignFlag );

	SetUseRender( strTextureName.GetLength () );
	SetControlNameEx( szControlKeyword );

	m_rcLocalPos = rcPos;	

	UIRECT rcMainPos = AlignMainControl ( rcPos );
	CUIControl::SetGlobalPos ( rcMainPos );
	CUIControl::SetTexturePos ( rcTexPos );
	CUIControl::SetTextureName ( strTextureName );
}

void CUIControl::CreateSub ( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag, UIGUID WndID )
{
	GASSERT( pParent && szControlKeyword );

	if ( !pParent )
	{
		GASSERT ( 0 && "pParent가 NULL입니다." );
		return ;
	}

	SetWndID ( WndID );
	SetAlignFlag ( wAlignFlag );
	SetParent ( pParent );

	UIRECT rcPos;
	UIRECT rcTexPos;
	CString strTextureName;

	if( RANPARAM::bXML_USE == TRUE )
	{
		INTERFACE_CFG uiCfg;
		if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword, uiCfg ) ) return ;

		rcPos = uiCfg.rcControlPos;
		rcTexPos = uiCfg.rcTexturePos;
		strTextureName = uiCfg.strTextureName;
	}
	else
	{
		if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword ) ) return ;

		rcPos = CInterfaceCfg::GetInstance().GetPosition ();	
		rcTexPos = CInterfaceCfg::GetInstance().GetTexturePosition ();
		strTextureName = CInterfaceCfg::GetInstance().GetTextureName ();
	}

	SetUseRender ( strTextureName.GetLength () );
	SetControlNameEx ( szControlKeyword );

	m_rcLocalPos = rcPos;

	UIRECT rcParentPos = pParent->GetGlobalPos ();

	UIRECT rcChildPos(	rcParentPos.left + rcPos.left, rcParentPos.top + rcPos.top,
						rcPos.sizeX, rcPos.sizeY );

	CUIControl::SetGlobalPos ( rcChildPos );
	CUIControl::SetTexturePos ( rcTexPos );
	CUIControl::SetTextureName ( strTextureName );
}

void CUIControl::CreateEx ( UIGUID WndID,	const char* szControlKeyword )
{
	GASSERT( szControlKeyword );

	INTERFACE_CFG uiCfg;
	if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword, uiCfg ) ) return ;

	SetWndID( WndID );
	SetAlignFlag( uiCfg.dwAlignFlag );

	SetUseRender( uiCfg.strTextureName.GetLength () );
	SetControlNameEx( szControlKeyword );

	m_rcLocalPos = uiCfg.rcControlPos;	

	UIRECT rcMainPos = AlignMainControl ( uiCfg.rcControlPos );
	CUIControl::SetGlobalPos ( rcMainPos );
	CUIControl::SetTexturePos ( uiCfg.rcTexturePos );
	CUIControl::SetTextureName ( uiCfg.strTextureName );
}

void CUIControl::CreateSubEx ( CUIControl* pParent, const char* szControlKeyword, UIGUID WndID )
{
	GASSERT( pParent && szControlKeyword );

	if ( !pParent )
	{
		GASSERT ( 0 && "pParent가 NULL입니다." );
		return ;
	}

	INTERFACE_CFG uiCfg;
	if( !CInterfaceCfg::GetInstance().ArrangeInfo( szControlKeyword, uiCfg ) ) return ;

	SetWndID ( WndID );
	SetParent ( pParent );
	SetAlignFlag ( uiCfg.dwAlignFlag );

	SetUseRender ( uiCfg.strTextureName.GetLength () );
	SetControlNameEx ( szControlKeyword );

	m_rcLocalPos = uiCfg.rcControlPos;

	UIRECT rcParentPos = pParent->GetGlobalPos ();

	UIRECT rcChildPos(	rcParentPos.left + uiCfg.rcControlPos.left, rcParentPos.top + uiCfg.rcControlPos.top,
						uiCfg.rcControlPos.sizeX, uiCfg.rcControlPos.sizeY );

	CUIControl::SetGlobalPos ( rcChildPos );
	CUIControl::SetTexturePos ( uiCfg.rcTexturePos );
	CUIControl::SetTextureName ( uiCfg.strTextureName );
}

void CUIControl::SetTexturePos ( const UIRECT& rcTexPos )
{
	m_rcTexurePos = rcTexPos;

	m_UIVertex[0].SetTexturePos(m_rcTexurePos.left, m_rcTexurePos.top);
	m_UIVertex[1].SetTexturePos(m_rcTexurePos.right, m_rcTexurePos.top);
	m_UIVertex[2].SetTexturePos(m_rcTexurePos.right, m_rcTexurePos.bottom);
	m_UIVertex[3].SetTexturePos(m_rcTexurePos.left, m_rcTexurePos.bottom);
}

void CUIControl::SetTexturePos(int index, const D3DXVECTOR2& vPos)
{
	m_UIVertex[index].SetTexturePos(vPos.x, vPos.y);
}

void CUIControl::SetDiffuse(D3DCOLOR _diffuse)
{
	m_UIVertex[0].SetDiffuse(_diffuse);
	m_UIVertex[1].SetDiffuse(_diffuse);
	m_UIVertex[2].SetDiffuse(_diffuse);
	m_UIVertex[3].SetDiffuse(_diffuse);
}

void CUIControl::SetTextureName ( const CString& strTextureName )
{
	if ( !strTextureName.IsEmpty() )	m_strTextureName = strTextureName;	
	else								m_strTextureName.Empty ();
}

void CUIControl::SetLocalPos ( const D3DXVECTOR2& vPos )
{
	UIRECT rcLocalPosNew = GetLocalPos();
	rcLocalPosNew.left = vPos.x;
	rcLocalPosNew.top = vPos.y;
	SetLocalPos( rcLocalPosNew );
}

BOOL CUIControl::SetFade ( BOOL bFadeIn )
{	
	if ( bFadeIn )	return CUIControl::SetFadeIn ();
	else			return CUIControl::SetFadeOut();
}

BOOL CUIControl::IsFading()
{
	if ( m_VisibleState == FADEIN_MODE || 
		 m_VisibleState == FADEOUT_MODE )
			return TRUE;
	else	return FALSE;
}

void CUIControl::SetTransparentOption ( BOOL bTransparency )
{
	m_bTransparency = bTransparency;
}