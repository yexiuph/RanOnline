//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x104 버전으로 변화 하였다.
//
#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxVertexFVF.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"
#include "./DxGlowMan.h"

#include "./DxLightMan.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"
#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharNeon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharNeon::TYPEID			= EMEFFCHAR_NEON;
DWORD		DxEffCharNeon::VERSION			= 0x0105;
char		DxEffCharNeon::NAME[MAX_PATH]	= "1.Glow";

LPDIRECT3DSTATEBLOCK9		DxEffCharNeon::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharNeon::m_pEffectStateBlock	= NULL;

DxEffCharNeon::DxEffCharNeon(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTempTex(NULL),
	m_pGlowTex(NULL),
	m_pBlackTex(NULL),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_pMaterials_EffUse(NULL)
{
}

DxEffCharNeon::~DxEffCharNeon(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );
}

DxEffChar* DxEffCharNeon::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharNeon *pEffChar = new DxEffCharNeon;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );
	pEffChar->SetMaterials ( pd3dDevice, m_dwMaterials, m_pMaterials );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

void DxEffCharNeon::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].bEffUse	= pMaterials[i].bEffUse;
		m_pMaterials[i].d3dMaterial = pMaterials[i].d3dMaterial;
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pMaterials[i].szTexture );
	}
}

HRESULT DxEffCharNeon::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharNeon::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharNeon::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	m_szBlackTex = "Black.dds";
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pGlowTex, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szBlackTex.c_str(), pd3dDevice, m_pBlackTex, 0, 0, TRUE );

	//	Note : 비어있다면 건너 뛴다.
	//
	if ( !m_pmcMesh )	return S_OK;

	if ( m_pTempMaterials )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			m_pMaterials[i].bEffUse = m_pTempMaterials[i].bEffUse;
			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}

		m_dwTempMaterials = 0L;
		SAFE_DELETE_ARRAY ( m_pTempMaterials );
	}
	else if ( m_pMaterials && (m_dwMaterials != m_pmcMesh->GetNumMaterials()) )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();		// 매트리얼 정보가 바뀌었을 때 그냥 삭제후 디폴트 생성
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else if ( !m_pMaterials )							// 매트리얼이 없을 때 디폴트 생성
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		DWORD dwR = (m_cColor&0xff0000)>>16;
		DWORD dwG = (m_cColor&0xff00)>>8;
		DWORD dwB = (m_cColor&0xff);

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
			m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;

			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}

	// 새로 생성
	m_pMaterials_EffUse = new BOOL [ m_pmcMesh->GetNumMaterials() ];

	return S_OK;
}

HRESULT DxEffCharNeon::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pGlowTex );
	TextureManager::ReleaseTexture( m_szBlackTex.c_str(), m_pBlackTex );

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );	

	return S_OK;
}

HRESULT DxEffCharNeon::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharNeon::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pmcMesh->MeshData.pMesh )					return;
	if ( !m_pSkinMesh )									return;
	if ( !DxGlowMan::GetInstance().IsOptionEnable() )	return;	// 옵션에서 꺼져 있으면 동작하지 않는다.

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;

	if( (m_dwFlag&USE_GENERAL) && DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON ();
	}
	else if( (m_dwFlag&USE_BURN) && DxSurfaceTex::GetInstance().m_pBurnNewSuf )
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pBurnNewSuf );
		pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

		BOOL bZBiasChange(FALSE);
		m_pmcMesh->SetMaterial( pd3dDevice, 0, bZBiasChange, pMaterialPiece );
		DxGlowMan::GetInstance().SetGlowON_Burn ();
	}

	// Note : Get Texture
	if( !m_pGlowTex )	TextureManager::GetTexture( m_szTexture, m_pGlowTex );
	if( !m_pBlackTex )	TextureManager::GetTexture( m_szBlackTex.c_str(), m_pBlackTex );

	m_pTempTex = NULL;
	if ( m_dwFlag & USE_GLOW_USER )		m_pTempTex = m_pGlowTex;

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pEffTex = m_pTempTex;

			m_pMaterials_EffUse[i] = m_pMaterials[i].bEffUse;	// Use 백업
			m_pMaterials[i].bEffUse = TRUE;						// 설정
		}
	}
	else //USE_SELECT_TEX
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if ( m_pMaterials[i].bEffUse )	m_pMaterials[i].pEffTex = m_pTempTex;
			else							m_pMaterials[i].pEffTex = m_pBlackTex;

			m_pMaterials_EffUse[i] = m_pMaterials[i].bEffUse;	// Use 백업
			m_pMaterials[i].bEffUse = TRUE;						// 설정
		}
	}

	//	Draw 
	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;
	
	m_pSkinMesh->SetDrawState( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );

	m_pSavedStateBlock->Apply();


	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pEffTex = NULL;
			m_pMaterials[i].bEffUse = m_pMaterials_EffUse[i];	// Use 백업
		}
	}
	else //USE_SELECT_TEX
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].pEffTex = NULL;
			m_pMaterials[i].bEffUse = m_pMaterials_EffUse[i];	// Use 백업
			
		}
	}

	pd3dDevice->SetRenderTarget( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );

	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

