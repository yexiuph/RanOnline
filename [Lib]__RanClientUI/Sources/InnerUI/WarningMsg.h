#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CWarningMsg : public CUIGroup
{
public:
	CWarningMsg ();
	virtual	~CWarningMsg ();

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl * m_pControl;

	float m_TotElapsedTime;
};