#pragma once
#include "afxcmn.h"




// CSearchDialog 대화 상자입니다.

class CSearchDialog : public CDialog
{
	DECLARE_DYNAMIC(CSearchDialog)

public:
	CSearchDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSearchDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEM_SEARCH_DIALOG };

	void InsertReportItem( std::vector<SSearchItemList> vecSearchItemList );
	int	 GetSelectItem() { return m_iSelectCount; };
	void ClearItem();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
