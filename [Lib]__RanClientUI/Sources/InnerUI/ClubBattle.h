#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBoxEx;
class CBasicScrollBarEx;

class CClubBattle : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubBattle();
	virtual	~CClubBattle();

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	const DWORD GetBattleClubID();
	const CString GetBattleClubName()				{ return m_strSelectName; }
	const bool GetIsAlliance()						{ return m_bSelAlliance; }

	BOOL IsSelectedIndex();
	void LoadClubBattleList();
	void ShowClubBattleInfo( int nIndex );

protected:
	int m_nSelectIndex;
	CString	m_strSelectName;
	bool m_bSelAlliance;

private:
	CBasicTextBoxEx*	m_pTextBox;
	CBasicScrollBarEx*	m_pScrollBarEx;
};