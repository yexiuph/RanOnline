#include "pch.h"

#include "Collision.h"
#include "DxOctreeMesh.h"
#include "./DxFrameMesh.h"
#include "DxLandMan.h"
#include "./TextureManager.h"

#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DxOctreeMesh::~DxOctreeMesh()
{
	ReleaseDxOctreeMesh();
	ReleaseAttribure();
	SAFE_DELETE(m_pAABBTreeRoot);
}

HRESULT		DxOctreeMesh::CreateDxOctreeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF )
{
	m_dwFVF = FVF;

	m_dwNumFaces = NumFaces;
	if ( FAILED ( pd3dDevice->CreateIndexBuffer( m_dwNumFaces * 3 * sizeof(WORD),
		0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL ) ) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::CreateDxOctreeMesh() -- CreateIndexBuffer() -- Failed" );
		return E_FAIL;
	}


	m_dwNumVertices = NumVertices;
	if ( FAILED ( pd3dDevice->CreateVertexBuffer( m_dwNumVertices * sizeof(OCTREEVERTEX),
		0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL ) ) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::CreateDxOctreeMesh() -- CreateVertexBuffer() -- Failed" );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxOctreeMesh::LockIndexBuffer ( VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pIB->Lock( 0, 0, ppbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::LockIndexBuffer(VOID) -- m_pIB->Lock() -- Failed" );
	}
	return hr;
}

HRESULT	DxOctreeMesh::LockIndexBuffer ( DWORD dwFlag, VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pIB->Lock( 0, 0, ppbData, dwFlag );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::LockIndexBuffer(DWORD,VOID) -- m_pIB->Lock() -- Failed" );
	}
	return hr;
}


HRESULT	DxOctreeMesh::LockVertexBuffer ( VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pVB->Lock( 0, 0, ppbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::LockVertexBuffer(VOID) -- m_pVB->Lock() -- Failed" );
	}
	return hr;
}

HRESULT	DxOctreeMesh::LockVertexBuffer ( DWORD dwFlag, VOID** ppbData )
{
	HRESULT hr(S_OK);
	hr = m_pVB->Lock( 0, 0, ppbData, dwFlag );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxOctreeMesh::LockVertexBuffer(DWORD,VOID) -- m_pVB->Lock() -- Failed" );
	}
	return hr;
}


HRESULT	DxOctreeMesh::UnlockVertexBuffer()
{
	return m_pVB->Unlock();
}

HRESULT	DxOctreeMesh::UnlockIndexBuffer	()
{
	return m_pIB->Unlock();
}

HRESULT	DxOctreeMesh::ReleaseDxOctreeMesh	()
{
	m_dwNumFaces = 0;
	m_dwNumVertices = 0;
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	return S_OK;
}


BOOL	DxOctreeMesh::CreateAttribute	( DWORD		AttribTableSize )
{
	if ( m_pAttribTable )
	{
		return	FALSE;
	}

	m_dwAttribTableSize = AttribTableSize;
	m_pAttribTable = new D3DXATTRIBUTERANGE [ m_dwAttribTableSize ];

	memset ( m_pAttribTable, 0, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

	return TRUE;
}

LPD3DXATTRIBUTERANGE	DxOctreeMesh::GetAttributeTbl	()
{
	return m_pAttribTable;
}

BOOL	DxOctreeMesh::ReleaseAttribure()
{
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_dwAttribTableSize = 0;

	return TRUE;
}

HRESULT	DxOctreeMesh::DrawSubset( DWORD	AttribID, LPDIRECT3DDEVICEQ pd3dDevice )
{		
	return pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
			0,
			m_pAttribTable[AttribID].VertexStart,
			m_pAttribTable[AttribID].VertexCount,
			m_pAttribTable[AttribID].FaceStart * 3,
			m_pAttribTable[AttribID].FaceCount
		);
}

