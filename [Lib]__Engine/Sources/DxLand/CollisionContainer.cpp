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
 
	_ASSERT ( !( m_NodeCount || m_pHead ) && "�޸𸮰� ������ �ݳ����� �ʾҽ��ϴ�." );

	return TRUE;
}

BOOL CCollisionContainer::AddCell ( SCollisionCell& Cell )
{
	SCollisionCell* pNewCell = new SCollisionCell;

    //	�� �ֱ�	
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
		//	Ư�� DxFrame ã�� ����[NULL�� ���� ����]
		if ( !pFrame || ( pFrame == pCurCell->pFrame ) )
		{
			//	V0�κ��� ���� ����� ���̽� ã��
			D3DXVECTOR3 vDist = pCurCell->vCollision - m_V0;
			float fCurDist = D3DXVec3LengthSq ( &vDist );			

			////////////////////////////////////////////////////////////////
			//_ASSERT ( int(fCurDist) && "vCollision�� m_V0�� ����?!" );
			//	v0�� vCollision�� ���� ��찡 �����Ҽ� �ִ�.
			//	����, �浹������ Vc��� �ϰ� ����������, V0(Vc)��� �Ѵٸ�,
			//	�浹 �Ÿ��� 0�� ������ �ȴ�. �׸���, ���� �浹�� ��������
			//	������ ���� �𼭸��κп� �����ϴ� ������ �ϳ��� �ƴ϶�,
			//	�ټ� ������ ���ɼ��� �ֱ� �����̴�.
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

