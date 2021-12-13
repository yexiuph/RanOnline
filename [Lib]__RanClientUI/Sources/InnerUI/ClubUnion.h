#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBoxEx;
class CBasicScrollBarEx;

class CClubUnion : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubUnion();
	virtual	~CClubUnion();

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	const DWORD GetUnionMemberID();
	const CString GetUnionMemberName()				{ return m_strSelectName; }

	BOOL IsSelectedIndex();
	void LoadUnionMemberList();

protected:
	int m_nSelectIndex;
	CString	m_strSelectName;

private:
	CBasicTextBoxEx*	m_pTextBox;
	CBasicScrollBarEx*	m_pScrollBarEx;
};