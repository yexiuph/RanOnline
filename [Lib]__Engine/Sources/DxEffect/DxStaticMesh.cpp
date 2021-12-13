#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"
#include "./DxRenderStates.h"
#include "./DxViewPort.h"
#include "./TextureSetDXT.h"
#include "./DxLandMan.h"

#include "./DxStaticMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	DxStaticMesh::VERSION = 0x0102;

DxStaticMesh::DxStaticMesh() :
	m_emRenderType(EMRT_OCTREE),
	m_pList(NULL),
	m_pAlphaList(NULL),
	m_pAlphaSoftList(NULL),
	m_pAlphaSoftList01(NULL),
	m_pAlphaSoftList02(NULL),
	m_pMeshTree(NULL),
	m_pMeshAlphaTree(NULL),
	m_pMeshSoftAlphaTree(NULL),
	m_pMeshSoftAlphaTree01(NULL),
	m_pMeshSoftAlphaTree02(NULL),
	m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
{
}

DxStaticMesh::~DxStaticMesh()
{
	CleanUp();

	// 파일을 닫는다.
	m_SFile.CloseFile();
}

void DxStaticMesh::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	InitializeCriticalSection(&m_CSLockLoading);
	m_sThread.StartThread( pd3dDevice, &m_SFile, &m_listLoadingData, &m_CSLockLoading );
}

void DxStaticMesh::EndThread()
{
	DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	관련 List를 지운다.

	m_sThread.EndThread();
	DeleteCriticalSection(&m_CSLockLoading);
}

void DxStaticMesh::CleanUp()
{
	m_vMin = D3DXVECTOR3 (FLT_MAX,FLT_MAX,FLT_MAX);
	m_vMax = D3DXVECTOR3 (-FLT_MAX,-FLT_MAX,-FLT_MAX);

	m_pList = NULL;
	m_pAlphaList = NULL;
	m_pAlphaSoftList = NULL;
	m_pAlphaSoftList01 = NULL;
	m_pAlphaSoftList02 = NULL;

	SAFE_DELETE ( m_pMeshTree );
	SAFE_DELETE ( m_pMeshAlphaTree );
	SAFE_DELETE ( m_pMeshSoftAlphaTree );
	SAFE_DELETE ( m_pMeshSoftAlphaTree01 );
	SAFE_DELETE ( m_pMeshSoftAlphaTree02 );

	std::for_each ( m_mapMesh.begin(), m_mapMesh.end(), std_afunc::DeleteMapObject() );
	m_mapMesh.clear();

	std::for_each ( m_mapMeshALPHA.begin(), m_mapMeshALPHA.end(), std_afunc::DeleteMapObject() );
	m_mapMeshALPHA.clear();

	std::for_each ( m_mapMeshSOFTALPHA.begin(), m_mapMeshSOFTALPHA.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA.clear();

	std::for_each ( m_mapMeshSOFTALPHA01.begin(), m_mapMeshSOFTALPHA01.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA01.clear();

	std::for_each ( m_mapMeshSOFTALPHA02.begin(), m_mapMeshSOFTALPHA02.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA02.clear();
}

void DxStaticMesh::MakeAABBOCTree( const LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* const pFrameMesh, const BOOL bProgress, BOOL bPiece )
{
	CleanUp();

	if ( pFrameMesh )
	{
		// SingleTexMeh를 만든다.
		NSSTATICMESH::ExportMaterialList( pd3dDevice, pFrameMesh, this, bProgress, bPiece );

		// TexEff에 맞는 형식으로 바꾸기 위함.
		NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMesh );
		NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshALPHA );
		NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA );
		NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA01 );
		NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA02 );

		// Tree를 만들기 위해 std::map 형식을 일반적인 list 형식으로 참조한다.
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

		// SingleMehs Tree를 생성한다.
		COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
		COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

		// 최대값과 최소값을 구한다.
		SetAABBMaxMin ();
	}
}

