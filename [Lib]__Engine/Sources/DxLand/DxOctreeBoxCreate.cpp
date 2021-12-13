#include "pch.h"

#include "DxOctree.h"	
#include "./DxFrameMesh.h"
#include "DxLandMan.h"

#include "CollisionMapCell.h"
#include "CollisionMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct	SCell
{
	DWORD*	m_pCells;
	DWORD	m_nCells;
};

void DxOctree::InsertFrameMesh ( DxFrame *pDxFrame )
{	
	pDxFrame->pframeSibling = m_pDxFrameHead;
	m_pDxFrameHead = pDxFrame;

	//	Note : 만약 Alpha Map 이 존재시에 이를 분류.
	//
	ModiflyAlphaMapFrame ( pDxFrame );

	OctreeDebugInfo::AmountDxFrame++;
}

void DxOctree::AddAlphaMapFrame ( DxFrame *pDxFrameAdd )
{
	GASSERT(pDxFrameAdd);

	//	Note :등록 리스트를 검색후 있으면 무시 / 없으면 등록.
	//
	DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead;
	while ( pDxFrameCur )
	{
		if ( pDxFrameCur == pDxFrameAdd )	return;
		pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
	}

	pDxFrameAdd->pNextAlphaMapFrm = m_pDxAlphaMapFrameHead;
	m_pDxAlphaMapFrameHead = pDxFrameAdd;
}

void DxOctree::DelAlphaMapFrame ( DxFrame *pDxFrameDel )
{
	GASSERT(pDxFrameDel);

	if ( m_pDxAlphaMapFrameHead == pDxFrameDel )
	{
		m_pDxAlphaMapFrameHead =  pDxFrameDel->pNextAlphaMapFrm;
	}
	else
	{
		DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead, *pDxFramePrev;
		while ( pDxFrameCur )
		{
			if ( pDxFrameCur == pDxFrameDel )
			{
				pDxFramePrev->pNextAlphaMapFrm = pDxFrameCur->pNextAlphaMapFrm;
				return;
			}

			pDxFramePrev = pDxFrameCur;
			pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
		}
	}
}

void DxOctree::ModiflyAlphaMapFrame ( DxFrame *pDxFrame )
{
	if ( pDxFrame->IsAlphaMapMesh() )
	{
		pDxFrame->bFlag |= DXFRM_ALPHAMAP;
		AddAlphaMapFrame ( pDxFrame );
	}
	else
	{
		pDxFrame->bFlag &= ~DXFRM_ALPHAMAP;
		DelAlphaMapFrame ( pDxFrame );
	}
}

HRESULT	DxOctree::Import( int MaxTriangles, int MaxSubdivisions,
						 CCollisionMap* pCollisionMap )
{
	if ( !pCollisionMap )
	{
		_ASSERT ( 0 && "pCollisionMap에 NULL이 설정되어 있습니다." );
		return E_FAIL;
	}

	m_MaxTriangles = MaxTriangles;
	m_MaxSubdivisions = MaxSubdivisions;

	//	Note : 센터 가져오기
	D3DXVECTOR3	vTreeCenter = pCollisionMap->GetTreeCenter ();

	//	Note : 너비가 가장 넓은거 가져오기
	float MaxWidth = 0.0f;
	float XWidth = pCollisionMap->GetTreeXWidth ();
	float YWidth = pCollisionMap->GetTreeYWidth ();
	float ZWidth = pCollisionMap->GetTreeZWidth ();
		 
	if(XWidth > YWidth && XWidth > ZWidth)
		MaxWidth = XWidth;
	else if(YWidth > XWidth && YWidth > ZWidth)
		MaxWidth = YWidth;
	else
		MaxWidth = ZWidth;

	//	Note : Make	Box	&	Set Data
	SCell CellInfo;
	CellInfo.m_nCells = pCollisionMap->GetCellCount();
	CellInfo.m_pCells = new DWORD[CellInfo.m_nCells];

	for ( DWORD i = 0; i < CellInfo.m_nCells; i++ )
	{
		CellInfo.m_pCells[i] = i;
	}

	//	Note : 프로그래스바 EndPos 설정
	ExportProgress::EndPos = CellInfo.m_nCells;
	
	CreateNode ( vTreeCenter, MaxWidth, &CellInfo, pCollisionMap );
	SAFE_DELETE_ARRAY ( CellInfo.m_pCells );

	D3DXVECTOR3 vMax, vMin;
	ResizeAllBox ( &vMax, &vMin );

	return S_OK;
}

