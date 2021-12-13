// DxDecal.cpp: implementation of the DxDecal class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "./DxDecal.h"

#include "./DxOctree.h"
#include "./Collision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxDecal& DxDecal::GetInstance()
{
	static DxDecal Instance;
	return Instance;
}
 
const DWORD	DECALVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const float DxDecal::m_fDecalEpsilon = 0.12f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxDecal::DxDecal() :
	m_dwVertexCount(0),
	m_dwTriangleCount(0),
	m_pVertexBuffer(NULL),
	m_pIndexBuffer(NULL)
{
}

DxDecal::~DxDecal()
{

}

void DxDecal::CreateDecal ( D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vNormal, D3DXVECTOR3 &vTangent,
		D3DXVECTOR3 &vSize, DxOctree *pOctree )
{
#ifdef _DEBUG
	float fOldCount = DXUtil_Timer( TIMER_GETAPPTIME );
#endif

	if ( !pOctree )		return;

	m_dwVertexCount = 0;
	m_dwTriangleCount = 0;

	m_vCenter = vCenter;
	m_vNormal = vNormal;

	D3DXVECTOR3 vBiNormal;
	D3DXVec3Cross ( &vBiNormal, &vNormal, &vTangent );

	float d;
	D3DXVECTOR3 vPoint;
	
	vPoint = vCenter + vTangent*0.5f*vSize.x;
	d = D3DXVec3Dot ( &vPoint, &vTangent );
	m_PlaneLeft = D3DXPLANE ( vTangent.x, vTangent.y, vTangent.z, - d );

	vPoint = vCenter - vTangent*0.5f*vSize.x;
	d = D3DXVec3Dot ( &vPoint, &vTangent );
	m_PlaneRight = D3DXPLANE ( -vTangent.x, -vTangent.y, -vTangent.z, + d );

	vPoint = vCenter + vNormal*0.5f*vSize.y;
	d = D3DXVec3Dot ( &vPoint, &vNormal );
	m_PlaneBottom = D3DXPLANE ( vNormal.x, vNormal.y, vNormal.z, - d );

	vPoint = vCenter - vNormal*0.5f*vSize.y;
	d = D3DXVec3Dot ( &vPoint, &vNormal );
	m_PlaneTop = D3DXPLANE ( -vNormal.x, -vNormal.y, -vNormal.z, + d );

	vPoint = vCenter + vBiNormal*0.5f*vSize.z;
	d = D3DXVec3Dot ( &vPoint, &vBiNormal );
	m_PlaneBack = D3DXPLANE ( vBiNormal.x, vBiNormal.y, vBiNormal.z, - d );

	vPoint = vCenter - vBiNormal*0.5f*vSize.z;
	d = D3DXVec3Dot ( &vPoint, &vBiNormal );
	m_PlaneFront = D3DXPLANE ( -vBiNormal.x, -vBiNormal.y, -vBiNormal.z, + d );

	//	Note : 데칼을 만들기전 초기화.
	//
	m_dwVertexCount = 0;
	m_dwTriangleCount = 0;

	//	Note : 데칼 만들기.
	//		데칼로 만들어질 서페이스를 가지고 있는 각각의 메쉬들을 ClipOctree() 호출로써
	//		잘라내어서 폴리곤을 생성한다.
	//
	ClipOctree ( pOctree );

	////	Note : 텍스쳐 메핑 좌표 생성.
	////
	//float fOneOverW = 1.0f / vSize.x;
	//float fOneOverH = 1.0f / vSize.y;

	//for ( DWORD i=0; i < m_dwVertexCount; i++ )
	//{
	//	D3DXVECTOR3 v;
	//	v.x = m_VertexArray[i].x - vCenter.x;
	//	v.y = m_VertexArray[i].y - vCenter.y;
	//	v.z = m_VertexArray[i].z - vCenter.z;

	//	float s = D3DXVec3Dot ( &v, &vTangent ) * fOneOverW + 0.5f;
	//	float t = D3DXVec3Dot ( &v, &vBiNormal ) * fOneOverH + 0.5f;

	//	m_VertexArray[i].tu = s;
	//	m_VertexArray[i].tv = t;
	//}

#ifdef _DEBUG
	float fDxCount = DXUtil_Timer( TIMER_GETAPPTIME ) - fOldCount;
	CDebugSet::ToView ( 17, "Decal Face:%d  Vertex:%d  Time:%f",
		m_dwTriangleCount, m_dwVertexCount, fDxCount );
#endif
}