void DxStaticMesh::FrameMove( const float fElapsedTime )
{
	DxSingleTexMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->FrameMove( fElapsedTime );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaList;
	while( pCur )
	{
		pCur->FrameMove( fElapsedTime );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList;
	while( pCur )
	{
		pCur->FrameMove( fElapsedTime );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList01;
	while( pCur )
	{
		pCur->FrameMove( fElapsedTime );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList02;
	while( pCur )
	{
		pCur->FrameMove( fElapsedTime );
		pCur = pCur->m_pNext;
	}

	// Note : 조각 파일 일 경우는 계산하면 안된다.
	if( m_emRenderType==EMRT_PIECE )	return;

	// Note : 컬러 계산을 다하여서 리스트가 비어있다면 다시 삽입한다.
	if( DxStaticMeshColor_THREAD::GetInstance().IsInsertListData() )
	{
		DxStaticMeshColor_THREAD::GetInstance().EnterCS();
		{
			LOADINGDATALIST& sListColorData = DxStaticMeshColor_THREAD::GetInstance().m_listColorData;

			DxSingleTexMesh* pSTMesh = m_pList;
			while( pSTMesh )
			{
				pSTMesh->InsertColorList( sListColorData );
				pSTMesh = pSTMesh->m_pNext;
			}

			pSTMesh = m_pAlphaList;
			while( pSTMesh )
			{
				pSTMesh->InsertColorList( sListColorData );
				pSTMesh = pSTMesh->m_pNext;
			}

			pSTMesh = m_pAlphaSoftList;
			while( pSTMesh )
			{
				pSTMesh->InsertColorList( sListColorData );
				pSTMesh = pSTMesh->m_pNext;
			}

			pSTMesh = m_pAlphaSoftList01;
			while( pSTMesh )
			{
				pSTMesh->InsertColorList( sListColorData );
				pSTMesh = pSTMesh->m_pNext;
			}

			pSTMesh = m_pAlphaSoftList02;
			while( pSTMesh )
			{
				pSTMesh->InsertColorList( sListColorData );
				pSTMesh = pSTMesh->m_pNext;
			}

			//COLLISION::InsertColorList( m_pMeshTree, sListColorData );
			//COLLISION::InsertColorList( m_pMeshAlphaTree, sListColorData );
			//COLLISION::InsertColorList( m_pMeshSoftAlphaTree, sListColorData );
			//COLLISION::InsertColorList( m_pMeshSoftAlphaTree01, sListColorData );
			//COLLISION::InsertColorList( m_pMeshSoftAlphaTree02, sListColorData );
		}
		DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
	}
}

// Note : Thread Loading Render
void DxStaticMesh::Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	PROFILE_BEGIN("DxOctree::Render_THREAD_");
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, sCV, m_pMeshTree, m_listLoadingData, m_CSLockLoading );
	PROFILE_END("DxOctree::Render_THREAD_");
}

void DxStaticMesh::Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	PROFILE_BEGIN("DxOctree::Render_THREAD_Alpha");
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, sCV, m_pMeshAlphaTree, m_listLoadingData, m_CSLockLoading );
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	PROFILE_END("DxOctree::Render_THREAD_Alpha");
}

void DxStaticMesh::Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	PROFILE_BEGIN("DxOctree::Render_THREAD_SoftAlpha");
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, sCV, m_pMeshSoftAlphaTree, m_listLoadingData, m_CSLockLoading );

	float fBias;
	fBias = -0.0001f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, sCV, m_pMeshSoftAlphaTree01, m_listLoadingData, m_CSLockLoading );

	fBias = -0.0002f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, sCV, m_pMeshSoftAlphaTree02, m_listLoadingData, m_CSLockLoading );

	fBias = 0.0000f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	PROFILE_END("DxOctree::Render_THREAD_SoftAlpha");
}

// Note : Dynamic Loading Render	|| General Render <SFile==NULL>
void DxStaticMesh::Render_DYNAMIC_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile )
{
	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshTree, SFile );
}

void DxStaticMesh::Render_DYNAMIC_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile )
{
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshAlphaTree, SFile );
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMesh::Render_DYNAMIC_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree, SFile );
	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree01, SFile );
	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree02, SFile );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

// Piece File Render
void DxStaticMesh::Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff )
{
	DxSingleTexMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->Render( pd3dDevice, pmapTexEff );
		pCur = pCur->m_pNext;
	}
}