HRESULT	DxOctreeMesh::MakeAABBTree ( D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	WORD *pLockIndices=NULL;
	PBYTE pbLockPoints = NULL;

	hr = LockIndexBuffer( (VOID**)&pLockIndices );
	if ( FAILED ( hr ) )
		return hr;

	hr = LockVertexBuffer( (VOID**)&pbLockPoints );
	if ( FAILED ( hr ) )
		return hr;


	WORD *pFaces;
	pFaces = new WORD[m_dwNumFaces];
	if ( pFaces == NULL )	return E_OUTOFMEMORY;

	for ( WORD i=0; i<m_dwNumFaces; i++ )
		pFaces[i] = i;

	D3DXVECTOR3 vMax, vMin;
	hr = COLLISION::GetSizeNode ( matComb, pLockIndices, pbLockPoints, m_dwFVF, pFaces, m_dwNumFaces, vMax, vMin );
	if ( FAILED ( hr ) )
	{
		delete[] pFaces;
		return hr;
	}

	DxAABBNode *pNode = new DxAABBNode;
	pNode->fMaxX = vMax.x;
	pNode->fMaxY = vMax.y;
	pNode->fMaxZ = vMax.z;
	pNode->fMinX = vMin.x;
	pNode->fMinY = vMin.y;
	pNode->fMinZ = vMin.z;

	hr = COLLISION::MakeAABBNode ( pNode, matComb, pLockIndices, pbLockPoints, m_dwFVF,
		pFaces, m_dwNumFaces, vMax, vMin );
	delete[] pFaces;

	m_pAABBTreeRoot = pNode;
	
	UnlockIndexBuffer();
	UnlockVertexBuffer();	
	
	ExportProgress::CurPos += m_dwNumFaces;

	return hr;
}

static BOOL s_bCollision;
void DxOctreeMesh::IsCollision ( DxAABBNode *pAABBCur, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
								D3DXVECTOR3 &vNormal, DWORD& dwAttribid, const BOOL bFrontColl )
{
	D3DXVECTOR3 vNewP1=vP1, vNewP2=vP2;

	if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
	{
		if ( pAABBCur->dwFace != AABB_NONINDEX)
		{
			PBYTE pbDataVB;
			PWORD pwIndexB;

			if ( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pbDataVB, D3DLOCK_READONLY ) ) )
			{
				CDebugSet::ToLogFile( "DxOctreeMesh::IsCollision() -- m_pVB->Lock() -- Failed" );
				return;
			}

			if ( FAILED( m_pIB->Lock( 0, 0, (VOID**)&pwIndexB, D3DLOCK_READONLY ) ) )
			{
				CDebugSet::ToLogFile( "DxOctreeMesh::IsCollision() -- m_pIB->Lock() -- Failed" );
				m_pVB->Unlock();
				return;
			}

			DWORD fvfsize = D3DXGetFVFVertexSize ( m_dwFVF );
			PWORD pwIndex = pwIndexB + pAABBCur->dwFace*3;
			LPD3DXVECTOR3 pvT0 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex)));
			LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+1)));
			LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+2)));


			D3DXVECTOR3 vNewCollision;
			D3DXVECTOR3 vNewNormal;
			if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vNewP1, &vNewP2, &vNewCollision, &vNewNormal, bFrontColl ) )
			{
				D3DXVECTOR3 vDxVect;
				vDxVect = COLLISION::vColTestStart - vNewCollision;
				float fNewLength = D3DXVec3LengthSq ( &vDxVect );
				vDxVect = COLLISION::vColTestStart - vCollision;
				float fOldLength = D3DXVec3LengthSq ( &vDxVect );

				if ( !s_bCollision )
				{
					vCollision = vNewCollision;
					vNormal = vNewNormal;
				}

				if ( fNewLength < fOldLength )
				{
					vCollision = vNewCollision;
					vNormal = vNewNormal;
				}

				s_bCollision = TRUE;

				// Tex 정보 알아오기
				for( DWORD i=0; i<m_dwAttribTableSize; ++i )
				{
					if( (pAABBCur->dwFace>=m_pAttribTable[i].FaceStart) &&
						(pAABBCur->dwFace<=(m_pAttribTable[i].FaceStart+m_pAttribTable[i].FaceCount)) )
					{
						dwAttribid = m_pAttribTable[i].AttribId;
					}
				}
			}

			m_pVB->Unlock();
			m_pIB->Unlock();
		}
		else
		{
			if ( pAABBCur->pLeftChild )
				IsCollision ( pAABBCur->pLeftChild, vNewP1, vNewP2, vCollision, vNormal, dwAttribid, bFrontColl );

			if ( pAABBCur->pRightChild )
				IsCollision ( pAABBCur->pRightChild, vNewP1, vNewP2, vCollision, vNormal, dwAttribid, bFrontColl );
		}
	}
}

