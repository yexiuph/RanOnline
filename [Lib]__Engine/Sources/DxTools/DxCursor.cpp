#include "pch.h"

#include "./TextureManager.h"

#include "../[Lib]__MfcEx/Sources/RANPARAM.h"
#include "DxVertexFVF.h"
#include "DxDynamicVB.h"

#include "DxCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------------------	ANICURSOR
DxCursor::ANICURSOR::ANICURSOR() :
	m_bAni(FALSE),
	m_dwSize(1),
	m_pTexData(NULL)
{
}

DxCursor::ANICURSOR::~ANICURSOR()
{
	SAFE_DELETE_ARRAY( m_pTexData );
}

//------------------------------------------------------------------------------------------	DxCursor
//	Note : 싱글톤 클래스 제어.
//
DxCursor& DxCursor::GetInstance()
{
	static DxCursor Instance;
	return Instance;
}

DxCursor::DxCursor() :
	m_fTime(0.f),
	m_dwCurIndex(0),
	m_dwWidth(32),
	m_dwHeight(32),
	m_dwGridW(7),
	m_dwTexSizeW(256),
	m_dwTexSizeH(128),
	m_pSavedSB(NULL),
	m_pDrawSB(NULL),
	m_bWindowCursor(FALSE)
{
	//for( DWORD i=0; i<EMCS_SIZE; ++i )
	//{
	//	m_sAniCursor[i].m_bAni = FALSE;
	//	m_sAniCursor[i].m_dwSize = 1;
	//	m_sAniCursor[i].m_pTexData = new TexData[ m_sAniCursor[i].m_dwSize ];
	//}

	//// Note : Select 작업시.
	//m_sAniCursor[EMCS_SELECT].m_bAni = TRUE;
	//m_sAniCursor[EMCS_SELECT].m_dwSize = 16;	// 갯수 셋팅.

	//SAFE_DELETE_ARRAY( m_sAniCursor[EMCS_SELECT].m_pTexData );
	//m_sAniCursor[EMCS_SELECT].m_pTexData = new TexData[ m_sAniCursor[EMCS_SELECT].m_dwSize ];

	//// Note : 위치셋팅.
	//m_sAniCursor[EMCS_NORMAL].m_pTexData->m_dwIndexX = 3;
	//m_sAniCursor[EMCS_NORMAL].m_pTexData->m_dwIndexY = 2;

	//m_sAniCursor[EMCS_ATTACK].m_pTexData->m_dwIndexX = 4;
	//m_sAniCursor[EMCS_ATTACK].m_pTexData->m_dwIndexY = 2;

	//m_sAniCursor[EMCS_SCROLL].m_pTexData->m_dwIndexX = 5;
	//m_sAniCursor[EMCS_SCROLL].m_pTexData->m_dwIndexY = 2;

	//m_sAniCursor[EMCS_TALK].m_pTexData->m_dwIndexX = 6;
	//m_sAniCursor[EMCS_TALK].m_pTexData->m_dwIndexY = 2;

	//for( DWORD i=0; i<m_sAniCursor[EMCS_SELECT].m_dwSize; ++i )
	//{
	//	m_sAniCursor[EMCS_SELECT].m_pTexData[i].m_dwIndexX	= i%m_dwGridW;
	//	m_sAniCursor[EMCS_SELECT].m_pTexData[i].m_dwIndexY	= i/m_dwGridW;
	//}
}

DxCursor::~DxCursor()
{
	//for( DWORD i=0; i<EMCS_SIZE; ++i )
	//{
	//	SAFE_DELETE_ARRAY( m_sAniCursor[i].m_pTexData );
	//}
}

void DxCursor::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd )
{
	//m_hWnd = hWnd;
	//TextureManager::LoadTexture( _T("Cursor.dds"), pd3dDevice, m_pTexture, 0L, 0L );

	//// Note : TexSize를 구함.
	//m_vTexSizeUV.x = (float)m_dwWidth / m_dwTexSizeW;
	//m_vTexSizeUV.y = (float)m_dwHeight / m_dwTexSizeH;

	//// Note : Tex UV 셋팅
	//m_sAniCursor[EMCS_NORMAL].m_pTexData->m_vTexUV = GetTexUV( m_sAniCursor[EMCS_NORMAL].m_pTexData->m_dwIndexX,	m_sAniCursor[EMCS_NORMAL].m_pTexData->m_dwIndexY );
	//m_sAniCursor[EMCS_ATTACK].m_pTexData->m_vTexUV = GetTexUV( m_sAniCursor[EMCS_ATTACK].m_pTexData->m_dwIndexX,	m_sAniCursor[EMCS_ATTACK].m_pTexData->m_dwIndexY );
	//m_sAniCursor[EMCS_SCROLL].m_pTexData->m_vTexUV = GetTexUV( m_sAniCursor[EMCS_SCROLL].m_pTexData->m_dwIndexX,	m_sAniCursor[EMCS_SCROLL].m_pTexData->m_dwIndexY );
	//m_sAniCursor[EMCS_TALK].m_pTexData->m_vTexUV	= GetTexUV( m_sAniCursor[EMCS_TALK].m_pTexData->m_dwIndexX,		m_sAniCursor[EMCS_TALK].m_pTexData->m_dwIndexY );

	//for( DWORD i=0; i<m_sAniCursor[EMCS_SELECT].m_dwSize; ++i )
	//{
	//	m_sAniCursor[EMCS_SELECT].m_pTexData[i].m_vTexUV = GetTexUV( m_sAniCursor[EMCS_SELECT].m_pTexData[i].m_dwIndexX,	m_sAniCursor[EMCS_SELECT].m_pTexData[i].m_dwIndexY );
	//}
}

