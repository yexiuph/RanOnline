#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CGambleResultEvenBox : public CUIGroup
{

public:
	CGambleResultEvenBox(void);
	virtual ~CGambleResultEvenBox(void);

public:
	void	CreateSubControl ();
	
public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
public:
	void SetWinLose(BOOL bWin);

private:
	CUIControl * m_pControl;
	float m_TotElapsedTime;
	BOOL  m_bWinLose;
};