#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CGambleResultBox : public CUIGroup
{

public:
	CGambleResultBox(void);
	virtual ~CGambleResultBox(void);

public:
	void	CreateSubControl ();
	
public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl * m_pControl;
	float m_TotElapsedTime;
};