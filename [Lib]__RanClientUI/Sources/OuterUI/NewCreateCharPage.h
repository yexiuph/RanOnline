#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"


class CBasicTextButton;
class CBasicButton;
class CBasicTextBox;
class CSelectSchoolPage;
class CSelectSexPage;
class CSelectClassPage;
class CSelectStylePage;

class	CNewCreateCharPage : public CUIGroup
{
private:
	enum
	{
		SELECT_PREV_BUTTON = NO_ID + 1,
		SELECT_NEXT_BUTTON,
		SELECT_LEFT_BUTTON,
		SELECT_RIGHT_BUTTON,		
//		SELECT_STAGE_TITLE,
//		SELECT_STAGE_DEC,

		SELECT_SCHOOL_PAGE,
		SELECT_SEX_PAGE,
		SELECT_CLASS_PAGE,
		SELECT_STYLE_PAGE
	};

private:
	CBasicTextButton*	m_pButtonPrev;
	CBasicTextButton*	m_pButtonNext;
	CBasicButton*		m_pButtonLeft;
	CBasicButton*		m_pButtonRight;

	CUIControl*			m_pDummyImage;

	CBasicTextBox*		m_pStageTitle;
	CBasicTextBox*		m_pStageDec;

	CSelectSchoolPage*  m_pSchoolPage;
	CSelectSexPage*		m_pSexPage;
	CSelectClassPage*	m_pClassPage;
	CSelectStylePage*	m_pStylePage;

	int					m_nStage;

	int					m_nSchool;
	int					m_nSex;
	int					m_nClass;
	int					m_nHair;
	int					m_nFace;

	BOOL m_bUseCancel;

	int					m_nCreatExtream;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	CUIControl*			m_pBackImage;

	CNewCreateCharPage ();
	virtual	~CNewCreateCharPage ();

	void	InitData();
	
	void	CREATE_CHAR_PROCESS();

    void	LEFT_BUTTON_PROCESS( );
	void	RIGHT_BUTTON_PROCESS( );
	void	PREV_STEP( );
	void	NEXT_STEP( );

	int		GetCharIndex();
	
	BOOL	CheckStringName( CString strTemp );
	
	void UseCancelButton()				{ m_bUseCancel = TRUE; }
	void ResetAll();

private:
	HMODULE m_hCheckStrDLL;
	BOOL (_stdcall *m_pCheckString)(CString);


public:
	void	CreateSubControl ();
public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
protected:
	
};

