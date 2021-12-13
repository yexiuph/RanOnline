#pragma	once

#include "BasicQuickSkillSlot.h"
#include "SkillTrayTab.h"

class	CBasicTextBox;

class	CBasicQuickSkillSlotEx : public CBasicQuickSkillSlot
{
private:
static	const	int		nNOT_INIT;

public:
	CBasicQuickSkillSlotEx ();
	virtual	~CBasicQuickSkillSlotEx ();

public:
	virtual	void	CreateSubControl ( INT nTabIndex );
	void	CreateNumberText ( CD3DFontPar* pFont8, int nNumber );

	INT GetTabIndex()			{ return m_nTabIndex; }
	void	SetShotcutText( CString strTemp );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );	

private:
	int				m_nNumber;
	CBasicTextBox*	m_pNumberText;
	INT				m_nTabIndex;
};