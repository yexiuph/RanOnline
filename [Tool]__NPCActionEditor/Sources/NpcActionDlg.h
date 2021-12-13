// NpcActionDlg.h : 헤더 파일
//

#pragma once

// 데이터 로드용 구조체
struct SAnswerTextData 
{
	std::string		   answerTalkText;
	int			   dwNpcNumber;
	int			   dwTextNumber;
	int			   dwAnswerNumber;
};

struct SNpcTextData 
{
	std::string		   TalkText;
	int			   dwNpcNumber;
	int			   dwTextNumber;
};

class	CsheetWithTab;

// CNpcActionDlg 대화 상자
class CNpcActionDlg : public CDialog
{
// 생성
public:
	CNpcActionDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_NPCACTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	CsheetWithTab*	m_pSheetTab;

	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

private:
	CToolBar    m_wndToolBar;

private:
	void	MakeToolBar ();

public:	
	void	EnableMenu ( UINT uIDEnableItem, BOOL bEnable );
	void	EnableToobar ( UINT uIDEnableItem, BOOL bEnable );

	void	SetReadMode();

private:
	bool	DoSave ();
	bool	TextExport( CString strFileName );

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnMenuitemNew();
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuAllTextExport();
	afx_msg void OnMenuAllTextExport2();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCsvSaveButton();
	afx_msg void OnBnClickedTextexportButton();
	afx_msg void OnBnClickedTextimportButton();
	afx_msg void OnBnClickedCsvexportButton();
	afx_msg void OnBnClickedCsvimportButton();
	afx_msg void OnBnClickedAllSaveButton();
	afx_msg void OnBnClickedCsvexportButton2();
	afx_msg void OnBnClickedCsvimportButton2();
};
