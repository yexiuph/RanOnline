#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicTextBox;
class	CBasicTextButton;
class	CBasicComboBox;
class	CBasicComboBoxRollOver;
class	CBasicButton;

class	CConftModalWindow : public CUIWindow
{
protected:
	enum
	{
		MODAL_CONFT_OK = ET_CONTROL_NEXT,
		MODAL_CONFT_CANCEL,
		MODAL_CONFT_YES,
		MODAL_CONFT_NO,
		CONFT_MODAL_RANGELIMIT_BUTTON,
		CONFT_MODAL_USABLEHP_COMBO_OPEN,
		CONFT_MODAL_HPSTATEMY_COMBO_OPEN,
		CONFT_MODAL_HPSTATEHIS_COMBO_OPEN,
		CONFT_MODAL_USABLEHP_COMBO_ROLLOVER,
		CONFT_MODAL_HPSTATEMY_COMBO_ROLLOVER,
		CONFT_MODAL_HPSTATEHIS_COMBO_ROLLOVER
	};

public:
	CConftModalWindow ();
	virtual	~CConftModalWindow ();

public:
	void	CreateBaseModal ( char* szWidowKeyword );
	void	CreateSubControl ();

protected:
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetCallerID ( UIGUID ControlID )		{ m_CallerID = ControlID; }
	UIGUID	GetCallerID ()							{ return m_CallerID; }

private:
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	void	LoadComboData ();

public:
	void	LoadDefaultCondition ();

	void	LoadUsableHP ();	
	void	LoadRangeLimit ();

	void	LoadHPStateMy ();
	void	LoadHPStateHis ();

public:
	void	DoModal ( CString strText, UIGUID CallerID, CString strMyName, CString strHisName );

protected:
	void	ResizeControl ( char* szWindowKeyword );

private:
    UIGUID	m_CallerID;
	BOOL	m_bReqModal;

private:
	CBasicTextBox*	m_pTextBox;

private:
	CBasicTextButton*	m_pOK;
	CBasicTextButton*	m_pCANCEL;
	CBasicTextButton*	m_pYES;
	CBasicTextButton*	m_pNO;

	CBasicButton*		m_pRangeLimitButton;

	CBasicComboBox*			m_pComboBoxUsableHPOpen;
	CBasicComboBoxRollOver*	m_pComboBoxUsableHPRollOver;

	CBasicComboBox*			m_pComboBoxHPStateMyOpen;	
	CBasicComboBoxRollOver*	m_pComboBoxHPStateMyRollOver;

	CBasicComboBox*			m_pComboBoxHPStateHisOpen;	
	CBasicComboBoxRollOver*	m_pComboBoxHPStateHisRollOver;

	CBasicTextBox*	m_pMyTextBox;
	CBasicTextBox*	m_pHisTextBox;

private:
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

};

void DoConftModal ( CString strText, UIGUID CallerID, CString strMyName, CString strHisName );