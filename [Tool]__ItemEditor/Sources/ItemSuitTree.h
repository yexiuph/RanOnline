#pragma once
#include "afxcmn.h"
#include "GLItemMan.h"

// CItemSuitTree 대화 상자입니다.
class	CsheetWithTab;
class CItemSuitTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemSuitTree)

public:
	CItemSuitTree( LOGFONT logfont );
	virtual ~CItemSuitTree();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMSUITTREE_DIALOG };

private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	HTREEITEM		m_TreeRoot;
	CFont*			m_pFont;
	HTREEITEM		m_TreeItemList[GLItemMan::MAX_MID][GLItemMan::MAX_SID];	

public:
	BOOL	UpdateTree ();
	void	UpdateName ();
	void	SearchItemSelect( BYTE dwType, char *szItemName );
	void	SetSelectIndex( int iSelectIndex );

	std::vector<SSearchItemList> m_vecSearchItemList;

private:
	BOOL	UpdateItem ( SITEM* pItem, HTREEITEM hMainItem );
	const char*	GetFormatName ( SITEM* pItem );

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	CleanAllItem ();
	void	SetReadMode( CMenu* pMenu );

private:
	PITEMNODE		m_pItemNode;

	BOOL			m_bDummyHasItem;
	PITEMNODE		m_pDummyItemNode;

private:
	BOOL	AddItem ();
	BOOL	AddItemMain( HTREEITEM hMainItem );
	BOOL	AddItemSub ();    

	BOOL	DelItem ();

	BOOL	CopyItem ();
	BOOL	PasteItemNew ();
	BOOL	PasteItemValue ();


private:
	int		GetSelectMenu ( POINT pt );
	void	ForceSelectOnRightClick ( POINT pt );
	void	RunSelectMenu ( int nSelect );

	void		SetSelectItem ( PITEMNODE pItemNode );
	PITEMNODE	GetSelectItem ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlItemTree;	
	virtual BOOL OnInitDialog();	
	afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
};