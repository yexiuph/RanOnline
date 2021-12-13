#include "pch.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxViewPort.h"

#include "./DxCharPart.h"

#include "./DxEffCharAlpha.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharAlpha::TYPEID			= EMEFFCHAR_ALPHA;
DWORD		DxEffCharAlpha::VERSION			= 0x0102;
char		DxEffCharAlpha::NAME[MAX_PATH]	= "1.알파 사용";

DxEffCharAlpha::DxEffCharAlpha(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),

	m_fElapsedTime(0.f),
	m_fTimeSum(0.f)
{
	m_dwZWriteEnable = NULL;
	m_dwFogEnable = NULL;
	m_dwAlphaBlendEnable = NULL;
	m_dwSecBlend = NULL;
	m_dwDestBlend = NULL;
}

DxEffCharAlpha::~DxEffCharAlpha(void)
{
}

DxEffChar* DxEffCharAlpha::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharAlpha *pEffChar = new DxEffCharAlpha;
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

HRESULT DxEffCharAlpha::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
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

	return S_OK;
}

HRESULT DxEffCharAlpha::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharAlpha::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTimeSum		+= fElapsedTime;
	m_fElapsedTime	= fElapsedTime;

	float	fFullTime = m_fWaitTime + m_fPlayTime;

	if ( m_fTimeSum > fFullTime )
	{
		m_fTimeSum = fFullTime;
	}

	return S_OK;
}

HRESULT	DxEffCharAlpha::SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )
{
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&m_dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,		&m_dwFogEnable );
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&m_dwAlphaBlendEnable );
	pd3dDevice->GetRenderState( D3DRS_SRCBLEND,			&m_dwSecBlend );
	pd3dDevice->GetRenderState( D3DRS_DESTBLEND,		&m_dwDestBlend );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );


	if ( !m_pmcMesh->MeshData.pMesh )	return S_OK;
	if ( !m_pSkinMesh )					return S_OK;

	if ( m_dwFlags & USETRANSLUCENT )
	{
		for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
		{
			m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= m_fAlpha;
			m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= m_fAlpha;
			m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 0.f;
		}
	}
	else
	{
		float	fFullTime	= m_fWaitTime + m_fPlayTime;

		if ( m_fTimeSum >= m_fWaitTime )
		{
			float	fAlpha = ((fFullTime-m_fTimeSum)/m_fPlayTime);

			if ( fAlpha > 1.f )	fAlpha = 1.f;
			if ( fAlpha < 0.f )	fAlpha = 0.f;

			fAlpha = ((1.f-m_fAlpha)*fAlpha) + m_fAlpha;		// 최소 알파값이 생겼다. ^^;

			for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
			{
				m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= fAlpha;
				m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= fAlpha;
				m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= fAlpha;
			}
		}
	}

	return S_OK;
}

HRESULT	DxEffCharAlpha::RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		m_dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		m_dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	m_dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			m_dwSecBlend );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		m_dwDestBlend );

	if ( !m_pmcMesh->MeshData.pMesh )	return S_OK;
	if ( !m_pSkinMesh )					return S_OK;

	if ( m_dwFlags & USETRANSLUCENT )
	{
		for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
		{
			m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= 1.0f;
			m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= 1.0f;
			m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 1.0f;
		}
	}
	else
	{
		float	fFullTime	= m_fWaitTime + m_fPlayTime;

		if ( m_fTimeSum >= m_fWaitTime )
		{

			for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
			{
				m_pmcMesh->pMaterials[i].MatD3D.Diffuse.a	= 1.0f;
				m_pmcMesh->pMaterials[i].MatD3D.Ambient.a	= 1.0f;
				m_pmcMesh->pMaterials[i].MatD3D.Specular.a	= 1.0f;
			}
		}
	}

	return S_OK;
}

HRESULT	DxEffCharAlpha::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	else if ( dwVer == 0x0100 )
	{
		EFFCHAR_PROPERTY_ALPHA_100	pProp;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_ALPHA_100) );

		m_fWaitTime = 2.f;
		m_fPlayTime = 1.f;
		m_fHeight	= 50.f;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0101 )
	{
		EFFCHAR_PROPERTY_ALPHA_101	pProp;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &pProp, sizeof(EFFCHAR_PROPERTY_ALPHA_101) );

		m_fHeight	= pProp.m_fHeight;
		m_fPlayTime = pProp.m_fPlayTime;
		m_fWaitTime	= pProp.m_fWaitTime;

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

HRESULT	DxEffCharAlpha::SaveFile ( basestream &SFile )
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


