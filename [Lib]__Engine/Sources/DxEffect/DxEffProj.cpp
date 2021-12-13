#include "pch.h"
#include "./DxEffProj.h"

#include "./DxSurfaceTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffProjMan& DxEffProjMan::GetInstance()
{
	static DxEffProjMan Instance;
	return Instance;
}

DxEffProjMan::DxEffProjMan()
{

}

DxEffProjMan::~DxEffProjMan()
{

}

void DxEffProjMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnCreateDevice( pd3dDevice );
}

void DxEffProjMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnResetDevice( pd3dDevice );
}

void DxEffProjMan::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnLostDevice( pd3dDevice );
}

void DxEffProjMan::OnDestroyDevice()
{
	m_sGetItemEFF.OnDestroyDevice();
}

void DxEffProjMan::FrameMove( float fElapsedTime )
{
	if( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	m_sGetItemEFF.FrameMove( fElapsedTime );
}

void DxEffProjMan::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	m_sGetItemEFF.Render( pd3dDevice );
}

