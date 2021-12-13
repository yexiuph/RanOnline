//
//	DXMATERIAL_CHAR_EFF 이 변화하면서 0x105 버전으로 변화 하였다.
//
#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxInputDevice.h"

#include "./DxCubeMap.h"
#include "./DxGlowMan.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharTexDiff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharTexDiff::TYPEID			= EMEFFCHAR_TEXDIFF;
DWORD		DxEffCharTexDiff::VERSION			= 0x0101;
char		DxEffCharTexDiff::NAME[MAX_PATH]	= "1.Tex Diff Up  ,  Down";

LPDIRECT3DSTATEBLOCK9		DxEffCharTexDiff::m_pSavedSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharTexDiff::m_pDrawSB		= NULL;

DxEffCharTexDiff::DxEffCharTexDiff(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwTempMaterials(0L),		// 기본적인 것들..
	m_pMaterials(NULL),
	m_pTempMaterials(NULL),
	m_pTexture(NULL),
	m_bUp(TRUE),
	m_fTime(-D3DX_PI),
	m_fRandSpeed(1.f),
	m_dwColor(0L),
	m_fColor(0.f)
{
}

DxEffCharTexDiff::~DxEffCharTexDiff(void)
{
	m_dwMaterials = 0L;
	m_dwTempMaterials = 0L;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_DELETE_ARRAY ( m_pTempMaterials );
}

DxEffChar* DxEffCharTexDiff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharTexDiff *pEffChar = new DxEffCharTexDiff;
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

void DxEffCharTexDiff::SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials )
{
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_CHAR_EFF[dwMaterials];
	m_dwMaterials = dwMaterials;

	for ( DWORD i=0; i<dwMaterials; i++ )
	{
		m_pMaterials[i].bEffUse	= pMaterials[i].bEffUse;
		m_pMaterials[i].d3dMaterial = pMaterials[i].d3dMaterial;
		StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, pMaterials[i].szTexture );
		StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, pMaterials[i].szEffTex );

		if ( strlen(m_pMaterials[i].szEffTex) )
		{
			TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
		}
	}
}

HRESULT DxEffCharTexDiff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	// Anisotropic Filter 확인
	DWORD dwMinFilter;
	if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		// Note : SetRenderState()	 변해도 상관없음.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

		// Note : SetRenderState()
		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	return S_OK;
}

HRESULT DxEffCharTexDiff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	return S_OK;
}

HRESULT DxEffCharTexDiff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	TextureManager::LoadTexture ( m_strTex.c_str(), pd3dDevice, m_pTexture, 0, 0, TRUE );

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
			if( !m_pmcMesh->pMaterials[i].pTextureFilename )	continue;

			StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );

			if ( i<m_dwTempMaterials )
			{
				m_pMaterials[i].bEffUse = m_pTempMaterials[i].bEffUse;
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, m_pTempMaterials[i].szEffTex );
			}
			else
			{
				m_pMaterials[i].bEffUse = FALSE;
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, m_pMaterials[i].szTexture );
			}

			if ( strlen(m_pMaterials[i].szEffTex) )
			{
				TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
			}
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

			std::string	strName;
			strName = GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_e" );

			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			hr = TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
			if ( FAILED(hr) )
			{
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, "" );
			}
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

			std::string	strName;
			strName = GetSpecularName ( m_pmcMesh->pMaterials[i].pTextureFilename, "_e" );

			StringCchCopy( m_pMaterials[i].szTexture,	MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
			StringCchCopy( m_pMaterials[i].szEffTex,	MAX_PATH, strName.c_str() );

			hr = TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
			if ( FAILED(hr) )
			{
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, "" );
			}

			m_pMaterials[i].d3dMaterial.Power = 1.f;
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwMaterials; i++ )
		{
			hr = TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
			if ( FAILED(hr) )
			{
				StringCchCopy( m_pMaterials[i].szEffTex, MAX_PATH, "" );
			}
		}
	}

	return S_OK;
}

HRESULT DxEffCharTexDiff::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_strTex.c_str(), m_pTexture );

	SAFE_DELETE_ARRAY ( m_pMaterials );

	return S_OK;
}

