#pragma once
#include <string>

// CItemParent 대화 상자입니다.
class	CsheetWithTab;
struct	SITEM;

class CItemParent : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemParent)

public:
	CItemParent( LOGFONT logfont );
	virtual ~CItemParent();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMPARENT_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SITEM*			m_pDummyItem;
	SITEM*			m_pItem;

	BOOL			m_bDlgInit;

	std::string		strKeyName;
	std::string		strName;
	std::string		strKeyDesc;
	std::string		strDesc;
	CFont*			m_pFont;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SITEM* pItem );

protected:
	void	UpdateItems ();
	BOOL	InverseUpdateItems ();
	void	InitDefaultCtrls ();
	void	LoadWearingfile( int nID );
	void	LoadABLFile( int nID );
	void	LoadVPSFile( int nID );

	//////////////////////////////////////////////////////////////////////////////
	//	리팩토링 적용펑션
public:
	void	InitAllItem ();
	void	SetReadMode ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonInvenfile();
	afx_msg void OnBnClickedButtonFieldfile();
	
	afx_msg void OnBnClickedButtonWearingfileC1();
	afx_msg void OnBnClickedButtonWearingfileC2();
	afx_msg void OnBnClickedButtonWearingfileC3();
	afx_msg void OnBnClickedButtonWearingfileC4();
	afx_msg void OnBnClickedButtonWearingfileC5();
	afx_msg void OnBnClickedButtonWearingfileC6();
	afx_msg void OnBnClickedButtonWearingfileC7();
	afx_msg void OnBnClickedButtonWearingfileC8();
	afx_msg void OnBnClickedButtonWearingfileC9();
	afx_msg void OnBnClickedButtonWearingfileC10();
	afx_msg void OnBnClickedButtonWearingfileC11();
	afx_msg void OnBnClickedButtonWearingfileC12();
	afx_msg void OnBnClickedButtonWearingfileC13();

	afx_msg void OnBnClickedButtonSkinEffectfile();
	afx_msg void OnBnClickedButtonEffectfile();
	afx_msg void OnBnClickedButtonSkinTarEffectfile();
	afx_msg void OnEnChangeEditItemname();
	afx_msg void OnEnChangeEditItemdesc();

	afx_msg void OnCbnSelchangeComboItemType();
	afx_msg void OnEnChangeEditSpecid();
	afx_msg void OnBnClickedButtonAblMLeft();
	afx_msg void OnBnClickedButtonAblMRight();
	afx_msg void OnBnClickedButtonAblWLeft();
	afx_msg void OnBnClickedButtonAblWRight();
	afx_msg void OnBnClickedButtonVehiclePart1();
	afx_msg void OnBnClickedButtonVehiclePart2();
	afx_msg void OnBnClickedButtonVehiclePart3();
	afx_msg void OnBnClickedButtonVehiclePart4();
	afx_msg void OnBnClickedButtonVehiclePart5();
	afx_msg void OnCbnSelchangeComboVehicleType();
	afx_msg void OnBnClickedButtonWearingfileC14();
	afx_msg void OnBnClickedButtonWearingfileC15();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioMan();
	afx_msg void OnBnClickedRadioWoman();
};
