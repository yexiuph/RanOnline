//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x105 버전으로 변화 하였다.
//
#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"

#include "./DxEffCharMultiTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharMultiTex::TYPEID			= EMEFFCHAR_MULTITEX;
DWORD		DxEffCharMultiTex::VERSION			= 0x0105;
char		DxEffCharMultiTex::NAME[MAX_PATH]	= "1.멀티텍스쳐 Multi";

LPDIRECT3DSTATEBLOCK9		DxEffCharMultiTex::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharMultiTex::m_pEffectStateBlock	= NULL;

DxEffCharMultiTex::DxEffCharMultiTex(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTex1(NULL),		// ^^;
	m_pTex2(NULL),		// ^^;
	m_pTexture(NULL),
	m_vSumTex01(0.f,0.f),
	m_vSumTex02(0.f,0.f),
	m_dwTempMaterials(0L),
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_pMaterials_EffUse(NULL)
{
}

DxEffCharMultiTex::~DxEffCharMultiTex(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );
}

DxEffChar* DxEffCharMultiTex::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharMultiTex *pEffChar = new DxEffCharMultiTex;
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

void DxEffCharMultiTex::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].bEffUse	= pMaterials[i].bEffUse;
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pMaterials[i].szTexture );
	}
}

HRESULT DxEffCharMultiTex::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharMultiTex::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharMultiTex::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	TextureManager::LoadTexture ( m_szTex1, pd3dDevice, m_pTex1, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTex2, pd3dDevice, m_pTex2, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	//	Note : 비어있다면 건너 뛴다.
	//
	if ( !m_pmcMesh )	return S_OK;

	if ( m_pTempMaterials )
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( i >= m_dwTempMaterials )						continue;	// 잘 못 만들어서 이렇게 되었다. Loading시 m_dwTempMaterials = 3 으로 강제 셋팅 될 경우가 있다.
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

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

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}
	else if ( !m_pMaterials )							// 매트리얼이 없을 때 디폴트 생성
	{
		m_dwMaterials = m_pmcMesh->GetNumMaterials();
		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}

	// 새로 생성
	m_pMaterials_EffUse = new BOOL [ m_pmcMesh->GetNumMaterials() ];

	return S_OK;
}

HRESULT DxEffCharMultiTex::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTex1, m_pTex1 );
	TextureManager::ReleaseTexture( m_szTex2, m_pTex2 );
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pMaterials_EffUse );	

	return S_OK;
}

HRESULT DxEffCharMultiTex::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_vSumTex01 += m_vTex01*fElapsedTime;
	m_vSumTex02 += m_vTex02*fElapsedTime;

	return S_OK;
}

void	DxEffCharMultiTex::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pSkinMesh )	return;
	if ( DxEffectMan::GetInstance().GetSkinDetail() != SKD_BEST )	return;

	D3DXVECTOR2	vMove;

	// Note : Get Texture
	if( !m_pTex1 )	TextureManager::GetTexture( m_szTex1, m_pTex1 );
	if( !m_pTex2 )	TextureManager::GetTexture( m_szTex2, m_pTex2 );
	if( !m_pTexture )	TextureManager::GetTexture( m_szTexture, m_pTexture );

	//	기본
	vMove.x = m_vSumTex01.x * m_vTexUV01.x;	// m_vTexUV01 이것은 UV 스케일 값이다.
	vMove.y = m_vSumTex01.y * m_vTexUV01.y;
	DxImageMove ( pd3dDevice, m_pTex1, DxSurfaceTex::GetInstance().m_pTempSuf128,
				vMove, TRUE, D3DFVF_DIFFUSE, m_cDiffuse1, m_vTexUV01, 128 );			// Alpha 를 써야 한다면 m_cDiffuse를 건들어야 할 듯.
	//	추가 ( 선택 사항 )
	if ( m_nTexNum == 2 )
	{
		vMove.x = m_vSumTex02.x * m_vTexUV02.x;
		vMove.y = m_vSumTex02.y * m_vTexUV02.y;
		DxImageMove ( pd3dDevice, m_pTex2, DxSurfaceTex::GetInstance().m_pTempSuf128, 
					vMove, FALSE, D3DFVF_DIFFUSE, m_cDiffuse2, m_vTexUV02, 128 );
	}

	if ( m_dwFlag & USETEXMAP )
	{
		DxImageMapRHW ( pd3dDevice, m_pTexture, DxSurfaceTex::GetInstance().m_pTempSuf128, 
					FALSE, 128 );
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwAlphaBlendEnable, dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 0 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 1:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		break;			
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].pEffTex = DxSurfaceTex::GetInstance().m_pTempTex128;
	}

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials_EffUse[i] = m_pMaterials[i].bEffUse;	// Use 백업
			m_pMaterials[i].bEffUse = TRUE;						// 설정
		}
	}

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	// Draw
	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting );

	if ( m_dwFlag & USE_ALL_TEX )
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			m_pMaterials[i].bEffUse = m_pMaterials_EffUse[i];	// Use 백업
		}
	}

	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		m_pMaterials[i].pEffTex = NULL;
	}

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );

	m_pSavedStateBlock->Apply();
}

