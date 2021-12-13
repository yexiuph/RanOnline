#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;
class	CBasicTextButton;

class CMapMoveDisplay : public CUIGroup
{
private:
	enum
	{
		MAPMOVE_OKBUTTON = NO_ID + 1,
	};

public:
	CMapMoveDisplay ();
	virtual	~CMapMoveDisplay ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	DoMAPMOVE ();

public:
	void	SetMapName ( CString strName );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pMoveOKButton;
};