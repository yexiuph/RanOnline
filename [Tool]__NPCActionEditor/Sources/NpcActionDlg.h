// NpcActionDlg.h : ��� ����
//

#pragma once

// ������ �ε�� ����ü
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

// CNpcActionDlg ��ȭ ����
class CNpcActionDlg : public CDialog
{
// ����
public:
	CNpcActionDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_NPCACTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

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

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
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