void DxCursor::OnDestroyDevice()
{
	//TextureManager::ReleaseTexture( _T("Cursor.dds"), m_pTexture );
}

void DxCursor::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//for( UINT which=0; which<2; which++ )
	//{
	//	pd3dDevice->BeginStateBlock();

	//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
	//	pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
	//	pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );
	//	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
	//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x33 );

	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
	//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

	//	if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
	//	else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	//}
}

void DxCursor::OnLostDevice()
{
	//SAFE_RELEASE( m_pSavedSB );
	//SAFE_RELEASE( m_pDrawSB );
}

D3DXVECTOR2 DxCursor::GetTexUV( DWORD i, DWORD j )
{
	// 맨 모서리 위치를 구함.
	D3DXVECTOR2 vTexUV;
	vTexUV.x = float(i*(m_dwWidth+1) + 1) / m_dwTexSizeW;
	vTexUV.y = float(j*(m_dwHeight+1) + 1) / m_dwTexSizeH;

	return vTexUV;
}

void DxCursor::SetCursorType( CURSOR_TYPE emCursor )
{
	//if( m_emCursorType != emCursor )
	//{
	//	m_fTime = 0.f;
	//	m_dwCurIndex = 0;
	//	m_emCursorType = emCursor;
	//}
}

void DxCursor::FrameMove( const float fElapsedTime )
{
	//if( m_bWindowCursor )
	//{
	//	if( fElapsedTime < 0.05f )		// 20fps UP
	//	{
	//		m_bWindowCursor = FALSE;
	//	}
	//}
	//else
	//{
	//	if( fElapsedTime > 0.1f )		// 20fps DOWN
	//	{
	//		m_bWindowCursor = TRUE;
	//	}
	//}

	//if( m_sAniCursor[m_emCursorType].m_bAni )
	//{
	//	m_fTime += fElapsedTime;
	//	if( m_fTime > 0.1f )
	//	{
	//		m_fTime = 0.f;
	//		++m_dwCurIndex;
	//		if( m_dwCurIndex >= m_sAniCursor[m_emCursorType].m_dwSize )
	//		{
	//			m_dwCurIndex = 0;
	//		}
	//	}
	//}
}