static BOOL		s_bLockMeshB;
static DWORD	s_dwFVF;
static PBYTE	s_pbDataVB;
static PWORD	s_pwIndexB;
DWORD DxDecal::ClipOctree ( DxOctree *pOctree )
{
	HRESULT hr = S_OK;

	//	Note	:	
	//
	if ( !pOctree->BoxIsInTheFrustum(m_ClipVolume) )	return 0;
	
	if ( pOctree->IsSubDivision() )
	{
		DxOctree* pOctreeNode;
		pOctreeNode = pOctree->GetOctreeNode(0);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(1);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(2);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(3);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(4);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(5);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(6);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);

		pOctreeNode = pOctree->GetOctreeNode(7);
		if ( pOctreeNode )	ClipOctree (pOctreeNode);
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = pOctree->GetDxFrameHead();

		while ( pDxFrameCur )
		{
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				DxOctreeMesh *pOctreeMesh = pmsMeshs->m_pDxOctreeMesh;
				if ( pOctreeMesh )
				{
					PBYTE pbDataVB;
					PWORD pwIndexB;

					hr = pOctreeMesh->LockVertexBuffer ( (VOID**)&pbDataVB );
					if ( FAILED(hr) )	return 0;

					hr = pOctreeMesh->LockIndexBuffer ( (VOID**)&pwIndexB );
					if ( FAILED(hr) )	return 0;

					s_bLockMeshB = TRUE;
					s_dwFVF = pOctreeMesh->m_dwFVF;
					s_pbDataVB = pbDataVB;
					s_pwIndexB = pwIndexB;
					ClipMeshAABB ( pOctreeMesh->m_pAABBTreeRoot );

					pOctreeMesh->UnlockVertexBuffer ();
					pOctreeMesh->UnlockIndexBuffer ();
				}
				pmsMeshs = pmsMeshs->pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}

	return 0;
}

BOOL DxDecal::AddPolygon ( DWORD dwVertexCount, const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal )
{
	DWORD dwCount = m_dwVertexCount;
	if ( dwCount+dwVertexCount >= _MAXDECALVERTICES ) return FALSE;

	WORD *pTriangle = m_TriangleArray + m_dwTriangleCount*3;
	m_dwTriangleCount += dwVertexCount - 2;

	for ( DWORD i=2; i<dwVertexCount; ++i )
	{
		*pTriangle = (WORD) ( dwCount );
		pTriangle++;

		*pTriangle = (WORD) ( dwCount + i - 1 );
		pTriangle++;

		*pTriangle = (WORD) ( dwCount + i );
		pTriangle++;
	}

	float f = 1.0f / ( 1.0f - m_fDecalEpsilon );
	for ( DWORD j=0; j<dwVertexCount; ++j )
	{
		m_VertexArray[dwCount].x = pVertex[j].x;
		m_VertexArray[dwCount].y = pVertex[j].y;
		m_VertexArray[dwCount].z = pVertex[j].z;

		m_VertexArray[dwCount].DiffuseColor = 0xffffffff;

		//const D3DXVECTOR3 &n = pNormal[j];
		//float fAlpha = ( D3DXVec3Dot ( &m_vNormal, &n) / D3DXVec3Length(&n) - m_fDecalEpsilon ) * f;
		//
		//m_VertexArray[dwCount].DiffuseColor = D3DCOLOR_COLORVALUE ( 1.0f, 1.0f, 1.0f, fAlpha>0.0f ? fAlpha : 0.0f );
		dwCount++;
	}

	m_dwVertexCount = dwCount;
	return TRUE;
}

