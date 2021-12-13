#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"

#include "DxGlowMan.h"
#include "DxDynamicVB.h"
#include "DxCubeMap.h"
#include "DxSurfaceTex.h"
#include "DxEnvironment.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxSkinCharData.h"


#include "./DxEffectSkinMesh.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD SKINMESH_PROPERTY::VERSION = 0x0101;
const char	SKINMESH_PROPERTY::NAME[]   = "��Ų�޽� PLAY";

//	Note: �⺻���� �ʱ�ȭ�� �� ����Ÿ�� ���� ���� Ŭ������ �Ѱ��ش�.
//		  ���� ���õ� ��� ����Ÿ�� �ѱ�� �κ��̴�.
//        �� �κ��� ȣ��Ǳ� �������� ����Ŭ������ �������� �ʴ´�.
DxEffSingle* SKINMESH_PROPERTY::NEWOBJ ()
{
	//DxEffectSkinMesh *pEffect = new DxEffectSkinMesh;
	DxEffectSkinMesh *pEffect = DxEffectSkinMesh::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : �ð� ����.			[���뼳��]
	//
	pEffect->m_fGBeginTime		= m_fGBeginTime;
	pEffect->m_fGLifeTime		= m_fGLifeTime;		

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	//
	pEffect->m_bMoveObj = m_bMoveObj;
	
	// Note: �ʱ� ���� �κ� 
	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_pSkinChar = &m_sSkinChar;
	
	//	Note : ���� ����
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT SKINMESH_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT SKINMESH_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	if ( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

HRESULT SKINMESH_PROPERTY::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_szMeshFile, pd3dDevice, TRUE );

	if( pSkinCharData == NULL ) return E_FAIL;

	m_sSkinChar.SetCharData( pSkinCharData, pd3dDevice );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );	

	return S_OK;
}


//HRESULT SKINMESH_PROPERTY::DeleteDeviceObjects ()
//{	
//	//	Note : ����, �ڽ�.
//	//
//	EFF_PROPERTY::DeleteDeviceObjects ();	
//
//	return S_OK;
//}


//------------------------------- ���� Ŭ����(CMeshEffect)���� ----------------------------------//
const DWORD DxEffectSkinMesh::TYPEID = EFFSINGLE_SKINMESH;
const DWORD DxEffectSkinMesh::FLAG = NULL;
const char DxEffectSkinMesh::NAME[] = "��Ų�޽� PLAY";

CMemPool<DxEffectSkinMesh> *DxEffectSkinMesh::m_pPool = NULL;

DxEffectSkinMesh::DxEffectSkinMesh(void):
				m_fElapsedTime(0.f),
				m_pd3dDevice(NULL),
				m_pSkinChar(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectSkinMesh::~DxEffectSkinMesh(void)
{
	CleanUp ();
}

//Note: CreateDivece�� ReleaseDevice�� ��� DxEffSingleMan����
//      InvalidateDeviceObjects()�� RestoreDeviceObjects()���� ȣ���ϴ� �κ��̴�.
HRESULT DxEffectSkinMesh::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
HRESULT DxEffectSkinMesh::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectSkinMesh::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectSkinMesh>;
}

void DxEffectSkinMesh::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectSkinMesh::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vMaxMesh = m_pSkinChar->m_vMax;
	D3DXVECTOR3 vMinMesh = m_pSkinChar->m_vMin;

	D3DXMatrixAABBTransform( vMaxMesh, vMinMesh, m_matWorld );

	if( vMax.x < vMaxMesh.x )	vMax.x = vMaxMesh.x;
	if( vMax.y < vMaxMesh.y )	vMax.y = vMaxMesh.y;
	if( vMax.z < vMaxMesh.z )	vMax.z = vMaxMesh.z;

	if( vMin.x > vMinMesh.x )	vMin.x = vMinMesh.x;
	if( vMin.y > vMinMesh.y )	vMin.y = vMinMesh.y;
	if( vMin.z > vMinMesh.z )	vMin.z = vMinMesh.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectSkinMesh::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );	

	if( m_pSkinChar ) m_pSkinChar->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectSkinMesh::InvalidateDeviceObjects()
{	
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::DeleteDeviceObjects()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::DeleteDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fElapsedTime	= fElapsedTime; 
	m_fGAge			+= fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� z�ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	m_pSkinChar->FrameMove ( fTime, fElapsedTime );

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );
	return S_OK;
}

HRESULT DxEffectSkinMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;


	D3DXMATRIX matWorld;

	D3DXMatrixMultiply( &matWorld, &m_matLocal, &matComb );
	m_matWorld = matWorld;

	m_pSkinChar->Render ( pd3dDevice, matWorld );

	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	
	return S_OK;	
}


HRESULT DxEffectSkinMesh::FinalCleanup ()
{
	
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