void DxStaticMesh::Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff )
{
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	DxSingleTexMesh* pCur = m_pAlphaList;
	while( pCur )
	{
		pCur->Render( pd3dDevice, pmapTexEff );
		pCur = pCur->m_pNext;
	}
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMesh::Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	DxSingleTexMesh* pCur;
	pCur = m_pAlphaSoftList;
	while( pCur )
	{
		pCur->Render( pd3dDevice, pmapTexEff );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList01;
	while( pCur )
	{
		pCur->Render( pd3dDevice, pmapTexEff );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList02;
	while( pCur )
	{
		pCur->Render( pd3dDevice, pmapTexEff );
		pCur = pCur->m_pNext;
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

// Note : Piece File Render
void DxStaticMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
{
	MAPSINGLETEXMESH_ITER iter = m_mapMesh.find( szName );
	if( iter!=m_mapMesh.end() )
	{
		(*iter).second->Render( pd3dDevice, sCV );
	}
}

void DxStaticMesh::RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
{
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	{
		MAPSINGLETEXMESH_ITER iter = m_mapMeshALPHA.find( szName );
		if( iter!=m_mapMeshALPHA.end() )
		{
			(*iter).second->Render( pd3dDevice, sCV );
		}
	}
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMesh::RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	{
		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA.find( szName );
		if( iter!=m_mapMeshSOFTALPHA.end() )
		{
			(*iter).second->Render( pd3dDevice, sCV );
		}
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxStaticMesh::RenderSoftAlpha01( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	{
		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA01.find( szName );
		if( iter!=m_mapMeshSOFTALPHA01.end() )
		{
			(*iter).second->Render( pd3dDevice, sCV );
		}
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxStaticMesh::RenderSoftAlpha02( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
{
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	{
		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA02.find( szName );
		if( iter!=m_mapMeshSOFTALPHA02.end() )
		{
			(*iter).second->Render( pd3dDevice, sCV );
		}
	}
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

// Note : vEnd 이 값은 계속 줄어든다.
//		그래서 제일 적은 위치에 있는것을 찾을 수 있다.
BOOL DxStaticMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, const BOOL bFrontColl )
{
	BOOL		bUse		= FALSE;

	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshALPHA.begin();
	for( ; iter!=m_mapMeshALPHA.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA.begin();
	for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA01.begin();
	for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA02.begin();
	for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, bFrontColl ) )	bUse = TRUE;
	}

	return bUse;
}

// Note : vEnd 이 값은 계속 줄어든다.
//		그래서 제일 적은 위치에 있는것을 찾을 수 있다.
BOOL DxStaticMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl )
{
	BOOL		bUse		= FALSE;

	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshALPHA.begin();
	for( ; iter!=m_mapMeshALPHA.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA.begin();
	for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA01.begin();
	for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	bUse = TRUE;
	}

	iter = m_mapMeshSOFTALPHA02.begin();
	for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, bFrontColl ) )	bUse = TRUE;
	}

	return bUse;
}

void DxStaticMesh::SetAABBMaxMin ()
{
	m_vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	D3DXVECTOR3	vMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	D3DXVECTOR3	vMin( FLT_MAX, FLT_MAX, FLT_MAX );

	MAPSINGLETEXMESH_ITER iter;
	for( iter=m_mapMesh.begin(); iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshALPHA.begin(); iter!=m_mapMeshALPHA.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA.begin(); iter!=m_mapMeshSOFTALPHA.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA01.begin(); iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA02.begin(); iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}
}

void DxStaticMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	float fDis = DxFogMan::GetInstance().GetFogRangeFar();
	D3DXVECTOR3 vMax = vPos + D3DXVECTOR3( fDis, fDis, fDis );
	D3DXVECTOR3 vMin = vPos - D3DXVECTOR3( fDis, fDis, fDis );

	BaseLoad( pd3dDevice, vMax, vMin );
}

void DxStaticMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
{
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshTree, &m_SFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshAlphaTree, &m_SFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree, &m_SFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree01, &m_SFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree02, &m_SFile );
}

void DxStaticMesh::Save ( const char* szName )
{
	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )	strName += '0';		// wld0 로 만듬.

	if ( m_SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	Save( m_SFile, FALSE );

	m_SFile.CloseFile();
}

void DxStaticMesh::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )	strName += '0';		// wld0 로 만듬.

	m_SFile.CloseFile();
	if ( m_SFile.OpenFile ( FOT_READ, strName.c_str() ) )	Load( pd3dDevice, m_SFile, TRUE, FALSE );
}

