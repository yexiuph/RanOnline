#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CSelectCharacterCard;
class CBasicTextButton;
struct SCHARINFO_LOBBY;

const DWORD UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE = UIMSG_USER1;

class CSelectCharacterLeftPage : public CUIGroup
{
private:
	enum
	{
		SELECT_CHARACTER_NEW = NO_ID + 1,
		SELECT_CHARACTER_DELETE,
	};

public:
	CSelectCharacterLeftPage ();
	virtual	~CSelectCharacterLeftPage ();

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	SCHARINFO_LOBBY * GetDelCharInfo()				{ return m_pDelCharInfo; }

private:
	SCHARINFO_LOBBY* m_pDelCharInfo;
	CSelectCharacterCard* m_pSelectCharacterCard;
	CBasicTextButton* m_pNewButton;
};