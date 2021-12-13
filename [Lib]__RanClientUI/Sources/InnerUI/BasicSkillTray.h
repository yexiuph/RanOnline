#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLSkill.h"
#include "SkillTrayTab.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_SKILLSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicQuickSkillSlotEx;
class	CBasicVarTextBox;
struct	SCHARSKILL;

class CBasicSkillTray : public CUIGroup
{
private:
static	const	int	nLBUPSKIP;

protected:
	enum
	{
		QUICK_SKILL_SLOT1 = NO_ID + 1,
		QUICK_SKILL_SLOT2,
		QUICK_SKILL_SLOT3,
		QUICK_SKILL_SLOT4,
		QUICK_SKILL_SLOT5,

		QUICK_SKILL_SLOT6,
		QUICK_SKILL_SLOT7,
		QUICK_SKILL_SLOT8,
		QUICK_SKILL_SLOT9,
		QUICK_SKILL_SLOT0,

		QUICK_SKILL_TRAY_TAB_INFO
	};

public:
	CBasicSkillTray ();
	virtual	~CBasicSkillTray ();

public:
	void	CreateSubControl( INT nTabIndex );

private:
	void	UpdateSkillInfo ( SNATIVEID sNativeID );
	void	LoadSkillInfo ( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	void 	SetShotcutText ( DWORD nID, CString& strTemp );

public:
	CBasicQuickSkillSlotEx*		m_pSlotEx[QUICK_SKILL_SLOT_MAX];
private:	
	CBasicVarTextBox*	m_pSkillInfo;
private:
	int	m_nLBUPSKIP;
};