#pragma once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicTextBox;
class	CBasicTextButton;

enum EMMAPCHECK_TYPE
{
	EMMAPCHECK_LEVELUP   = 0,		// ���������� �������� ���
	EMMAPCHECK_ENTRYFAIL = 1,		// ���� ���� ���з� �������� ���
	EMMAPCHECK_LIMITTIME = 2,		// �ð� �������� �������� ���
};


class CMapRequireCheck : public CUIWindow
{
private:
	static const float fDEFAULT_TIME_LEFT;
	enum
	{
		MAP_REQUIRE_CHECK_BUTTON = ET_CONTROL_NEXT
	};
public:
	CMapRequireCheck(void);
	~CMapRequireCheck(void);

	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	virtual void SetVisibleSingle ( BOOL bVisible );
	void SetMapCheckType( EMMAPCHECK_TYPE emMapCheckType ) { m_emMapCheckType = emMapCheckType; }

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pButton;
	float				m_fLeftTime;
	EMMAPCHECK_TYPE		m_emMapCheckType;

};