void DxCursor::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//HRESULT hr(S_OK);

	//if( !RANPARAM::bGameCursor )	return;
	//if( m_bWindowCursor )			return;

	///*
	//GetWindowRect
	//윈도우의 현재 위치와 크기를 구해준다. (left, top)은 윈도우의 현재 좌상단 위치를 나타내는데 이 좌표는 전체 화면을 기준으로 한 좌표이다.
	//(right, bottom)은 윈도우의 우하단 위치를 나타내며 역시 전체 화면을 기준으로 한 좌표이다. 윈도우의 현재 크기(폭과 높이)를 구하고 싶으면 
	//right-left, bottom-top을 계산하면 된다.

	//GetClientRect
	//윈도우의 작업영역 크기를 계산해 준다. 크기만 계산해 주기 때문에 좌상단(left, top)값은 항상 0,0이며 우하단 좌표(right, bottom)가 곧 윈도우의 크기를 나타낸다. 
	//작업영역이란 윈도우의 타이틀바, 스크롤 바, 경계선, 메뉴 등을 제외한 영역이며 윈도우가 그리기를 하는 대상 영역이다

	//ScreenToClient
	//화면의 원점을 기준으로 하는 좌표 lpPoint를 hWnd의 작업 영역을 기준으로 하는 좌표로 변환한다. 
	//hWnd윈도우의 작업 영역 원점의 화면 좌표가 cx, cy일 때 lpPoint는 lpPoint.x + cx, lpPoint + cy로 변환된다.
	//GetCursorPos, MoveWindow, GetWindowRect 등과 같이 화면 좌표를 리턴하는 함수로부터 작업 영역의 좌표로 변환하고자 할 때 이 함수를 사용한다.
	//*/

	//RECT rtWindow;
	//::GetClientRect( m_hWnd, &rtWindow );

	//POINT ptCursorPos;
	//::GetCursorPos ( &ptCursorPos );

	//POINT ptTOP;
	//ptTOP.x = 0;
	//ptTOP.y = 0;
	//::ClientToScreen( m_hWnd, &ptTOP );

	//bool bMouseIn = ptTOP.x<ptCursorPos.x && ptCursorPos.x<(ptTOP.x+rtWindow.right) &&
	//				ptTOP.y<ptCursorPos.y && ptCursorPos.y<(ptTOP.y+rtWindow.bottom);

	//// Note : 화면 안에 없다면 렌더하지 않는다.
	//if( !bMouseIn )	return;

	//ptCursorPos.x -= ptTOP.x;
	//ptCursorPos.y -= ptTOP.y;


	//DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
	//DWORD dwVertexSizeFULL = sizeof(VERTEXRHW)*4;
	//if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
	//{
	//	dwFlag = D3DLOCK_DISCARD; 
	//	DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
	//	DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
	//}

	//VERTEXRHW *pVertices;
	//hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
	//if ( FAILED(hr) )	return;

	//pVertices[0].vPos = D3DXVECTOR4( float(ptCursorPos.x)-0.5f,				float(ptCursorPos.y)-0.5f, 1.f, 1.f );
	//pVertices[1].vPos = D3DXVECTOR4( float(ptCursorPos.x+m_dwWidth)-0.5f,	float(ptCursorPos.y)-0.5f, 1.f, 1.f );
	//pVertices[2].vPos = D3DXVECTOR4( float(ptCursorPos.x)-0.5f,				float(ptCursorPos.y+m_dwHeight)-0.5f, 1.f, 1.f );
	//pVertices[3].vPos = D3DXVECTOR4( float(ptCursorPos.x+m_dwWidth)-0.5f,	float(ptCursorPos.y+m_dwHeight)-0.5f, 1.f, 1.f );

	//pVertices[0].vPos = D3DXVECTOR4( float(ptCursorPos.x)-0.5f,				float(ptCursorPos.y)-0.5f, 1.f, 1.f );
	//pVertices[1].vPos = D3DXVECTOR4( float(ptCursorPos.x+m_dwWidth)-0.5f,	float(ptCursorPos.y)-0.5f, 1.f, 1.f );
	//pVertices[2].vPos = D3DXVECTOR4( float(ptCursorPos.x)-0.5f,				float(ptCursorPos.y+m_dwHeight)-0.5f, 1.f, 1.f );
	//pVertices[3].vPos = D3DXVECTOR4( float(ptCursorPos.x+m_dwWidth)-0.5f,	float(ptCursorPos.y+m_dwHeight)-0.5f, 1.f, 1.f );

	//if( m_sAniCursor[m_emCursorType].m_bAni )
	//{
	//	pVertices[0].vTex = m_sAniCursor[m_emCursorType].m_pTexData[m_dwCurIndex].m_vTexUV;
	//	pVertices[1].vTex = m_sAniCursor[m_emCursorType].m_pTexData[m_dwCurIndex].m_vTexUV + D3DXVECTOR2(m_vTexSizeUV.x,0.f);
	//	pVertices[2].vTex = m_sAniCursor[m_emCursorType].m_pTexData[m_dwCurIndex].m_vTexUV + D3DXVECTOR2(0.f,m_vTexSizeUV.y);
	//	pVertices[3].vTex = m_sAniCursor[m_emCursorType].m_pTexData[m_dwCurIndex].m_vTexUV + D3DXVECTOR2(m_vTexSizeUV.x,m_vTexSizeUV.y);
	//}
	//else
	//{
	//	pVertices[0].vTex = m_sAniCursor[m_emCursorType].m_pTexData->m_vTexUV;
	//	pVertices[1].vTex = m_sAniCursor[m_emCursorType].m_pTexData->m_vTexUV + D3DXVECTOR2(m_vTexSizeUV.x,0.f);
	//	pVertices[2].vTex = m_sAniCursor[m_emCursorType].m_pTexData->m_vTexUV + D3DXVECTOR2(0.f,m_vTexSizeUV.y);
	//	pVertices[3].vTex = m_sAniCursor[m_emCursorType].m_pTexData->m_vTexUV + D3DXVECTOR2(m_vTexSizeUV.x,m_vTexSizeUV.y);
	//}

	//DxDynamicVB::m_sVB_PDT.pVB->Unlock ();


	//m_pSavedSB->Capture();
	//m_pDrawSB->Apply();
 //
	//pd3dDevice->SetFVF ( VERTEXRHW::FVF );
	//pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(VERTEXRHW) );
	//pd3dDevice->SetTexture( 0, m_pTexture );

	//pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, 2 );

	//m_pSavedSB->Apply();

	//// Note : 값을 더해줌. 
	//DxDynamicVB::m_sVB_PDT.nVertexCount += 4;
	//DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
}