HRESULT DxEffCharTexDiff::FrameMove ( const float fTime, const float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime += m_fSpeed * m_fRandSpeed * fElapsedTime;

	if( m_dwFlag&NO_UPDOWN )
	{
		// Note : Random Speed Setting
		if( m_fTime > 0.f && m_bUp )		
		{
			m_bUp = FALSE;
			m_fRandSpeed = (RANDOM_POS*5.f+1.f);
		}
		else if( m_fTime>D3DX_PI && !m_bUp )
		{
			m_fTime -= D3DX_PI*2.f;	// 값을 빼준다.
			m_bUp = TRUE;
			m_fRandSpeed = (RANDOM_POS*5.f+1.f);
		}

		// Note : 오르락 내리락
		m_fColor = cosf( m_fTime );
		m_fColor += 1.f;
		m_fColor *= 0.5f;
	}
	else
	{
		m_fColor = 1.f;
	}

	//// Note : Color 설정
	//m_dwColor = (DWORD)(fSpeed*255.f);
	//m_dwColor = 0xff000000 + (m_dwColor<<16) + (m_dwColor<<8) + m_dwColor;

	return S_OK;
}

void	DxEffCharTexDiff::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pmcMesh )		return;
	if ( !m_pSkinMesh )		return;

	// Note : Get Texture
	for ( DWORD i=0; i<m_dwMaterials; i++ )
	{
		if( !m_pMaterials[i].pEffTex )	TextureManager::GetTexture( m_pMaterials[i].szEffTex, m_pMaterials[i].pEffTex );
		if( m_pMaterials[i].pEffTex )
		{
			m_pMaterials[i].d3dMaterial.Diffuse.r = m_fColor * m_pMaterials[i].d3dMaterial.Power;
			m_pMaterials[i].d3dMaterial.Diffuse.g = m_fColor * m_pMaterials[i].d3dMaterial.Power;
			m_pMaterials[i].d3dMaterial.Diffuse.b = m_fColor * m_pMaterials[i].d3dMaterial.Power;			
		}
	}

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	switch( m_dwIntensity )
	{
	case 0:
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 넣을 필요가 없다. 기본값이기 때문에.. 
		break;
	case 1:
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 2:
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	};

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;

	CHARSETTING sCharSetting;
	sCharSetting.pMeshContainerBase = m_pmcMesh;
	sCharSetting.pmtrlPiece			= pMaterialPiece;
	sCharSetting.pmtrlSpecular		= m_pMaterials;
	sCharSetting.bWorldIdentity		= bPieceRender;
	sCharSetting.emRDOP				= CTOP_BASE;

	m_pSkinMesh->SetDrawState ( FALSE, FALSE, FALSE, FALSE );
	m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting );

	m_pSavedSB->Apply();
}

HRESULT	DxEffCharTexDiff::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
			SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );

			for ( DWORD i=0; i<m_dwMaterials; i++ )
			{
				m_pMaterials[i].pEffTex = NULL;
				if ( strlen(m_pMaterials[i].szEffTex) )
				{
					TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
				}
			}
		}

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( 0x0100 == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		EFFCHAR_PROPERTY_TEXDIFF_100 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_TEXDIFF_100) );

		m_Property.m_dwFlag		= sProp.m_dwFlag;
		m_Property.m_dwMaterials = sProp.m_dwMaterials;
		m_Property.m_fSpeed		= sProp.m_fSpeed;
		m_Property.m_strTex		= sProp.m_strTex.c_str();

		m_Property.m_dwIntensity	= 0;	// Ver.101 에서 추가
		m_Property.m_dwFlag			|= NO_UPDOWN;

		SFile >> m_dwMaterials;
		if ( m_dwMaterials )
		{
			SAFE_DELETE_ARRAY ( m_pMaterials );
			m_pMaterials = new DXMATERIAL_CHAR_EFF[m_dwMaterials];
			SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_CHAR_EFF)*m_dwMaterials );

			for ( DWORD i=0; i<m_dwMaterials; i++ )
			{
				m_pMaterials[i].pEffTex = NULL;
				if ( strlen(m_pMaterials[i].szEffTex) )
				{
					TextureManager::LoadTexture ( m_pMaterials[i].szEffTex, pd3dDevice, m_pMaterials[i].pEffTex, 0, 0, TRUE );
				}

				m_pMaterials[i].d3dMaterial.Power = 1.f;	// Ver.101 에서 추가
			}
		}

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

HRESULT	DxEffCharTexDiff::SaveFile ( basestream &SFile )
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
