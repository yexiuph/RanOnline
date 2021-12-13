#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharDust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharDust::TYPEID			= EMEFFCHAR_DUST;
DWORD		DxEffCharDust::VERSION			= 0x0100;
char		DxEffCharDust::NAME[MAX_PATH]	= "1.먼지,때";

LPDIRECT3DSTATEBLOCK9		DxEffCharDust::m_pSavedBaseSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharDust::m_pEffectBaseSB		= NULL;

DxEffCharDust::DxEffCharDust(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTexture(NULL),
	m_pMaterials(NULL),
	m_dwMaterials(0L),
	m_dwMipMapCount(2)
{
}

DxEffCharDust::~DxEffCharDust(void)
{
}

DxEffChar* DxEffCharDust::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharDust *pEffChar = new DxEffCharDust;
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

HRESULT DxEffCharDust::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffCharDust::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffCharDust::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, m_dwMipMapCount, TRUE );

	for ( DWORD i=0; i<m_dwMaterials; ++i )	
	{
		m_pMaterials[i].pEffTex = NULL;
	}
	SAFE_DELETE_ARRAY ( m_pMaterials );

	m_dwMaterials = m_pmcMesh->GetNumMaterials();
	m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

	for ( DWORD i=0; i<m_dwMaterials; ++i )
	{
		m_pMaterials[i].bEffUse = TRUE;
		m_pMaterials[i].pEffTex = m_pTexture;
	}

	return S_OK;
}

HRESULT DxEffCharDust::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	for ( DWORD i=0; i<m_dwMaterials; ++i )	
	{
		m_pMaterials[i].pEffTex = NULL;
	}
	SAFE_DELETE_ARRAY ( m_pMaterials );

	m_dwMaterials = 0L;

	return S_OK;
}

HRESULT DxEffCharDust::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharDust::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	if ( !m_pmcMesh )		return;
	if ( !m_pSkinMesh )		return;

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;

	if ( DxEffectMan::GetInstance().GetDetailFlag() & REALSPECULAR ) 
	{
		// Note : Get Texture
		if( !m_pTexture )
		{
			TextureManager::GetTexture( m_szTexture, m_pTexture );

			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				m_pMaterials[i].pEffTex = m_pTexture;
			}
		}

		m_pSavedBaseSB->Capture();
		m_pEffectBaseSB->Apply();

		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= pMaterialPiece;
		sCharSetting.pmtrlSpecular		= m_pMaterials;
		sCharSetting.bWorldIdentity		= bPieceRender;
		sCharSetting.emRDOP				= CTOP_BASE;

		m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting );

		m_pSavedBaseSB->Apply();
	}
}

HRESULT	DxEffCharDust::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
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

HRESULT	DxEffCharDust::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}