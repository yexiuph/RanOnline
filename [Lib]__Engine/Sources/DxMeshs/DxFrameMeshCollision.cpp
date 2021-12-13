#include "pch.h"

#include "DxAnimationManager.h"
#include "./Collision.h"

#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : 아랫 변수들이 초기 화 되어 있는걸로 가정을 하고 수행한다.
//
//	COLLISION::vColTestStart = vP1;
//	COLLISION::vColTestEnd = vP2;
//
void DxMeshes::IsCollision( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
						   BOOL &bCollision, char* pName, const BOOL bFrontColl )
{
	PBYTE pbDataVB;
	PBYTE pbIndexB;
	BOOL b32bitIndex = m_pLocalMesh->GetOptions () & D3DXMESH_32BIT;

	DWORD dwNumFaces = m_pLocalMesh->GetNumFaces ();

	if ( FAILED( m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbDataVB ) ) )
		return;
	if ( FAILED( m_pLocalMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pbIndexB ) ) )
	{
		m_pLocalMesh->UnlockVertexBuffer();
		return;
	}

	DWORD fvfsize = D3DXGetFVFVertexSize ( m_pLocalMesh->GetFVF() );
	for ( DWORD dwFaceCur=0; dwFaceCur<dwNumFaces; dwFaceCur++ )
	{
		DWORD dwIndex0, dwIndex1, dwIndex2;
		if ( b32bitIndex )
		{
			PBYTE pbIndex = pbIndexB + dwFaceCur*3*sizeof(DWORD);

			dwIndex0 = * (DWORD*) ( pbIndex + 0 * sizeof(DWORD) );
			dwIndex1 = * (DWORD*) ( pbIndex + 1 * sizeof(DWORD) );
			dwIndex2 = * (DWORD*) ( pbIndex + 2 * sizeof(DWORD) );
		}
		else
		{
			PBYTE pbIndex = pbIndexB + dwFaceCur*3*sizeof(WORD);

			dwIndex0 = * (WORD*) ( pbIndex + 0 * sizeof(WORD) );
			dwIndex1 = * (WORD*) ( pbIndex + 1 * sizeof(WORD) );
			dwIndex2 = * (WORD*) ( pbIndex + 2 * sizeof(WORD) );
		}

		D3DXVECTOR3 vT0 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*dwIndex0);
		D3DXVECTOR3 vT1 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*dwIndex1);
		D3DXVECTOR3 vT2 = * (D3DXVECTOR3*) (pbDataVB+fvfsize*dwIndex2);

		D3DXVec3TransformCoord ( &vT0, &vT0, &matCom );
		D3DXVec3TransformCoord ( &vT1, &vT1, &matCom );
		D3DXVec3TransformCoord ( &vT2, &vT2, &matCom );

		D3DXVECTOR3 vNewCollision;
		if ( COLLISION::IsLineTriangleCollision( &vT0, &vT1, &vT2, (D3DXVECTOR3*)&vP1, (D3DXVECTOR3*)&vP2, &vNewCollision, NULL, bFrontColl ) )
		{
			D3DXVECTOR3 vDxVect;
			vDxVect = COLLISION::vColTestStart - vNewCollision;

			float fNewLength = D3DXVec3LengthSq ( &vDxVect );
			vDxVect = COLLISION::vColTestStart - vCollision;
			float fOldLength = D3DXVec3LengthSq ( &vDxVect );

			if ( fNewLength < fOldLength )
			{
				vCollision = vNewCollision;
				bCollision = TRUE;

				// Note : 텍스쳐 알아내기.
				DWORD dwAttrib = 0;
				m_pLocalMesh->GetAttributeTable( NULL, &dwAttrib );
				D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
				m_pLocalMesh->GetAttributeTable( pAttrib, &dwAttrib );
				for( DWORD i=0; i<dwAttrib; ++i )
				{
					DWORD dwAttribID = pAttrib[i].AttribId;
					if( dwFaceCur>=pAttrib[i].FaceStart && 
						dwFaceCur<pAttrib[i].FaceStart+pAttrib[i].FaceCount )
					{
						if ( pName )
							StringCchCopy( pName, MAX_PATH, strTextureFiles[dwAttribID].GetString() );
					}
				}

				SAFE_DELETE_ARRAY( pAttrib );
			}
		}
	}

	m_pLocalMesh->UnlockVertexBuffer();
	m_pLocalMesh->UnlockIndexBuffer();
}

