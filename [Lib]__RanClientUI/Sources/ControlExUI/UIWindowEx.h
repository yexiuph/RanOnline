#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicButton;
class	CBasicTextBox;

class CUIWindowEx : public CUIWindow
{
public:
	CUIWindowEx ();
	virtual	~CUIWindowEx ();

public:
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void	CreateBaseWindowBlack ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowLightGray ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowMiniParty ( char* szWindowKeyword, char* szWindowName );

protected:
	CUIControl*		CreateControl ( const char* szControl, const UIGUID& cID = NO_ID );
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

protected:
	void	ResizeControl ( char* szWindowKeyword );
};
