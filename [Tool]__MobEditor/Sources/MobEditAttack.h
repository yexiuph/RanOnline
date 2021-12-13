#pragma once
#include <string>
#include "GLDefine.h"
#include "GLCrow.h"
#include "MobEditTree.h"
class	CsheetWithTab;
struct	SCROWDATA;

// MobEditAttack dialog

class MobEditAttack : public CPropertyPage
{
	DECLARE_DYNAMIC(MobEditAttack)

public:
	MobEditAttack(LOGFONT logfont);
	virtual ~MobEditAttack();

// Dialog Data
	enum { IDD = IDD_MOBEDIT_ATTACK };

public:
	MobEditTree m_MobEditTree;

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	BOOL			m_bDlgInit;
	std::string		strKeyName;
	std::string		strName;
	CFont*			m_pFont;
	int				iIndex;
	CListBox		m_listAni;
	CListBox		m_listAniDiv;

public:
	PCROWDATA		m_pDummyCrow;
	PCROWDATA		m_pCrow;
	void			SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void			SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	BOOL			SetCrow ( PCROWDATA pCrow );

protected:
	void	UpdateCrow (int iIndex);
	void	InitDefaultCtrls ();
	BOOL	InverseUpdateCrow(int iIndex);



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedButtonIndexback();
	afx_msg void OnBnClickedButtonIndexnext();
	afx_msg void OnBnClickedButtonAniuse();
	afx_msg void OnCbnSelchangeComboTroubletype();
	afx_msg void OnBnClickedButtonSelfbodyeffect();
	afx_msg void OnBnClickedButtonTargetbodyeffect();
	afx_msg void OnBnClickedButtonTargeteffect();
	afx_msg void LoadEGP( int nID );
	afx_msg void LoadEFFSKIN( int nID );
};
