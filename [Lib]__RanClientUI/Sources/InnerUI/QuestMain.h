#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;
class	CBasicScrollBarEx;
class	CItemSlot;
class	CBasicTextButton;
struct	SINVENITEM;
class	CQuestGiftView;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BUTTON_GIVEUP = UIMSG_USER3;
const DWORD UIMSG_BUTTON_GO_QUESTSTEP = UIMSG_USER4;
const DWORD UIMSG_BUTTON_BACK = UIMSG_USER5;
const DWORD UIMSG_BUTTON_COMPLETE = UIMSG_USER6;
////////////////////////////////////////////////////////////////////

class	CQuestMain : public CUIGroup
{
private:
	static	const	float	fANSWER_LINE_INTERVAL;
	static	const	DWORD	dwMINITE_BY_HOUR;

private:
	enum
	{
		QUEST_MAIN_DESC_TEXTBOX = NO_ID + 1,
		QUEST_MAIN_DESC_SCROLLBAR,
		QUEST_MAIN_CONDITION,
		QUEST_MAIN_TEXTBOX,
		QUEST_MAIN_SCROLLBAR,
		QUEST_MAIN_ITEMSLOT,
		QUEST_MAIN_BUTTON_L,
		QUEST_MAIN_BUTTON_M,
		QUEST_MAIN_BUTTON_R,
		QUEST_MAIN_BUTTON_COMPLETE,
		QUEST_MAIN_BUTTON_GIFT,
		QUEST_MAIN_GIFT_VIEW
	};

public:
	CQuestMain ();
	virtual	~CQuestMain ();

public:
	void	CreateSubControl ();

private:
	CItemSlot*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual	void	SetVisibleSingle ( BOOL bVisible );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	void	LoadItem ( SINVENITEM& ref_InvenItem );
	SINVENITEM&	GetItem ( int nPosX );
	void	UnLoadItem ( int nPosX );

public:
	bool	LoadQuestMain ( DWORD dwQuestID );

private:
	CBasicTextBox*		m_pDescText;
	CBasicScrollBarEx*	m_pDescScrollBar;
	CBasicTextBox*		m_pCondition;
	CBasicTextBox*		m_pMainText;
	CBasicScrollBarEx*	m_pMainScrollBar;

	CItemSlot*	m_pItemSlot;

	CBasicTextButton*	m_pButtonL;
	CBasicTextButton*	m_pButtonM;
	CBasicTextButton*	m_pButtonR;

	CBasicTextButton*	m_pButtonCOMPLETE;
	CBasicTextButton*	m_pButtonGIFT;

	DWORD				m_dwQuestID;
	CQuestGiftView*		m_pGiftView;	
};