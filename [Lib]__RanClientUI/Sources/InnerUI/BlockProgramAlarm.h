//	��ų �̵�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.8]
//			@ �ۼ�
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