#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLSkill.h"

class	CPetSkillSlot;
class	CBasicScrollBarEx;

class	CPetSkillPage : public CUIGroup
{
protected:
static	const	int		nSTARTLINE;
static	const	int		nOUTOFRANGE;

protected:
	enum
	{
		nMAX_ONE_VIEW_SLOT = 4
	};

	enum
	{
		SKILL_SLOT0 = NO_ID + 1,
		SKILL_SLOT1,
		SKILL_SLOT2,
		SKILL_SLOT3,
		SKILL_SLOT_END = SKILL_SLOT0 + GLSkillMan::MAX_CLASSSKILL,
	};

public:
	CPetSkillPage ();
	virtual	~CPetSkillPage ();

public:
	void	CreateSubControl ( SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount );	

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	void	ResetAllSkillSlotRender ( int nTotal );
	void	SetSkillSlotRender ( int nStartIndex, int nTotal );

public:
	int			GetSkillIndex ()						{ return m_nSlotIndex; }
	SNATIVEID	GetSkillID ( int nIndex );

protected:
	CUIControl*	CreateControl ( const char* szControl );
	CPetSkillSlot*	CreateSkillSlot ( CString strKeyword, UIGUID ControlID, SNATIVEID sNativeID );

private:
	CPetSkillSlot*	m_pSkillSlotArray[GLSkillMan::MAX_CLASSSKILL];
	CUIControl*	m_pSkillSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

private:
	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nSlotIndex;
	int		m_nCurPos;
};