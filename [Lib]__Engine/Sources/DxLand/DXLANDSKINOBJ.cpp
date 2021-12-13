#include "pch.h"
#include "SerialFile.h"

#include "./DxMethods.h"
#include "./DxShadowMap.h"

#include "DxLandSkinObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const DWORD	DXLANDSKINOBJ::VERSION = 0x0102;
DWORD		DXLANDSKINOBJ::SIZE = sizeof(char)*LANDSKINOBJ_MAXSZ +
								sizeof(char)*MAX_PATH +
								sizeof(D3DXMATRIX) +
								sizeof(D3DXVECTOR3) +
								sizeof(D3DXVECTOR3) +
								sizeof(D3DXVECTOR3) +
								sizeof(D3DXVECTOR3);

float		DXLANDSKINOBJ::m_fTime = 0.0f;
float		DXLANDSKINOBJ::m_fElapsedTime = 0.0f;

DXLANDSKINOBJ::DXLANDSKINOBJ(void) :
	m_vMax(3,18,3),
	m_vMin(-3,0,-3),
	m_vMaxOrg(3,18,3),
	m_vMinOrg(-3,0,-3),
	m_pSkinCharData(NULL),
	m_pSkinChar(NULL),
	m_pNext(NULL),
	m_bRender(TRUE)
{
	StringCchCopy( m_szName, LANDSKINOBJ_MAXSZ, "이름없음" );
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

	D3DXMatrixIdentity( &m_matWorld );
}

DXLANDSKINOBJ::~DXLANDSKINOBJ(void)
{
	SAFE_DELETE(m_pSkinChar);

	SAFE_DELETE(m_pNext);
}

HRESULT DXLANDSKINOBJ::PlaySkinChar ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pSkinCharData )
	{
		m_pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_szFileName, pd3dDevice, FALSE );
		if ( !m_pSkinCharData )		return E_FAIL;
	}
	
	if ( !m_pSkinChar )
	{
		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData ( m_pSkinCharData, pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::SetSkinChar ( char* szName, const D3DXMATRIX& matWorld, char* szFile, LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;
	GASSERT(szFile);

	StringCchCopy( m_szName, LANDSKINOBJ_MAXSZ, szName );
	StringCchCopy( m_szFileName, MAX_PATH, szFile );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	m_matWorld = matWorld;
	COLLISION::TransformAABB( m_vMax, m_vMin, m_matWorld );

	hr = PlaySkinChar ( pd3dDevice );
	if (FAILED(hr) )	return hr;

	SetAABBBOX( m_vMaxOrg, m_vMinOrg );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );

	return S_OK;
}

void DXLANDSKINOBJ::SetAABBBOX( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if( m_pSkinChar )
	{
		vMax = m_pSkinChar->m_vMax;
		vMin = m_pSkinChar->m_vMin;
	}
}

HRESULT DXLANDSKINOBJ::Load ( CSerialFile	&SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DWORD dwVer, dwSize;
	
	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer( m_matWorld, sizeof(D3DXMATRIX) );

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile >> m_vMaxOrg;
		SFile >> m_vMinOrg;
	}
	else if ( dwVer == 0x0101 )
	{
		DXAFFINEPARTS m_sAffineParts;

		SFile.ReadBuffer ( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer ( &m_sAffineParts, sizeof(DXAFFINEPARTS) );

		D3DXMatrixCompX( m_matWorld, m_sAffineParts );

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile >> m_vMaxOrg;
		SFile >> m_vMinOrg;
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 버전이 틀려 로드가 생략되었습니다.",
			m_szName, m_szFileName );

		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
		hr = E_FAIL;
	}

	return hr;
}

HRESULT DXLANDSKINOBJ::Save ( CSerialFile	&SFile )
{
	SFile << VERSION;
	SFile << SIZE;		// 변한다면 수정해야함.

	SFile.WriteBuffer ( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
	SFile.WriteBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.WriteBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	SFile << m_vMax;
	SFile << m_vMin;

	SFile << m_vMaxOrg;
	SFile << m_vMinOrg;

	return S_OK;
}

HRESULT DXLANDSKINOBJ::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	if( !m_bRender ) return S_OK;

	HRESULT hr;

	if ( !m_pSkinChar )
	{
		hr = PlaySkinChar ( pd3dDevice );
		if (FAILED(hr) )	return hr;
	}

	if ( m_pSkinChar )
	{
		hr = m_pSkinChar->FrameMove ( m_fTime, m_fElapsedTime );
		if ( FAILED(hr) )	return hr;

		hr = m_pSkinChar->Render ( pd3dDevice, m_matWorld );
		if ( FAILED(hr) )	return hr;

		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matWorld, pd3dDevice );		// 그림자 그리기
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		return m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		return m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

