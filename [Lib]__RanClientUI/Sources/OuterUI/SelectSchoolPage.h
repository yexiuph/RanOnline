#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "NewCreateCharEnum.h"

class CBasicTextBox;

class CSelectSchoolPage	: public CUIGroup
{
private:
	enum
	{
		SELECT_SCHOOL_NAME = NO_ID + 1,
		SELECT_SCHOOL_DEC,
		SELECT_SCHOOL_IMAGE0,
		SELECT_SCHOOL_IMAGE1,
		SELECT_SCHOOL_IMAGE2,
		SELECT_SCHOOL_SET_IMAGE0,
		SELECT_SCHOOL_SET_IMAGE1,
		SELECT_SCHOOL_SET_IMAGE2,
	};

	CBasicTextBox*		m_pSchoolNameStatic;	
	CBasicTextBox*		m_pSchoolDec;

	CUIControl*			m_pSelectSchoolImg[MAX_SELSCHOOL];
	CUIControl*			m_pSelectSchoolSetImg[MAX_SELSCHOOL];

	int					m_nSchool;

protected:


public:
	CSelectSchoolPage();
	virtual ~CSelectSchoolPage();


	void	SELECT_SCHOOL_PROCESS( int nSelect );
	
	int		GetSelectSchool() { return m_nSchool; }
	void	SetSelectSchool( int nSchool) { m_nSchool = nSchool; }

public:
	void	CreateSubControl ();
public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
};