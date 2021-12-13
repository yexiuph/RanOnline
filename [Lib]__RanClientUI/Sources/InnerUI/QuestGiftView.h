#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;
class	CBasicTextButton;

class	CQuestGiftView : public CUIGroup
{
public:
	enum
	{
		QUEST_GIFT_VIEW_OK = NO_ID + 1
	};

public:
	CQuestGiftView ();
	virtual	~CQuestGiftView ();

public:
	void	CreateSubControl ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetQuestID ( const DWORD& dwQuestID );	

private:
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

private:
	CBasicTextBox*	m_pTextBox;

private:
	CBasicTextButton*	m_pOK;
};