void DxOctreeMesh::IsCollision ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2,
								D3DXVECTOR3 &vCollision, BOOL &bCollision, D3DXVECTOR3 &vNormal, DWORD& dwAttribid, const BOOL bFrontColl )
{
	if ( !m_pAABBTreeRoot )	return;

	s_bCollision = FALSE;

	IsCollision ( m_pAABBTreeRoot, vPoint1, vPoint2, vCollision, vNormal, dwAttribid, bFrontColl );

	if ( s_bCollision )		bCollision = TRUE;
}

HRESULT DxOctreeMesh::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXMATRIX *pmatComb )
{
	HRESULT hr;
	if ( !pMesh )		return E_FAIL;
	GASSERT(!(pMesh->GetOptions()&D3DXMESH_32BIT)&&"32bit index를 지원하지 않습니다.");
	
	ReleaseDxOctreeMesh	();

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	m_dwNumVertices = pMesh->GetNumVertices ();
	m_dwNumFaces = pMesh->GetNumFaces ();
	m_dwFVF = pMesh->GetFVF ();

	DWORD dwVertexSize = D3DXGetFVFVertexSize ( m_dwFVF );

	hr = CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );
	if ( FAILED(hr) )	return E_FAIL;

	BYTE*			pDestVertices;
	WORD*			pDestIndices;
	BYTE*			pSrcVertices;
	WORD*			pSrcIndices;

	//	Note : 버텍스 버퍼 카피
	//
	hr = pMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockVertexBuffer ( (VOID**)&pDestVertices );
	if ( FAILED(hr) )	return E_FAIL;

	memcpy ( pDestVertices, pSrcVertices, m_dwNumVertices*dwVertexSize );

	if ( pmatComb )
	{
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			D3DXVec3TransformCoord ( (D3DXVECTOR3*)pDestVertices, (D3DXVECTOR3*)pSrcVertices, pmatComb );
			pDestVertices += sizeof(BYTE)*dwVertexSize;
			pSrcVertices += sizeof(BYTE)*dwVertexSize;
		}
	}

	pMesh->UnlockVertexBuffer ();
	UnlockVertexBuffer();

	
	//	Note : 인덱스 버퍼 카피
	//
	hr = pMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockIndexBuffer ( (VOID**)&pDestIndices );
	if ( FAILED(hr) )	return E_FAIL;

	memcpy ( pDestIndices, pSrcIndices, m_dwNumFaces*3*sizeof(WORD) );

	pMesh->UnlockIndexBuffer ();
	UnlockIndexBuffer	();

	//	Note : Attribute Table 가져오기.
	//
	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	pMesh->GetAttributeTable ( m_pAttribTable, &m_dwAttribTableSize );

	return S_OK;
}

HRESULT DxOctreeMesh::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOctreeMesh* pMesh, D3DXMATRIX *pmatComb )
{
	HRESULT hr;
	if ( !pMesh )		return E_FAIL;
	
	ReleaseDxOctreeMesh	();

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	m_dwNumVertices = pMesh->GetNumVertices ();
	m_dwNumFaces = pMesh->GetNumFaces ();
	m_dwFVF = pMesh->GetFVF ();

	DWORD dwVertexSize = D3DXGetFVFVertexSize ( m_dwFVF );

	hr = CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );
	if ( FAILED(hr) )	return E_FAIL;

	BYTE*			pDestVertices;
	WORD*			pDestIndices;
	BYTE*			pSrcVertices;
	WORD*			pSrcIndices;

	//	Note : 버텍스 버퍼 카피
	//
	hr = pMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockVertexBuffer ( (VOID**)&pDestVertices );
	if ( FAILED(hr) )	return E_FAIL;

	memcpy ( pDestVertices, pSrcVertices, m_dwNumVertices*dwVertexSize );

	if ( pmatComb )
	{
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			D3DXVec3TransformCoord ( (D3DXVECTOR3*)pDestVertices, (D3DXVECTOR3*)pSrcVertices, pmatComb );
			pDestVertices += sizeof(BYTE)*dwVertexSize;
			pSrcVertices += sizeof(BYTE)*dwVertexSize;
		}
	}

	pMesh->UnlockVertexBuffer ();
	UnlockVertexBuffer();

	
	//	Note : 인덱스 버퍼 카피
	//
	hr = pMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );
	if ( FAILED(hr) )	return E_FAIL;

	hr = LockIndexBuffer ( (VOID**)&pDestIndices );
	if ( FAILED(hr) )	return E_FAIL;

	memcpy ( pDestIndices, pSrcIndices, m_dwNumFaces*3*sizeof(WORD) );

	pMesh->UnlockIndexBuffer ();
	UnlockIndexBuffer	();

	//	Note : Attribute Table 가져오기.
	//
	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	pMesh->GetAttributeTable ( m_pAttribTable, &m_dwAttribTableSize );

	return S_OK;
}

