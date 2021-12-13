#include "pch.h"
#include "./DxEffKeepCTShade.h"

#include "./SerialFile.h"
#include "./TextureManager.h"
#include "./DxSkinChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffKeepData_CTShade::TYPEID	= EFFKT_CT_SHADE;
DWORD		DxEffKeepData_CTShade::VERSION	= 0x0100;
char		DxEffKeepData_CTShade::NAME[64]	= "Toon Shade Change";

EFFKEEP_PROPERTY_CTSHADE::EFFKEEP_PROPERTY_CTSHADE()
{
	StringCchCopy( szShadeTex, 32, _T("") );
}

DxEffKeepData_CTShade::DxEffKeepData_CTShade()
{
}

DxEffKeepData_CTShade::~DxEffKeepData_CTShade()
{
}

DxEffKeep* DxEffKeepData_CTShade::NEWOBJ()
{
	DxEffKeepCTShade*	pEff = new DxEffKeepCTShade;

	StringCchCopy( pEff->m_sProp.szShadeTex, 32, m_sProp.szShadeTex );

	pEff->Create ( m_pd3dDevice );

	return	pEff;
}

HRESULT	DxEffKeepData_CTShade::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer( m_sProp.szShadeTex, 32 );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffKeepData_CTShade::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		SFile.WriteBuffer( m_sProp.szShadeTex, 32 );
	}
	SFile.EndBlock( EMBLOCK_00 );

	return S_OK;
}

//	-----------------------------------------------	--------------------------------------------------

DxEffKeepCTShade::DxEffKeepCTShade(void) :
	DxEffKeep(),
	m_pTexture(NULL)
{
}

DxEffKeepCTShade::~DxEffKeepCTShade(void)
{
	CleanUp ();
}

HRESULT DxEffKeepCTShade::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTShade::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffKeepCTShade::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture( m_sProp.szShadeTex, pd3dDevice, m_pTexture, 0L, 0, FALSE );
	return S_OK;
}

HRESULT DxEffKeepCTShade::DeleteDeviceObjects()
{
	TextureManager::ReleaseTexture( m_sProp.szShadeTex, m_pTexture );
	return S_OK;
}

HRESULT DxEffKeepCTShade::FrameMove ( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	sSKINEFFDATA.m_rToonShadeTex = m_pTexture;

	return S_OK;
}

HRESULT	DxEffKeepCTShade::Render ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}