void DxMeshes::GetClosedPointIndex ( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vCollision, DWORD &dwIndex, D3DXVECTOR3 &vVert )
{
	float fLength = FLT_MAX;
	dwIndex = 0xFFFFFFFF;

	PBYTE pbDataVB;
	BOOL b32bitIndex = m_pLocalMesh->GetOptions () & D3DXMESH_32BIT;

	DWORD dwNumVerts = m_pLocalMesh->GetNumVertices ();
	if ( FAILED( m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbDataVB ) ) )
		return;

	DWORD fvfsize = D3DXGetFVFVertexSize ( m_pLocalMesh->GetFVF() );
	for ( DWORD dwVertCur=0; dwVertCur<dwNumVerts; dwVertCur++ )
	{
		D3DXVECTOR3 vT = * (D3DXVECTOR3*) (pbDataVB+fvfsize*dwVertCur);
		D3DXVec3TransformCoord ( &vT, &vT, &matCom );

		D3DXVECTOR3 vDist = vT - vCollision;
		float fThisLength = D3DXVec3Length ( &vDist );

		//	Note : 더 가까운 포인터 검출시 선택.
		//
		if ( fThisLength < fLength )
		{
			fLength = fThisLength;
			dwIndex = dwVertCur;
			vVert = vT;
		}
	}

	m_pLocalMesh->UnlockVertexBuffer();
}

BOOL DxFrame::IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXMATRIX* pMat )
{
	D3DXVECTOR3 vP1=vPoint1, vP2=vPoint2;
	float fRatio;
	D3DXVECTOR3 vCullPos;

	D3DXVECTOR3 vMin = vTreeMin;
	D3DXVECTOR3 vMax = vTreeMax;

	//	Note : 메트릭스가 존재시에 AABB의 영역을 재계산.
	//
	if ( pMat )
		COLLISION::TransformAABB ( vMax, vMin, *pMat );

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<vMin.x && vP2.x<vMin.x )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<vMin.x || vP2.x<vMin.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + vMin.x ) / ( -vP1.x + vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < vMin.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>vMax.x && vP2.x>vMax.x )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>vMax.x || vP2.x>vMax.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - vMax.x ) / ( vP1.x - vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > vMax.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<vMin.y && vP2.y<vMin.y )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<vMin.y || vP2.y<vMin.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + vMin.y ) / ( -vP1.y + vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < vMin.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>vMax.y && vP2.y>vMax.y )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>vMax.y || vP2.y>vMax.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - vMax.y ) / ( vP1.y - vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > vMax.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<vMin.z && vP2.z<vMin.z )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<vMin.z || vP2.z<vMin.z )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + vMin.z ) / ( -vP1.z + vP2.z );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < vMin.z )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > vMax.z && vP2.z > vMax.z )
		return FALSE;	// back 부분이 없으므로 충돌하지 않음.

	return TRUE;
}

BOOL DxFrame::CollisionDetect ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, char* pName, const BOOL bFrontColl )
{
	BOOL bCollision = FALSE;

	vCollision = vP2;

	COLLISION::vColTestStart = vP1;
	COLLISION::vColTestEnd = vP2;
	CollisionDetect( this, vP1, vP2, bCollision, vCollision, pName, bFrontColl );

	return bCollision;
}