void DxStaticMesh::Save ( CSerialFile& SFile, BOOL bPiece )
{
	DWORD	dwSize = 0;

	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;
	}
	SFile.EndBlock();

	SingleTexMesh_Save( m_mapMesh,			SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshALPHA,		SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA, SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA01, SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA02, SFile, bPiece );
}

void DxStaticMesh::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece )
{
	CleanUp();	// 모두 초기화

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;
	}
	else if( dwVer==0x0101 )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		Load_101( pd3dDevice, SFile, bDynamicLoad, bPiece );

		return;
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		Load_101( pd3dDevice, SFile, bDynamicLoad, bPiece );

		// Ver.0x0100 에서 Max, Min 설정이 잘못 되어 있었다.
		SetAABBMaxMin ();

		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );

		return;
	}

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );
#endif

	// Note : DxSingleTexMesh 내부에서 Version 체크가 일어난다.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA01, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA02, SFile, bDynamicLoad, bPiece );

#ifndef  NDEBUG
	s_fEndTime = DXUtil_Timer( TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );
#endif

	// Tree를 만들기 위해 std::map 형식을 일반적인 list 형식으로 참조한다.
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

	// SingleMehs Tree를 생성한다.
	COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
	COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

#ifndef  NDEBUG
	s_fEndTime = DXUtil_Timer( TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMesh::SingleTexMesh_Save( MAPSINGLETEXMESH& mapMesh, CSerialFile& SFile, BOOL bPiece )
{
	DWORD	dwSize = 0;

	dwSize = (DWORD)mapMesh.size();
	SFile << dwSize;

	MAPSINGLETEXMESH_ITER iter;
	for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->Save( SFile, bPiece );
	}
}

void DxStaticMesh::SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, CSerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	DWORD	dwSize = 0;

	if( bPiece )	m_emRenderType = EMRT_PIECE;
	else			m_emRenderType = EMRT_OCTREE;

	std::string szName;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
		DxSingleTexMesh* pNew = new DxSingleTexMesh;
		pNew->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );

		mapMesh.insert( std::make_pair ( szName.c_str(), pNew ) );
	}
}

