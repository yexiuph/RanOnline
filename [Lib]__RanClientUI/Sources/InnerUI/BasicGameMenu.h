#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

class CBasicButton;
class CBasicVarTextBox;

#define BASIC_MENU_NUM	12

class	CBasicGameMenu : public CUIGroup
{
protected:
	enum
	{
		MENU_INVENTORY_BUTTON = NO_ID + 1,
		MENU_CHARACTER_BUTTON,
		MENU_SKILL_BUTTON,
		MENU_PARTY_BUTTON,
		MENU_GUILD_BUTTON,
		MENU_QUEST_BUTTON,
		MENU_FRIEND_BUTTON,
		MENU_LARGEMAP_BUTTON,
		MENU_CHATMACRO_BUTTON,
		MENU_ITEMBANK_BUTTON,
		MENU_ITEMSHOP_BUTTON,
		MENU_RUN_BUTTON,
		
		MENU_CLOSE_BUTTON
	};

private:
	int	m_nOverMenu;
	CBasicVarTextBox* m_pInfo;
	CBasicButton* m_pRunButton;
	CString m_ShotcutText[BASIC_MENU_NUM];

public:
    CBasicGameMenu ();
	virtual	~CBasicGameMenu ();

public:
	void CreateSubControl ();
	void SetFlipRunButton( BOOL bRun )			{ m_pRunButton->SetFlip( bRun ); }
	void SetShotcutText ( DWORD nID, CString& strTemp );
	void InitShotCutString();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};