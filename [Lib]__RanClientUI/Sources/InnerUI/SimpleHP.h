#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicProgressBar;

class	CSimpleHP : public CUIGroup
{
public:
	CSimpleHP ();
	virtual	~CSimpleHP ();

public:
	void	SetHP ( WORD wNOW, WORD wMAX );
	void	CreateSubControl ();
	void	SetUseSummonHP( bool bUseSummonHP ) { m_bUseSummonHP = bUseSummonHP; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicProgressBar*	m_pHP;
	bool				m_bUseSummonHP;
};