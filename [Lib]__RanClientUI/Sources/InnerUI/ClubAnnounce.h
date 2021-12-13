#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicTextBox;
class CUIMultiEditBox;

class CClubAnnounce : public CUIGroup
{
protected:
	enum
	{
		CLUB_ANNOUNCE_TEXTBOX = NO_ID + 1,
		CLUB_ANNOUNCE_EDITBOX_L01,
		CLUB_ANNOUNCE_EDITBOX_L02,
		CLUB_ANNOUNCE_EDITBOX_L03,
		CLUB_ANNOUNCE_EDITBOX_L04,
		CLUB_ANNOUNCE_EDITBOX_L05,
		CLUB_ANNOUNCE_EDITBOX_L06,
		CLUB_ANNOUNCE_EDITBOX_L07,
		CLUB_ANNOUNCE_EDITBOX_L08,
		CLUB_ANNOUNCE_EDITBOX_L09,
		CLUB_ANNOUNCE_EDITBOX_L10
	};

public:
	CClubAnnounce();
	virtual	~CClubAnnounce();

public:
	void CreateSubControl();

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void SetAnnouncement();
	void BeginEdit();
	void EndEdit( BOOL bOK );
	void ClearText();

private:
	CBasicTextBox *	m_pTextBox;
	CUIMultiEditBox * m_pMultiEditBox;
};