HRESULT	DxEffCharMultiTex::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 읽기
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
	else if ( dwVer == 0x0104 )
	{
		//	Note : 이팩트의 Property 를 읽기
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		if ( !( m_dwFlag&(USE_ALL_TEX|USE_SELECT_TEX) ) )
		{
			m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항
		}

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			DXMATERIAL_CHAR_EFF_100*	pMaterials;
			pMaterials = new DXMATERIAL_CHAR_EFF_100[m_dwMaterials];
			SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_CHAR_EFF_100)*m_dwMaterials );

			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];

			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				m_pMaterials[i].bEffUse				= pMaterials[i].bEffUse;
				m_pMaterials[i].d3dMaterial.Power	= pMaterials[i].fMaterial_Power;
				m_pMaterials[i].pEffTex				= NULL;
				StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
			}

			SAFE_DELETE_ARRAY ( pMaterials );
		}

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0103 )
	{
		//	Note : 이팩트의 Property 를 읽기
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			DXMATERIAL_SPECULAR*	pMaterials;
			pMaterials = new DXMATERIAL_SPECULAR[m_dwMaterials];
			SFile.ReadBuffer ( pMaterials, sizeof(DXMATERIAL_SPECULAR)*m_dwMaterials );

			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
			for ( DWORD i=0; i<m_dwMaterials; ++i )
			{
				m_pMaterials[i].bEffUse				= pMaterials[i].bSpecUse;
				m_pMaterials[i].d3dMaterial.Power	= 14.f;
				StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szSpecTex );
				StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
				m_pMaterials[i].pEffTex			= NULL;
			}

			SAFE_DELETE_ARRAY ( pMaterials );
		}

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0102 )
	{
		EFFCHAR_PROPERTY_MULTITEX_102	sProp;
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_102) );

		m_dwFlag	= sProp.m_dwFlag;
		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

		m_nBlend	= sProp.m_nBlend;
		m_nTexNum	= sProp.m_nTexNum;

		m_vTex01	= sProp.m_vTex01;
		m_vTex02	= sProp.m_vTex02;

		m_vTexUV01	= sProp.m_vTexUV01;
		m_vTexUV02	= sProp.m_vTexUV02;

		StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
		StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

		m_cDiffuse1 = 0xff000000;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

		m_cDiffuse2 = 0xff000000;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


		SAFE_DELETE_ARRAY ( m_pTempMaterials );
		m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
		m_dwTempMaterials = 3;

		if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].bEffUse = TRUE;

		StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
		StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
		StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0101 )
	{
		EFFCHAR_PROPERTY_MULTITEX_100_1	sProp;
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

		m_dwFlag	= sProp.m_dwFlag;
		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

		m_nBlend	= sProp.m_nBlend;
		m_nTexNum	= sProp.m_nTexNum;

		m_vTex01	= sProp.m_vTex01;
		m_vTex02	= sProp.m_vTex02;

		m_vTexUV01	= D3DXVECTOR2 ( 1.f, 1.f );
		m_vTexUV02	= D3DXVECTOR2 ( 1.f, 1.f );

		StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
		StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

		m_cDiffuse1 = 0xff000000;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

		m_cDiffuse2 = 0xff000000;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


		SAFE_DELETE_ARRAY ( m_pTempMaterials );
		m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
		m_dwTempMaterials = 3;

		if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].bEffUse = TRUE;

		StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
		StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
		StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0100 )
	{
		EFFCHAR_PROPERTY_MULTITEX_100_1	sProp;
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_MULTITEX_100_1) );

		m_dwFlag	= sProp.m_dwFlag;
		m_dwFlag	|= USE_SELECT_TEX;	// Ver.105 에서 새롭게 추가된 사항

		m_nBlend	= sProp.m_nBlend;
		m_nTexNum	= sProp.m_nTexNum;

		m_vTex01	= sProp.m_vTex01;
		m_vTex02	= sProp.m_vTex02;

		m_vTexUV01	= D3DXVECTOR2 ( 1.f, 1.f );
		m_vTexUV02	= D3DXVECTOR2 ( 1.f, 1.f );

		StringCchCopy( m_szTex1, MAX_PATH, sProp.m_szTex1 );
		StringCchCopy( m_szTex2, MAX_PATH, sProp.m_szTex2 );

		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

		m_cDiffuse1 = 0xff000000;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse1 += (DWORD)(sProp.m_cDiffuse1.b*255);

		m_cDiffuse2 = 0xff000000;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.r*255)<<16;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.g*255)<<8;
		m_cDiffuse2 += (DWORD)(sProp.m_cDiffuse1.b*255);


		SAFE_DELETE_ARRAY ( m_pTempMaterials );
		m_pTempMaterials = new DXMATERIAL_CHAR_EFF[3];
		m_dwTempMaterials = 3;

		if ( m_dwFlag&USETEXTURE1 )			m_pTempMaterials[0].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE2 )			m_pTempMaterials[1].bEffUse = TRUE;
		if ( m_dwFlag&USETEXTURE3 )			m_pTempMaterials[2].bEffUse = TRUE;

		StringCchCopy( m_pTempMaterials[0].szTexture, MAX_PATH, sProp.m_szSrcTex_1 );
		StringCchCopy( m_pTempMaterials[1].szTexture, MAX_PATH, sProp.m_szSrcTex_2 );
		StringCchCopy( m_pTempMaterials[2].szTexture, MAX_PATH, sProp.m_szSrcTex_3 );

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

HRESULT	DxEffCharMultiTex::SaveFile ( basestream &SFile )
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