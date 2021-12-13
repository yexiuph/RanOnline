#include "pch.h"

#include "./SerialFile.h"
#include "./TextureManager.h"
#include "./DxCubeMap.h"

#include "./DxTexEffSpecular.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxTexEffSpecular::TYPEID = DEF_TEXEFF_SPECULAR;
const DWORD	DxTexEffSpecular::VERSION = 0x00000100;
const char DxTexEffSpecular::NAME[] = "[ Specular ]";

// Note : TEXEFF_SPECULAR_PROPERTY
TEXEFF_SPECULAR_PROPERTY::TEXEFF_SPECULAR_PROPERTY() :
	pTex(NULL)
{
	strTex ="";
}

TEXEFF_SPECULAR_PROPERTY::~TEXEFF_SPECULAR_PROPERTY()
{
	TextureManager::ReleaseTexture( strTex.c_str(), pTex );
}

void TEXEFF_SPECULAR_PROPERTY::Save( CSerialFile& SFile )
{
	SFile << strTex;
}

void TEXEFF_SPECULAR_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	TextureManager::ReleaseTexture( strTex.c_str(), pTex );

	SFile >> strTex;

	TextureManager::LoadTexture( strTex.c_str(), pd3dDevice, pTex, 0 , 0 );
}

// Note : DxTexEffSpecular
LPDIRECT3DSTATEBLOCK9	DxTexEffSpecular::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffSpecular::m_pDrawSB = NULL;

DxTexEffSpecular::DxTexEffSpecular()
{
}

DxTexEffSpecular::~DxTexEffSpecular()
{
}

void DxTexEffSpecular::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TextureManager::ReleaseTexture( m_sProp.strTex.c_str(), m_sProp.pTex );

	TEXEFF_SPECULAR_PROPERTY* pTemp = (TEXEFF_SPECULAR_PROPERTY*)pProp;
	m_sProp.strTex		= pTemp->strTex;

	TextureManager::LoadTexture( m_sProp.strTex.c_str(), pd3dDevice, m_sProp.pTex, 0, 0 );
}

void DxTexEffSpecular::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 識情
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		//	Note : SetTextureStageState() 識情
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,			D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffSpecular::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

void DxTexEffSpecular::FrameMove( const float fElapsedTime )	{}

void DxTexEffSpecular::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib )
{
	SetRenderBegin( pd3dDevice );
	pMesh->DrawSubset( dwAttrib );
	SetRenderEnd( pd3dDevice );
}

void DxTexEffSpecular::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : GetTexture
	if( !m_sProp.pTex )
	{
		TextureManager::GetTexture( m_sProp.strTex.c_str(), m_sProp.pTex );
	}
	pd3dDevice->SetTexture( 0, m_sProp.pTex );
	pd3dDevice->SetTexture( 1, DxCubeMap::GetInstance().GetCubeTexTEST() );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();
}

void DxTexEffSpecular::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedSB->Apply();

	pd3dDevice->SetTexture( 1, NULL );
}

void DxTexEffSpecular::SavePSF( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_02 );
	m_sProp.Save( SFile );
	SFile.EndBlock( EMBLOCK_02 );
}

void DxTexEffSpecular::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}
}