void DxDecal::ClipMeshAABB ( DxAABBNode* pAABBNode )
{
	D3DXVECTOR3 vNewVertex[9];
	D3DXVECTOR3 vNewNormal[9];

	if ( pAABBNode->IsCollisionVolume ( m_ClipVolume ) )
	{
		if ( pAABBNode->dwFace != AABB_NONINDEX )
		{
			DWORD fvfsize = D3DXGetFVFVertexSize ( s_dwFVF );

			PWORD pwIndex = s_pwIndexB + pAABBNode->dwFace*3;
			vNewVertex[0] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex)));
			vNewVertex[1] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex+1)));
			vNewVertex[2] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex+2)));

			vNewNormal[0] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex)) + sizeof(D3DXVECTOR3) );
			vNewNormal[1] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex+1)) + sizeof(D3DXVECTOR3) );
			vNewNormal[2] = * (D3DXVECTOR3*) (s_pbDataVB+fvfsize*(*(pwIndex+2)) + sizeof(D3DXVECTOR3) );

			//AddPolygon ( 3, vNewVertex, vNewNormal );

			DWORD dwCount = ClipPolygon ( 3, vNewVertex, vNewNormal, vNewVertex, vNewNormal );
			if ( dwCount != 0 )	AddPolygon ( dwCount, vNewVertex, vNewNormal );
		}
		else
		{
			if ( pAABBNode->pLeftChild )
				ClipMeshAABB ( pAABBNode->pLeftChild );
			if ( pAABBNode->pRightChild )
				ClipMeshAABB ( pAABBNode->pRightChild );
		}
	}
}

void DxDecal::ClipMesh ( D3DXMATRIX &matCom, PBYTE pbDataVB, PWORD pwIndexB, DWORD dwFVF, DWORD dwFaceNum )
{
	D3DXVECTOR3 vNewVertex[9];
	D3DXVECTOR3 vNewNormal[9];

	PWORD pwIndex = NULL;
	D3DXVECTOR3 vCross;
	DWORD fvfsize = D3DXGetFVFVertexSize ( dwFVF );
	for ( DWORD dwFaceCur=0; dwFaceCur<dwFaceNum; ++dwFaceCur )
	{
		pwIndex = pwIndexB + dwFaceCur*3;
		D3DXVECTOR3 vT0 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex)));
		D3DXVECTOR3 vT1 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+1)));
		D3DXVECTOR3 vT2 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+2)));

		D3DXVec3TransformCoord ( &vT0, &vT0, &matCom );
		D3DXVec3TransformCoord ( &vT1, &vT1, &matCom );
		D3DXVec3TransformCoord ( &vT2, &vT2, &matCom );
		
		D3DXVECTOR3 v10 = vT1 - vT0;
		D3DXVECTOR3 v21 = vT2 - vT1;
		D3DXVec3Cross ( &vCross, &v10, &v21 );

		if ( D3DXVec3Dot(&m_vNormal,&vCross) > m_fDecalEpsilon * D3DXVec3Length(&vCross) )
		{
			vNewVertex[0] = vT0;
			vNewVertex[1] = vT1;
			vNewVertex[2] = vT2;

			vNewNormal[0] = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex)) + sizeof(D3DXVECTOR3) );
			vNewNormal[1] = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+1)) + sizeof(D3DXVECTOR3) );
			vNewNormal[2] = * (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+2)) + sizeof(D3DXVECTOR3) );

			D3DXVec3TransformCoord ( vNewNormal, vNewNormal, &matCom );
			D3DXVec3TransformCoord ( vNewNormal+1, vNewNormal+1, &matCom );
			D3DXVec3TransformCoord ( vNewNormal+2, vNewNormal+2, &matCom );
	
			DWORD dwCount = ClipPolygon ( 3, vNewVertex, vNewNormal, vNewVertex, vNewNormal );
			if ( (dwCount!=0) && (!AddPolygon(dwCount, vNewVertex, vNewNormal)) )	break;
		}
	}

}

