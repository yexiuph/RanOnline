#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicTextBox;
class	CBasicButton;

class	CUIEditBox;
class	CBasicTextButton;

class	CClubMake : public CUIWindow
{
	static	const int nLIMIT_CHAR;
	static	const char BLANK_SYMBOL;

protected:
	enum
	{
		CLUB_MAKE_OK = ET_CONTROL_NEXT,
		CLUB_MAKE_CANCEL,
		CLUB_MAKE_EDIT
	};

public:
	CClubMake ();
	virtual	~CClubMake ();

public:
	void	Init ( const DWORD& dwNpcID );

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
	DWORD		m_dwNpcID;
};
