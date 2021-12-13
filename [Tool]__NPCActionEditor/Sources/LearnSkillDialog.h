#pragma once


#include "NpcTalkCondition.h"

// CLearnSkillDialog ��ȭ �����Դϴ�.


class CLearnSkillDialog : public CDialog
{
	DECLARE_DYNAMIC(CLearnSkillDialog)

public:
	CLearnSkillDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLearnSkillDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LEARNSKILLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
