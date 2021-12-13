#include "pch.h"

#include "./DxLandMan.h"
#include "DxAnimationManager.h"

#include "./TextureManager.h"

#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxFrame::GetAllNodeData ( DWORD* pIndices, DWORD* pPrevIndexPos,
								 OCTREEVERTEX* pVertices, DWORD* pPrevVertexPos,
								 DxFrame** ppDxFrames, DWORD* pPrevDxFramePos, DxFrame* pDxFrameThis, bool breplace_in, const BOOL bEdit )
{	
	//	Note	:	��Ʈ�� ����Ÿ���� �ʿ���� ���� ���� �ڵ�
	//				Edit ��� �� ���� ������ �����.
	if( !bEdit )
	{
		if ( pframeFromAnimate )				return;
		if ( !breplace_in && IsReplace() )		return;
	}

	if ( pmsMeshs )
	{
		DxMeshes *pMesh = pmsMeshs;

		while ( pMesh )
		{
			//	Note	:	�ε��� ������ ����
			//
			LPD3DXMESH	pMeshData = pMesh->m_pLocalMesh;
			
			if ( pMeshData->GetOptions() & D3DXMESH_32BIT )
			{
				DWORD	*pLockIndices = NULL;
				int		NumOfIndices = pMeshData->GetNumFaces() * 3;
				
				//	�ε����� NULL�� �Ѿ����, �ش��ϴ� DxFrame�� �ε��� ������
				//	üũ�Ѵ�.
				if ( pIndices )
				{
					pMeshData->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pLockIndices );
					for ( int i = 0; i < NumOfIndices; i++ )
					{
						pIndices[i + *pPrevIndexPos] = DWORD(pLockIndices[i] + *pPrevVertexPos);


						//////////////////////////////////////////////////////
						//
						//	DxFrame �ܾ����, Octree���ҽ� ���� ������ ���� ����
						if ( !(i % 3) && ppDxFrames )
						{							
							ppDxFrames[(*pPrevDxFramePos)++] = pDxFrameThis;							
						}
						//////////////////////////////////////////////////////
					}
					pMeshData->UnlockIndexBuffer();
				}
				*pPrevIndexPos += NumOfIndices;
				
			}
			else
			{
				WORD	*pLockIndices = NULL;
				int		NumOfIndices = pMeshData->GetNumFaces() * 3;
				
				//	�ε����� NULL�� �Ѿ����, �ش��ϴ� DxFrame�� �ε��� ������
				//	üũ�Ѵ�.
				if ( pIndices )
				{
					pMeshData->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pLockIndices );
					for ( int i = 0; i < NumOfIndices; i++ )
					{
						pIndices[i + *pPrevIndexPos] = DWORD(pLockIndices[i] + *pPrevVertexPos);

						//////////////////////////////////////////////////////
						//
						//	DxFrame �ܾ����, Octree���ҽ� ���� ������ ���� ����
						if ( !(i % 3) && ppDxFrames )
						{							
							ppDxFrames[(*pPrevDxFramePos)++] = pDxFrameThis;							
						}
						//////////////////////////////////////////////////////
					}
					pMeshData->UnlockIndexBuffer();
				}
				*pPrevIndexPos += NumOfIndices;
			}

			//	Note	:	���ؽ� ������ ����
			//
			
			OCTREEVERTEX	*pLockVertices = NULL;
			int				NumOfVertices = pMeshData->GetNumVertices();

			//	���ؽ����ۿ� NULL�� �Ѿ���� ������ üũ�Ѵ�.
			if ( pVertices )
			{
				pMeshData->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pLockVertices );
				for ( int i = 0; i < NumOfVertices; i++ )
				{			
					D3DXVECTOR3	TransformVertex = D3DXVECTOR3 ( pLockVertices[i].x, pLockVertices[i].y, pLockVertices[i].z );
					D3DXVec3TransformCoord ( &TransformVertex, &TransformVertex, &matCombined );
					pVertices[*pPrevVertexPos + i].x = TransformVertex.x;
					pVertices[*pPrevVertexPos + i].y = TransformVertex.y;
					pVertices[*pPrevVertexPos + i].z = TransformVertex.z;
				}
				pMeshData->UnlockVertexBuffer();
			}
			*pPrevVertexPos += NumOfVertices;

			pMesh = pMesh->pMeshNext;
		}
	}

	DxFrame		*pframeChild = pframeFirstChild;
	while ( pframeChild )
	{
		pframeChild->GetAllNodeData( pIndices, pPrevIndexPos, pVertices, pPrevVertexPos,
			ppDxFrames, pPrevDxFramePos, pframeChild, false, bEdit );
		pframeChild = pframeChild->pframeSibling;
	}

	return;
}

