#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"

#include "./DxEffCharEmit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharEmit::TYPEID			= EMEFFCHAR_EMIT;
DWORD		DxEffCharEmit::VERSION			= 0x0100;
char		DxEffCharEmit::NAME[MAX_PATH]	= "1.�߱� ȿ�� ";

LPDIRECT3DSTATEBLOCK9		DxEffCharEmit::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharEmit::m_pEffectStateBlock	= NULL;

DxEffCharEmit::DxEffCharEmit(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTexture(NULL)
{
}

DxEffCharEmit::~DxEffCharEmit(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharEmit::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharEmit *pEffChar = new DxEffCharEmit;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharEmit::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA  );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharEmit::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharEmit::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharEmit::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffCharEmit::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharEmit::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharEmit::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	return S_OK;
}

HRESULT DxEffCharEmit::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharEmit::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharEmit::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pmcMesh->MeshData.pMesh )	return;
	if ( !m_pSkinMesh )					 return;

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	BOOL bZBiasChange(FALSE);
	m_pmcMesh->SetMaterial ( pd3dDevice, 0, bZBiasChange );

	//m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, FALSE, TRUE, FALSE, 0, 0, 0, m_pTexture );

	m_pSavedStateBlock->Apply();
}

HRESULT	DxEffCharEmit::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharEmit::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}