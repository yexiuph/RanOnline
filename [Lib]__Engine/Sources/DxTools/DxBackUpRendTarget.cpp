#include "pch.h"
#include "./DxBackUpRendTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxBackUpRendTarget::DxBackUpRendTarget(void) :
	m_pTempSurface(NULL),
	m_pTempZBuffer(NULL)
{
}

DxBackUpRendTarget::DxBackUpRendTarget ( LPDIRECT3DDEVICEQ pd3dDevice ) :
	m_pTempSurface(NULL),
	m_pTempZBuffer(NULL)
{
	HRESULT hr;
	hr = pd3dDevice->GetRenderTarget ( 0, &m_pTempSurface );
	GASSERT(SUCCEEDED(hr)&&"GetRenderTarget()");

	//hr = pd3dDevice->GetDepthStencilSurface ( &m_pTempZBuffer );
	//GASSERT(SUCCEEDED(hr)&&"GetDepthStencilSurface()");
	pd3dDevice->GetDepthStencilSurface ( &m_pTempZBuffer );
}

DxBackUpRendTarget::~DxBackUpRendTarget(void)
{
	GASSERT ( !m_pTempSurface && !m_pTempZBuffer && "RestoreTarget()을 호출하지 않았습니다." );
}

void DxBackUpRendTarget::RestoreTarget ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//GASSERT ( m_pTempSurface && m_pTempZBuffer );

	pd3dDevice->SetRenderTarget ( 0, m_pTempSurface );
	pd3dDevice->SetDepthStencilSurface ( m_pTempZBuffer );
	SAFE_RELEASE(m_pTempSurface);
	SAFE_RELEASE(m_pTempZBuffer);
}

