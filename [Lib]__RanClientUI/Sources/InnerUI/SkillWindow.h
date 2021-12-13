#pragma	once

#include "UIWindowEx.h"
#include "GLSkill.h"

class	CBasicTextButton;
class	CSkillPage;
class	CBasicTextBox;
struct	SCHARSKILL;

class	CSkillWindow : public CUIWindowEx
{
protected:
	enum
	{
		SKILL_WINDOW_PAGE_BUTTON0 = ET_CONTROL_NEXT,
		SKILL_WINDOW_PAGE_BUTTON1,
		SKILL_WINDOW_PAGE_BUTTON2,
		SKILL_WINDOW_PAGE_BUTTON3,
		SKILL_WINDOW_PAGE0,
		SKILL_WINDOW_PAGE1,
		SKILL_WINDOW_PAGE2,
		SKILL_WINDOW_PAGE3,
	};

	enum
	{
		nMAXPAGE = 4,
	};

public:
	CSkillWindow ();
	virtual	~CSkillWindow ();

public:
	void	CreateSubControl ();

private:
	void	SetVisiblePage ( int nPage );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextButton*	CreateTextButton ( const char* szButton, UIGUID ControlID, const char* szText );	
	CSkillPage*			CreateSkillPage ( UIGUID ControlID, SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount );

private:
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

private:
	void	GetSkillIndexRange ( EMCHARCLASS emCharClass, int* pnBeginIndex, int* pnEndIndex );
	void	SortSkill ( EMCHARCLASS emCharClass, SNATIVEID sNativeIDArrayAll[EMSKILL_PC_CLASSNUM][GLSkillMan::MAX_CLASSSKILL], int nSkillCountAll[EMSKILL_PC_CLASSNUM] );

private:
	CSkillPage*			m_pPage[nMAXPAGE];
	CBasicTextButton*	m_pPageButton[nMAXPAGE];

private:
	DWORD				m_dwSkillPointBACK;
	CBasicTextBox*		m_pRestPoint;
};