DWORD DxDecal::ClipPolygon ( DWORD dwVertexCount,
				const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal,
				D3DXVECTOR3 *pNewVertex, D3DXVECTOR3 *pNewNormal )
{
	D3DXVECTOR3 vTempVertex[9];
	D3DXVECTOR3 vTempNormal[9];

	//	Note : 6개의 평면에 대해서 클리핑.
	//
	DWORD dwCount = ClipPolygonAgainstPlane ( m_PlaneLeft, dwVertexCount,
		pVertex, pNormal, vTempVertex, vTempNormal );

	if ( dwCount != 0 )
	{
		dwCount = ClipPolygonAgainstPlane ( m_PlaneRight, dwCount,
			vTempVertex, vTempNormal, pNewVertex, pNewNormal );

		if ( dwCount != 0 )
		{
			dwCount = ClipPolygonAgainstPlane ( m_PlaneBottom, dwCount,
				pNewVertex, pNewNormal, vTempVertex, vTempNormal );

			if ( dwCount != 0 )
			{
				dwCount = ClipPolygonAgainstPlane ( m_PlaneTop, dwCount,
					vTempVertex, vTempNormal, pNewVertex, pNewNormal );

				if ( dwCount != 0 )
				{
					dwCount = ClipPolygonAgainstPlane ( m_PlaneBack, dwCount,
						pNewVertex, pNewNormal, vTempVertex, vTempNormal );

					if ( dwCount != 0 )
					{
						dwCount = ClipPolygonAgainstPlane ( m_PlaneFront, dwCount,
							vTempVertex, vTempNormal, pNewVertex, pNewNormal );
					}
				}
			}
		}
	}

	return dwCount;
}

DWORD DxDecal::ClipPolygonAgainstPlane ( const D3DXPLANE &Plane, DWORD dwVertexCount,
							   const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal,
							   D3DXVECTOR3 *pNewVertex, D3DXVECTOR3 *pNewNormal )
{
	BOOL bNegative[10];

	//	Note : 버텍스 분류
	//
	BOOL bNeg = FALSE;
	DWORD dwNegativeCount = 0;
	for ( DWORD i=0; i<dwVertexCount; ++i )
	{
		bNeg = ( D3DXPlaneDotCoord ( &Plane, pVertex+i ) > 0.0f );
		bNegative[i] = bNeg;

		dwNegativeCount += bNeg;
	}

	//	Note : 컬링할 폴리곤 없음.
	//
	if ( dwNegativeCount == dwVertexCount )		return 0;

	//	Note : 
	//
	DWORD b = 0;
	DWORD dwCount = 0;
	for ( DWORD a=0; a<dwVertexCount; ++a )
	{
		b = (a!=0) ? a-1 : dwVertexCount-1;

		if ( bNegative[a] )
		{
			if ( !bNegative[b] )
			{
				const D3DXVECTOR3 & v1 = pVertex[b];
				const D3DXVECTOR3 & v2 = pVertex[a];

				float t = D3DXPlaneDotCoord ( &Plane, &v1 )
					/ ( Plane.a * ( v1.x - v2.x ) + Plane.b * ( v1.y - v2.y ) + Plane.c * ( v1.z - v2.z ) );

				pNewVertex[dwCount] = v1 * (1.0f-t) + v2 * t;

				const D3DXVECTOR3 & n1 = pNormal[b];
				const D3DXVECTOR3 & n2 = pNormal[a];

				pNewNormal[dwCount] = n1 * (1.0f-t) + n2*t;

				dwCount++;

			}
		}
		else
		{
			if ( bNegative[b] )
			{
				const D3DXVECTOR3 & v1 = pVertex[a];
				const D3DXVECTOR3 & v2 = pVertex[b];

				float t = D3DXPlaneDotCoord ( &Plane, &v1 )
					/ ( Plane.a * ( v1.x - v2.x ) + Plane.b * ( v1.y - v2.y ) + Plane.c * ( v1.z - v2.z ) );

				pNewVertex[dwCount] = v1 * (1.0f-t) + v2 * t;

				const D3DXVECTOR3 & n1 = pNormal[a];
				const D3DXVECTOR3 & n2 = pNormal[b];

				pNewNormal[dwCount] = n1 * (1.0f-t) + n2*t;

				dwCount++;
			}

			pNewVertex[dwCount] = pVertex[a];
			pNewNormal[dwCount] = pNormal[a];
			dwCount++;
		}
	}

	return (dwCount);
}

LPDIRECT3DVERTEXBUFFERQ DxDecal::CreateVertexBuffer( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	LPDIRECT3DVERTEXBUFFERQ pVertexBuffer = NULL;

	hr = pd3dDevice->CreateVertexBuffer( m_dwVertexCount * sizeof(DECALVERTEX),
		0, DECALVERTEX::FVF, D3DPOOL_MANAGED, &pVertexBuffer, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateVertexBuffer() -- CreateVertexBuffer -- Failed" );
		return NULL;
	}

	PBYTE pbData;
	hr = pVertexBuffer->Lock ( 0, 0, (VOID**)&pbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateVertexBuffer() -- Lock -- Failed" );
		return NULL;
	}

	memcpy ( pbData, m_VertexArray, sizeof(DECALVERTEX)*m_dwVertexCount );

	pVertexBuffer->Unlock();

	return pVertexBuffer;
}