HRESULT	DxFrame::DxFrameNodeCopy ( DxFrame *pDst, DxFrame *pSrc, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax,
								  LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName )
{
	HRESULT		hr = S_OK;

	pDst->vTreeMax = pSrc->vTreeMax;
	pDst->vTreeMin = pSrc->vTreeMin;

	if ( !pvMin && !pvMax )
	{
		pDst->matRot = pSrc->matRot;				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
		pDst->matRotOrig = pSrc->matRotOrig;		//	���� �������� �ƴ� ���� Ʈ������.
		pDst->matCombined = pSrc->matCombined;		//	�θ� ���� ������ ���� Ʈ������.
	}

	if ( pSrc->szName )
	{
		pDst->szName = new char [ strlen ( pSrc->szName ) + 1 ];
		memset ( pDst->szName, 0, strlen ( pSrc->szName ) + 1 );
		memcpy ( pDst->szName, pSrc->szName, strlen ( pSrc->szName ) );	
	}	
	else if ( pParentName )	//	�θ��� �̸�
	{
		_ASSERT(0);
		pDst->szName = new char [ strlen ( pParentName ) + 1 ];
		memset ( pDst->szName, 0, strlen ( pParentName ) + 1 );
		memcpy ( pDst->szName, pParentName, strlen ( pParentName ) );
	}

	//	Note	:	Mesh�� ��� �´�. ( ����Ÿ �������� )
	DxMeshes *pMesh = pSrc->pmsMeshs;
	DxMeshes *pNewMesh = NULL;
	DxMeshes **ppTailMesh = &pDst->pmsMeshs;

	while ( pMesh )
	{
		if ( FAILED ( ConvertMesh ( &pNewMesh, pMesh, pvMin, pvMax, pd3dDevice ) ) )
		{
			pMesh = pMesh->pMeshNext;
			continue;
		}

		//	Note	:	FIFO ������ ����Ʈ ����
		*ppTailMesh = pNewMesh;			
		ppTailMesh = &pNewMesh->pMeshNext;
		
		pMesh = pMesh->pMeshNext;
	}

	return	hr;
}

HRESULT	DxFrame::AniFrameTreeCopy ( DxFrame *pDst, DxFrame *pSrc, DxAnimationMan *pDxAnimationMan,
							LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName )
{
	HRESULT hr = S_OK;

	//	Note : ���� ������ ����.
	//
	DxFrameNodeCopy ( pDst, pSrc, NULL, NULL, pd3dDevice, pParentName );

	//	Note : �ش� ���ϸ��̼� ����.
	//
	if ( pSrc->pframeFromAnimate )
	{			
		DxAnimation *pNewAnimation = new DxAnimation();
		*pNewAnimation = *(pSrc->pframeFromAnimate);

		pNewAnimation->pframeToAnimate = pDst;
		pDst->pframeFromAnimate = pNewAnimation;

		pDxAnimationMan->InsertDxAnimation ( pNewAnimation );
	}

	//	Note : �ڽ� Ž��.
	//
	char *pName = NULL;
	DxFrame *pframeDst= NULL;
	DxFrame *pframeDstSiblingHead = NULL;
	
	DxFrame	 *pframeSrc = pSrc->pframeFirstChild;
	while ( pframeSrc )
	{
		pframeDst = new DxFrame();

		if ( pSrc->szName && !pframeSrc->szName )	//	�̸� ����.
		{
			int nStrLen = strlen(pSrc->szName)+1;
			pName = new char[nStrLen];
			StringCchCopy( pName, nStrLen, pSrc->szName );
		}

		//	Note : pframeSrc�� �ڽ� ����.
		//
		AniFrameTreeCopy ( pframeDst, pframeSrc, pDxAnimationMan, pd3dDevice, pName );

		//	Note : ����Ʈ�� ����.
		//
		pframeDst->pframeSibling = pframeDstSiblingHead;
		pframeDstSiblingHead = pframeDst;

		SAFE_DELETE_ARRAY ( pName );
		pframeSrc = pframeSrc->pframeSibling;	//	����(Next) ������.
	}

	pDst->pframeFirstChild = pframeDstSiblingHead;

	return	hr;
}

