#include "pch.h"

#include "basestream.h"
#include "./TextureManager.h"

#include "DxMaterialHLSL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DXMATERIAL_HLSL::DXMATERIAL_HLSL() :
	m_emRenderType(EMNT_BASE),
	m_fNormalPower(1.f),
	m_fSpecPower(1.f),
	m_fIllumiPower(1.f),
	m_pNormalMapTex(NULL),
	m_pSpecularMapTex(NULL),
	m_pIlluminationTex(NULL)
{
	m_strSrcTex			= _T("");
	m_strNormal			= _T("");
	m_strSpecular		= _T("");
	m_strIllumination	= _T("");
}

DXMATERIAL_HLSL::~DXMATERIAL_HLSL()
{
	CleanUp();
}

void DXMATERIAL_HLSL::CleanUp()
{
	ReleaseTexture();
}

void DXMATERIAL_HLSL::LodingTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_strNormal.empty() )
	{
		TextureManager::LoadTexture( m_strNormal.c_str(), pd3dDevice, m_pNormalMapTex, 0, 0, TRUE );
	}
	if( !m_strSpecular.empty() )
	{
		TextureManager::LoadTexture( m_strSpecular.c_str(), pd3dDevice, m_pSpecularMapTex, 0, 0, TRUE );
	}
	if( !m_strIllumination.empty() )
	{
		TextureManager::LoadTexture( m_strIllumination.c_str(), pd3dDevice, m_pIlluminationTex, 0, 0, TRUE );
	}
}

void DXMATERIAL_HLSL::ReleaseTexture()
{
	if(	m_pNormalMapTex )		TextureManager::ReleaseTexture( m_strNormal.c_str(),		m_pNormalMapTex );
	if(	m_pSpecularMapTex )		TextureManager::ReleaseTexture( m_strSpecular.c_str(),		m_pSpecularMapTex );
	if(	m_pIlluminationTex )	TextureManager::ReleaseTexture( m_strIllumination.c_str(),	m_pIlluminationTex );
}

void DXMATERIAL_HLSL::Clone( LPDIRECT3DDEVICEQ pd3dDevice, DXMATERIAL_HLSL* pSrc )
{
	*this = *pSrc;

	m_pNormalMapTex = NULL;
	m_pSpecularMapTex = NULL;
	m_pIlluminationTex = NULL;

	LodingTexture( pd3dDevice );
}

void DXMATERIAL_HLSL::Save( basestream& SFile )
{
	SFile << m_emRenderType;

	SFile << m_fNormalPower;
	SFile << m_fSpecPower;
	SFile << m_fIllumiPower;

	SFile << m_strSrcTex;
	SFile << m_strNormal;
	SFile << m_strSpecular;
	SFile << m_strIllumination;
}

void DXMATERIAL_HLSL::Load( LPDIRECT3DDEVICEQ pd3dDevice, basestream& SFile )
{
	DWORD dwType;
	SFile >> dwType;
	m_emRenderType = (EMNORMALTYPE)dwType;

	SFile >> m_fNormalPower;
	SFile >> m_fSpecPower;
	SFile >> m_fIllumiPower;

	SFile >> m_strSrcTex;
	SFile >> m_strNormal;
	SFile >> m_strSpecular;
	SFile >> m_strIllumination;

	// Note : 텍스쳐를 로드한다.
	LodingTexture( pd3dDevice );
}

///---------------------------------------------------------------------------------------------------------------
//							D	x		M	a	t	e	r	i	a	l		H	L	S	L
///---------------------------------------------------------------------------------------------------------------
const DWORD	DxMaterialHLSL::VERSION = 0x0100;

DxMaterialHLSL::DxMaterialHLSL() :
	m_pMaterials(NULL),
	m_dwMaterialNUM(0L)
{
}

DxMaterialHLSL::~DxMaterialHLSL()
{
	CleanUp();
}

void DxMaterialHLSL::CleanUp()
{
	m_dwMaterialNUM = 0;
	SAFE_DELETE_ARRAY( m_pMaterials );
}

