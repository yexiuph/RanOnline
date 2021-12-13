#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicButton;
class	CBasicTextBox;
class	CSelectCharacterInfoPage;
class	CBasicTextButton;
struct	SCHARINFO_LOBBY;

const DWORD UIMSG_MOUSEIN_SELECT_CHARACTERPAGE = UIMSG_USER1;

class CSelectCharacterPage : public CUIGroup
{
private:
	enum
	{
		SELECT_CHARACTER_START = NO_ID + 1,
		SELECT_CHARACTER_CANCEL,
		SELECT_CHARACTER_LEFT_BUTTON,
		SELECT_CHARACTER_LEFT_BUTTON_FLIP,
		SELECT_CHARACTER_RIGHT_BUTTON,
		SELECT_CHARACTER_RIGHT_BUTTON_FLIP,
	};

public:
	CSelectCharacterPage ();
	virtual	~CSelectCharacterPage ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	BOOL	SndGameJoin ();

public:
	void	ResetAll ();
	void    SetLeftButtonState(BOOL bState);
	void    SetRightButtonState(BOOL bState);
	void	SetPageNum(int nPageNum);
	int		GetPageNum();

private:
	CSelectCharacterInfoPage* m_pSelectCharacterPage;

protected:
	CBasicTextBox* m_pPageNumTextBox;
	CBasicButton* m_pButtonLeftOn;
	CBasicButton* m_pButtonLeftOff;
	CBasicButton* m_pButtonRightOn;
	CBasicButton* m_pButtonRightOff;

	CBasicTextButton* m_pStartButton;
};