HRESULT DxFrame::GetAniFrameMesh ( LPDXANIMATIONMAN &pDxAnimationMan,
									float fUNITTIME, LPDIRECT3DDEVICEQ pd3dDevice )
{
	const char* const szDefaultName = "[ANIROOT]";

	HRESULT	hr = S_OK;

	DxFrame		*pframeCur = pframeFirstChild;
	while ( pframeCur )
	{
		//	Note : Ʈ���� Ž���� ���ϸ��̼� �ִ� Frame�� ��� ����.
		//
		if ( pframeCur->pframeFromAnimate )
		{
			//	Note : ���� Frame ����.
			//
			DxFrame		*pNewFrame = new DxFrame();

			//	Note : ���ϸ��̼��� �ݿ��� �ݰ�����.
			//
			D3DXVECTOR3	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vMin(FLT_MAX,FLT_MAX,FLT_MAX);
			pframeCur->CalculateBoundingBox ( &matRot, vMax, vMin, fUNITTIME, TRUE );
			
			pNewFrame->vTreeMax = vMax;
			pNewFrame->vTreeMin = vMin;
			pNewFrame->matRot = pNewFrame->matRotOrig = matCombined;
			pNewFrame->bParentMatrixUSE = bParentMatrixUSE;

			if ( pframeCur->szName )
			{
				size_t nStrLen = strlen(pframeCur->szName)+strlen(szDefaultName)+1;
				pNewFrame->szName = new char[ nStrLen ];
				StringCchPrintf( pNewFrame->szName, nStrLen, "%s%s", pframeCur->szName, szDefaultName );
			}
			else
			{
				char szNum[256];
				StringCchPrintf( szNum, 256, "%03d", DxAnimationMan::m_dwAniManCount );

				size_t nStrLen = strlen(szDefaultName)+strlen(szNum)+1;
				pNewFrame->szName = new char[ nStrLen ];
				StringCchPrintf( pNewFrame->szName, nStrLen, "%s%s", szDefaultName, szNum );
			}

			//	Note : DxAnimationMan �߰� �ڵ�
			//
			DxAnimationMan *pNewDxAnimationMan = new DxAnimationMan;
			pNewDxAnimationMan->SetDxFrameRoot ( pNewFrame );
			pNewDxAnimationMan->SetUnitTIme ( fUNITTIME );

			//	Note : ����Ʈ�� ����.
			//
			DxAnimationMan::m_dwAniManCount++;
			pNewDxAnimationMan->m_pNext = pDxAnimationMan;
			pDxAnimationMan = pNewDxAnimationMan;			

			//	Note : ���ϸ��̼��� �Ǵ� Frame ���� Ʈ�� ����.
			//
			pNewFrame->pframeFirstChild = new DxFrame();
			AniFrameTreeCopy ( pNewFrame->pframeFirstChild, pframeCur, pNewDxAnimationMan, pd3dDevice, NULL  );			
		}
		else
		{	
			hr = pframeCur->GetAniFrameMesh ( pDxAnimationMan, fUNITTIME, pd3dDevice );
		}

		pframeCur = pframeCur->pframeSibling;
	}	

	return hr;
}

HRESULT DxFrame::FrameMeshToOctree ( DxOctree *pDxOctree, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice, int debug, int debug2 )
{
	//	Note	:	�޽��� ������ ���� ��Ʈ�� ��忡�� ���δ�.
	if ( pmsMeshs )
	{
		//	Note	:	DxFrame �⺻ ���� ����
		DxFrame		*pDxFrame = new DxFrame();
	
		DxFrameNodeCopy ( pDxFrame, this, pvMin, pvMax, pd3dDevice, NULL );

		if ( !pDxFrame->pmsMeshs )
		{
			SAFE_DELETE ( pDxFrame )
		}
		else
		{
			pDxOctree->InsertFrameMesh ( pDxFrame );			
		}
	}

	return S_OK;
}

