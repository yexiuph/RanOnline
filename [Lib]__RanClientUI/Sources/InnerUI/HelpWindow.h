//	도움말 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2004.02.24]
//			@ 작성

#pragma	once

#include "UIWindowEx.h"
#include "HelpDataMan.h"

class	CD3DFontPar;
class	CBasicTextBox;
class	CBasicScrollBarEx;
class	CBasicTreeEx;
class	CBasicTreeNode;
struct	SHELPNODE;

class CHelpWindow : public CUIWindowEx
{
private:
static	const int	nSTARTLINE;
static	const int	nLIMIT_LINE;

private:
	enum
	{
		HELP_WINDOW_TREE = ET_CONTROL_NEXT,
		HELP_WINDOW_TEXTBOX
	};

public:
	CHelpWindow ();
	virtual	~CHelpWindow ();

public:
	void	CreateSubControl ();

public:
	void	LoadDataFile ();
	void	LoadHelpNode ( CBasicTreeNode* pParent, const HELPNODE_LIST& list );

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTreeEx*	m_pTree;
	CBasicTextBox*	m_pTextBox;
    
	CBasicScrollBarEx*	m_pTreeScroll;
	CBasicScrollBarEx*	m_pTextBoxScroll;
};