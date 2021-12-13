#pragma once


#include "NpcTalkCondition.h"

// CLearnSkillDialog 대화 상자입니다.


class CLearnSkillDialog : public CDialog
{
	DECLARE_DYNAMIC(CLearnSkillDialog)

public:
	CLearnSkillDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLearnSkillDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LEARNSKILLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	SSkillCondition	m_Condition;
	BOOL			m_bConfirm;

public:
	SSkillCondition	GetSkillCondition ();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
};
