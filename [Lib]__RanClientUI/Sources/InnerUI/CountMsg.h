#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CCountMsg : public CUIGroup
{
public:
	CCountMsg ();
	virtual	~CCountMsg ();

public:
	void CreateSubControl();
	CUIControl*	CreateControl( CONST TCHAR* szControl );
	void SetCount( INT nCount );

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl * m_pTEN[10];
	CUIControl * m_pONE[10];

	INT m_nCount;
	float m_fElapsedTime;
};