#include "pch.h"

#include "UIControl.h"
#include "DxInputDevice.h"
#include "TextureManager.h" //../[Lib]__Engine/Sources/DxCommon
#include "InterfaceCfg.h"
#include "UIMan.h"
#include "UIRenderQueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CUIControl::SetFadeIn ()
{
	if ( !m_bTransparency )
	{
		GASSERT ( 0 && "알람입니다. - Transparency가 FALSE입니다." );
		return FALSE;
	}

	if ( m_VisibleState == VISIBLE_MODE || m_VisibleState == FADEIN_MODE )
	{
		return TRUE;
	}

	m_VisibleState = FADEIN_MODE;
	m_bVisible = TRUE;
	m_fFadeElapsedTime = 0;
	m_fVisibleRate = 0.0f;

	return TRUE;
}

BOOL CUIControl::SetFadeOut()
{
	if ( !m_bTransparency )
	{
		GASSERT ( 0 && "알람입니다. - Transparency가 FALSE입니다." );
		return FALSE;
	}

	if ( m_VisibleState == INVISIBLE_MODE || m_VisibleState == FADEOUT_MODE )
	{
		return TRUE;
	}

	m_VisibleState = FADEOUT_MODE;
	m_fFadeElapsedTime = 0;
	m_fVisibleRate = 1.0f;

	return TRUE;
}

HRESULT	CUIControl::LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( pd3dDevice && m_strTextureName.GetLength () )
	{
		//HRESULT hr = S_OK;
		TextureManager::LoadTexture( m_strTextureName.GetString(), pd3dDevice, m_pTexture, 0, 0 );
		//if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIControl::UnLoadTexture ()
{
	// MEMO : ReleaseTexture에서 m_pTexture를 NULL로 만든다.
	//HRESULT hr = S_OK;
	TextureManager::ReleaseTexture ( m_strTextureName.GetString(), m_pTexture );
	//if ( FAILED ( hr ) ) return hr;
	
	return S_OK;
}

HRESULT CUIControl::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return LoadTexture( pd3dDevice );
}

HRESULT CUIControl::DeleteDeviceObjects()
{	
	return UnLoadTexture ();
}

HRESULT CUIControl::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	if( IsNoRender() )
		return S_OK;

	if( CUIRenderQueue::Get()->Render( m_pTexture, m_UIVertex ) )
		return S_OK;

	HRESULT hr = pd3dDevice->SetTexture ( 0, m_pTexture );
	if ( FAILED ( hr ) )
		return hr;

	hr = pd3dDevice->SetFVF ( UIVERTEX::D3DFVF_UIVERTEX );
	if ( FAILED ( hr ) )
		return hr;

	hr = pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, (void*)m_UIVertex, sizeof(UIVERTEX) );
	if ( FAILED ( hr ) )
		return hr;

	return S_OK;
}

HRESULT CUIControl::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pD3DDevice)
{
	if( !GetParent() )
	{
		UIRECT rcPos = GetLocalPos ();
		UIRECT rcAlignPos = AlignMainControl ( rcPos );
		BOOL bCheckProtectSize = m_bCheckProtectSize;
		m_bCheckProtectSize = FALSE;
		CUIControl::SetGlobalPos ( rcAlignPos );
		m_bCheckProtectSize = bCheckProtectSize;
	}

	return S_OK;
}

void CUIControl::Reset()
{	
	m_fFadeElapsedTime = 0.0f;
}

void CUIControl::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	const UIRECT& rcOldPos = GetLocalPos ();
	UIRECT	rcNewPos = rcOldPos;

	const WORD wFlag = GetAlignFlag ();

    if ( wFlag & UI_FLAG_RIGHT )
	{
		float fRightGap = rcParentOldPos.sizeX - ( rcOldPos.left - rcParentOldPos.left );
		rcNewPos.left = rcParentNewPos.right - fRightGap;
	}
	else if ( wFlag & UI_FLAG_CENTER_X )
	{
		rcNewPos.left = (rcParentOldPos.sizeX - rcNewPos.sizeX) / 2.f;
	}

	if ( wFlag & UI_FLAG_BOTTOM )
	{
		float fBottomGap = rcParentOldPos.sizeY - ( rcOldPos.top - rcParentOldPos.top );
		rcNewPos.top = rcParentNewPos.bottom - fBottomGap;
	}
	else if ( wFlag & UI_FLAG_CENTER_Y )
	{
		rcNewPos.top = (rcParentOldPos.sizeY - rcNewPos.sizeY) / 2.f;
	}

	if ( wFlag & UI_FLAG_XSIZE )
	{
		rcNewPos.sizeX = rcParentNewPos.sizeX - (rcParentOldPos.sizeX - rcOldPos.sizeX);
	}
	else if ( wFlag & UI_FLAG_XSIZE_PROPORTION )
	{		
		rcNewPos.sizeX = rcParentNewPos.sizeX * rcOldPos.sizeX / rcParentOldPos.sizeX;
	}

	if ( wFlag & UI_FLAG_YSIZE )
	{
		rcNewPos.sizeY = rcParentNewPos.sizeY - (rcParentOldPos.sizeY - rcOldPos.sizeY);
	}
	else if ( wFlag & UI_FLAG_YSIZE_PROPORTION )
	{
		rcNewPos.sizeY = rcParentNewPos.sizeY * rcOldPos.sizeY / rcParentOldPos.sizeY;
	}

	m_rcLocalPos.SetPosition( rcNewPos.left, rcNewPos.top, rcNewPos.sizeX, rcNewPos.sizeY );

	CheckProtectSize (); // MEMO : 사이즈를 일정 크기로 유지한다.
}

