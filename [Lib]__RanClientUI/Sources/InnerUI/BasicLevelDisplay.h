#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class CBasicLevelDisplay : public CUIGroup
{
public:
	CBasicLevelDisplay ();
	virtual	~CBasicLevelDisplay ();

public:
	void	CreateSubControl ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*	m_pLevelText;
	WORD m_wLevelBACK;
};