void CloneSameSrcTex( DXMATERIAL_HLSL& sDest, DXMATERIAL_HLSL* pSrc, DWORD dwMaterialSRC )
{
	if( !pSrc )	return;

	for( DWORD i=0; i<dwMaterialSRC; ++i )
	{
		if( sDest.m_strSrcTex == pSrc[i].m_strSrcTex )
		{
			sDest = pSrc[i];
			sDest.m_pNormalMapTex = NULL;
			sDest.m_pSpecularMapTex = NULL;
			sDest.m_pIlluminationTex = NULL;

			return;
		}
	}
}

void DxMaterialHLSL::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATERIAL pMaterials, DWORD dwMaterialNUM )
{
	DXMATERIAL_HLSL* m_pTemp = new DXMATERIAL_HLSL[ dwMaterialNUM ];

	// Note : Clone 작업.
	for( DWORD i=0; i<dwMaterialNUM; ++i )
	{
		m_pTemp[i].m_strSrcTex = pMaterials[i].pTextureFilename;		// 소스 텍스쳐 이름만 얻는다.
		CloneSameSrcTex( m_pTemp[i], m_pMaterials, m_dwMaterialNUM );	// 텍스쳐가 같은 것을 복제해 놓는다.
	}

	// Note : 텍스쳐를 로드한다.
	for( DWORD i=0; i<dwMaterialNUM; ++i )
	{
		m_pTemp[i].LodingTexture( pd3dDevice );
	}

	// Note : 데이터 정리
	CleanUp();
	m_dwMaterialNUM = dwMaterialNUM;
	m_pMaterials = m_pTemp;
}

void DxMaterialHLSL::OnDestroyDevice()
{
	// Note : 텍스쳐를 로드한다.
	for( DWORD i=0; i<m_dwMaterialNUM; ++i )
	{
		m_pMaterials[i].ReleaseTexture();
	}
}

void DxMaterialHLSL::ReleaseTexture()
{
	for ( DWORD i=0; i<m_dwMaterialNUM; i++ )
	{
		m_pMaterials[i].ReleaseTexture();
	}
}

void DxMaterialHLSL::ReloadTexture()
{
	for ( DWORD i=0; i<m_dwMaterialNUM; i++ )
	{
		if( !m_pMaterials[i].m_pNormalMapTex )		TextureManager::GetTexture( m_pMaterials[i].m_strNormal.c_str(),		m_pMaterials[i].m_pNormalMapTex );
		if( !m_pMaterials[i].m_pSpecularMapTex )	TextureManager::GetTexture( m_pMaterials[i].m_strSpecular.c_str(),		m_pMaterials[i].m_pSpecularMapTex );
		if( !m_pMaterials[i].m_pIlluminationTex )	TextureManager::GetTexture( m_pMaterials[i].m_strIllumination.c_str(),	m_pMaterials[i].m_pIlluminationTex );
	}
}

void DxMaterialHLSL::Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxMaterialHLSL* pSrc )
{
	CleanUp();

	m_dwMaterialNUM = pSrc->m_dwMaterialNUM;

	m_pMaterials = new DXMATERIAL_HLSL[ m_dwMaterialNUM ];
	for( DWORD i=0; i<m_dwMaterialNUM; ++i )
	{
		m_pMaterials[i].Clone( pd3dDevice, &pSrc->m_pMaterials[i] );
	}
}

void DxMaterialHLSL::Save( basestream& SFile )
{
	SFile << VERSION;
	SFile.BeginBlock( EMBLOCK_01 );
	{
		SFile << m_dwMaterialNUM;
		for( DWORD i=0; i<m_dwMaterialNUM; ++i )
		{
			m_pMaterials[i].Save( SFile );
		}
	}
	SFile.EndBlock( EMBLOCK_01 );
}

void DxMaterialHLSL::Load( LPDIRECT3DDEVICEQ pd3dDevice, basestream& SFile )
{
	CleanUp();

	DWORD dwVER(0L);
	DWORD dwBufferSize(0L);

	SFile >> dwVER;
	SFile >> dwBufferSize;

	if( dwVER==VERSION )
	{
		SFile >> m_dwMaterialNUM;

		m_pMaterials = new DXMATERIAL_HLSL[ m_dwMaterialNUM ];
		for( DWORD i=0; i<m_dwMaterialNUM; ++i )
		{
			m_pMaterials[i].Load( pd3dDevice, SFile );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

