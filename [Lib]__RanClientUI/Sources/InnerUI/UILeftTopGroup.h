//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.21]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSE_IN = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicPotionTray;
class	CBasicQuickPotionSlot;
class	CBasicButton;

class	CUILeftTopGroup : public CUIGroup
{
private:
	enum
	{
		BASIC_QUICK_POTION_SLOT = NO_ID + 1,
		QUICK_POTION_TRAY_OPEN_BUTTON,
		BASIC_QUICK_POTION_TRAY,
		BASIC_LEVEL_DISPLAY
	};

public:
	CUILeftTopGroup ();
	virtual	~CUILeftTopGroup ();

public:
	void	CreateSubControl ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	void	SetShotcutText ( DWORD nID, CString& strTemp );

private:
	CBasicButton*		m_pPotionTrayOpenButton;
	CBasicPotionTray*	m_pPotionTray;
	CBasicQuickPotionSlot*	m_pBasicQuickSlot;
};