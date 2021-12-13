#include "pch.h"
#include "BasicCarrat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float	CBasicCarrat::fZERO_TIME = 0.0f;
const float	CBasicCarrat::fSTART_TIME = 0.3f;
const float CBasicCarrat::fEND_TIME = 0.6f;

CBasicCarrat::CBasicCarrat():
	m_TotElapsedTime(fZERO_TIME)
	, m_bRender( false )
{
}

CBasicCarrat::~CBasicCarrat(void)
{
}

void CBasicCarrat::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_TotElapsedTime += fElapsedTime;
}

HRESULT CBasicCarrat::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !IsVisible () || !IsUseRender () ) return S_OK;

	if ( fSTART_TIME <= m_TotElapsedTime && m_TotElapsedTime <= fEND_TIME)
	{
		m_bRender = true;
		CUIGroup::Render ( pd3dDevice );
	}

	if ( fEND_TIME <= m_TotElapsedTime )
	{
		m_bRender = false;
		m_TotElapsedTime = fZERO_TIME;
	}

	return S_OK;
}