void DxFrame::CollisionDetect ( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2,
							BOOL &bCollision, D3DXVECTOR3 &vCollision, char* pName, const BOOL bFrontColl )
{
	if ( !pframeCur )	return;

	BOOL bCollisionCur = FALSE;

	//	Note : 메쉬 충돌 검사.
	//
	DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		if ( pmsMeshs->m_pLocalMesh )
		{
			pmsMeshs->IsCollision( pframeCur->matCombined, vP1, vP2, vCollision, bCollisionCur, pName, bFrontColl );
		}

		pmsMeshs = pmsMeshs->pMeshNext;
	}

	if ( bCollisionCur )
	{
		bCollision = TRUE;
	}

	//	Note : 자식 프레임 충돌 검사.
	//
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		CollisionDetect ( pframeChild, vP1, vP2, bCollision, vCollision, pName, bFrontColl );

		pframeChild = pframeChild->pframeSibling;
	}
}

BOOL DxFrameMesh::IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, char* pName, const BOOL bFrontColl )
{
	BOOL bCollision = FALSE;

	m_vCollionPos = vP2;
	m_pCollDetectFrame = NULL;


	COLLISION::vColTestStart = vP1;
	COLLISION::vColTestEnd = vP2;
	CollisionDetect ( GetFrameRoot(), vP1, vP2, bCollision, pName, bFrontColl );

	return bCollision;
}

void DxFrameMesh::CollisionDetect ( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2,
								   BOOL &bCollision, char* pName, const BOOL bFrontColl )
{
	if ( !pframeCur )	return;

	//	Note : 내비게이션 메시 그리지 않음 검사
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return;


	//if ( pframeCur->IsCollision ( vP1, vP2, &m_pDxFrame->matRot ) )  -- 문제점 노출.
	{
		BOOL bCollisionCur = FALSE;

		//	Note : 메쉬 충돌 검사.
		//
		DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->IsCollision( pframeCur->matCombined, vP1, vP2, m_vCollionPos, bCollisionCur, pName, bFrontColl );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}

		if ( bCollisionCur )
		{
			bCollision = TRUE;
			m_pCollDetectFrame = pframeCur;
		}
	}

	//	Note : 자식 프레임 충돌 검사.
	//
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		CollisionDetect ( pframeChild, vP1, vP2, bCollision, pName, bFrontColl );

		pframeChild = pframeChild->pframeSibling;
	}
}

