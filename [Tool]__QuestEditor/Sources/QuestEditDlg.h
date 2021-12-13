// QuestEditDlg.h : 헤더 파일
//

#pragma once

#include "GLQuestMan.h"
#include "GLMapList.h"

// CQuestEditDlg 대화 상자
class CsheetWithTab;
class CQuestEditDlg : public CDialog
{
public:	
	void	EnableMenu ( UINT uIDEnableItem, BOOL bEnable );
	void	EnableToobar ( UINT uIDEnableItem, BOOL bEnable );

// 생성
public:
	CQuestEditDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_QUESTEDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	static GLQuestMan::SNODE m_sQuestNode;

public:
	CsheetWithTab	*m_pSheetTab;

public:
	static GLMapList m_sMapList;

public:
	bool DoSave ();
	int  GetID ( CString& strID );

	void SetReadMode();

public:
	void MakeToolBar ();

public:
	char m_szAppPath[MAX_PATH];
	CString m_strAppPath;

// 구현
protected:
	HICON m_hIcon;
	CToolBar m_wndToolBar;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMenuitemNew();
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemExit();
	afx_msg void OnMenuitemCsvLO();
	afx_msg void OnMenuitemCsvSA();

protected:
	virtual void PostNcDestroy();

public:
	afx_msg void OnClose();
	afx_msg void OnMenuitemTextExport();
	afx_msg void OnMenuitemTextImport();
};