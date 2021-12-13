#pragma	once

#include "UIWindowEx.h"

class	CItemBankPage;
class	CBasicTextBox;
class	CBasicTextButton;

class	CItemBankWindow : public CUIWindowEx	
{
protected:
	enum
	{
		ITEMBANK_REFRESH_BUTTON = ET_CONTROL_NEXT,
		ITEMBANK_PAGE,
	};

	CItemBankPage*	m_pPage;

public:
	CItemBankWindow ();
	virtual	~CItemBankWindow ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitItemBank ();
	void	ClearItemBank();

	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
};