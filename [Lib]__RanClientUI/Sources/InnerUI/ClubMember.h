#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLClubMan.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW = UIMSG_USER5;
////////////////////////////////////////////////////////////////////

class CBasicTextBoxEx;
class CBasicScrollBarEx;

class CClubMember : public CUIGroup
{
protected:
	enum
	{
		CLUB_LIST_TEXTBOX = NO_ID + 1,
		CLUB_LIST_SCROLLBAR
	};

public:
	CClubMember();
	virtual	~CClubMember();

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	const DWORD GetClubMemberID();
	const CString GetClubMemberName()				{ return m_strSelectName; }

	void LoadClubMemberList();
	BOOL IsSelectedIndex();

private:
	void LoadClubMember( const CString& strText, const bool bOnline, const DWORD& dwGaeaID );

private:
	int m_nSelectIndex;
	CString	m_strSelectName;
	GLCLUBMEMBER m_sClubMember;

private:
	CBasicTextBoxEx*	m_pListText;
	CBasicScrollBarEx*	m_pListScrollBar;
};