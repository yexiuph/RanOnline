#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

//	NOTE
//		사용자 정의 메시지
const DWORD UIMSG_MOUSEIN_BUTTON = UIMSG_USER1;
const DWORD UIMSG_LEVEL_UP = UIMSG_USER2;


class	CSkillImage;
class	CBasicButton;
class	CBasicTextBox;

class	CSkillSlot : public CUIGroup
{
protected:
	enum
	{
		SKILL_SLOT_LEVEL_UP = NO_ID + 1,
		SKILL_SLOT_IMAGE,
	};

public:
	CSkillSlot ();
	virtual	~CSkillSlot ();

public:
	void	CreateSubControl ( SNATIVEID sNativeID );

public:
	void		SetNativeSkillID ( SNATIVEID sNativeID )			{ m_sNativeID = sNativeID; }
	SNATIVEID	GetNativeSkillID ()									{ return m_sNativeID; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	CUIControl*		CreateControl ( char* szControl );
	CSkillImage*	CreateSkillImage ();
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, char* szMouseOver, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

private:
	CSkillImage*	m_pSkillImage;
	CBasicButton*	m_pSkillUpButton;
	CBasicTextBox*	m_pTextBox;
	CUIControl*		m_pNotLearnImage;

private:
	WORD		m_wLevel;

	CString		m_strLine1;
	CString		m_strLine2;

private:
	SNATIVEID		m_sNativeID;

	BOOL	m_bLearnSkill;
	BOOL	m_bLEVELUP_ENABLE;
};