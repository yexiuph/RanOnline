#pragma	once

#include "UIGroup.h"
#include "BasicTreeNode.h"
#include <string>
                                                                       
class	CBasicTreeNode;
class	CD3DFontPar;

class	CBasicTree : public CUIGroup
{
protected:
	enum
	{
		TREENODE_BASE = NO_ID + 1000,
		TREENODE_SIZE = 1000,
		TREENODE_END = TREENODE_BASE + TREENODE_SIZE
	};

private:
	static const float fX_GAP;
	static const float fY_GAP;

public:
	CBasicTree ();
	virtual	~CBasicTree ();

protected:
	void	InsertNewNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode );

public:
	CBasicTreeNode*		GetSelectedNode ()			{ return m_pSelectedNode; }
	CBasicTreeNode*		GetRootNode ();

	CBasicTreeNode*		GetPrevSiblingNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode );
	CBasicTreeNode*		GetNextSiblingNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode );

public:
	void	SetUseLabelOverColor ( BOOL bUse )		{ m_bUseLabelOverColor = bUse; }
	void	SetLabelOverColor ( D3DCOLOR dwColor )	{ m_dwLabelOverColor = dwColor; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	
	//virtual HRESULT InvalidateDeviceObjects ();

public:
	void	UpdateNodePos ( );
	void	UpdateNodePos ( const CBasicTreeNode::TREENODE_LIST& list, D3DXVECTOR2& ref_vRenderPos );

public:
	int		GetLinePerOneView ()				{ return m_nLinePerOneView; }
	void	SetCurLine ( int nCurLine )			{ m_nStartIndex = nCurLine; }
	int		GetTotalLine ()						{ return m_nRenderCount; }

private:
	void	CalcMaxLine ();

private:	
	int		m_nStartIndex;

	BOOL	m_bRender;
	int		m_nRenderCount;
	int		m_nLinePerOneView;

public:
	void	SetFont ( CD3DFontPar* pFont );

private:
	CD3DFontPar*		m_pFont;

	BOOL		m_bUseLabelOverColor;
	D3DCOLOR	m_dwLabelOverColor;

private:	
	CBasicTreeNode*		m_pSelectedNode;
	CBasicTreeNode::TREENODE_LIST m_ChildList;
};