DxFrame* DxFrameMesh::GetAniFrame( const char*szName )
{
	DxFrame* pRoot = NULL;
	DxFrame* pFrame = NULL;
	DxAnimationMan* pCur = pAnimManHead;
	while( pCur )
	{
		pRoot = pCur->GetDxFrameRoot();
		if( pRoot )
		{
			pFrame = pRoot->FindFrame( szName );
			if( pFrame )	return pFrame;
		}
		pCur = pCur->m_pNext;
	}

	return NULL;
}

//void DxFrameMesh::SetAABBBOXProcessing( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
//{
//	if( !pAnimHead )	return;
//	if( !pAnimManHead )	return;
//
//	D3DXMATRIX matidentity;
//	D3DXMatrixIdentity( &matidentity );
//
//	float fTime = 0.f;
//	float fElapsedTime = 0.033f;
//	float fEndTime = (float)pAnimHead->m_pMatrixKeys[pAnimHead->m_cMatrixKeys-1].dwTime;
//	while( fTime < fEndTime )
//	{
//		//	Note : ���ϸ��̼� ������Ʈ.
//		DxAnimation* pAnimCur = pAnimHead;
//		while ( pAnimCur != NULL )
//		{
//			pAnimCur->SetTime( fTime );
//			pAnimCur = pAnimCur->pAnimNext;
//		}
//
//		UpdateFrames( m_pDxFrame, matidentity );
//		SetAABBBOXProcessingChild( m_pDxFrame, vMax, vMin );
//
//		//	Note : �ð��� ��ȭ
//		fTime += fElapsedTime * UNITANIKEY_PERSEC;
//		if ( fTime > 1.0e15f )	return;
//	}
//}
//
//void DxFrameMesh::SetAABBBOXProcessingChild( DxFrame* pFrame, D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
//{
//	D3DXVECTOR3 vMaxAABB = pFrame->vTreeMax;
//	D3DXVECTOR3 vMinAABB = pFrame->vTreeMin;
//	D3DXMatrixAABBTransform( vMaxAABB, vMinAABB, pFrame->matCombined );
//
//	if( vMax.x < vMaxAABB.x )	vMax.x = vMaxAABB.x;
//	if( vMax.y < vMaxAABB.y )	vMax.y = vMaxAABB.y;
//	if( vMax.z < vMaxAABB.z )	vMax.z = vMaxAABB.z;
//
//	if( vMin.x > vMinAABB.x )	vMin.x = vMinAABB.x;
//	if( vMin.y > vMinAABB.y )	vMin.y = vMinAABB.y;
//	if( vMin.z > vMinAABB.z )	vMin.z = vMinAABB.z;
//
//	if( pFrame->pframeFirstChild )
//		SetAABBBOXProcessingChild( pFrame->pframeFirstChild, vMax, vMin );
//
//	if( pFrame->pframeSibling )
//		SetAABBBOXProcessingChild( pFrame->pframeSibling, vMax, vMin );
//}

void DxFrameMesh::SetPieceUseFALSE()
{
	SetPieceUseChild( m_pDxFrame, FALSE );
}

void DxFrameMesh::SetPieceUseChild( DxFrame* pFrame, const BOOL bUse )
{
	pFrame->bPieceUSE = bUse;

	DxFrame *pframeChild = pFrame->pframeFirstChild;
    while ( pframeChild != NULL )
    {
        SetPieceUseChild( pframeChild, bUse );

        pframeChild = pframeChild->pframeSibling;
    }
}

void DxFrameMesh::AddPiece2( const char* pName )
{
	DxFrame* pFrame = m_pDxFrame->FindFrame( pName );
	if( pFrame )	SetPieceUseChild( pFrame, TRUE );
}

void DxFrameMesh::DelPiece2( const char* pName )
{
	DxFrame* pFrame = m_pDxFrame->FindFrame( pName );
	if( pFrame )	SetPieceUseChild( pFrame, FALSE );	// �� ������ ���� ����
}