void CUIControl::ReSizeControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	const UIRECT& rcOldPos = GetLocalPos ();
	UIRECT	rcNewPos = rcOldPos;

	UIRECT	rcGlobalPos = GetGlobalPos ();


	float fWidth = (  rcParentNewPos.sizeX / rcParentOldPos.sizeX );
	float fHeight = ( rcParentNewPos.sizeY / rcParentOldPos.sizeY );

	rcNewPos.sizeX = rcOldPos.sizeX * fWidth ;
	rcNewPos.sizeY = rcOldPos.sizeY * fHeight ;
	rcNewPos.left = rcOldPos.left * fWidth;
	rcNewPos.top = rcOldPos.top * fHeight;
	rcNewPos.right = rcNewPos.left + rcNewPos.sizeX ;
	rcNewPos.bottom = rcNewPos.top + rcNewPos.sizeY;


//	if ( rcGlobalPos.left <  rcOldPos.left ) 
//	{
//		rcGlobalPos.left = rcOldPos.left;
//	}
//	if ( rcGlobalPos.top < rcOldPos.top )
//	{
//		rcGlobalPos.top = rcOldPos.top;
//	}
	

	rcGlobalPos.sizeX = rcNewPos.sizeX;
	rcGlobalPos.sizeY = rcNewPos.sizeY;
	rcGlobalPos.left =  rcParentNewPos.left + rcNewPos.left;
//	rcGlobalPos.left =  rcGlobalPos.left + ( rcNewPos.left - rcOldPos.left);
	rcGlobalPos.right = rcGlobalPos.left + rcGlobalPos.sizeX;
	rcGlobalPos.top =  rcParentNewPos.top + rcNewPos.top;
//	rcGlobalPos.top =  rcGlobalPos.top + ( rcNewPos.top - rcOldPos.top );
	rcGlobalPos.bottom =  rcGlobalPos.top + rcGlobalPos.sizeY;

	SetLocalPos( rcNewPos );
	SetGlobalPos ( rcGlobalPos );

	


/*
	if ( wFlag & UI_FLAG_RIGHT )
	{
		float fRightGap = rcParentOldPos.sizeX - ( rcOldPos.left - rcParentOldPos.left );
		rcNewPos.left = rcParentNewPos.right - fRightGap;
	}
	else if ( wFlag & UI_FLAG_CENTER_X )
	{
		rcNewPos.left = (rcParentOldPos.sizeX - rcNewPos.sizeX) / 2.f;
	}

	if ( wFlag & UI_FLAG_BOTTOM )
	{
		float fBottomGap = rcParentOldPos.sizeY - ( rcOldPos.top - rcParentOldPos.top );
		rcNewPos.top = rcParentNewPos.bottom - fBottomGap;
	}
	else if ( wFlag & UI_FLAG_CENTER_Y )
	{
		rcNewPos.top = (rcParentOldPos.sizeY - rcNewPos.sizeY) / 2.f;
	}

	if ( wFlag & UI_FLAG_XSIZE )
	{
		rcNewPos.sizeX = rcParentNewPos.sizeX - (rcParentOldPos.sizeX - rcOldPos.sizeX);
	}
	else if ( wFlag & UI_FLAG_XSIZE_PROPORTION )
	{		
		rcNewPos.sizeX = rcParentNewPos.sizeX * rcOldPos.sizeX / rcParentOldPos.sizeX;
	}

	if ( wFlag & UI_FLAG_YSIZE )
	{
		rcNewPos.sizeY = rcParentNewPos.sizeY - (rcParentOldPos.sizeY - rcOldPos.sizeY);
	}
	else if ( wFlag & UI_FLAG_YSIZE_PROPORTION )
	{
		rcNewPos.sizeY = rcParentNewPos.sizeY * rcOldPos.sizeY / rcParentOldPos.sizeY;
	}
*/
//	m_rcGlobalPos.SetPosition( rcNewPos.left, rcNewPos.top, rcNewPos.sizeX, rcNewPos.sizeY );

