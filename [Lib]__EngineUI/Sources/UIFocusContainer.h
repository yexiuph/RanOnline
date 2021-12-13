//	UIControlNodeFocusMan 기반 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.13] - 포커스용으로 제작.

#pragma	once

#include "UIDataType.h"
#include "UIControl.h"

class	CUIFocusContainer
{
public:
	struct SUICONTROL
	{
		UIGUID cID;
		CUIControl * pUIControl;
	};

public:
	typedef std::list<SUICONTROL>					UICONTROL_FOCUSLIST;
	typedef	UICONTROL_FOCUSLIST::iterator			UICONTROL_FOCUSLIST_ITER;
	typedef	UICONTROL_FOCUSLIST::const_iterator		UICONTROL_FOCUSLIST_CITER;
	typedef	UICONTROL_FOCUSLIST::reverse_iterator	UICONTROL_FOCUSLIST_RITER;

public:
	CUIFocusContainer ();
	virtual	~CUIFocusContainer ();

public:
	bool empty()		{ return m_UIFocusList.empty (); }
	int size()			{ return static_cast<int>(m_UIFocusList.size()); }

	void RemoveAll()	{ m_UIFocusList.clear(); }
	bool EraseControl( UIGUID cID );

public:
	CUIControl * IsInserted( UIGUID cID );

	bool InsertHead( UIGUID cID, CUIControl * pUIContorl );
	bool InsertTail( UIGUID cID, CUIControl * pUIContorl );
	bool InsertAfter( UIGUID Where_cID, UIGUID cID, CUIControl * pUIContorl );

	CUIControl * GetTail();

public:	
	CUIControl * GetFocusControl();
	const UICONTROL_FOCUSLIST& GetFocusList() const { return m_UIFocusList; }

private:
	UICONTROL_FOCUSLIST	m_UIFocusList;
};