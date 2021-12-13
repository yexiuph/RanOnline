#include "pch.h"

#include "DxSequenceUV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------------------------------------------------------------
//										D	x		S	e	q	u	e	n	c	e		U	V
//--------------------------------------------------------------------------------------------------------------------------
DxSequenceUV::~DxSequenceUV()
{
	SAFE_DELETE_ARRAY( m_pTexUV );
}

void DxSequenceUV::CreateData( const int nCol, const int nRow )
{
	SAFE_DELETE_ARRAY( m_pTexUV );

	m_dwFullGrid = nCol*nRow;
	m_fOffsetWidth = 1.f/nCol;
	m_fOffsetHeight = 1.f/nRow;
	m_pTexUV = new D3DXVECTOR2[ m_dwFullGrid ];

	int nColTemp(0);
	int nRowTemp(0);
	float fCol(0.f);
	float fRow(0.f);

	for( DWORD i=0; i<m_dwFullGrid; ++i )
	{
		nColTemp = 	i % nCol;
		nRowTemp = 	i / nCol;
		fCol = (float)nColTemp/nCol;
		fRow = (float)nRowTemp/nRow;

		m_pTexUV[i] = D3DXVECTOR2( fCol, fRow );
	}
}

D3DXVECTOR2* DxSequenceUV::GetTexUV( const DWORD nCount )
{
	if( nCount >= m_dwFullGrid )	return NULL;

	return &m_pTexUV[nCount];
}
