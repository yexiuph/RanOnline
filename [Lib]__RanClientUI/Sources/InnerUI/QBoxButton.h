#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CQBoxButton : public CUIGroup
{
public:
	CQBoxButton(void);
	~CQBoxButton(void);

protected:
	enum
	{
		QBOX_ON_OFF_BUTTON = NO_ID + 1,
	};

public:
	void	CreateSubControl ();
	bool	GetQBoxEnable() { return m_bQBoxEnable; }
	void	SetQBoxEnable( bool bQBoxEnable );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicButton* m_pQBoxButton;
	CUIControl*	  m_pAlarmBlink;
	bool		  m_bQBoxEnable;
	bool		  m_bQBoxAlarm;
	float		  m_fBLINK_TIME;
};
