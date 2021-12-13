#include "pch.h"

#include "Collision.h"
#include "DxQUADTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace NSQUADTREE
{
	DWORD	dwLeafCountQUAD = 0;
	DWORD	dwNodeCountQUAD = 0;
	BASEMAP	m_mapBaseLOADING;
	BASEMAP m_mapBaseEDIT;

	void MakeQUADTree1 ( const POBJQUAD *const ppArray, const DWORD dwArraySize,
		const int &nMaxX, const int &nMaxZ, const int &nMinX, const int &nMinZ, POBJQUADNODE &pTreeNode )
	{
		if ( dwArraySize == 1 )
		{
			int nObjX = 0;
			int nObjZ = 0;
			D3DXVECTOR3 vObjMax, vObjMin;

			ppArray[0]->GetQUADSize ( nObjX, nObjZ );
			ppArray[0]->GetQUADSize ( vObjMax, vObjMin );

			pTreeNode->nNodeNUM	= 1;
			pTreeNode->nMaxX	= nObjX;
			pTreeNode->nMinX	= nObjX;
			pTreeNode->nMaxZ	= nObjZ;
			pTreeNode->nMinZ	= nObjZ;
			pTreeNode->vMax		= vObjMax;
			pTreeNode->vMin		= vObjMin;

			dwLeafCountQUAD++;
			pTreeNode->pObject = ppArray[0];
			return;
		}

		int nSizeX = nMaxX - nMinX;
		int nSizeY = nMaxZ - nMinZ;

		int nDivX = nMinX;
		int nDivZ = nMinZ;

		if ( nSizeX >= nSizeY )
		{
			nDivX = nMinX + (nSizeX/2);
			nDivZ = nMaxZ;
		}
		else			
		{
			nDivX = nMaxX;
			nDivZ = nMinZ + (nSizeY/2);
		}

		DWORD dwLeftCount = 0, dwRightCount = 0;
		POBJQUAD* pLeftArray = new POBJQUAD[dwArraySize];
		POBJQUAD* pRightArray = new POBJQUAD[dwArraySize];

		//	Note : 분할된 공간으로 요소 분리.
		DWORD i;
		for ( i=0; i<dwArraySize; i++ )
		{
			if ( ppArray[i]->IsDetectDivision (nDivX,nDivZ,nMinX,nMinZ) )
				pLeftArray[dwLeftCount++] = ppArray[i];
			else
				pRightArray[dwRightCount++] = ppArray[i];
		}

		int nNodeX = 0;
		int nNodeZ = 0;
		D3DXVECTOR3 vNodeMax, vNodeMin;

		//	Note : 왼쪽 자식 노드 생성.
		//
		int nCMaxX, nCMaxZ, nCMinX, nCMinZ;
		nCMaxX = INT_MIN;
		nCMaxZ = INT_MIN;
		nCMinX = INT_MAX;
		nCMinZ = INT_MAX;

		D3DXVECTOR3 vCMax, vCMin;
		vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		for ( i=0; i<dwLeftCount; i++ )
		{
			pLeftArray[i]->GetQUADSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;

			pLeftArray[i]->GetQUADSize ( nNodeX, nNodeZ );

			if ( nCMaxX < nNodeX )	nCMaxX = nNodeX;
			if ( nCMaxZ < nNodeZ )	nCMaxZ = nNodeZ;

			if ( nCMinX > nNodeX )	nCMinX = nNodeX;
			if ( nCMinZ > nNodeZ )	nCMinZ = nNodeZ;
		}

		pTreeNode->pLeftChild = new OBJQUADNode;
		pTreeNode->pLeftChild->nNodeNUM	= dwLeftCount;
		pTreeNode->pLeftChild->vMax		= vCMax;
		pTreeNode->pLeftChild->vMin		= vCMin;
		pTreeNode->pLeftChild->nMaxX	= nCMaxX;
		pTreeNode->pLeftChild->nMaxZ	= nCMaxZ;
		pTreeNode->pLeftChild->nMinX	= nCMinX;
		pTreeNode->pLeftChild->nMinZ	= nCMinZ;
		MakeQUADTree1 ( pLeftArray, dwLeftCount, nCMaxX, nCMaxZ, nCMinX, nCMinZ, pTreeNode->pLeftChild );

		//	Note : 오른쪽 자식 노드 생성.
		//
		nCMaxX = INT_MIN;
		nCMaxZ = INT_MIN;
		nCMinX = INT_MAX;
		nCMinZ = INT_MAX;

		vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		for ( i=0; i<dwRightCount; i++ )
		{
			pRightArray[i]->GetQUADSize ( vNodeMax, vNodeMin );

			if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
			if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
			if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

			if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
			if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
			if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;

			pRightArray[i]->GetQUADSize ( nNodeX, nNodeZ );

			if ( nCMaxX < nNodeX )	nCMaxX = nNodeX;
			if ( nCMaxZ < nNodeZ )	nCMaxZ = nNodeZ;

			if ( nCMinX > nNodeX )	nCMinX = nNodeX;
			if ( nCMinZ > nNodeZ )	nCMinZ = nNodeZ;
		}

		pTreeNode->pRightChild = new OBJQUADNode;
		pTreeNode->pRightChild->nNodeNUM = dwRightCount;
		pTreeNode->pRightChild->vMax	= vCMax;
		pTreeNode->pRightChild->vMin	= vCMin;
		pTreeNode->pRightChild->nMaxX	= nCMaxX;
		pTreeNode->pRightChild->nMaxZ	= nCMaxZ;
		pTreeNode->pRightChild->nMinX	= nCMinX;
		pTreeNode->pRightChild->nMinZ	= nCMinZ;
		MakeQUADTree1 ( pRightArray, dwRightCount, nCMaxX, nCMaxZ, nCMinX, nCMinZ, pTreeNode->pRightChild );

		SAFE_DELETE_ARRAY(pRightArray);
		SAFE_DELETE_ARRAY(pLeftArray);
	}

	void MakeQUADTree ( POBJQUADNODE &pTreeHead, POBJQUAD pListHead, BOOL bArray )
	{
		SAFE_DELETE(pTreeHead);

		dwLeafCountQUAD = 0;
		dwNodeCountQUAD = 0;
		POBJQUAD pNodeCur;

		//	Note : 갯수 파악.
		//
		pNodeCur = pListHead;
		while ( pNodeCur )
		{
			dwNodeCountQUAD++;
			pNodeCur = pNodeCur->GetNext();
		}

		if ( dwNodeCountQUAD == 0 )	return;

		//	Note : 포인터 배열에 저장후 정렬수행.
		//
		POBJQUAD *ppArray = new POBJQUAD[dwNodeCountQUAD];

		dwNodeCountQUAD = 0;
		pNodeCur = pListHead;
		while ( pNodeCur )
		{
			ppArray[dwNodeCountQUAD++] = pNodeCur;
			pNodeCur = pNodeCur->GetNext();
		}

		int nCMaxX = INT_MIN;
		int nCMaxZ = INT_MIN;
		int nCMinX = INT_MAX;
		int nCMinZ = INT_MAX;

		D3DXVECTOR3 vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		D3DXVECTOR3 vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		int	nNodeX = 0;
		int nNodeZ = 0;
		D3DXVECTOR3 vNodeMax, vNodeMin;
		for ( DWORD i=0; i<dwNodeCountQUAD; i++ )
		{
			ppArray[i]->GetQUADSize ( vNodeMax, vNodeMin );

			if ( vMax.x < vNodeMax.x )	vMax.x = vNodeMax.x;
			if ( vMax.y < vNodeMax.y )	vMax.y = vNodeMax.y;
			if ( vMax.z < vNodeMax.z )	vMax.z = vNodeMax.z;

			if ( vMin.x > vNodeMin.x )	vMin.x = vNodeMin.x;
			if ( vMin.y > vNodeMin.y )	vMin.y = vNodeMin.y;
			if ( vMin.z > vNodeMin.z )	vMin.z = vNodeMin.z;

			ppArray[i]->GetQUADSize ( nNodeX, nNodeZ );

			if ( nCMaxX < nNodeX )	nCMaxX = nNodeX;
			if ( nCMaxZ < nNodeZ )	nCMaxZ = nNodeZ;

			if ( nCMinX > nNodeX )	nCMinX = nNodeX;
			if ( nCMinZ > nNodeZ )	nCMinZ = nNodeZ;
		}

		pTreeHead = new OBJQUADNode;
		pTreeHead->nNodeNUM	= dwNodeCountQUAD;
		pTreeHead->nMaxX	= nCMaxX;
		pTreeHead->nMaxZ	= nCMaxZ;
		pTreeHead->nMinX	= nCMinX;
		pTreeHead->nMinZ	= nCMinZ;
		pTreeHead->vMax		= vMax;
		pTreeHead->vMin		= vMin;

		MakeQUADTree1 ( ppArray, dwNodeCountQUAD, nCMaxX, nCMaxZ, nCMinX, nCMinZ, pTreeHead );

		//if ( bArray )	MakeQUADTree ( ppArray, dwNodeCountQUAD, nCMaxX, nCMaxZ, nCMinX, nCMinZ, pTreeHead );
		//else			MakeQUADTree ( ppArray, dwNodeCountQUAD, vMax, vMin, pTreeHead );

		SAFE_DELETE_ARRAY(ppArray);
	}

	D3DXVECTOR2 ReMaxMinQUADTree ( POBJQUADNODE pNode )
	{
		D3DXVECTOR2 vTemp(0.f,0.f);

		if ( !pNode )	return vTemp;

		if ( pNode->pObject )
		{
			D3DXVECTOR3 vMax(0.f,0.f,0.f);
			D3DXVECTOR3 vMin(0.f,0.f,0.f);
			pNode->pObject->GetQUADSize ( vMax, vMin );

			vTemp.x = pNode->m_fMaxY = vMax.y;
			vTemp.y = pNode->m_fMinY = vMin.y;

			return vTemp;
		}

		D3DXVECTOR2 vTemp1(0.f,0.f);
		D3DXVECTOR2 vTemp2(0.f,0.f);

		vTemp.x = pNode->m_fMaxY;
		vTemp.y = pNode->m_fMinY;

		vTemp1 = ReMaxMinQUADTree ( pNode->pLeftChild );
		vTemp2 = ReMaxMinQUADTree ( pNode->pRightChild );

		if ( vTemp.x < vTemp1.x )
		{
			vTemp.x = vTemp1.x;
			pNode->m_fMaxY = vTemp.x;
		}
		if ( vTemp.y > vTemp1.y )
		{
			vTemp.y = vTemp1.y;
			pNode->m_fMinY = vTemp.y;
		}
		if ( vTemp.x < vTemp2.x )
		{
			vTemp.x = vTemp2.x;
			pNode->m_fMaxY = vTemp.x;
		}
		if ( vTemp.y > vTemp2.y )
		{
			vTemp.y = vTemp2.y;
			pNode->m_fMinY = vTemp.y;
		}

		return vTemp;
	}

	void MakeCollisionList ( POBJQUADNODE pNode, COLL_QUAD_LIST& listCollQuad, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd )
	{
		if ( !pNode )	return;

		if ( !COLLISION::IsCollisionLineToAABB ( vStart, vEnd, pNode->vMax, pNode->vMin ) )	return;
		
		if ( pNode->pObject )
		{
			listCollQuad.push_back ( pNode->pObject );

			return;
		}

		MakeCollisionList ( pNode->pLeftChild, listCollQuad, vStart, vEnd );

		MakeCollisionList ( pNode->pRightChild, listCollQuad, vStart, vEnd );
	}

	POBJQUAD FindQUADTree_VECTOR3 ( POBJQUADNODE pNode, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vDir )
	{
		POBJQUAD pObject=NULL;

		D3DXVECTOR3 vEnd	= vStart + (vDir*1000000.f);				// 끝 부분을 구함.
		D3DXVECTOR3 vLength = vEnd - vStart;
		float		fLength = D3DXVec3Length ( &vLength );				// 최대로 먼 것을 구함.

		COLL_QUAD_LIST	listCollQuad;
		MakeCollisionList ( pNode, listCollQuad, vStart, vEnd );		// 충돌 된 것 리스트로 만듬.

		COLL_QUAD_LIST_ITER iter = listCollQuad.begin();
		for ( ; iter!=listCollQuad.end(); ++iter )
		{
			if ( (*iter)->IsCollisionLine ( vStart, vDir, fLength ) )
			{
				pObject = (*iter);	
			}
		}

		//std::for_each ( listCollQuad.begin(), listCollQuad.end(), std_afunc::DeleteObject() );	// 참조만 하므로 다 삭제 하면 위험할 듯 하네.
		listCollQuad.clear();

		return pObject;
	}

	void FindQUADTree1 ( POBJQUADNODE pNode, const int &nID_X, const int &nID_Z, POBJQUAD& pFindOBJ )
	{
		if ( !pNode )	return;

		if ( !NSQUADTREE::IsCollisionPoint ( pNode->nMaxX, pNode->nMaxZ, pNode->nMinX, pNode->nMinZ, nID_X, nID_Z ) )	return;
		
		if ( pNode->pObject )
		{
			pFindOBJ = pNode->pObject;
			return;
		}

		FindQUADTree1 ( pNode->pLeftChild, nID_X, nID_Z, pFindOBJ );

		FindQUADTree1 ( pNode->pRightChild, nID_X, nID_Z, pFindOBJ );

		return;
	}

	POBJQUAD FindQUADTree ( POBJQUADNODE pNode, const int &nID_X, const int &nID_Z )
	{
		POBJQUAD	pObjQuad = NULL;
		FindQUADTree1 ( pNode, nID_X, nID_Z, pObjQuad );

		return pObjQuad;
	}

	HRESULT RenderQUADTree ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJQUADNODE &pNode, BOOL bGame )
	{
		HRESULT hr;
		if ( !pNode )	return S_OK;

		if ( !COLLISION::IsCollisionVolume ( *pCV, pNode->vMax, pNode->vMin ) )	return S_OK;

		if ( pNode->pObject )
		{
			hr = pNode->pObject->FrameMove ( pd3dDevice, pCV, bGame );
			hr = pNode->pObject->Render ( pd3dDevice, pCV );
			return hr;
		}

		hr = RenderQUADTree ( pd3dDevice, pCV, pNode->pLeftChild, bGame );
		if ( FAILED(hr) )	return E_FAIL;

		hr = RenderQUADTree ( pd3dDevice, pCV, pNode->pRightChild, bGame );
		if ( FAILED(hr) )	return E_FAIL;

		return S_OK;
	}

	//void MakeQUADTree ( const POBJQUAD *const ppArray, const DWORD dwArraySize,
	//	const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin, POBJQUADNODE &pTreeNode )
	//{
	//	if ( dwArraySize == 1 )
	//	{
	//		int nObjX = 0;
	//		int nObjZ = 0;
	//		D3DXVECTOR3 vObjMax, vObjMin;

	//		ppArray[0]->GetQUADSize ( nObjX, nObjZ );
	//		ppArray[0]->GetQUADSize ( vObjMax, vObjMin );

	//		pTreeNode->nMaxX	= nObjX;
	//		pTreeNode->nMinX	= nObjX;
	//		pTreeNode->nMaxZ	= nObjZ;
	//		pTreeNode->nMinZ	= nObjZ;
	//		pTreeNode->vMax		= vObjMax;
	//		pTreeNode->vMin		= vObjMin;

	//		dwLeafCountQUAD++;
	//		pTreeNode->pObject = ppArray[0];
	//		return;
	//	}

	//	float SizeX = vMax.x - vMin.x;
	//	float SizeZ = vMax.z - vMin.z;

	//	D3DXVECTOR3 vDivMax = vMax;
	//	D3DXVECTOR3 vDivMin = vMin;

	//	if ( SizeX >= SizeZ )		vDivMin.x = vMin.x + SizeX/2;
	//	else						vDivMin.z = vMin.z + SizeZ/2;

	//	DWORD dwLeftCount = 0, dwRightCount = 0;
	//	POBJQUAD* pLeftArray = new POBJQUAD[dwArraySize];
	//	POBJQUAD* pRightArray = new POBJQUAD[dwArraySize];

	//	//	Note : 분할된 공간으로 요소 분리.
	//	DWORD i;
	//	for ( i=0; i<dwArraySize; i++ )
	//	{
	//		if ( ppArray[i]->IsDetectDivision (vDivMax,vDivMin) )
	//			pLeftArray[dwLeftCount++] = ppArray[i];
	//		else
	//			pRightArray[dwRightCount++] = ppArray[i];
	//	}

	//	//	Note : 무작위로 요소 분리.
	//	if ( dwLeftCount == 0 || dwRightCount == 0 )
	//	{
	//		DWORD dwDivCount = dwArraySize / 2;
	//	
	//		dwLeftCount = 0;
	//		dwRightCount = 0;

	//		for ( i=0; i<dwDivCount; i++ )
	//			pLeftArray[dwLeftCount++] = ppArray[i];

	//		for ( i=dwDivCount; i<dwArraySize; i++ )
	//			pRightArray[dwRightCount++] = ppArray[i];
	//	}

	//	int nNodeX = 0;
	//	int nNodeZ = 0;
	//	D3DXVECTOR3 vNodeMax, vNodeMin;

	//	//	Note : 왼쪽 자식 노드 생성.
	//	//
	//	int nCMaxX, nCMaxZ, nCMinX, nCMinZ;
	//	nCMaxX = INT_MIN;
	//	nCMaxZ = INT_MIN;
	//	nCMinX = INT_MAX;
	//	nCMinZ = INT_MAX;

	//	D3DXVECTOR3 vCMax, vCMin;
	//	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	//	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	//	for ( i=0; i<dwLeftCount; i++ )
	//	{
	//		pLeftArray[i]->GetQUADSize ( vNodeMax, vNodeMin );

	//		if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
	//		if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
	//		if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

	//		if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
	//		if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
	//		if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;

	//		pLeftArray[i]->GetQUADSize ( nNodeX, nNodeZ );

	//		if ( nCMaxX < nNodeX )	nCMaxX = nNodeX;
	//		if ( nCMaxZ < nNodeZ )	nCMaxZ = nNodeZ;

	//		if ( nCMinX > nNodeX )	nCMinX = nNodeX;
	//		if ( nCMinZ > nNodeZ )	nCMinZ = nNodeZ;
	//	}

	//	pTreeNode->pLeftChild = new OBJQUADNode;
	//	pTreeNode->pLeftChild->vMax = vCMax;
	//	pTreeNode->pLeftChild->vMin = vCMin;
	//	pTreeNode->pLeftChild->nMaxX = nCMaxX;
	//	pTreeNode->pLeftChild->nMaxZ = nCMaxZ;
	//	pTreeNode->pLeftChild->nMinX = nCMinX;
	//	pTreeNode->pLeftChild->nMinX = nCMinZ;
	//	MakeQUADTree ( pLeftArray, dwLeftCount, vCMax, vCMin, pTreeNode->pLeftChild );

	//	//	Note : 오른쪽 자식 노드 생성.
	//	//
	//	nCMaxX = INT_MIN;
	//	nCMaxZ = INT_MIN;
	//	nCMinX = INT_MAX;
	//	nCMinZ = INT_MAX;

	//	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	//	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	//	for ( i=0; i<dwRightCount; i++ )
	//	{
	//		pRightArray[i]->GetQUADSize ( vNodeMax, vNodeMin );

	//		if ( vCMax.x < vNodeMax.x )	vCMax.x = vNodeMax.x;
	//		if ( vCMax.y < vNodeMax.y )	vCMax.y = vNodeMax.y;
	//		if ( vCMax.z < vNodeMax.z )	vCMax.z = vNodeMax.z;

	//		if ( vCMin.x > vNodeMin.x )	vCMin.x = vNodeMin.x;
	//		if ( vCMin.y > vNodeMin.y )	vCMin.y = vNodeMin.y;
	//		if ( vCMin.z > vNodeMin.z )	vCMin.z = vNodeMin.z;

	//		pRightArray[i]->GetQUADSize ( nNodeX, nNodeZ );

	//		if ( nCMaxX < nNodeX )	nCMaxX = nNodeX;
	//		if ( nCMaxZ < nNodeZ )	nCMaxZ = nNodeZ;

	//		if ( nCMinX > nNodeX )	nCMinX = nNodeX;
	//		if ( nCMinZ > nNodeZ )	nCMinZ = nNodeZ;
	//	}

	//	pTreeNode->pRightChild = new OBJQUADNode;
	//	pTreeNode->pRightChild->vMax = vCMax;
	//	pTreeNode->pRightChild->vMin = vCMin;
	//	pTreeNode->pRightChild->nMaxX = nCMaxX;
	//	pTreeNode->pRightChild->nMaxZ = nCMaxZ;
	//	pTreeNode->pRightChild->nMinX = nCMinX;
	//	pTreeNode->pRightChild->nMinX = nCMinZ;
	//	MakeQUADTree ( pRightArray, dwRightCount, vCMax, vCMin, pTreeNode->pRightChild );

	//	SAFE_DELETE_ARRAY(pRightArray);
	//	SAFE_DELETE_ARRAY(pLeftArray);
	//}
};