void DxOctree::CreateNode(D3DXVECTOR3 vCenter, float Width,
		SCell* pAllCellInfo, CCollisionMap* pCollisionMap )
{
	//	Note	:	박스의 각 꼭지점 체크 ( 결정되지 않은 꼭지점으로 추후, 삼각형 잘리는 것에 따라 수정됨 )
	//	Note	:	박스의 TOP_RIGHT_BACK과 BOTTOM_LEFT_FRONT를 잡는다.,, 멤버임
	SetMinMaxCorners( vCenter, Width );

	if( (pAllCellInfo->m_nCells >= m_MaxTriangles) && (m_CurrentSubdivision < m_MaxSubdivisions))
	{
		m_bSubDivided = TRUE;

		SCell CellInfo[8];
		for ( DWORD i = 0; i < 8; i++ )
		{
			CellInfo[i].m_nCells = 0;
			CellInfo[i].m_pCells = new DWORD[pAllCellInfo->m_nCells];
		}

		for ( DWORD i = 0; i < pAllCellInfo->m_nCells; i++ )
		{
			//	Note	:	삼각형의 중심(이게 무슨 중심인지..?! 하여튼,)을 기준으로 그 놈이 들어간 박스로 결정
			D3DXVECTOR3 vPointArray[3];
			pCollisionMap->GetCellVertex ( vPointArray, pAllCellInfo->m_pCells[i] );

			D3DXVECTOR3 vTriCenter =
				GetTriCenter ( vPointArray[0], vPointArray[1], vPointArray[2] );			

			// Check if the point lines within the TOP LEFT FRONT node
			if( (vTriCenter.x <= vCenter.x) && (vTriCenter.y >= vCenter.y) && (vTriCenter.z <= vCenter.z) ) 
				CellInfo[0].m_pCells[CellInfo[0].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the TOP LEFT BACK node
			else if( (vTriCenter.x <= vCenter.x) && (vTriCenter.y >= vCenter.y) && (vTriCenter.z >= vCenter.z) ) 
				CellInfo[1].m_pCells[CellInfo[1].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the TOP RIGHT BACK node
			else if( (vTriCenter.x >= vCenter.x) && (vTriCenter.y >= vCenter.y) && (vTriCenter.z >= vCenter.z) ) 
				CellInfo[2].m_pCells[CellInfo[2].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the TOP RIGHT FRONT node
			else if( (vTriCenter.x >= vCenter.x) && (vTriCenter.y >= vCenter.y) && (vTriCenter.z <= vCenter.z) ) 
				CellInfo[3].m_pCells[CellInfo[3].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the BOTTOM LEFT FRONT node
			else if( (vTriCenter.x <= vCenter.x) && (vTriCenter.y <= vCenter.y) && (vTriCenter.z <= vCenter.z) ) 
				CellInfo[4].m_pCells[CellInfo[4].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the BOTTOM LEFT BACK node
			else if( (vTriCenter.x <= vCenter.x) && (vTriCenter.y <= vCenter.y) && (vTriCenter.z >= vCenter.z) ) 
				CellInfo[5].m_pCells[CellInfo[5].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the BOTTOM RIGHT BACK node
			else if( (vTriCenter.x >= vCenter.x) && (vTriCenter.y <= vCenter.y) && (vTriCenter.z >= vCenter.z) ) 
				CellInfo[6].m_pCells[CellInfo[6].m_nCells++] = pAllCellInfo->m_pCells[i];

			// Check if the point lines within the BOTTOM RIGHT FRONT node
			else if( (vTriCenter.x >= vCenter.x) && (vTriCenter.y <= vCenter.y) && (vTriCenter.z <= vCenter.z) ) 
				CellInfo[7].m_pCells[CellInfo[7].m_nCells++] = pAllCellInfo->m_pCells[i];
		}

		//	Note : 작은 박스 센타와 너비 가져오기
		const float fSmallWidth = Width / 4;
		const float fINTERPOLATION= 0.0f;		
		D3DXVECTOR3 vSmallCenter[8];
		GetNewNodeCenter ( vCenter, Width, vSmallCenter );

		//	Note	:	하위 정점들을 박스단위로 재귀적으로 나눈다.
		for ( int i = 0; i < 8; i++ )
		{
			CreateNewNode ( &CellInfo[i], Width, vSmallCenter[i], i, pCollisionMap );
		};

		for ( int i = 0; i < 8; i++ )
		{			
			SAFE_DELETE_ARRAY ( CellInfo[i].m_pCells );
		}		
	}	
	else
	{
		//	Note	:	더 이상 쪼갤 필요가 없을 경우 정점들을 박스에 넣는다.
		AssignVerticesToNode ( pAllCellInfo, pCollisionMap );
	}
}

void DxOctree::CreateNewNode ( SCell* pCell, float fWidth, D3DXVECTOR3 vCenter,
							  int NodeID, CCollisionMap* pCollisionMap )
{
	//	Note	:	삼각형의 개수가 0이 아니면, 박스를 생성한다.
	if(pCell->m_nCells)		
	{
		//	Note	:	정점의 개수만큼 메모리를 잡는다.
		SCell CellInfo;
		CellInfo.m_nCells = pCell->m_nCells;
		CellInfo.m_pCells = new DWORD[pCell->m_nCells];

		memcpy ( CellInfo.m_pCells, pCell->m_pCells, sizeof ( DWORD ) * CellInfo.m_nCells );

		//	Note	:	새로운 박스 메모리 할당
		m_pOctreeNodes[NodeID] = new DxOctree;
		m_pOctreeNodes[NodeID]->SetLoadState ( TRUE );

		//	Note	:	최대 레벨의 사이즈를 올린다.
		//				레벨 제한을 위한 꽁수
		m_CurrentSubdivision++;

		//	Note	:	아직 더 분할되어야 하는지 체크한다.
		m_pOctreeNodes[NodeID]->CreateNode(	vCenter, fWidth/2, &CellInfo, pCollisionMap );

		//	Note	:	최대 레벨의 사이즈를 줄인다.
		m_CurrentSubdivision--;
		
		SAFE_DELETE_ARRAY ( CellInfo.m_pCells );
	}
}

void DxOctree::GetNewNodeCenter(D3DXVECTOR3 vCenter, float width, D3DXVECTOR3 vSmallCenter[8])
{
	vSmallCenter[TOP_LEFT_FRONT]	= D3DXVECTOR3(vCenter.x - width/4, vCenter.y + width/4, vCenter.z - width/4);
	vSmallCenter[TOP_LEFT_BACK]		= D3DXVECTOR3(vCenter.x - width/4, vCenter.y + width/4, vCenter.z + width/4);
	vSmallCenter[TOP_RIGHT_BACK]	= D3DXVECTOR3(vCenter.x + width/4, vCenter.y + width/4, vCenter.z + width/4);
	vSmallCenter[TOP_RIGHT_FRONT]	= D3DXVECTOR3(vCenter.x + width/4, vCenter.y + width/4, vCenter.z - width/4);
	
	vSmallCenter[BOTTOM_LEFT_FRONT]	= D3DXVECTOR3(vCenter.x - width/4, vCenter.y - width/4, vCenter.z - width/4);
	vSmallCenter[BOTTOM_LEFT_BACK]	= D3DXVECTOR3(vCenter.x - width/4, vCenter.y - width/4, vCenter.z + width/4);
	vSmallCenter[BOTTOM_RIGHT_BACK]	= D3DXVECTOR3(vCenter.x + width/4, vCenter.y - width/4, vCenter.z + width/4);
	vSmallCenter[BOTTOM_RIGHT_FRONT]= D3DXVECTOR3(vCenter.x + width/4, vCenter.y - width/4, vCenter.z - width/4);
}

static	int OctreeFaceCount = 0;
HRESULT	DxOctree::AssignVerticesToNode ( SCell* pCell, CCollisionMap* pCollisionMap )
{
	m_bSubDivided = FALSE;

	DxFrame* pFrame = NULL;
	DxFrame* pFrameBack = NULL;

	for ( DWORD i = 0; i < pCell->m_nCells; i++ )
	{
		pFrame = pCollisionMap->GetDxFrame ( pCell->m_pCells[i] );

		if ( pFrame == pFrameBack )
		{
			continue;
		}

		pFrame->FrameMeshToOctree( this, &m_vMin, &m_vMax, m_pd3dDevice, 0, 0 );
		pFrameBack = pFrame;
	}

	//	Note	:	FrameMesh에 대한 박스 리사이징.
	ModifyFrameMeshBoundary ( pCell, pCollisionMap );	

	OctreeFaceCount += pCell->m_nCells;
	OctreeDebugInfo::EndNodeCount++;

	return S_OK;
}

void DxOctree::ModifyFrameMeshBoundary( SCell* pCell, CCollisionMap* pCollisionMap )
{
	D3DXVECTOR3		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3		vMin(FLT_MAX,FLT_MAX,FLT_MAX);

	for ( DWORD i = 0; i < pCell->m_nCells; i++ )
	{	
		D3DXVECTOR3 vPos[3];
		if ( !pCollisionMap->GetCellVertex ( vPos, pCell->m_pCells[i] ) )
		{
			_ASSERT ( 0 && "버텍스를 가져올수 없었습니다." );
			return;
		}

		for ( int j = 0; j < 3; j++ )
		{
			if ( vMax.x < vPos[j].x ) vMax.x = vPos[j].x;		
			if ( vMax.y < vPos[j].y ) vMax.y = vPos[j].y;
			if ( vMax.z < vPos[j].z ) vMax.z = vPos[j].z;
				
			if ( vMin.x > vPos[j].x ) vMin.x = vPos[j].x;
			if ( vMin.y > vPos[j].y ) vMin.y = vPos[j].y;
			if ( vMin.z > vPos[j].z ) vMin.z = vPos[j].z;
		}
	}

	m_vMax = vMax;
	m_vMin = vMin;
}

D3DXVECTOR3 DxOctree::GetTriCenter ( D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3 )
{
	D3DXVECTOR3	Center;

	Center.x = ( v1.x + v2.x + v3.x ) / 3.0f;
	Center.y = ( v1.y + v2.y + v3.y ) / 3.0f;
	Center.z = ( v1.z + v2.z + v3.z ) / 3.0f;

	return Center;
}