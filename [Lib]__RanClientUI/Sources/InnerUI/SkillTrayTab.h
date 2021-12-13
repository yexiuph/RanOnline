#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "SkillTrayTab.h"

const INT MAX_TAB_INDEX = 3;
const INT QUICK_SKILL_SLOT_MAX = 10;

class CBasicSkillTray;
class CBasicTextButton;

class CSkillTrayTab : public CUIGroup
{
	static INT NEXT_INDEX[MAX_TAB_INDEX];

public:
	enum
	{
		BASIC_QUICK_SKILL_TRAY_F2 = NO_ID + 1,
		BASIC_QUICK_SKILL_TRAY_F3,
		BASIC_QUICK_SKILL_TRAY_F4,

		BASIC_TEXT_BUTTON_F2,
		BASIC_TEXT_BUTTON_F3,
		BASIC_TEXT_BUTTON_F4,

		BASIC_LEVEL_DISPLAY,

		QUICK_SKILL_TRAY_CLOSE_BUTTON
	};

	enum
	{
		TABBUTTON_FOLDGROUP_ID = 1
	};

public:
	void CreateSubControl();

	void SetTabIndex( INT nIndex )			{ GASSERT( 0 <= nIndex && nIndex <= MAX_TAB_INDEX ); m_nTabIndex = nIndex; }
	INT GetTabIndex()						{ return m_nTabIndex; }

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	void	SetShotcutText ( DWORD nID, CString& strTemp );

public:
	CSkillTrayTab(void);
	virtual ~CSkillTrayTab(void);

protected:
	CBasicSkillTray * m_pSkillTray[MAX_TAB_INDEX];
	CBasicTextButton * m_pTextButton[MAX_TAB_INDEX];

	INT m_nTabIndex;
	INT m_nCount;
};
