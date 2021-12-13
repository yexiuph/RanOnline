#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxLoadShader.h"
#include "./DxRenderStates.h"

#include "./DxLightMan.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./RENDERPARAM.h"
#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharHLSL.h"
#include "./DxSkinMesh9_HLSL.h"

#include "./DxEffCharNormal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharNormal::TYPEID			= EMEFFCHAR_NORMALMAP;
DWORD		DxEffCharNormal::VERSION			= 0x0100;
char		DxEffCharNormal::NAME[MAX_PATH]	= "3.Normal Map";

DxEffCharNormal::DxEffCharNormal(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL)
{
}

DxEffCharNormal::~DxEffCharNormal(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharNormal::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharNormal *pEffChar = new DxEffCharNormal;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );
	pEffChar->m_sMaterialHLSL.Clone( pd3dDevice, &m_sMaterialHLSL );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharNormal::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_pSkinMesh;
		m_pmcMesh = m_pCharPart->m_pmcMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_pSkinMesh;
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	if( !m_pmcMesh )	return S_OK;
	m_sMaterialHLSL.OnCreateDevice( pd3dDevice, m_pmcMesh->pMaterials, m_pmcMesh->GetNumMaterials() );

	return S_OK;
}

HRESULT DxEffCharNormal::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharNormal::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharNormal::DeleteDeviceObjects ()
{
	m_sMaterialHLSL.OnDestroyDevice();

	return S_OK;
}

HRESULT DxEffCharNormal::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void DxEffCharNormal::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	if( !m_pmcMesh )		return;
	if( !m_pSkinMesh )		return;

    // Note : ReloadTexture - Loading이 안 된 것을 위한 조치.
	m_sMaterialHLSL.ReloadTexture();

	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshNORMALMAP( pd3dDevice, m_pmcMesh, NULL, &m_sMaterialHLSL, bPieceRender );
}

HRESULT	DxEffCharNormal::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	if( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
		m_sMaterialHLSL.Load( pd3dDevice, SFile );

		//	Note : Device 자원을 생성한다.
		HRESULT hr(S_OK);
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharNormal::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );
		m_sMaterialHLSL.Save( SFile );
	}
	SFile.EndBlock( EMBLOCK_00 );

	return S_OK;
}

