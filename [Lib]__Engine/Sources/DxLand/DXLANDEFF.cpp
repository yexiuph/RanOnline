#include "pch.h"

#include "./DxLandEff.h"
#include "./SerialFile.h"
#include "./DxMethods.h"
#include "./EditMeshs.h"
#include "./DxViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL					g_bWORLD_TOOL;

DWORD DXLANDEFF::VERSION = 0x0101;
DWORD DXLANDEFF::SIZE = sizeof(char)*MAX_PATH +
						sizeof(D3DXMATRIX) + 
						sizeof(char)*DXLANDEFF::LANDEFF_MAXSZ;

float DXLANDEFF::m_fTime = 0.0f;
float DXLANDEFF::m_fElapsedTime = 0.0f;

DXLANDEFF::DXLANDEFF(void) :
	m_vMax(0,0,0),
	m_vMin(0,0,0),
	m_pPropGroup(NULL),
	m_pSingleGroup(NULL),
	m_pNext(NULL)
{
	StringCchCopy( m_szName, LANDEFF_MAXSZ, "�̸�����" );

	D3DXMatrixIdentity( &m_matWorld );
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );
}

DXLANDEFF::~DXLANDEFF(void)
{
	SAFE_DELETE(m_pSingleGroup);

	SAFE_DELETE(m_pNext);
}

HRESULT DXLANDEFF::SetEffect ( char* szName, const D3DXMATRIX &matWorld, const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	GASSERT(szFile);

	StringCchCopy( m_szName, LANDEFF_MAXSZ, szName );

	StringCchCopy( m_szFileName, MAX_PATH, szFile );
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( szFile );
	if ( !m_pPropGroup )	return E_FAIL;

	m_vMax = m_pPropGroup->m_vMax;
	m_vMin = m_pPropGroup->m_vMin;
	m_matWorld = matWorld;
	COLLISION::TransformAABB ( m_vMax, m_vMin, matWorld );

	if ( m_pSingleGroup )
	{
		hr = PlaySingleGroup ( pd3dDevice );
		if (FAILED(hr) )	return hr;
	}

	return S_OK;
}

void DXLANDEFF::ReSetAABBBox()
{
	if( !m_pPropGroup )	return;

	m_vMax = m_pPropGroup->m_vMax;
	m_vMin = m_pPropGroup->m_vMin;
	COLLISION::TransformAABB( m_vMax, m_vMin, m_matWorld );
}

HRESULT DXLANDEFF::Load ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DWORD dwVer, dwSize;
	
	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
		SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

		m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName );
		if ( m_pPropGroup )
		{
			m_vMax = m_pPropGroup->m_vMax;
			m_vMin = m_pPropGroup->m_vMin;
			COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
		}
		else
		{
			CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] �ܵ� ����Ʈ�� ���������� �о���µ� �����Ͽ����ϴ�.",
				m_szName, m_szFileName );

			hr = E_FAIL;
		}
	}
	else if ( dwVer == 0x0100 )
	{
		DXAFFINEPARTS	m_sAffineParts;

		SFile.ReadBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
		SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer ( &m_sAffineParts, sizeof(DXAFFINEPARTS) );

		D3DXMatrixCompX ( m_matWorld, m_sAffineParts );

		m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName );
		if ( m_pPropGroup )
		{
			m_vMax = m_pPropGroup->m_vMax;
			m_vMin = m_pPropGroup->m_vMin;
			COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );
		}
		else
		{
			CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] �ܵ� ����Ʈ�� ���������� �о���µ� �����Ͽ����ϴ�.",
				m_szName, m_szFileName );

			hr = E_FAIL;
		}
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] �ܵ� ����Ʈ�� ������ Ʋ�� �ε尡 �����Ǿ����ϴ�.",
			m_szName, m_szFileName );

		SFile.SetOffSet ( SFile.GetfTell() + dwSize );

		hr = E_FAIL;
	}

	return hr;
}

HRESULT DXLANDEFF::Save ( CSerialFile	&SFile )
{
	SFile << VERSION;
	SFile << SIZE;		// ���Ѵٸ� �����ؾ���.

	SFile.WriteBuffer ( m_szName, sizeof(char)*LANDEFF_MAXSZ );
	SFile.WriteBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
	SFile.WriteBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

	return S_OK;
}

HRESULT DXLANDEFF::PlaySingleGroup ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	if ( m_pPropGroup )
	{
		SAFE_DELETE(m_pSingleGroup);

		//	Note : Eff Group instance ����.
		//
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP ();
		if ( !m_pSingleGroup )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		hr = m_pSingleGroup->Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

		//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
		//
		m_pSingleGroup->m_matWorld = DXLANDEFF::m_matWorld;

		//	Note : ������ ����. (���� �ܰ迡�� �׸��� �˻� �Ϸ��ϱ� ����. )
		//
		//m_pSingleGroup->m_vMax = m_vMax;
		//m_pSingleGroup->m_vMin = m_vMin;
	}

	return S_OK;
}

HRESULT DXLANDEFF::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr;

	if ( !m_pSingleGroup && m_pPropGroup )
	{
		hr = PlaySingleGroup ( pd3dDevice );
		if (FAILED(hr) )	return hr;
	}

	if ( m_pSingleGroup )
	{
		if ( m_pSingleGroup->GetAppElapsedTime()!=m_fElapsedTime )
		{
			hr = m_pSingleGroup->FrameMove ( m_fTime, m_fElapsedTime );
			if ( FAILED(hr) )	return hr;
		}

	//	EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );

		//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
		m_pSingleGroup->m_matWorld = DXLANDEFF::m_matWorld;

		hr = m_pSingleGroup->Render ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

void DXLANDEFF::RenderName( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vOut;
	D3DXVECTOR3 vSrc( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(), &matIdentity );
	CDebugSet::ToPos( vOut.x, vOut.y, "%s", m_szName );
}

HRESULT DXLANDEFF::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSingleGroup )
	{
		return m_pSingleGroup->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDEFF::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSingleGroup )
	{
		return m_pSingleGroup->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDEFF::InvalidateDeviceObjects ()
{
	if ( m_pSingleGroup )
	{
		return m_pSingleGroup->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DXLANDEFF::DeleteDeviceObjects()
{
	if ( m_pSingleGroup )
	{
		return m_pSingleGroup->DeleteDeviceObjects ();
	}

	SAFE_DELETE(m_pSingleGroup);

	return S_OK;
}

BOOL DXLANDEFF::IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor )
{
	return COLLISION::IsCollisionLineToAABB( vP1, vP2, m_vMax, m_vMin );
}





