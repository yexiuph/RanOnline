#pragma once

class DxSequenceUV
{
private:
	DWORD			m_dwFullGrid;
	float			m_fOffsetWidth;
	float			m_fOffsetHeight;
    D3DXVECTOR2*	m_pTexUV;

public:
	void CreateData( const int nCol, const int nRow );

	D3DXVECTOR2* GetTexUV( const DWORD nCount );
	DWORD		GetTexCount( const float fRate )	{ return (DWORD)(m_dwFullGrid*fRate); }
	float		GetOffsetWidth()					{ return m_fOffsetWidth; }
	float		GetOffsetHeight()					{ return m_fOffsetHeight; }

public:
	DxSequenceUV() :
		m_dwFullGrid(0),
		m_fOffsetWidth(0.f),
		m_fOffsetHeight(0.f),
		m_pTexUV(NULL)
	{
	};
	~DxSequenceUV();
};

