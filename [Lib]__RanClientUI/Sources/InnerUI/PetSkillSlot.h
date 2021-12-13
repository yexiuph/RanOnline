#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

class	CPetSkillImage;
class	CBasicTextBox;

class	CPetSkillSlot : public CUIGroup
{
protected:
	enum
	{
		SKILL_SLOT_IMAGE = NO_ID + 1
	};

public:
	CPetSkillSlot ();
	virtual	~CPetSkillSlot ();

public:
	void	CreateSubControl ( SNATIVEID sNativeID );

public:
	void		SetNativeSkillID ( SNATIVEID sNativeID )			{ m_sNativeID = sNativeID; }
	SNATIVEID	GetNativeSkillID ()									{ return m_sNativeID; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	CUIControl*		CreateControl ( char* szControl );
	CPetSkillImage*	CreateSkillImage ();
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

private:
	CPetSkillImage*	m_pSkillImage;
	CBasicTextBox*	m_pTextBox;
	CUIControl*		m_pNotLearnImage;

	CString		m_strLine1;
	SNATIVEID	m_sNativeID;
	BOOL		m_bLearnSkill;
};