//	CheckProtectSize (); // MEMO : 사이즈를 일정 크기로 유지한다.
}

UIRECT CUIControl::AlignMainControl ( const UIRECT& rcPos )
{
	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	UIRECT rcNewPos = rcPos;
	const WORD wFlag = GetAlignFlag ();

	if ( wFlag & UI_FLAG_RIGHT )
	{
		rcNewPos.left = (float)X_RES - HIWORD( UI_DEFRESOLUTION ) + rcPos.left;
	}
	else if ( wFlag & UI_FLAG_CENTER_X )
	{
		rcNewPos.left = ((float)X_RES - rcPos.sizeX)/2.f;
	}

	if ( wFlag & UI_FLAG_BOTTOM )
	{
		rcNewPos.top = (float)Y_RES - LOWORD( UI_DEFRESOLUTION ) + rcPos.top;
	}
	else if ( wFlag & UI_FLAG_CENTER_Y )
	{
		rcNewPos.top = ((float)Y_RES - rcPos.sizeY)/2.f;
	}

	if ( wFlag & UI_FLAG_XSIZE )
	{
		rcNewPos.sizeX = (float)X_RES - HIWORD( UI_DEFRESOLUTION ) + rcPos.sizeX;
	}

	if ( wFlag & UI_FLAG_YSIZE )
	{
		rcNewPos.sizeY = (float)Y_RES - LOWORD( UI_DEFRESOLUTION ) + rcPos.sizeY;
	}

	return rcNewPos;
}

void CUIControl::CheckBoundary ()
{
	long lResolution = CUIMan::GetResolution ();

	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	if ( m_rcGlobalPos.left < 0 )
	{
		m_rcGlobalPos.left = 0;
		m_rcGlobalPos.right = m_rcGlobalPos.sizeX;
	}
	if ( m_rcGlobalPos.right > X_RES )
	{
		m_rcGlobalPos.left = X_RES - m_rcGlobalPos.sizeX;
		m_rcGlobalPos.right = X_RES;
	}
	if ( m_rcGlobalPos.top < 0 )
	{
		m_rcGlobalPos.top = 0;
		m_rcGlobalPos.bottom = m_rcGlobalPos.sizeY;
	}
	if ( m_rcGlobalPos.bottom > Y_RES )
	{
		m_rcGlobalPos.top = Y_RES - m_rcGlobalPos.sizeY;
		m_rcGlobalPos.bottom = Y_RES;
	}
}

void CUIControl::SetProtectSizePoint ( const D3DXVECTOR2 vSIZE ) 
{ // MEMO : 일정 크기 이하로 더이상 작아지지 않도록 한다.
	m_vProtectSize.x = vSIZE.x;
	m_vProtectSize.y = vSIZE.y;

	m_bCheckProtectSize = TRUE;
}

void CUIControl::SetProtectSizePoint ()
{
	if ( m_rcLocalPos.sizeX != m_rcGlobalPos.sizeX )
	{
		m_rcGlobalPos.sizeX = m_rcLocalPos.sizeX;
	}

	if ( m_rcLocalPos.sizeY != m_rcGlobalPos.sizeY )
	{
		m_rcGlobalPos.sizeY = m_rcLocalPos.sizeY;
	}

	m_vProtectSize.x = m_rcLocalPos.sizeX;
	m_vProtectSize.y = m_rcLocalPos.sizeY;

	m_bCheckProtectSize = TRUE;
}

void CUIControl::CheckProtectSize ()
{
	if ( !m_bCheckProtectSize ) return ;

	if ( m_rcGlobalPos.sizeX < m_vProtectSize.x ) // 글로벌 사이즈가 로컬 사이즈 보다 작다면
	{
		m_rcGlobalPos.sizeX = m_vProtectSize.x; // 클로벌 사이즈를 최소한 로컬사이즈와 같도록 만든다.
		m_rcGlobalPos.right = m_rcGlobalPos.left + m_rcGlobalPos.sizeX; // 크기에 맞게 좌표 수정
	}

	if ( m_rcGlobalPos.sizeY < m_vProtectSize.y )
	{
		m_rcGlobalPos.sizeY = m_vProtectSize.y;
		m_rcGlobalPos.bottom = m_rcGlobalPos.top + m_rcGlobalPos.sizeY;
	}

	if ( m_rcLocalPos.sizeX < m_vProtectSize.x )
	{		
		m_rcLocalPos.sizeX = m_vProtectSize.x;
		m_rcLocalPos.right = m_rcLocalPos.left + m_rcLocalPos.sizeX;
	}

	if ( m_rcLocalPos.sizeY < m_vProtectSize.y )
	{
		m_rcLocalPos.sizeY = m_vProtectSize.y;
		m_rcLocalPos.bottom = m_rcLocalPos.top + m_rcLocalPos.sizeY;
	}
}