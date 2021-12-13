#include "pch.h"

#include "DxOctree.h"
#include "./DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void DxOctree::SetMinMaxCorners ( D3DXVECTOR3 vCenter, float width )
{
	width = width / 2;

	m_vMax.x = vCenter.x + width;
	m_vMax.y = vCenter.y + width;
	m_vMax.z = vCenter.z + width;

	m_vMin.x = vCenter.x - width;
	m_vMin.y = vCenter.y - width;
	m_vMin.z = vCenter.z - width;
}

//	Note	:	�� �ڽ��� ��Ÿ(�����󿡼�..)�� �����´�.
//				������ �θ��� ��Ÿ���� �ڱ��� ��ġ����(nodeID)�� �����Ͽ� ���� ������
//				��Ÿ�� ã�� ���̴�.
D3DXVECTOR3 DxOctree::GetNewNodeCenter(D3DXVECTOR3 vCenter, float width, int nodeID)
{	
	D3DXVECTOR3 vNodeCenter(0, 0, 0);

	switch(nodeID)							
	{
		case TOP_LEFT_FRONT:			
			vNodeCenter = D3DXVECTOR3(vCenter.x - width/4, vCenter.y + width/4, vCenter.z - width/4);
			break;

		case TOP_LEFT_BACK:
			vNodeCenter = D3DXVECTOR3(vCenter.x - width/4, vCenter.y + width/4, vCenter.z + width/4);
			break;

		case TOP_RIGHT_BACK:
			vNodeCenter = D3DXVECTOR3(vCenter.x + width/4, vCenter.y + width/4, vCenter.z + width/4);
			break;

		case TOP_RIGHT_FRONT:
			vNodeCenter = D3DXVECTOR3(vCenter.x + width/4, vCenter.y + width/4, vCenter.z - width/4);
			break;

		case BOTTOM_LEFT_FRONT:
			vNodeCenter = D3DXVECTOR3(vCenter.x - width/4, vCenter.y - width/4, vCenter.z - width/4);
			break;

		case BOTTOM_LEFT_BACK:
			vNodeCenter = D3DXVECTOR3(vCenter.x - width/4, vCenter.y - width/4, vCenter.z + width/4);
			break;

		case BOTTOM_RIGHT_BACK:
			vNodeCenter = D3DXVECTOR3(vCenter.x + width/4, vCenter.y - width/4, vCenter.z + width/4);
			break;

		case BOTTOM_RIGHT_FRONT:
			vNodeCenter = D3DXVECTOR3(vCenter.x + width/4, vCenter.y - width/4, vCenter.z - width/4);
			break;
	}

	return vNodeCenter;
}

void DxOctree::ResizeAllBox ( D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax )
{	
	if ( m_bSubDivided )
	{
		D3DXVECTOR3	vAllMin[8];
		D3DXVECTOR3	vAllMax[8];

		if ( m_pOctreeNodes[TOP_LEFT_FRONT] )		m_pOctreeNodes[TOP_LEFT_FRONT]->ResizeAllBox( &vAllMin[TOP_LEFT_FRONT], &vAllMax[TOP_LEFT_FRONT] );
		if ( m_pOctreeNodes[TOP_LEFT_BACK] )		m_pOctreeNodes[TOP_LEFT_BACK]->ResizeAllBox( &vAllMin[TOP_LEFT_BACK], &vAllMax[TOP_LEFT_BACK] );
		if ( m_pOctreeNodes[TOP_RIGHT_BACK] )		m_pOctreeNodes[TOP_RIGHT_BACK]->ResizeAllBox( &vAllMin[TOP_RIGHT_BACK], &vAllMax[TOP_RIGHT_BACK] );
		if ( m_pOctreeNodes[TOP_RIGHT_FRONT] )		m_pOctreeNodes[TOP_RIGHT_FRONT]->ResizeAllBox( &vAllMin[TOP_RIGHT_FRONT], &vAllMax[TOP_RIGHT_FRONT] );
		if ( m_pOctreeNodes[BOTTOM_LEFT_FRONT] )	m_pOctreeNodes[BOTTOM_LEFT_FRONT]->ResizeAllBox( &vAllMin[BOTTOM_LEFT_FRONT], &vAllMax[BOTTOM_LEFT_FRONT] );
		if ( m_pOctreeNodes[BOTTOM_LEFT_BACK] )		m_pOctreeNodes[BOTTOM_LEFT_BACK]->ResizeAllBox( &vAllMin[BOTTOM_LEFT_BACK], &vAllMax[BOTTOM_LEFT_BACK] );
		if ( m_pOctreeNodes[BOTTOM_RIGHT_BACK] )	m_pOctreeNodes[BOTTOM_RIGHT_BACK]->ResizeAllBox( &vAllMin[BOTTOM_RIGHT_BACK], &vAllMax[BOTTOM_RIGHT_BACK] );
		if ( m_pOctreeNodes[BOTTOM_RIGHT_FRONT] )	m_pOctreeNodes[BOTTOM_RIGHT_FRONT]->ResizeAllBox( &vAllMin[BOTTOM_RIGHT_FRONT], &vAllMax[BOTTOM_RIGHT_FRONT] );
		
		D3DXVECTOR3		vMin;
		D3DXVECTOR3		vMax;
		for ( int i = 0; i < 8; i++ )
		{
			vMin = vAllMin[i];
			vMax = vAllMax[i];

			if ( vMin.x < m_vMin.x ) m_vMin.x = vMin.x;
			if ( vMin.y < m_vMin.y ) m_vMin.y = vMin.y;
			if ( vMin.z < m_vMin.z ) m_vMin.z = vMin.z;

			if ( vMax.x > m_vMax.x ) m_vMax.x = vMax.x;
			if ( vMax.y > m_vMax.y ) m_vMax.y = vMax.y;
			if ( vMax.z > m_vMax.z ) m_vMax.z = vMax.z;
		}
	}

	//	Note	:	�߶���� ���̽����� ��鿡 ������ �پ� �־,
	//				�ڽ��� �ʹ� ��� �߶��� ��� �ణ�� ������ �ش�.		
	m_vMax.x += 0.0001f;
	m_vMin.x -= 0.0001f;
	m_vMax.y += 0.0001f;
	m_vMin.y -= 0.0001f;
	m_vMax.z += 0.0001f;
	m_vMin.z -= 0.0001f;


	*pvMin = m_vMin;
	*pvMax = m_vMax;
}

