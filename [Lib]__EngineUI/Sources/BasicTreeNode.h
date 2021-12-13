#pragma	once

#include <list>
#include "UIGroup.h"
#include "BasicTextBox.h"

class	CBasicButton;
//class	CBasicTextBox;

class	CBasicTreeNode : public CUIGroup
{
public:
	typedef	std::list<CBasicTreeNode*>	TREENODE_LIST;
	typedef	TREENODE_LIST::iterator			TREENODE_LIST_ITER;
	typedef	TREENODE_LIST::const_iterator	TREENODE_LIST_CITER;

protected:
	enum
	{
		TREENODE_EXPAND_BUTTON = NO_ID + 1,
		TREENODE_TEXTBOX,
	};

public:
	CBasicTreeNode ();
	virtual	~CBasicTreeNode ();

public:
	BOOL IsBranchVisible ()								{ return m_bBranchVisible; }
	void SetBranchVisible ( BOOL bBranchVisible );

	void CreateButton ( char* szKeyword, char* szFlipKeyword );
	void CreateLabel ( char* szKeyword );

	void SetParentNode ( CBasicTreeNode*	pParent )	{ m_pParentNode = pParent; }
	CBasicTreeNode*	GetParentNode ()					{ return m_pParentNode; }

	void InsertChildNode ( CBasicTreeNode* pChild );
	CBasicTreeNode* GetChildNode ();

private:
	void SetVisibleChild ( const TREENODE_LIST& list, BOOL bVisible );

public:
	void SetNodeData( DWORD_PTR dwData )				{ m_pLabel->SetTextData( 0, (DWORD)dwData ); }
	DWORD_PTR GetNodeData()								{ return m_pLabel->GetTextData( 0 ); }

public:
	void SetLabel ( CString strLabelName )				{ m_pLabel->SetText ( strLabelName ); }
	void SetLabelOverColor ( D3DCOLOR dwColor )			{ m_pLabel->SetOverColor ( 0, dwColor ); }
	void SetLabelUseOverColor( BOOL bUseOverColor )		{ m_pLabel->SetUseOverColor( bUseOverColor ); }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	TREENODE_LIST& GetChildList()						{ return m_ChildList; }

private:
	BOOL	m_bBranchVisible;
	CBasicButton*		m_pButton;
	CBasicTextBox*		m_pLabel;

	CBasicTreeNode*		m_pParentNode;
	TREENODE_LIST		m_ChildList;
};