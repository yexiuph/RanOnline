#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"

#include "./DxStaticPisMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//								D	x		A	A	B	B	P	i	e	c	e		O	B	J
// -----------------------------------------------------------------------------------------------------------------------------------------
DxAABBPieceOBJ::DxAABBPieceOBJ() :
	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
	vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	pStaticPisEdit(NULL),
	pNext(NULL)
{
	
}

DxAABBPieceOBJ::~DxAABBPieceOBJ()
{
	DxPieceContainer::GetInstance().ReleasePiece ( szSrcName.c_str() );
	szSrcName = "";
	D3DXMatrixIdentity ( &matWorld );

	SAFE_DELETE ( pNext );
}

void DxAABBPieceOBJ::Create ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szDir )
{
	pStaticPisEdit = DxPieceContainer::GetInstance().LoadPiece ( pd3dDevice, szSrcName.c_str(), szDir );
	if ( !pStaticPisEdit )	return;

	vMax = pStaticPisEdit->GetAABBMax();
	vMin = pStaticPisEdit->GetAABBMin();

	ModifyMaxMin ( vMax, vMin, matWorld );	// World 매트릭스에 의한 Max Min 변형
}

void DxAABBPieceOBJ::ModifyMaxMin ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3 vPos[8];

	vPos[0] = D3DXVECTOR3 ( vMax.x, vMax.y, vMax.z );
	vPos[1] = D3DXVECTOR3 ( vMax.x, vMax.y, vMin.z );
	vPos[2] = D3DXVECTOR3 ( vMax.x, vMin.y, vMax.z );
	vPos[3] = D3DXVECTOR3 ( vMin.x, vMax.y, vMax.z );
	vPos[4] = D3DXVECTOR3 ( vMax.x, vMin.y, vMin.z );
	vPos[5] = D3DXVECTOR3 ( vMin.x, vMax.y, vMin.z );
	vPos[6] = D3DXVECTOR3 ( vMin.x, vMin.y, vMax.z );
	vPos[7] = D3DXVECTOR3 ( vMin.x, vMin.y, vMin.z );

	vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

	for ( DWORD i=0; i<8; ++i )
	{
		D3DXVec3TransformCoord ( &vPos[i], &vPos[i], &matWorld );

		if ( vMax.x < vPos[i].x )	vMax.x = vPos[i].x;
		if ( vMax.y < vPos[i].y )	vMax.y = vPos[i].y;
		if ( vMax.z < vPos[i].z )	vMax.z = vPos[i].z;

		if ( vMin.x > vPos[i].x )	vMin.x = vPos[i].x;
		if ( vMin.y > vPos[i].y )	vMin.y = vPos[i].y;
		if ( vMin.z > vPos[i].z )	vMin.z = vPos[i].z;
	}
}

//virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, OBJAABB** ppObj  )	{ return FALSE; }
BOOL DxAABBPieceOBJ::IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor )
{
//	if ( pStaticPis->IsCollisionLine ( vP1, vP2, vColl, vNor, matWorld ) )	return TRUE;
//	else																	return FALSE;
	return TRUE;
}

void DxAABBPieceOBJ::Save ( CSerialFile& SFile )
{
	SFile << vMax;
	SFile << vMin;
	SFile << szSrcName;
	SFile.WriteBuffer ( &matWorld, sizeof(D3DXMATRIX) );
}

void DxAABBPieceOBJ::Load ( CSerialFile& SFile )
{
	SFile >> vMax;
	SFile >> vMin;
	SFile >> szSrcName;
	SFile.ReadBuffer ( &matWorld, sizeof(D3DXMATRIX) );
}





// -----------------------------------------------------------------------------------------------------------------------------------------
//												D	x		A	A	B	B	P	i	e	c	e
// -----------------------------------------------------------------------------------------------------------------------------------------
DxAABBPiece::DxAABBPiece() :
	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
	vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	pAABBList(NULL),
	pAABBTree(NULL),
	pNext(NULL)
{
}

DxAABBPiece::~DxAABBPiece()
{
	SAFE_DELETE ( pAABBTree );
	SAFE_DELETE ( pAABBList );

	pNext = NULL;
}

BOOL DxAABBPiece::IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, POBJAABB& pObj )
{
	COLLISION::CollisionLineToTree( pAABBTree, vP1, vP2, pObj, vColl, vNor );

	if ( pObj )	return TRUE;
	else		return FALSE;
	//DxAABBPieceOBJ* pCur = pAABBPieceOBJ;
	//while ( pCur )
	//{
	//	pCur->IsCollisionLine ( vP1, vP2, vColl, vNor );
	//	pCur = pCur->pNext;
	//}
}

HRESULT DxAABBPiece::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME& sCV )
{
	if ( !pAABBList )	return S_OK;

//	DxPiece* pPis = pAABBList->pStaticPis;
//	if( !pPis )			return S_OK;

//	DxStaticMesh* pStaticMesh = pPis->GetStaticMesh();
//	if( !pStaticMesh )	return S_OK;


	//MAPSINGLETEXMESH_ITER iter;
	//iter = pStaticMesh->m_mapMesh.begin();
	//for( ; iter!=pStaticMesh->m_mapMesh.end(); ++iter )
	//{
	//	COLLISION::RenderAABBTree ( pd3dDevice, sCV, pAABBTree, (*iter).first.c_str() );
	//}

	//iter = pStaticMesh->m_mapMeshALPHA.begin();
	//for( ; iter!=pStaticMesh->m_mapMeshALPHA.end(); ++iter )
	//{
	//	COLLISION::RenderAABBTree ( pd3dDevice, sCV, pAABBTree, (*iter).first.c_str() );
	//}

	//iter = pStaticMesh->m_mapMeshSOFTALPHA.begin();
	//for( ; iter!=pStaticMesh->m_mapMeshSOFTALPHA.end(); ++iter )
	//{
	//	COLLISION::RenderAABBTree ( pd3dDevice, sCV, pAABBTree, (*iter).first.c_str() );
	//}

	return S_OK;
}