void DxStaticMesh::Load_101( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );

	// Note : DxSingleTexMesh 내부에서 Version 체크가 일어난다.
	DWORD	dwSize = 0;
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );

	// Tree를 만들기 위해 std::map 형식을 일반적인 list 형식으로 참조한다.
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMesh,			m_pList );
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMeshALPHA,	m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMeshSOFTALPHA, m_pAlphaSoftList );

	// SingleMehs Tree를 생성한다.
	COLLISION::MakeAABBTree ( m_pMeshTree,			m_pList );
	COLLISION::MakeAABBTree ( m_pMeshAlphaTree,		m_pAlphaList );
	COLLISION::MakeAABBTree ( m_pMeshSoftAlphaTree, m_pAlphaSoftList );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		S	T	A	T	I	C		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSSTATICMESH
{
	DWORD			dwMAXFACE	= 21000;	// 보다 크면 자체적으로 AABB 에 자체적 Tree를 구성한다.
	DWORD			dwMAXDIS	= 2000;

	MAPSTATICDATA	mapMeshData;
	MAPSTATICDATA	mapMeshAlpha;
	MAPSTATICDATA	mapMeshAlphaSOFT;
	MAPSTATICDATA	mapMeshAlphaSOFT01;
	MAPSTATICDATA	mapMeshAlphaSOFT02;

	void MakeStaticMesh3 ( char* szName, MAPSTATICDATA& mapMesh, VERTEX* pVertices, DWORD* pIndices, DWORD dwFaceStart, DWORD dwFaceCount, D3DXMATRIX& matWorld )
	{
		MAPSTATICDATA_ITER iter = mapMesh.find(szName);
		if ( iter==mapMesh.end() )	// New
		{
			STATICDATA* pData = new STATICDATA;
			pData->dwFaceNUM = dwFaceCount;
			pData->pVertices = new VERTEX[dwFaceCount*3];

			DWORD dwCount=0;

			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+0]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+1]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+2]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;
			}

			mapMesh.insert ( std::make_pair(szName,pData) );
		}
		else								// Add
		{
			STATICDATA* pData = (STATICDATA*)(*iter).second;

			// 기존 데이터 백업
			STATICDATA OldData;
			OldData.dwFaceNUM = pData->dwFaceNUM;
			OldData.pVertices = new VERTEX[pData->dwFaceNUM*3];
			memcpy ( OldData.pVertices, pData->pVertices, sizeof(VERTEX)*OldData.dwFaceNUM*3 );

			// 데이터 추가 삽입
			pData->dwFaceNUM += dwFaceCount;
			SAFE_DELETE_ARRAY ( pData->pVertices );
			pData->pVertices = new VERTEX[pData->dwFaceNUM*3];
			memcpy ( pData->pVertices, OldData.pVertices, sizeof(VERTEX)*OldData.dwFaceNUM*3 );
			SAFE_DELETE_ARRAY ( OldData.pVertices );

			DWORD dwCount=OldData.dwFaceNUM*3;

			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+0]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+1]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;

				pData->pVertices[dwCount] = pVertices [pIndices[(i*3)+2]];
				D3DXVec3TransformCoord ( &pData->pVertices[dwCount].vPos, &pData->pVertices[dwCount].vPos, &matWorld );
				D3DXVec3TransformNormal ( &pData->pVertices[dwCount].vNor, &pData->pVertices[dwCount].vNor, &matWorld );
				++dwCount;
			}
		}
	}

	void MakeStaticMesh2 ( DxMeshes* pMesh, D3DXMATRIX& matWorld )
	{
		VERTEX*	pVertices;

		DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		DWORD dwFaces	= pMesh->m_pLocalMesh->GetNumFaces();

		DWORD*	pIndices = new DWORD[dwFaces*3];
		if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		{
			DWORD*	pIndex32;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
			memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}
		else
		{
			WORD*	pIndex16;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				pIndices[i] = (DWORD)pIndex16[i];
			}
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}


		pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

		DWORD dwAttibTableSize;
		pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

		LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );

		for ( DWORD i=0; i<dwAttibTableSize; ++i )
		{
			char szName[64] = "";
			DWORD dwAttribID = pAttribTable[i].AttribId;
			StringCchCopy( szName, 64, pMesh->strTextureFiles[dwAttribID].GetString() );

			// Note : 먼저 예외적인 텍스쳐를 고른다.
			int nType = 0;
			if( CTextureSetDXT::GetInstance().FindTexture( szName, nType ) )
			{
				if( nType==0 )
					MakeStaticMesh3( szName, mapMeshData, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				else if( nType==1 )
					MakeStaticMesh3( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				else if( nType==2 )
					MakeStaticMesh3( szName, mapMeshAlphaSOFT, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				else if( nType==3 )
					MakeStaticMesh3( szName, mapMeshAlphaSOFT01, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				else if( nType==4 )
					MakeStaticMesh3( szName, mapMeshAlphaSOFT02, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );

				continue;
			}

			// Note : 기본 작업
			D3DSURFACE_DESC sDesc;
			sDesc.Format = D3DFMT_UNKNOWN;
			if ( pMesh->pTextures[dwAttribID] )	pMesh->pTextures[dwAttribID]->GetLevelDesc( 0, &sDesc );	// 텍스쳐가 없는 것들이 있을 수 있다.

			std::string strName;
			std::string strNewName;
			std::string::size_type idx;
			switch ( sDesc.Format )
			{
			case D3DFMT_DXT1:
				strName = szName;
				std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
				idx = strName.find("_a.");
				if( idx!=std::string::npos )
				{
					// 알파 사용
					MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
					break;
				}
				idx = strName.find("_a1.");
				if( idx!=std::string::npos )
				{
					// 알파 사용
					MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
					break;
				}
				idx = strName.find("_a2.");
				if( idx!=std::string::npos )
				{
					// 알파 사용
					MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
					break;
				}

				// 알파 아니다
				MakeStaticMesh3 ( szName, mapMeshData, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;

			case D3DFMT_DXT5:
				strName = szName;
				std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );

				idx = strName.find("_a1.");		// 2번째 뿌린다.
				if( idx!=std::string::npos )
				{
					MakeStaticMesh3 ( szName, mapMeshAlphaSOFT01, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
					break;
				}
				idx = strName.find("_a2.");		// 3번째 뿌린다.
				if( idx!=std::string::npos )
				{
					MakeStaticMesh3 ( szName, mapMeshAlphaSOFT02, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
					break;
				}
				
				// 일반적이다. Soft ~!
				MakeStaticMesh3 ( szName, mapMeshAlphaSOFT, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;

				// 데이터 정리를 위한 것
			case D3DFMT_DXT2:
			case D3DFMT_DXT3:
			case D3DFMT_DXT4:
				MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;

				// 압축 다시 하길 바람.	알파 있는 것
			case D3DFMT_A8R8G8B8:
			case D3DFMT_A1R5G5B5:
			case D3DFMT_A4R4G4B4:
			case D3DFMT_A8R3G3B2:
			case D3DFMT_A8:
				MakeStaticMesh3 ( szName, mapMeshAlpha, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;

				// 압축 다시 하길 바람.	알파 없는 것
			case D3DFMT_X8R8G8B8:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_X4R4G4B4:
			case D3DFMT_R8G8B8:
			case D3DFMT_R5G6B5:
			case D3DFMT_R3G3B2:
			default:
				MakeStaticMesh3 ( szName, mapMeshData, pVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld );
				break;
			};
		}
		pMesh->m_pLocalMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( pAttribTable );
		SAFE_DELETE_ARRAY ( pIndices );
	}

	void MakeStaticMesh1 ( DxFrame *pframeCur )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : 여기서는 아무것도 하면 안된다.
			return;
		}
		else
		{
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				MakeStaticMesh2 ( pMesh, pframeCur->matCombined );
				pMesh = pMesh->pMeshNext;
			}

			ExportProgress::CurPos++;
		}

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeStaticMesh1 ( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	// Note : 여기에서 하나씩의 DxSingleTexMesh들을 생성한다.
	void MakeStaticMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, const BOOL bProgress, BOOL bPiece )
	{
		MAPSTATICDATA_ITER iter;

		if( bProgress )
		{
			ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...2/3" );
			ExportProgress::CurPos= 0;
			ExportProgress::EndPos = (int)mapMeshData.size();
		}

		for ( iter=mapMeshData.begin(); iter!=mapMeshData.end(); ++iter )
		{
			DxSingleTexMesh*	pNew = new DxSingleTexMesh;
			pNew->Create( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (BYTE*)(*iter).second->pVertices, VERTEX::FVF, bPiece );
			pStaticMesh->m_mapMesh.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

			if( bProgress )	ExportProgress::CurPos++;
		}


		if( bProgress )
		{
			ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...3/3" );
			ExportProgress::CurPos= 0;
			ExportProgress::EndPos = int(mapMeshAlpha.size() + mapMeshAlphaSOFT.size() + mapMeshAlphaSOFT01.size() + mapMeshAlphaSOFT02.size());
		}

		for ( iter=mapMeshAlpha.begin(); iter!=mapMeshAlpha.end(); ++iter )
		{
			DxSingleTexMesh*	pNew = new DxSingleTexMesh;
			pNew->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (BYTE*)(*iter).second->pVertices, VERTEX::FVF, bPiece );
			pStaticMesh->m_mapMeshALPHA.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

			if( bProgress )	ExportProgress::CurPos++;
		}

		for ( iter=mapMeshAlphaSOFT.begin(); iter!=mapMeshAlphaSOFT.end(); ++iter )
		{
			DxSingleTexMesh*	pNew = new DxSingleTexMesh;
			pNew->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (BYTE*)(*iter).second->pVertices, VERTEX::FVF, bPiece );
			pStaticMesh->m_mapMeshSOFTALPHA.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

			if( bProgress )	ExportProgress::CurPos++;
		}

		for ( iter=mapMeshAlphaSOFT01.begin(); iter!=mapMeshAlphaSOFT01.end(); ++iter )
		{
			DxSingleTexMesh*	pNew = new DxSingleTexMesh;
			pNew->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (BYTE*)(*iter).second->pVertices, VERTEX::FVF, bPiece );
			pStaticMesh->m_mapMeshSOFTALPHA01.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

			if( bProgress )	ExportProgress::CurPos++;
		}

		for ( iter=mapMeshAlphaSOFT02.begin(); iter!=mapMeshAlphaSOFT02.end(); ++iter )
		{
			DxSingleTexMesh*	pNew = new DxSingleTexMesh;
			pNew->Create ( pd3dDevice, (*iter).first.c_str(), (*iter).second->dwFaceNUM, (BYTE*)(*iter).second->pVertices, VERTEX::FVF, bPiece );
			pStaticMesh->m_mapMeshSOFTALPHA02.insert ( std::make_pair ( (*iter).first.c_str(), pNew ) );

			if( bProgress )	ExportProgress::CurPos++;
		}
	}

	
	void FindNodeNUM ( DxFrame *pframeCur )
	{
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : 여기서는 아무것도 하면 안된다.
			return;
		}
		else															ExportProgress::EndPos++;

		//	Note : 자식들을 재귀적으로 모두 탐색.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			FindNodeNUM ( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	void ExportMaterialList ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* const pFrameMesh, DxStaticMesh* pStaticMesh, BOOL bProgress, BOOL bPiece )
	{
		DxFrame* pFrameRoot = pFrameMesh->GetFrameRoot();
		if ( pFrameRoot )
		{
			std::for_each ( mapMeshData.begin(), mapMeshData.end(), std_afunc::DeleteMapObject() );
			mapMeshData.clear();
			std::for_each ( mapMeshAlpha.begin(), mapMeshAlpha.end(), std_afunc::DeleteMapObject() );
			mapMeshAlpha.clear();
			std::for_each ( mapMeshAlphaSOFT.begin(), mapMeshAlphaSOFT.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT.clear();
			std::for_each ( mapMeshAlphaSOFT01.begin(), mapMeshAlphaSOFT01.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT01.clear();
			std::for_each ( mapMeshAlphaSOFT02.begin(), mapMeshAlphaSOFT02.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT02.clear();

			if( bProgress )
			{
				ExportProgress::SetStateString ( "Optimize Mesh로 생성 중...1/3" );
				ExportProgress::CurPos= 0;
				ExportProgress::EndPos = 0;

				FindNodeNUM( pFrameRoot );
			}

			MakeStaticMesh1( pFrameRoot );

			MakeStaticMesh( pd3dDevice, pStaticMesh, bProgress, bPiece );

			std::for_each ( mapMeshData.begin(), mapMeshData.end(), std_afunc::DeleteMapObject() );
			mapMeshData.clear();
			std::for_each ( mapMeshAlpha.begin(), mapMeshAlpha.end(), std_afunc::DeleteMapObject() );
			mapMeshAlpha.clear();
			std::for_each ( mapMeshAlphaSOFT.begin(), mapMeshAlphaSOFT.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT.clear();
			std::for_each ( mapMeshAlphaSOFT01.begin(), mapMeshAlphaSOFT01.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT01.clear();
			std::for_each ( mapMeshAlphaSOFT02.begin(), mapMeshAlphaSOFT02.end(), std_afunc::DeleteMapObject() );
			mapMeshAlphaSOFT02.clear();
		}
	}

	void ConvertMAPtoLIST ( MAPSINGLETEXMESH& mapData, PDXSINGLETEXMESH& pList )
	{
		MAPSINGLETEXMESH_ITER iter = mapData.begin();
		for ( ; iter!=mapData.end(); ++iter )
		{
			(*iter).second->m_pNext = pList;
			pList = (*iter).second;
		}
	}

	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, MAPSINGLETEXMESH& mapData )
	{
		if( !pTexEff )			return;

		MAPSINGLETEXMESH_ITER iter = mapData.begin();
		for( ; iter!=mapData.end(); ++iter )
		{
			(*iter).second->ConvertTexEffMesh( pd3dDevice, pTexEff );
		}
	}
};
