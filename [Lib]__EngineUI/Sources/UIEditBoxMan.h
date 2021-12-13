#pragma	once

#include "UIGroup.h"

class	CUIEditBox;
class	CD3DFontPar;

class CUIEditBoxMan : public CUIGroup
{
private:
static	const	int		nDEFAULT_TABINDEX;

public:
	CUIEditBoxMan ();
	virtual	~CUIEditBoxMan ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	Init ();

	void	BeginEdit ();	
	void	EndEdit ();

	void	ClearEdit ( UIGUID ControlID );

	bool	IsMODE_NATIVE ();
	void	DoMODE_TOGGLE ();

	void	GoNextTab ();

	CString	GetEditString ();
	int		GetEditLength ();

	CString	GetEditString ( UIGUID ControlID );
	int		GetEditLength ( UIGUID ControlID );

	void	SetEditString ( UIGUID ControlID, const CString& strInput );

	CUIEditBox*	GetEditBox ( const UIGUID& cID );

	void	SetVisibleEdit ( const UIGUID& cID, const BOOL& bVisible );

	void	SetHide ( UIGUID ControlID, BOOL bHide );
	BOOL	IsHide ( UIGUID ControlID );
	
	void	CreateEditBox ( UIGUID WndID, char* szEditBox, char* szCarrat,
							BOOL bCarratUseRender, DWORD dwDiffuse, CD3DFontPar* pFont, int nLIMITCHAR );

	UIGUID GetBeginEditBox(); // Note : 현재 에디터중인 에디터 박스를 얻는다.

	void DisableKeyInput();
	void UsableKeyInput();

private:
	BOOL	RegisterControl ( CUIControl* pNewControl );

	BOOL	FindTabIndexControl ();
	void	SetEditBox ( CUIEditBox* pEditBox )		{ m_pEditBox = pEditBox; }

private:
	int		m_nCurTabIndex;
	CUIEditBox*	m_pEditBox;
};