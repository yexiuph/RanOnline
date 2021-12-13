#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "GLCharData.h"

// CEtcFuncPage dialog
class CsheetWithTab;

class CEtcFuncPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEtcFuncPage)

public:
	CEtcFuncPage();
	virtual ~CEtcFuncPage();

// Dialog Data
	enum { IDD = IDD_ETC_FUNC_PAGE };

public:
	static BOOL		m_bLimitTimeUse;
	static BOOL		m_bCamUse;
	static BOOL		m_bCamTest;
	static BOOL		m_bLandEffUse;

	BOOL			m_bInit;
	DWORD			m_dwSelectNum;

	CsheetWithTab*	m_pSheetTab;

	D3DXVECTOR2		m_vMinLandPos;
	D3DXVECTOR2		m_vMaxLandPos;

	VEC_LANDEFF		m_vecLandEffect;

public:
	void	SetSheetTab ( CsheetWithTab* SheetTab ) { m_pSheetTab = SheetTab; }
	void	SetEnableFunction ( DWORD dwEtcfunc, BOOL bEnable );
	void	SetEtcFuncData ();

	void	UpdateLandEffectList ();
	void	UpdatePage ();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_list_LandEff;
	CComboBox m_comob_LandEff;
	afx_msg void OnBnClickedCheckLimitTimeUse();
	afx_msg void OnBnClickedCheckCamUse();
	afx_msg void OnBnClickedButtonCamSetCenter();
	afx_msg void OnBnClickedCheckLandEffUse();
	afx_msg void OnNMClickListLandEff(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListLandEff(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonLandEffAdd();
	afx_msg void OnBnClickedButtonLandEffDel();
	afx_msg void OnBnClickedButtonEtcFuncSave();
	afx_msg void OnBnClickedCheckCamTest();
};
