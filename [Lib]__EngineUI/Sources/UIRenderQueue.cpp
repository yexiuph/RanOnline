#include "pch.h"
#include "UIControl.h"
#include "UIRenderQueue.h"

#define VERTEX_COUNT	1024 * 3			// 버텍스 수 = 프리미티브 수 * 3

//--------------------------------------------------------------------
// UIRenderQueue
//--------------------------------------------------------------------
BOOL CUIRenderQueue::m_bUsage = TRUE;
CUIRenderQueue* CUIRenderQueue::m_pInstance = NULL;

CUIRenderQueue::CUIRenderQueue()
	: m_pd3dDevice( NULL )
	, m_pTexture( NULL )
	, m_pVertexBuffer( NULL )

	, m_pVertex( NULL )
	, m_uiOffset( 0 )
	, m_uiVertex( 0 )
	, m_uiPrimitiv( 0 )
	, m_dwLockFlag( 0 )
{
	if( m_bUsage )
		m_pVertex = new UIVERTEX[ VERTEX_COUNT ];
}

CUIRenderQueue::~CUIRenderQueue()
{
	SAFE_DELETE_ARRAY( m_pVertex );
}

VOID CUIRenderQueue::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bUsage )
		m_pd3dDevice = pd3dDevice;
}

VOID CUIRenderQueue::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !m_bUsage )
		return;

	InvalidateDeviceObjects();

	HRESULT hr = pd3dDevice->CreateVertexBuffer( sizeof( UIVERTEX ) * VERTEX_COUNT,
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
												 UIVERTEX::D3DFVF_UIVERTEX,
												 D3DPOOL_DEFAULT,
												 &m_pVertexBuffer,
												 NULL );
	if( FAILED( hr ) )
	{
		m_pd3dDevice = NULL;
		return;
	}
}

VOID CUIRenderQueue::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVertexBuffer );
	m_pTexture = NULL;
	m_uiOffset = 0;
	m_uiVertex = 0;
	m_uiPrimitiv = 0;
	m_dwLockFlag = 0;
}

VOID CUIRenderQueue::DeleteDeviceObjects()
{
	m_pd3dDevice = NULL;
}

BOOL CUIRenderQueue::Render( LPDIRECT3DTEXTUREQ pTexture, VOID* pVertex )
{
	static UINT uiSize = sizeof( UIVERTEX );
	static INT iIndex[] = { 0, 1, 3, 3, 1, 2 };

	if( !m_pd3dDevice || !m_pVertexBuffer )
		return FALSE;

	if( ( m_pTexture != pTexture ) || ( ( m_uiOffset + m_uiVertex + 6 ) > VERTEX_COUNT ) )
		RenderImmediately();

	m_pTexture = pTexture;

	UIVERTEX* pUIVertex = (UIVERTEX*)pVertex;
	UINT uiStart = m_uiOffset + m_uiVertex;
	for( INT i=0; i<6; ++i )
		memcpy( &m_pVertex[ uiStart + i ], (VOID*)&pUIVertex[ iIndex[i] ], uiSize );

	m_uiVertex += 6;
	m_uiPrimitiv += 2;

	return TRUE;
}

VOID CUIRenderQueue::RenderImmediately()
{
	static UINT uiSize = sizeof( UIVERTEX );

	if( !m_pd3dDevice || !m_pVertexBuffer || !m_uiPrimitiv )
		return;

	VOID* pLocked = NULL;
	UINT uiBufferSize = uiSize * m_uiVertex;
	HRESULT hr = m_pVertexBuffer->Lock( uiSize * m_uiOffset,
										uiBufferSize,
										&pLocked,
										m_dwLockFlag );
	if( FAILED( hr ) )
		goto end;

	memcpy( pLocked, &m_pVertex[ m_uiOffset ], uiBufferSize );

	hr = m_pVertexBuffer->Unlock();
	if( FAILED( hr ) )
		goto end;

	m_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, 0, uiSize );
	m_pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );

	m_pd3dDevice->SetTexture( 0, m_pTexture );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, m_uiOffset, m_uiPrimitiv );

end:
	if( ( m_uiOffset + m_uiVertex + 6 ) > VERTEX_COUNT )
	{
		m_uiOffset = 0;
		m_dwLockFlag = D3DLOCK_DISCARD;
	}
	else
	{
		m_uiOffset += m_uiVertex;
		m_dwLockFlag = D3DLOCK_NOOVERWRITE;
	}
	m_uiVertex = 0;
	m_uiPrimitiv = 0;
}

CUIRenderQueue* CUIRenderQueue::Get()
{
	if( m_pInstance == NULL )
		m_pInstance = new CUIRenderQueue;

	return m_pInstance;
}

VOID CUIRenderQueue::CleanUp()
{
	SAFE_DELETE( m_pInstance );
}