#include "pch.h"
#include "CollisionContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCollisionContainer::CCollisionContainer () :
	m_pHead ( NULL ),
	m_NodeCount ( 0 )
{	
}

CCollisionContainer::~CCollisionContainer()
{
	CleanUp ();
}

BOOL CCollisionContainer::CleanUp ()
{	
	SCollisionCell* pDelCell = m_pHead;

	while ( m_pHead || m_NodeCount )
	{
		pDelCell = m_pHead;
		m_pHead = m_pHead->pNext;		

		SAFE_DELETE ( pDelCell );
		m_NodeCount--;
	}

	m_V0 = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	m_V1 = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
 
	_ASSERT ( !( m_NodeCount || m_pHead ) && "메모리가 완전히 반납되지 않았습니다." );

	return TRUE;
}

BOOL CCollisionContainer::AddCell ( SCollisionCell& Cell )
{
	SCollisionCell* pNewCell = new SCollisionCell;

    //	값 넣기	
	*pNewCell = Cell;

	//	Singly Linked List[LIFO]
	pNewCell->pNext = m_pHead;
	m_pHead = pNewCell;

	m_NodeCount++;

	return TRUE;
}

SCollisionCell*	const CCollisionContainer::GetHead ()
{
	return m_pHead;
}

int	CCollisionContainer::GetCount ()
{
	return m_NodeCount;
}

void CCollisionContainer::SetCollisionVertex ( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1 )
{
	m_V0 = v0;
	m_V1 = v1;
}

void CCollisionContainer::GetCollisionVertex ( D3DXVECTOR3& v0, D3DXVECTOR3& v1 )
{
	v0 = m_V0;
	v1 = m_V1;
}

D3DXVECTOR3	CCollisionContainer::GetVertexV0 ()
{
	return m_V0;
}

D3DXVECTOR3 CCollisionContainer::GetVertexV1 ()
{
	return m_V1;
}

void CCollisionContainer::SetVertexV0 ( D3DXVECTOR3 v0 )
{
	m_V0 = v0;
}

void CCollisionContainer::SetVertexV1 ( D3DXVECTOR3 v1 )
{
	m_V1 = v1;
}

SCollisionCell*	CCollisionContainer::GetMinDistFromV0 ( const DxFrame* pFrame )
{
	float fMinDist = FLT_MAX;
	SCollisionCell* pMinDistCell = NULL;
	SCollisionCell* pCurCell = GetHead ();		
	while ( pCurCell )
	{
		//	특정 DxFrame 찾기 조건[NULL일 경우는 무시]
		if ( !pFrame || ( pFrame == pCurCell->pFrame ) )
		{
			//	V0로부터 가장 가까운 페이스 찾기
			D3DXVECTOR3 vDist = pCurCell->vCollision - m_V0;
			float fCurDist = D3DXVec3LengthSq ( &vDist );			

			////////////////////////////////////////////////////////////////
			//_ASSERT ( int(fCurDist) && "vCollision과 m_V0가 같다?!" );
			//	v0와 vCollision이 같은 경우가 존재할수 있다.
			//	만약, 충돌지점이 Vc라고 하고 시작지점도, V0(Vc)라고 한다면,
			//	충돌 거리는 0가 나오게 된다. 그리고, 같은 충돌이 여러개가
			//	나오는 것은 모서리부분에 존재하는 정점이 하나가 아니라,
			//	다수 존재할 가능성이 있기 때문이다.
			////////////////////////////////////////////////////////////////
			
			if ( fCurDist < fMinDist )
			{
				fMinDist = fCurDist;			
				pMinDistCell = pCurCell;
			}
		}
			
		pCurCell = pCurCell->pNext;
	}

	return pMinDistCell;
}

