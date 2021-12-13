#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include "./StlFunctions.h"

#include "./TextureManager.h"

#include "./GLDefine.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxMapEditMan::ModifyGrid ( LPDIRECT3DDEVICEQ pd3dDevice )		// 완전히 새로 만들 때 ??
{
	SAFE_DELETE ( m_pQuadList );

	m_vMin.x = -(float)(m_dwGrid_Length*m_dwGrid*0.5f);
	m_vMin.y = 0.f;
	m_vMin.z = -(float)(m_dwGrid_Length*m_dwGrid*0.5f);

	m_vMax.x = (float)(m_dwGrid_Length*m_dwGrid*0.5f);
	m_vMax.y = 0.f;
	m_vMax.z = (float)(m_dwGrid_Length*m_dwGrid*0.5f);

	for ( DWORD y=0; y<m_dwGrid; ++y )
	{
		for ( DWORD x=0; x<m_dwGrid; ++x )
		{
			CreateQuadList ( x, y );
		}
	}

	NSQUADTREE::MakeQUADTree ( m_pQuadTree, m_pQuadList );
}

void DxMapEditMan::CreateQuadList ( int x, int z )
{
	QuadBase*	pQuad = new QuadBase;

	pQuad->nID_X = x;
	pQuad->nID_Z = z;

	pQuad->vMin.x = m_vMin.x + (m_dwGrid_Length*x);
	pQuad->vMin.z = m_vMin.z + (m_dwGrid_Length*z);
	pQuad->vMax.x = pQuad->vMin.x + m_dwGrid_Length;
	pQuad->vMax.z = pQuad->vMin.z + m_dwGrid_Length;

	pQuad->pNext = m_pQuadList;
	m_pQuadList = pQuad;
}
