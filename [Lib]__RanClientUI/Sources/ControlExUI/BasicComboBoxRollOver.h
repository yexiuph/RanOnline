#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UI_MSG_COMBOBOX_ROLLOVER_SCROLL = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicLineBox;

class	CBasicComboBoxRollOver : public CUIGroup
{
private:
static	const int nSTARTLINE;
static	const int nLIMITLINE;
static	const D3DCOLOR	dwMOUSEOVERCOLOR;
static	const int nNOT_SELECTED;

protected:
	enum
	{
		COMBOBOX_ROLLOVER_TEXT_BOX = 1,
		BASIC_COMBOBOX_ROLLOVER_SCROLLBAR
	};

public:
	CBasicComboBoxRollOver ();
	virtual	~CBasicComboBoxRollOver ();

public:
	void	CreateBaseComboBoxRollOver ( char* szComboBoxControl, bool bSelectInit = TRUE );

private:
	void	CreateComboBoxRollOverImage ( bool bSelectInit );
	void	CreateSelectionImage ();
	void	CreateScrollBar ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetScrollPercent ( float fPercent );

	int AddText( CString str, bool bVisibleScroll = FALSE, bool bOneLine = FALSE );
	void ClearText();

	void SetSelectIndex ( int nIndex );
	int GetSelectIndex ()					{ return m_nSelectIndex; }
	CString	GetSelectText ( int nIndex );

	void SetTextData( int nIndex, DWORD dwData );
	DWORD GetTextData( int nIndex );

	void SetVisibleThumb( bool bVisible );

private:
	int	m_nSelectIndex;

	CBasicLineBox* m_pLineBox;
	CBasicTextBoxEx* m_pTextBox;
	CBasicScrollBarEx* m_pScrollBar;
};