BOOL DxFrame::BoxIsInTheFrustum ( D3DXMATRIX* pMat, CLIPVOLUME &cv, LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vMin = vTreeMin;
	D3DXVECTOR3 vMax = vTreeMax;

	//	Note : 메트릭스가 존재시에 AABB의 영역을 재계산.
	//
	if ( pMat )
		COLLISION::TransformAABB ( vMax, vMin, *pMat );

	//	Note	:	프리즘과 옥트리 박스의 관계연산
	//				어떤 면에 대해서, 모든 정점이 벗어나면 그것은 컬링이 되는 것이다.
	if (((cv.pNear.a * vMin.x + cv.pNear.b * vMax.y + cv.pNear.c * vMin.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pNear.a * vMin.x + cv.pNear.b * vMax.y + cv.pNear.c * vMax.z + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pNear.a * vMax.x + cv.pNear.b * vMax.y + cv.pNear.c * vMax.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pNear.a * vMax.x + cv.pNear.b * vMax.y + cv.pNear.c * vMin.z + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pNear.a * vMin.x + cv.pNear.b * vMin.y + cv.pNear.c * vMin.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pNear.a * vMin.x + cv.pNear.b * vMin.y + cv.pNear.c * vMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pNear.a * vMax.x + cv.pNear.b * vMin.y + cv.pNear.c * vMax.z + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pNear.a * vMax.x + cv.pNear.b * vMin.y + cv.pNear.c * vMin.z + cv.pNear.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pFar.a * vMin.x + cv.pFar.b * vMax.y + cv.pFar.c * vMin.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pFar.a * vMin.x + cv.pFar.b * vMax.y + cv.pFar.c * vMax.z + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pFar.a * vMax.x + cv.pFar.b * vMax.y + cv.pFar.c * vMax.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pFar.a * vMax.x + cv.pFar.b * vMax.y + cv.pFar.c * vMin.z + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pFar.a * vMin.x + cv.pFar.b * vMin.y + cv.pFar.c * vMin.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pFar.a * vMin.x + cv.pFar.b * vMin.y + cv.pFar.c * vMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pFar.a * vMax.x + cv.pFar.b * vMin.y + cv.pFar.c * vMax.z + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pFar.a * vMax.x + cv.pFar.b * vMin.y + cv.pFar.c * vMin.z + cv.pFar.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pLeft.a * vMin.x + cv.pLeft.b * vMax.y + cv.pLeft.c * vMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pLeft.a * vMin.x + cv.pLeft.b * vMax.y + cv.pLeft.c * vMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pLeft.a * vMax.x + cv.pLeft.b * vMax.y + cv.pLeft.c * vMax.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pLeft.a * vMax.x + cv.pLeft.b * vMax.y + cv.pLeft.c * vMin.z + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pLeft.a * vMin.x + cv.pLeft.b * vMin.y + cv.pLeft.c * vMin.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pLeft.a * vMin.x + cv.pLeft.b * vMin.y + cv.pLeft.c * vMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pLeft.a * vMax.x + cv.pLeft.b * vMin.y + cv.pLeft.c * vMax.z + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pLeft.a * vMax.x + cv.pLeft.b * vMin.y + cv.pLeft.c * vMin.z + cv.pLeft.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pRight.a * vMin.x + cv.pRight.b * vMax.y + cv.pRight.c * vMin.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pRight.a * vMin.x + cv.pRight.b * vMax.y + cv.pRight.c * vMax.z + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pRight.a * vMax.x + cv.pRight.b * vMax.y + cv.pRight.c * vMax.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pRight.a * vMax.x + cv.pRight.b * vMax.y + cv.pRight.c * vMin.z + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pRight.a * vMin.x + cv.pRight.b * vMin.y + cv.pRight.c * vMin.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pRight.a * vMin.x + cv.pRight.b * vMin.y + cv.pRight.c * vMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pRight.a * vMax.x + cv.pRight.b * vMin.y + cv.pRight.c * vMax.z + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pRight.a * vMax.x + cv.pRight.b * vMin.y + cv.pRight.c * vMin.z + cv.pRight.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pTop.a * vMin.x + cv.pTop.b * vMax.y + cv.pTop.c * vMin.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pTop.a * vMin.x + cv.pTop.b * vMax.y + cv.pTop.c * vMax.z + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pTop.a * vMax.x + cv.pTop.b * vMax.y + cv.pTop.c * vMax.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pTop.a * vMax.x + cv.pTop.b * vMax.y + cv.pTop.c * vMin.z + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pTop.a * vMin.x + cv.pTop.b * vMin.y + cv.pTop.c * vMin.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pTop.a * vMin.x + cv.pTop.b * vMin.y + cv.pTop.c * vMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pTop.a * vMax.x + cv.pTop.b * vMin.y + cv.pTop.c * vMax.z + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pTop.a * vMax.x + cv.pTop.b * vMin.y + cv.pTop.c * vMin.z + cv.pTop.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pBottom.a * vMin.x + cv.pBottom.b * vMax.y + cv.pBottom.c * vMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pBottom.a * vMin.x + cv.pBottom.b * vMax.y + cv.pBottom.c * vMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pBottom.a * vMax.x + cv.pBottom.b * vMax.y + cv.pBottom.c * vMax.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pBottom.a * vMax.x + cv.pBottom.b * vMax.y + cv.pBottom.c * vMin.z + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pBottom.a * vMin.x + cv.pBottom.b * vMin.y + cv.pBottom.c * vMin.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pBottom.a * vMin.x + cv.pBottom.b * vMin.y + cv.pBottom.c * vMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pBottom.a * vMax.x + cv.pBottom.b * vMin.y + cv.pBottom.c * vMax.z + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pBottom.a * vMax.x + cv.pBottom.b * vMin.y + cv.pBottom.c * vMin.z + cv.pBottom.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	return TRUE;
}

