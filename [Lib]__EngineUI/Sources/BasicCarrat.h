#pragma once

#include "UIGroup.h"

class CBasicCarrat : public CUIGroup
{
static	const	float	fZERO_TIME;
static	const	float	fSTART_TIME;
static	const	float	fEND_TIME;

public:
	CBasicCarrat ();
	virtual ~CBasicCarrat();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	BOOL IsRender() { return m_bRender; }

public:
	//�������¸� �����ϰ� ����Ʈ�� ����� ����� �׸��� ����
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float	m_TotElapsedTime;
	bool	m_bRender;
};