#pragma	once

#include "../[Lib]__EngineUI/Sources/UIModal.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"

enum
{
	OKCANCEL,
	YESNO,
	OK,
	CANCEL,
	EDITBOX,
	EDITBOX_PETNAME,
	EDITBOX_NUMBER,
	EDITBOX_MONEY,
	MODAL_EDITBOX_LOTTERY,
	MODAL_FILE_NAME,
	NO_CONTROL
};

enum
{	
	MODAL_INFOMATION,
	MODAL_QUESTION,
	MODAL_WARNING,
	MODAL_ERROR,
	MODAL_CRITICAL,
	MODAL_INPUT,
	MODAL_NOTITLE
};

class	CBasicTextBox;
class	CBasicTextButton;
class	CUIEditBox;
class	CBasicLineBox;

class	CModalWindow : public CUIModal
{
	static	const int nLIMIT_CHAR;
	static	const int nMONEY_UNIT;
	static	const CString strMONEY_SYMBOL;
	static	const int nLIMIT_MONEY_CHAR;
	static	const int nLIMIT_NUMBER_CHAR;

	static	UIGUID	m_nMODAL_NEWID;

private:
	UIGUID	m_PrevPageID;
	bool	m_bUseInner;

	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pOK;
	CBasicTextButton*	m_pCANCEL;
	CBasicTextButton*	m_pYES;
	CBasicTextButton*	m_pNO;
	CUIEditBox*			m_pEditBox;
	CBasicLineBox*		m_pEditBoxBack;

	CUIControl*	m_p1BUTTON;
	CUIControl*	m_p2BUTTONLEFT;
	CUIControl*	m_p2BUTTONRIGHT;

	int	m_nModalType;
	int	m_nData1;
	int	m_nData2;

public:
	bool m_bTEST_MULTI;

protected:
	enum
	{
		MODAL_OK = ET_CONTROL_NEXT,
		MODAL_CANCEL,
		MODAL_YES,
		MODAL_NO,
		MODAL_EDITBOX,
	};

public:
	CModalWindow ();
	virtual	~CModalWindow ();

public:
	void	CreateBaseModal ( char* szWidowKeyword );
	void	CreateSubControl ();

protected:
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetPrevPageID ( UIGUID ControlID )		{ m_PrevPageID = ControlID; }
	UIGUID	GetPrevPageID ()						{ return m_PrevPageID; }

	void	SetHide ( BOOL bHide )					{ m_pEditBox->SetHide( bHide ); }
	void	SetUseInner ( bool bUseInner )			{ m_bUseInner = bUseInner; }

	CString	GetEditString ()						{ return m_pEditBox->GetEditString(); }

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	DoModal ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID );
//	void	DoMODAL ( const CString& strMessage, const CString& strTitle, const int nType, const UIGUID CallerID );

public:
	void	SetModalData ( int nData1, int nData2 );
	void	GetModalData ( int* pnData1, int* pnData2 );

private:
	void	ReArrangeControl ( int nModalType );

protected:
	void	ResizeControl ( char* szWindowKeyword );
	bool	IsEditBoxType();

public:
	static int MAKE_MODAL_NEWID()					{ return m_nMODAL_NEWID++; }
};

void SetModalData ( int nData1, int nData2 );
void DoModal ( const CString& strText, int nModalTitle = MODAL_NOTITLE, int nModalType = OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE );
void DoModalOuter ( const CString& strText, int nModalTitle = MODAL_NOTITLE, int nModalType = OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE );