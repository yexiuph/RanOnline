//	스킬 이동
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.8]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;
class	CBasicTextButton;

const DWORD UIMSG_MOUSEIN_ALARMBUTTON = UIMSG_USER1;

class CBlockProgramAlarm : public CUIGroup
{
private:
	enum
	{
		ALARM_OKBUTTON = NO_ID + 1,
	};

public:
	CBlockProgramAlarm ();
	virtual	~CBlockProgramAlarm ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	DoALARM ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pAlarmOKButton;
};