#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CBasicTextButton;

class CInBoxWindow : public CUIGroup
{
	enum
	{
		INBOX_LIST_TEXTBOX = NO_ID + 1,
		INBOX_LIST_SCROLLBAR,

		INBOX_OPEN_BUTTON,
		INBOX_DEL_BUTTON,
		INBOX_RELOAD_BUTTON
	};

public:
	CInBoxWindow(void);
	virtual ~CInBoxWindow(void);

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );

protected:
	CUIControl * CreateControl( char* szControl, const UIGUID& cID = NO_ID );
	CBasicTextButton * CreateTextButton14( char* szButton, UIGUID ControlID , char* szText );

private:
	CBasicTextBoxEx * m_pListText;
	CBasicScrollBarEx * m_pListScrollBar;

	CBasicTextButton * m_pOpenButton;
	CBasicTextButton * m_pDelButton;
	CBasicTextButton * m_pReloadButton;
};