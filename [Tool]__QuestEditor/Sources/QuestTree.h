#pragma once
#include "afxcmn.h"
#include "GLQuestMan.h"


// CQuestTree 대화 상자입니다.
class	CsheetWithTab;
typedef GLQuestMan::SNODE QUESTNODE;
typedef QUESTNODE*		  PQUESTNODE;
class CQuestTree : public CPropertyPage
{
	DECLARE_DYNAMIC(CQuestTree)

public:
	CQuestTree( LOGFONT logfont );
	virtual ~CQuestTree();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_QUESTTREE_DIALOG };
private:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bDlgInit;
	HTREEITEM		m_TreeRoot;
	CFont*			m_pFont;

public:
	BOOL	UpdateTree ();
	void	UpdateName ();

private:
	BOOL	UpdateItem ( PQUESTNODE pQuestNode, HTREEITEM hMainItem );
	const char*	GetFormatName ( PQUESTNODE pQuestNode );

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	CleanAllQuest ();

private:
	PQUESTNODE	m_pQuestNode;
	BOOL		m_bDummyHasQuest;
	PQUESTNODE	m_pDummyQuestNode;

private:
	BOOL	AddQuest();
	BOOL	AddQuestSub ();    
	
	BOOL	DelQuest ();

	BOOL	CopyQuest ();
	BOOL	PasteQuestNew ();
	BOOL	PasteQuestValue ();

private:
	int		GetSelectMenu ( POINT pt );
	void	ForceSelectOnRightClick ( POINT pt );
	void	RunSelectMenu ( int nSelect );

	void		SetSelectItem ( PQUESTNODE pQuestNode );
	PQUESTNODE	GetSelectItem ();

	void	SetReadMode( CMenu* pMenu );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlQuestTree;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
};
