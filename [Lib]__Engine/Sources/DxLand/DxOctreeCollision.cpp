#include "pch.h"

#include "DxOctree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


HRESULT DxOctree::MakeAABBTree ()
{
	HRESULT hr;

	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->MakeAABBTree();
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->MakeAABBTree();
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->MakeAABBTree();
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->MakeAABBTree();
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->MakeAABBTree();
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->MakeAABBTree();
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->MakeAABBTree();
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->MakeAABBTree();
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxFrameHead;

		while ( pDxFrameCur )
		{
			//	Note : AABB Tree 제작.
			//
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					D3DXMATRIX matIdentity;
					D3DXMatrixIdentity( &matIdentity );

					hr = pmsMeshs->m_pDxOctreeMesh->MakeAABBTree ( matIdentity );
					if ( FAILED(hr) )	return hr;
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}	

	return S_OK;
}

BOOL DxOctree::IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, 
							D3DXVECTOR3 &vNormal, BOOL &bCollision, LPDXFRAME &_pDxFrame, LPCSTR& szName, const BOOL bFrontColl )
{
	D3DXVECTOR3 vP1=vPoint1, vP2=vPoint2;
	float fRatio;
	D3DXVECTOR3 vCullPos;

	//	Note : X,min
	//		N(-1,0,0) : -X + X,min = 0
	//		X < X,min is front
	//
	if ( vP1.x<m_vMin.x && vP2.x<m_vMin.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x<m_vMin.x || vP2.x<m_vMin.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.x + m_vMin.x ) / ( -vP1.x + vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x < m_vMin.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : X,max
	//		N(1,0,0) : X - X,max = 0
	//		X > X,max is front
	//
	if ( vP1.x>m_vMax.x && vP2.x>m_vMax.x )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.x>m_vMax.x || vP2.x>m_vMax.x )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.x - m_vMax.x ) / ( vP1.x - vP2.x );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.x > m_vMax.x )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,min
	//		N(0,-1,0) : -Y + Y,min = 0
	//		Y < Y,min is front
	//
	if ( vP1.y<m_vMin.y && vP2.y<m_vMin.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y<m_vMin.y || vP2.y<m_vMin.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.y + m_vMin.y ) / ( -vP1.y + vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y < m_vMin.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Y,max
	//		N(0,1,0) : Y - Y,max = 0
	//		Y > Y,max is front
	//
	if ( vP1.y>m_vMax.y && vP2.y>m_vMax.y )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.y>m_vMax.y || vP2.y>m_vMax.y )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( vP1.y - m_vMax.y ) / ( vP1.y - vP2.y );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.y > m_vMax.y )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,min
	//		N(0,0,-1) : -Z + Z,min = 0
	//		Z < Z,min is front
	//
	if ( vP1.z<m_vMin.z && vP2.z<m_vMin.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( vP1.z<m_vMin.z || vP2.z<m_vMin.z )
	{
		//	Note : 충돌점 구하기.
		//
		fRatio = ( -vP1.z + m_vMin.z ) / ( -vP1.z + vP2.z );
		vCullPos = vP1 + fRatio * ( vP2 - vP1 );

		//	Note : 선분을 이등분하여 front 부분 버리기.
		//
		if ( vP1.z < m_vMin.z )	vP1 = vCullPos;
		else					vP2 = vCullPos;
	}

	//	Note : Z,max
	//		N(0,0,-1) : Z - Z,max = 0
	//		Z > Z,max is front
	//
	if ( vP1.z > m_vMax.z && vP2.z > m_vMax.z )
		goto _RETURN;	// back 부분이 없으므로 충돌하지 않음.

	if ( m_bSubDivided )
	{
		if (m_pOctreeNodes_0)	m_pOctreeNodes_0->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_1)	m_pOctreeNodes_1->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_2)	m_pOctreeNodes_2->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_3)	m_pOctreeNodes_3->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_4)	m_pOctreeNodes_4->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_5)	m_pOctreeNodes_5->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_6)	m_pOctreeNodes_6->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
		if (m_pOctreeNodes_7)	m_pOctreeNodes_7->IsCollision(vP1,vP2,vCollision,vNormal,bCollision,_pDxFrame,szName,bFrontColl);
	}
	else
	{		
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxFrameHead;

		BOOL bCollisionCur=FALSE;

		while ( pDxFrameCur )
		{
			pmsMeshs = pDxFrameCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					bCollisionCur=FALSE;
					DWORD dwAttribid=0;
					pmsMeshs->m_pDxOctreeMesh->IsCollision( vP1, vP2,
						vCollision, bCollisionCur, vNormal, dwAttribid, bFrontColl );

					if ( bCollisionCur )
					{
						bCollision = bCollisionCur;
						_pDxFrame = pDxFrameCur;

						szName = pmsMeshs->strTextureFiles[dwAttribid];
					}
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}

			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}

_RETURN:
	return FALSE;
}