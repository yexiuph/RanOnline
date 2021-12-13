//	'ESC' 키 눌렀을때 나오는 메뉴
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.12.4]
//			@ 최초 작성

#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicButton;
class	CBasicTextButton;

class CBasicEscMenu : public CUIGroup
{
protected:
	enum
	{
//		ESC_MENU_CHARACTER_CHANGE = 1,
		ESC_MENU_SERVER_CHANGE = NO_ID + 1,
		ESC_MENU_HWOPTION,
		ESC_MENU_HELP,
		ESC_MENU_CLOSEGAME,
		ESC_MENU_CLOSE,
		ESC_MENU_KEYSETTING

	};

public:
    CBasicEscMenu ();
	virtual	~CBasicEscMenu ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
};