HRESULT	DxEffCharNeon::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
			SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );
		}

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if( dwVer == 0x0104 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
			SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );
		}

		// Ver.105
		m_dwFlag &= ~USE_GENERAL;
		m_dwFlag |= USE_BURN;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0103 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		if ( !( m_dwFlag&(USE_ALL_TEX|USE_SELECT_TEX) ) )		m_dwFlag |= USE_SELECT_TEX;	// 새롭게 추가된 사항
		if ( !( m_dwFlag&(USE_GLOW_SRCTEX|USE_GLOW_USER) ) )	m_dwFlag |= USE_GLOW_USER;	// 새롭게 추가된 사항
		if ( !( m_dwFlag&(USE_GENERAL|USE_BURN) ) )				m_dwFlag |= USE_BURN;		// 새롭게 추가된 사항

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			DXMATERIAL_CHAR_EFF_100*	pMaterials;
			pMaterials = new DXMATERIAL_CHAR_EFF_100[m_dwMaterials];
			SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_CHAR_EFF_100)*m_dwMaterials );

			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

			DWORD dwR = (m_cColor&0xff0000)>>16;
			DWORD dwG = (m_cColor&0xff00)>>8;
			DWORD dwB = (m_cColor&0xff);

			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				m_pMaterials[i].bEffUse				= pMaterials[i].bEffUse;
				m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Power	= pMaterials[i].fMaterial_Power;
				m_pMaterials[i].pEffTex				= NULL;
				StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
			}

			SAFE_DELETE_ARRAY ( pMaterials );
		}

		// Ver.105
		m_dwFlag &= ~USE_GENERAL;
		m_dwFlag |= USE_BURN;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0102 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			DXMATERIAL_SPECULAR*	pMaterials;
			pMaterials = new DXMATERIAL_SPECULAR[m_dwMaterials];
			SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_SPECULAR)*m_dwMaterials );

			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

			DWORD dwR = (m_cColor&0xff0000)>>16;
			DWORD dwG = (m_cColor&0xff00)>>8;
			DWORD dwB = (m_cColor&0xff);

			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				m_pMaterials[i].bEffUse				= pMaterials[i].bSpecUse;
				m_pMaterials[i].d3dMaterial.Diffuse.r = (float)dwR*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Diffuse.g = (float)dwG*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Diffuse.b = (float)dwB*DIV_1_255;
				m_pMaterials[i].d3dMaterial.Power	= 14.f;
				StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szSpecTex );
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
				m_pMaterials[i].pEffTex				= NULL;
			}

			SAFE_DELETE_ARRAY ( pMaterials );
		}

		// Ver.105
		m_dwFlag &= ~USE_GENERAL;
		m_dwFlag |= USE_BURN;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0101 )
	{
		EFFCHAR_PROPERTY_NEON_101	sProp;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_NEON_101) );

		m_dwFlag	= sProp.m_dwFlag;
		m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항
		m_cColor	= sProp.m_cColor_1;

		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szGlowTex_1 );

		SAFE_DELETE_ARRAY ( m_pTempMaterials );
		m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
		m_dwTempMaterials = 3;

		if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].bEffUse = TRUE;

		StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
		StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
		StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

		// Ver.105
		m_dwFlag &= ~USE_GENERAL;
		m_dwFlag |= USE_BURN;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0100 )
	{
		EFFCHAR_PROPERTY_NEON_100	sProp;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_NEON_100) );

		m_dwFlag	= sProp.m_dwFlag;
		m_dwFlag	|= USE_SELECT_TEX;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_GLOW_USER;	// 새롭게 추가된 사항
		m_dwFlag	|= USE_BURN;		// 새롭게 추가된 사항
		m_cColor	= 0xffffffff;

		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szGlowTex_1 );

		if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].bEffUse = TRUE;

		StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
		StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
		StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

		// Ver.105
		m_dwFlag &= ~USE_GENERAL;
		m_dwFlag |= USE_BURN;

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

HRESULT	DxEffCharNeon::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) + sizeof(DWORD) + sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	SFile << m_dwMaterials;
	if ( m_dwMaterials )
	{
		SFile.WriteBuffer ( m_pMaterials, sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );
	}

	return S_OK;
}