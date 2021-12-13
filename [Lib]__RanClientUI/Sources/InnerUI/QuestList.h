#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBoxEx;
class	CBasicTextButton;
class	CBasicScrollBarEx;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_LIST_LB_DUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CQuestList : public CUIGroup
{
private:
static	const	int		nOUTOFRANGE;
static	const	int		nLimitQuest;

private:
	enum
	{
		QUEST_LIST_TEXTBOX_PROG = NO_ID + 1,
		QUEST_LIST_TEXTBOX_END,
		QUEST_LIST_SCROLLBAR_PROG,
		QUEST_LIST_SCROLLBAR_END,
		QUEST_LIST_BUTTON_L,
		QUEST_LIST_BUTTON_R,		

		QUEST_LIST_BUTTON_MENU = 2,
	};

public:
	CQuestList ();
	virtual	~CQuestList ();

public:
	void	CreateSubControl ();

public:
	DWORD	GetQuestID ();
	void	SetQuestID ( DWORD dwQuestID );

private:
	void	PageFlip ( BOOL bProg );

public:
	BOOL	IsPROGLIST ();

public:
	bool	LoadQuestList ();

	//virtual	void	SetVisibleSingle ( BOOL bVisible );

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextButton*	CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText );
	CBasicTextButton*	CreateTextButton19 ( char* szButton, UIGUID ControlID , char* szText );

private:
	DWORD	m_dwQuestID;
	int		m_nSelectIndex;
	BOOL	m_bPROG;

private:
	CBasicTextBoxEx*	m_pListTextProg;
	CBasicTextBoxEx*	m_pListTextEnd;
	CBasicScrollBarEx*	m_pListScrollBarProg;
	CBasicScrollBarEx*	m_pListScrollBarEnd;

private:
	CBasicTextButton*	m_pButtonMenuL;
	CBasicTextButton*	m_pButtonMenuR;
};