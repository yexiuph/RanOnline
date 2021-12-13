#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicTextBox;
class	CBasicButton;

class	CUIEditBox;
class	CBasicTextButton;

class	CPrivateMarketMake : public CUIWindow
{
private:
static	const int nLIMIT_CHAR;
static	const char BLANK_SYMBOL;

protected:
	enum
	{
		PRIVATE_MARKET_MAKE_OK = ET_CONTROL_NEXT,
		PRIVATE_MARKET_MAKE_CANCEL,
		PRIVATE_MARKET_MAKE_EDIT
	};

public:
	CPrivateMarketMake ();
	virtual	~CPrivateMarketMake ();

public:
	void	Init ();

public:	
	void	CreateSubControl ();

public:
	const CString& GetTITLE ();

private:
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	SetVisibleSingle ( BOOL bVisible );

private:
	CUIEditBox*	m_pEditBox;
};
