#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSimpleEdit 대화 상자입니다.

class CSimpleEdit : public CDialog
{
	DECLARE_DYNAMIC(CSimpleEdit)

public:
	CSimpleEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSimpleEdit();

	CNpcDialogueSet* m_pDialogueSet;
	CNpcDialogue*    m_pNpcDialogue;

	void SetNpcDialogueSet( CNpcDialogueSet* pDialogueSet ) { m_pDialogueSet = pDialogueSet; };
	void SetSimpleData();
	void SetNPCData( CNpcDialogue* pNpcDialogue );
	void UpdateAnswerList( DWORD dwCaseIndex );
	void UpdateAnswerData( int nIndex );
	void ApplyNPCData();
	void ApplyAnswerData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SIMPLEEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	

public:
	CSliderCtrl m_Basic_Slider;
	CEdit m_Basic_Edit;
	CEdit m_BasicConv_Edit;
	CListCtrl m_Answer_List;
	CEdit m_Text_Edit;
	int	  m_OldGetPos;
	int	  m_OldTextGetPos;

	DWORD m_dwSelectCase;

	DECLARE_MESSAGE_MAP()


	afx_msg void OnNMCustomdrawBasicSlider(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickAnswerList(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_Answer_Edit;
	CEdit m_AnswerID_Edit;
	CEdit m_AnswerType_Edit;
	CEdit m_AnswerData_Edit;
//	afx_msg void OnBnClickedApplyButton();
	afx_msg void OnBnClickedAnswerapplyButton();
	afx_msg void OnBnClickedTextapplyButton();
	afx_msg void OnNMCustomdrawTextSlider(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_Text_Slider;
	CEdit m_TextId_Edit;
};