HRESULT DxOcMeshes::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pMesh, D3DXMATRIX *pmatComb )
{
	GASSERT(pMesh);
	HRESULT hr;

	ClearAll ();

	pOcMesh = new DxOctreeMesh;
	hr = pOcMesh->CloneMesh ( pd3dDevice, pMesh->m_pLocalMesh, pmatComb );
	if ( FAILED(hr) )	return hr;

	cMaterials = pMesh->cMaterials;
	pMaterials = new DXOCMATERIAL[cMaterials];

	for ( DWORD i=0; i<cMaterials; i++ )
	{
		StringCchCopy( pMaterials[i].szTexture, MAX_PATH, pMesh->strTextureFiles[i].GetString () );

		hr = TextureManager::LoadTexture ( pMaterials[i].szTexture,
				pd3dDevice, pMaterials[i].pTexture, 0, 0 );
		if (FAILED(hr))
		{
			hr = S_OK;
			pMaterials[i].pTexture = NULL;
		}

		pMaterials[i].rgMaterial = pMesh->rgMaterials[i];
	}

	return S_OK;
}

HRESULT DxOcMeshes::CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOcMeshes *pMesh, D3DXMATRIX *pmatComb )
{
	GASSERT(pMesh);
	HRESULT hr;

	ClearAll ();

	pOcMesh = new DxOctreeMesh;
	hr = pOcMesh->CloneMesh ( pd3dDevice, pMesh->pOcMesh, pmatComb );
	if ( FAILED(hr) )	return hr;

	cMaterials = pMesh->cMaterials;
	pMaterials = new DXOCMATERIAL[cMaterials];

	memcpy ( pMaterials, pMesh->pMaterials, sizeof(DXOCMATERIAL)*cMaterials );

	for ( DWORD i=0; i<cMaterials; i++ )
	{
		hr = TextureManager::LoadTexture ( pMaterials[i].szTexture,
				pd3dDevice, pMaterials[i].pTexture, 0, 0 );
		if (FAILED(hr))
		{
			hr = S_OK;
			pMaterials[i].pTexture = NULL;
		}
	}

	return S_OK;
}

HRESULT DxOcMeshes::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pOcMesh || !pOcMesh->m_pVB )		return S_FALSE;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, pOcMesh->m_pVB, 0, D3DXGetFVFVertexSize(pOcMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( pOcMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( pOcMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;


	for ( DWORD i=0; i<pOcMesh->m_dwAttribTableSize; i++ )
	{
		DWORD dwMatID = pOcMesh->m_pAttribTable[i].AttribId;

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetMaterial ( &pMaterials[dwMatID].rgMaterial );
		pd3dDevice->SetTexture ( 0, pMaterials[dwMatID].pTexture );

		pOcMesh->DrawSubset ( i, pd3dDevice );
	}

	return S_OK;
}

void DxOcMeshes::ClearAll ()
{
	if ( pOcMesh )
	{
		SAFE_DELETE(pOcMesh);
		pOcMesh = NULL;
	}
	if ( pMaterials )
	{
		SAFE_DELETE_ARRAY(pMaterials);
		pMaterials = NULL;
	}
}


BOOL	 DxOcMeshes::SaveFile ( CSerialFile &SFile )
{
	SFile << cMaterials;
	SFile.WriteBuffer ( pMaterials, sizeof(DXOCMATERIAL)*cMaterials );

	pOcMesh->SaveFile ( SFile );

	return TRUE;
}

BOOL	 DxOcMeshes::LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	SFile >> cMaterials;

	SAFE_DELETE_ARRAY(pMaterials);
	pMaterials = new DXOCMATERIAL[cMaterials];
	SFile.ReadBuffer ( pMaterials, sizeof(DXOCMATERIAL)*cMaterials );

	for ( DWORD i=0; i<cMaterials; i++ )
	{
		hr = TextureManager::LoadTexture ( pMaterials[i].szTexture,
				pd3dDevice, pMaterials[i].pTexture, 0, 0 );
		if (FAILED(hr))
		{
			hr = S_OK;
			pMaterials[i].pTexture = NULL;
		}
	}
	
	SAFE_DELETE(pOcMesh);
	pOcMesh = new DxOctreeMesh;
	pOcMesh->LoadFile ( SFile, pd3dDevice );

	return TRUE;
}


