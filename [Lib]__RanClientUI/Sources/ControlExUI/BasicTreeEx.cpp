#include "pch.h"
#include "BasicTreeEx.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicTreeEx::CBasicTreeEx () :
	m_NodeWndID ( TREENODE_BASE )
{
}

CBasicTreeEx::~CBasicTreeEx ()
{
}

CBasicTreeNode* CBasicTreeEx::InsertNode ( CBasicTreeNode* pParent, CString strNodeName )
{
	CBasicTreeNode* pNewNode = new CBasicTreeNode;
	pNewNode->CreateSub ( this, "BASIC_TREE_NODE", UI_FLAG_DEFAULT, m_NodeWndID++ );
	pNewNode->CreateLabel ( "BASIC_TREE_LABEL" );
	pNewNode->CreateButton ( "BASIC_TREE_LABEL_BUTTON", "BASIC_TREE_LABEL_BUTTON_F" );
	pNewNode->SetLabel ( strNodeName );	
	pNewNode->SetLabelOverColor ( NS_UITEXTCOLOR::PRIVATE );
	pNewNode->SetVisibleSingle ( FALSE );
	RegisterControl ( pNewNode );

	InsertNewNode ( pParent, pNewNode );

	return pNewNode;
}