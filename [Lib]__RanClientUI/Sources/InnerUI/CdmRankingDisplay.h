#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBox;

class CCdmRankingDisplay : public CUIGroup
{
private:
	CBasicTextBox*	m_pCdmRankingText;

public:
	CCdmRankingDisplay();
	~CCdmRankingDisplay();

	void	CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:	
	void	RefreashCdmRanking();
};