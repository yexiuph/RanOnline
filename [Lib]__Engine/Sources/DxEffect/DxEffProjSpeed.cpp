#include "pch.h"

#include "./DxEffProjSpeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffProjSpeed::DxEffProjSpeed() 
	: m_bSpeedEffON( FALSE )
{
}

DxEffProjSpeed::~DxEffProjSpeed()
{
}

void DxEffProjSpeed::EnableSpeedEff( BOOL bUse )
{
	m_bSpeedEffON = bUse;
}

void DxEffProjSpeed::FrameMove( float fElapsedTime )
{

}

void DxEffProjSpeed::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{

}
