#pragma once
#include <string>
#include "GLDefine.h"
#include "GLCrow.h"
#include "MobEditTree.h"
class	CsheetWithTab;
struct	SCROWDATA;

// MobEditData dialog

class MobEditData : public CPropertyPage
{
	DECLARE_DYNAMIC(MobEditData)

public:
	MobEditData(LOGFONT logfont);
	virtual ~MobEditData();

// Dialog Data
	enum { IDD = IDD_MOBEDIT_DATA };

public:
	MobEditTree m_MobEditTree;

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	BOOL			m_bDlgInit;
	std::string		strKeyName;
	std::string		strName;
	CFont*			m_pFont;

public:
	PCROWDATA		m_pDummyCrow;
	PCROWDATA		m_pCrow;
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	BOOL			SetCrow ( PCROWDATA pCrow );
	CListBox		m_listPattern;
	bool			bAdd;

protected:
	void	UpdateCrow ();
	void	InitDefaultCtrls ();
	BOOL	InverseUpdateCrow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void LoadGENITEM( int nID );
	afx_msg void LoadCROWSALE( int nID );
	afx_msg void LoadNTK( int nID );
	afx_msg void LoadCHF( int nID );
	afx_msg void LoadEGP( int nID );
	afx_msg void LoadEFFSKIN( int nID );
	afx_msg void LoadTEXTURE( int nID );
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPatternadd();
	afx_msg void OnBnClickedButtonPatternedit();
	afx_msg void OnBnClickedButtonPatterndelete();
	afx_msg void OnBnClickedButtonPatternok();
	afx_msg void OnBnClickedButtonSkinfile();
	afx_msg void OnBnClickedButtonConversationfile();
	afx_msg void OnBnClickedButtonSale1();
	afx_msg void OnBnClickedButtonSale2();
	afx_msg void OnBnClickedButtonSale3();
	afx_msg void OnBnClickedButtonBirtheffect();
	afx_msg void OnBnClickedButtonDeatheffect();
	afx_msg void OnBnClickedButtonHiteffect();
	afx_msg void OnBnClickedButtonItemgen();
	afx_msg void OnBnClickedButtonItemgenQ();
};
