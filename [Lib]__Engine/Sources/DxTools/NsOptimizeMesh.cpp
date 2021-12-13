#include "pch.h"

#include <map>

#include "NsOptimizeMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		O	P	T	I	M	I	Z	E		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSOPTIMIZEMESH
{
	struct VERTEX 
	{ 
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor;
		D3DCOLOR	dwColor;
		D3DXVECTOR2 vTex;
	};

	struct VERTEX_S
	{
		DWORD		dwParent;
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor;
		D3DCOLOR	dwColor;
		D3DXVECTOR2 vTex;
	};

	struct VERTEX_P
	{
		DWORD		dwNum;
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor;
		D3DCOLOR	dwColor;
		D3DXVECTOR2 vTex;

		BOOL operator==(const VERTEX_S& sVertex)  const;
		friend BOOL operator==(const VERTEX_S& sVertex, const VERTEX_P& sVertex_P);
	};

	BOOL VERTEX_P::operator==(const VERTEX_S& sVertexS) const
	{
		if ( vPos.x+0.01f < sVertexS.vPos.x )	return FALSE;
		if ( vPos.y+0.01f < sVertexS.vPos.y )	return FALSE;
		if ( vPos.z+0.01f < sVertexS.vPos.z )	return FALSE;
		if ( vPos.x > sVertexS.vPos.x+0.01f )	return FALSE;
		if ( vPos.y > sVertexS.vPos.y+0.01f )	return FALSE;
		if ( vPos.z > sVertexS.vPos.z+0.01f )	return FALSE;

		if ( vNor.x+0.01f < sVertexS.vNor.x )	return FALSE;
		if ( vNor.y+0.01f < sVertexS.vNor.y )	return FALSE;
		if ( vNor.z+0.01f < sVertexS.vNor.z )	return FALSE;
		if ( vNor.x > sVertexS.vNor.x+0.01f )	return FALSE;
		if ( vNor.y > sVertexS.vNor.y+0.01f )	return FALSE;
		if ( vNor.z > sVertexS.vNor.z+0.01f )	return FALSE;

		if ( dwColor != sVertexS.dwColor )		return FALSE;

		if ( vTex.x+0.01f < sVertexS.vTex.x )	return FALSE;
		if ( vTex.y+0.01f < sVertexS.vTex.y )	return FALSE;
		if ( vTex.x > sVertexS.vTex.x+0.01f )	return FALSE;
		if ( vTex.y > sVertexS.vTex.y+0.01f )	return FALSE;

		return TRUE;
	}

	BOOL operator==(const VERTEX_S& sVertex, const VERTEX_P& sVertex_P)
	{
		return (sVertex_P==sVertex);
	}

	typedef	std::multimap<float,VERTEX_P>			MAPVERTEX;
	typedef	std::multimap<float,VERTEX_P>::iterator	MAPVERTEX_ITER;

	void SetPoint ( MAPVERTEX& mapVertex, VERTEX_S& sVertexS, DWORD& dwIndex )
	{
		MAPVERTEX_ITER iter = mapVertex.lower_bound(sVertexS.vPos.x-0.01f);
		for ( ; iter!=mapVertex.end(); ++iter )
		{
			if ( (*iter).second == sVertexS )
			{
				sVertexS.dwParent  = (*iter).second.dwNum;	// Set Parent
				dwIndex = sVertexS.dwParent;
				return;
			}
			else if ( (*iter).second.vPos.x > (sVertexS.vPos.x+0.01f) )		break;
		}

		VERTEX_P sVertexP;
		sVertexP.dwNum	= (DWORD)mapVertex.size();
		sVertexP.vPos	= sVertexS.vPos;
		sVertexP.vNor	= sVertexS.vNor;
		sVertexP.dwColor = sVertexS.dwColor;
		sVertexP.vTex	= sVertexS.vTex;

		sVertexS.dwParent  = sVertexP.dwNum;		// Set Parent
		dwIndex = sVertexS.dwParent;

		mapVertex.insert ( std::make_pair(sVertexP.vPos.x,sVertexP) );
	}

	void MakeOptimizeVertex ( PBYTE& pVertices, DWORD* pIndices, DWORD& dwVertNUM, const DWORD dwFaceNUM, const DWORD dwFVF )
	{
		MAPVERTEX	mapVertex;
		MAPVERTEX_ITER iter;

		VERTEX_S* pVertexS = new VERTEX_S[dwVertNUM];

		// 초기화
		for ( DWORD i=0; i<dwVertNUM; ++i )
		{
			pVertexS[i].dwParent = 0;
			pVertexS[i].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			pVertexS[i].vNor = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			pVertexS[i].dwColor = 0xffffffff;
			pVertexS[i].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		}

		UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

		DWORD dwSizeADD = 0L;

		if ( dwFVF & D3DFVF_XYZ )
		{
			for ( DWORD i=0; i<dwVertNUM; ++i )
			{
				D3DXVECTOR3* pVector = (D3DXVECTOR3*) ( pVertices + i*nSIZE );
				pVertexS[i].vPos = *pVector;
			}

			dwSizeADD += sizeof(D3DXVECTOR3);
		}
		if ( dwFVF & D3DFVF_NORMAL )
		{
			for ( DWORD i=0; i<dwVertNUM; ++i )
			{
				D3DXVECTOR3* pVector = (D3DXVECTOR3*) ( pVertices + i*nSIZE + dwSizeADD );
				pVertexS[i].vNor = *pVector;
			}

			dwSizeADD += sizeof(D3DXVECTOR3);
		}
		if ( dwFVF & D3DFVF_DIFFUSE )
		{
			for ( DWORD i=0; i<dwVertNUM; ++i )
			{
				D3DCOLOR* pColor = (D3DCOLOR*) ( pVertices + i*nSIZE + dwSizeADD );
				pVertexS[i].dwColor = *pColor;
			}

			dwSizeADD += sizeof(D3DCOLOR);
		}
		if ( dwFVF & D3DFVF_TEX1 )
		{
			for ( DWORD i=0; i<dwVertNUM; ++i )
			{
				D3DXVECTOR2* pVector = (D3DXVECTOR2*) ( pVertices + i*nSIZE + dwSizeADD );
				pVertexS[i].vTex = *pVector;
			}
		}

		//	Note : 최적화된 Vertex와 Index를 얻는다.
		//
		for ( DWORD i=0; i<dwFaceNUM; ++i )
		{
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+0]], pIndices[i*3+0] );
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+1]], pIndices[i*3+1] );
			SetPoint ( mapVertex, pVertexS[pIndices[i*3+2]], pIndices[i*3+2] );
		}

		// Note : Vertex 데이터를 수정한다.
		dwVertNUM = (DWORD)mapVertex.size();
		SAFE_DELETE_ARRAY ( pVertices );
		pVertices	= new BYTE[nSIZE*dwVertNUM];
		for ( iter=mapVertex.begin(); iter!=mapVertex.end(); ++iter )
		{
			dwSizeADD = 0L;

			if ( dwFVF & D3DFVF_XYZ )
			{
				D3DXVECTOR3* pVector = (D3DXVECTOR3*) ( pVertices + (*iter).second.dwNum*nSIZE );
				*pVector = (*iter).second.vPos;

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( dwFVF & D3DFVF_NORMAL )
			{
				D3DXVECTOR3* pVector = (D3DXVECTOR3*) ( pVertices + (*iter).second.dwNum*nSIZE + dwSizeADD );
				*pVector = (*iter).second.vNor;

				D3DXVec3Normalize( pVector, pVector );

				dwSizeADD += sizeof(D3DXVECTOR3);
			}
			if ( dwFVF & D3DFVF_DIFFUSE )
			{
				D3DCOLOR* pColor = (D3DCOLOR*) ( pVertices + (*iter).second.dwNum*nSIZE + dwSizeADD );
				*pColor = (*iter).second.dwColor;

				dwSizeADD += sizeof(D3DCOLOR);
			}
			if ( dwFVF & D3DFVF_TEX1 )
			{
				D3DXVECTOR2* pVector = (D3DXVECTOR2*) ( pVertices + (*iter).second.dwNum*nSIZE + dwSizeADD );
				*pVector = (*iter).second.vTex;
			}
		}

		SAFE_DELETE_ARRAY ( pVertexS );
	}

	void MakeOptimizeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH& pMesh )
	{
		DWORD dwFVF = pMesh->GetFVF();
		DWORD dwVertNUM = pMesh->GetNumVertices();
		DWORD dwFaceNUM = pMesh->GetNumFaces();

		UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

		BYTE* pNewVertex	= new BYTE[nSIZE*dwVertNUM];
		DWORD*	pNewIndex	= new DWORD[dwFaceNUM*3];

		// Note : Clone
		BYTE* pVert;
		pMesh->LockVertexBuffer(0L,(VOID**)&pVert);	
		memcpy ( pNewVertex, pVert, nSIZE*dwVertNUM );
		pMesh->UnlockVertexBuffer();

		if ( pMesh->GetOptions() & D3DXMESH_32BIT )
		{
			DWORD*	pIndex;
			pMesh->LockIndexBuffer(0L,(VOID**)&pIndex);
			memcpy ( pNewIndex, pIndex, sizeof(DWORD)*dwFaceNUM*3 );
			pMesh->UnlockIndexBuffer();
		}
		else
		{
			WORD*	pIndex;
			pMesh->LockIndexBuffer(0L,(VOID**)&pIndex);
			memcpy ( pNewIndex, pIndex, sizeof(WORD)*dwFaceNUM*3 );
			for ( DWORD i=0; i<dwFaceNUM*3; ++i )
			{
				pNewIndex[i] = (DWORD)pIndex[i];
			}
			pMesh->UnlockIndexBuffer();
		}

		// Note : 정보 갱신
		MakeOptimizeVertex ( pNewVertex, pNewIndex, dwVertNUM, dwFaceNUM, dwFVF );

		// Note : Mesh 새로 생성
		SAFE_RELEASE ( pMesh );
		if ( dwVertNUM > 65000 )
		{
			D3DXCreateMeshFVF ( dwFaceNUM, dwVertNUM, D3DXMESH_32BIT|D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pMesh );

			DWORD* pIndices;
			pMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			memcpy ( pIndices, pNewIndex, sizeof(DWORD)*dwFaceNUM*3 );
			pMesh->UnlockIndexBuffer();
		}
		else
		{
			D3DXCreateMeshFVF ( dwFaceNUM, dwVertNUM, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pMesh );

			WORD* pIndices;
			pMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD i=0; i<dwFaceNUM*3; ++i )
			{
				pIndices[i] = (WORD)pNewIndex[i];
			}
			pMesh->UnlockIndexBuffer();
		}

		BYTE* pVertices;
		pMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
		memcpy ( pVertices, pNewVertex, nSIZE*dwVertNUM );
		pMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY ( pNewVertex );
		SAFE_DELETE_ARRAY ( pNewIndex );

		// Note : 최적화 완료
		DWORD* pAdjacency = new DWORD[3*dwFaceNUM];
		pMesh->GenerateAdjacency ( 0.01f, pAdjacency );
		pMesh->OptimizeInplace ( D3DXMESHOPT_VERTEXCACHE, pAdjacency, NULL, NULL, NULL );
		SAFE_DELETE_ARRAY ( pAdjacency );
	}
};