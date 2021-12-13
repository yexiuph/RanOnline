#pragma once
#include "afxcmn.h"




// CSearchDialog ��ȭ �����Դϴ�.

class CSearchDialog : public CDialog
{
	DECLARE_DYNAMIC(CSearchDialog)

public:
	CSearchDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSearchDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ITEM_SEARCH_DIALOG };

	void InsertReportItem( std::vector<SSearchItemList> vecSearchItemList );
	int	 GetSelectItem() { return m_iSelectCount; };
	void ClearItem();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
private:
	std::vector<SSearchItemList> m_vecSearchItemList;
	CListCtrl					 m_SearchReportList;
	int							 m_iSelectCount;
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