LPDIRECT3DINDEXBUFFERQ DxDecal::CreateIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	LPDIRECT3DINDEXBUFFERQ pIndexBuffer = NULL;

	hr = pd3dDevice->CreateIndexBuffer( m_dwTriangleCount*sizeof(WORD)*3,
		0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateIndexBuffer() -- CreateIndexBuffer -- Failed" );
		return NULL;
	}

	PBYTE pbData;
	hr = pIndexBuffer->Lock( 0, 0, (VOID**)&pbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateIndexBuffer() -- Lock -- Failed" );
		return NULL;
	}

	memcpy ( pbData, m_TriangleArray, sizeof(WORD)*m_dwTriangleCount*3 );

	pIndexBuffer->Unlock();

	return pIndexBuffer;
}

void DxDecal::CreateVertexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ& pVB )
{
	HRESULT hr;
	hr = pd3dDevice->CreateVertexBuffer( m_dwVertexCount * sizeof(DECALVERTEX),
		0, DECALVERTEX::FVF, D3DPOOL_MANAGED, &pVB, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateVertexBuffer() -- CreateVertexBuffer -- Failed" );
		return;
	}

	PBYTE pbData;
	hr = pVB->Lock( 0, 0, (VOID**)&pbData, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateVertexBuffer() -- Lock -- Failed" );
		return;
	}

	memcpy ( pbData, m_VertexArray, sizeof(DECALVERTEX)*m_dwVertexCount );

	pVB->Unlock();
}
void DxDecal::CreateIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DINDEXBUFFERQ& pIB )
{
	HRESULT hr;
	hr = pd3dDevice->CreateIndexBuffer( m_dwTriangleCount*sizeof(WORD)*3,
		0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateIndexBuffer() -- CreateIndexBuffer -- Failed" );
		return;
	}

	PBYTE pbData;
	hr = pIB->Lock( 0, 0, (VOID**)&pbData, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CreateIndexBuffer() -- Lock -- Failed" );
		return;
	}

	memcpy ( pbData, m_TriangleArray, sizeof(WORD)*m_dwTriangleCount*3 );

	pIB->Unlock();
}

void DxDecal::CloneVertexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ& pVB )
{
	HRESULT hr;
	PBYTE pbData;
	hr = pVB->Lock( 0, 0, (VOID**)&pbData, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CloneVertexBuffer() -- Lock -- Failed" );
		return;
	}

	memcpy ( pbData, m_VertexArray, sizeof(DECALVERTEX)*m_dwVertexCount );

	pVB->Unlock();
}

void DxDecal::CloneIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DINDEXBUFFERQ& pIB )
{
	HRESULT hr;
	PBYTE pbData;
	hr = pIB->Lock( 0, 0, (VOID**)&pbData, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxDecal::CloneIndexBuffer() -- Lock -- Failed" );
		return;
	}

	memcpy ( pbData, m_TriangleArray, sizeof(WORD)*m_dwTriangleCount*3 );

	pIB->Unlock();
}

void DxDecal::CloneVertexArray ( void* pVertexArray )
{
	memcpy ( pVertexArray, m_VertexArray, sizeof(DECALVERTEX)*m_dwVertexCount );
}

//HRESULT DxDecal::DrawDecal ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	DWORD OldLighting;
//	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
//	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
//
//	//	Note : Set the world matrix
//	//
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity ( &matIdentity );
//	pd3dDevice->SetTransform ( D3DTS_WORLD,  &matIdentity );
//
//	pd3dDevice->SetFVF ( DECALVERTEX::FVF );
//	pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, m_dwVertexCount, m_dwTriangleCount,
//		m_TriangleArray, D3DFMT_INDEX16, m_VertexArray, D3DXGetFVFVertexSize(DECALVERTEX::FVF) );
//
//	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );
//
//	return S_OK;
//}