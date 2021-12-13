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
const char	SKINMESH_PROPERTY::NAME[]   = "스킨메쉬 PLAY";

//	Note: 기본적인 초기화를 한 데이타를 실제 구동 클래스로 넘겨준다.
//		  최종 셋팅된 모든 데이타를 넘기는 부분이다.
//        이 부분이 호출되기 전까지는 구동클래스가 생성되지 않는다.
DxEffSingle* SKINMESH_PROPERTY::NEWOBJ ()
{
	//DxEffectSkinMesh *pEffect = new DxEffectSkinMesh;
	DxEffectSkinMesh *pEffect = DxEffectSkinMesh::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	//
	pEffect->m_fGBeginTime		= m_fGBeginTime;
	pEffect->m_fGLifeTime		= m_fGLifeTime;		

	//	Note : 날아가는 오브젝트	[공통설정]
	//
	pEffect->m_bMoveObj = m_bMoveObj;
	
	// Note: 초기 셋팅 부분 
	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_pSkinChar = &m_sSkinChar;
	
	//	Note : 사운드 설정
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT SKINMESH_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
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

	//	Note : 버전이 일치할 경우. 
	if ( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
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

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );	

	return S_OK;
}


//HRESULT SKINMESH_PROPERTY::DeleteDeviceObjects ()
//{	
//	//	Note : 형제, 자식.
//	//
//	EFF_PROPERTY::DeleteDeviceObjects ();	
//
//	return S_OK;
//}


//------------------------------- 구동 클래스(CMeshEffect)시작 ----------------------------------//
const DWORD DxEffectSkinMesh::TYPEID = EFFSINGLE_SKINMESH;
const DWORD DxEffectSkinMesh::FLAG = NULL;
const char DxEffectSkinMesh::NAME[] = "스킨메쉬 PLAY";

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

//Note: CreateDivece와 ReleaseDevice는 모두 DxEffSingleMan에서
//      InvalidateDeviceObjects()와 RestoreDeviceObjects()에서 호출하는 부분이다.
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

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );	

	if( m_pSkinChar ) m_pSkinChar->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectSkinMesh::InvalidateDeviceObjects()
{	
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::DeleteDeviceObjects()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::DeleteDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fElapsedTime	= fElapsedTime; 
	m_fGAge			+= fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
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

	//	Note : 시작되지 않았는지 검사, 시작 z시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	m_pSkinChar->FrameMove ( fTime, fElapsedTime );

_RETURN:
	//	Note : 형제, 자식 노드.
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

	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	
	return S_OK;	
}


HRESULT DxEffectSkinMesh::FinalCleanup ()
{
	
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

