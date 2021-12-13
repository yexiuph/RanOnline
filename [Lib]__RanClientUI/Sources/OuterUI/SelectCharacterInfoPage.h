#pragma	once

#include "UIOuterWindow.h"

class	CBasicProgressBar;
class	CBasicTextBox;
struct	SCHARINFO_LOBBY;

class CSelectCharacterInfoPage : public CUIOuterWindow
{
public:
	CSelectCharacterInfoPage ();
	virtual	~CSelectCharacterInfoPage ();

public:
	void	CreateSubControl ();

public:
	void	SetExp ( );
	void	SetName ();
	void	SetClass();
	void	SetLevel();

	void	ResetAll ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*		m_pInfo0Box;
	CBasicTextBox*		m_pInfo1Box;
	CBasicTextBox*		m_pInfo2Box;

	CBasicProgressBar*	m_pEXP;
	CBasicTextBox*		m_pEXPText;

	CUIControl*			m_pBrightFrame;
	CUIControl*			m_pBrightSlider;

	SCHARINFO_LOBBY*	m_pCharInfoBack;
};