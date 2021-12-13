#pragma	once

#include "./UIWindowEx.h"

class	CD3DFontPar;
class	CBasicProgressBar;
class	CBasicTextBox;
class	CBasicButton;
struct	GLCHARLOGIC;

class CCharacterWindow : public CUIWindowEx
{
protected:
	enum
	{
		CHARACTER_POW_BUTTON = ET_CONTROL_NEXT,
		CHARACTER_DEX_BUTTON,
		CHARACTER_SPI_BUTTON,
		CHARACTER_INT_BUTTON,
		CHARACTER_STR_BUTTON,
		CHARACTER_STA_BUTTON,
		CHARACTER_MOBILE_BUTTON,

		CHARACTER_PREMIUM_TEXT,

		CHARACTER_BIRGHTER,
	};

public:
    CCharacterWindow ();
	virtual	~CCharacterWindow ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);

public:
	void	CreateSubControl ();

private:
	CBasicTextBox*	CreateStaticControl ( char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );

private:
	void	SetPointControl ( BOOL bPointUsable );

private:	//	이름,클래스
	void	SetName ( const GLCHARLOGIC& sCharData );

private:	//	경험치
	void	SetEXP( const GLCHARLOGIC& sCharData );

private:	//	Stat
	void	SetLevel ( const GLCHARLOGIC& sCharData );
	void	SetPow ( const GLCHARLOGIC& sCharData );
	void	SetDex ( const GLCHARLOGIC& sCharData );
	void	SetSpi ( const GLCHARLOGIC& sCharData );
	void	SetInt ( const GLCHARLOGIC& sCharData );
	void	SetStr ( const GLCHARLOGIC& sCharData );
	void	SetSta ( const GLCHARLOGIC& sCharData );
	void	SetPoint ( const GLCHARLOGIC& sCharData );

private:	//	Status
	void	SetDamage ( const GLCHARLOGIC& sCharData );
	void	SetDefense ( const GLCHARLOGIC& sCharData );
	void	SetReqPa ( const GLCHARLOGIC& sCharData );
	void	SetReqSa ( const GLCHARLOGIC& sCharData );
	void	SetReqMa ( const GLCHARLOGIC& sCharData );
	void	SetHitRate ( const GLCHARLOGIC& sCharData );
	void	SetHP ( const GLCHARLOGIC& sCharData );
	void	SetMP ( const GLCHARLOGIC& sCharData );
	void	SetSP ( const GLCHARLOGIC& sCharData );
	void	SetAvoidRate ( const GLCHARLOGIC& sCharData );
	void	SetPremium ();

private:	//	저항력
	void	SetFireResi ( const GLCHARLOGIC& sCharData );
	void	SetColdResi ( const GLCHARLOGIC& sCharData );
	void	SetElectricResi ( const GLCHARLOGIC& sCharData );
	void	SetPoisonResi ( const GLCHARLOGIC& sCharData );
	void	SetSpiritResi ( const GLCHARLOGIC& sCharData );

private:	//	사회 성향
//	void	SetPK ( const GLCHARLOGIC& sCharData );
	void	SetBright ( const GLCHARLOGIC& sCharData );
	void	SetPKState ( const GLCHARLOGIC& sCharData );
	void	SetGrade ( const GLCHARLOGIC& sCharData );
	void	SetActivityPoint ( const GLCHARLOGIC& sCharData );	//	생활점수
	void	SetAcademy ( const GLCHARLOGIC& sCharData );		//	학교
	void	SetDepartment ( const GLCHARLOGIC& sCharData );		//	부서

public:
	void	SetPhoneNumber( const CString & strPhoneNumber );

private:
	CBasicTextBox*		m_pNameText;

private:
	CBasicTextBox*	m_pPremiumTextStatic;

private:	//	경험치
	CBasicProgressBar*	m_pEXP;
	CBasicTextBox*		m_pEXPText;

private:	//	Stats
	CBasicButton*	m_pPowButton;
	CBasicButton*	m_pDexButton;
	CBasicButton*	m_pSpiButton;
	CBasicButton*	m_pIntButton;
	CBasicButton*	m_pStrButton;
	CBasicButton*	m_pStaButton;

	CUIControl*	m_pPowDiaableButton;
	CUIControl*	m_pDexDiaableButton;
	CUIControl*	m_pSpiDiaableButton;
	CUIControl*	m_pIntDiaableButton;
	CUIControl*	m_pStrDiaableButton;
	CUIControl*	m_pStaDiaableButton;

	CBasicTextBox*	m_pPowText;
	CBasicTextBox*	m_pDexText;
	CBasicTextBox*	m_pSpiText;
	CBasicTextBox*	m_pIntText;
	CBasicTextBox*	m_pStrText;
	CBasicTextBox*	m_pStaText;

	CBasicTextBox*	m_pPowTextStatic;
	CBasicTextBox*	m_pDexTextStatic;
	CBasicTextBox*	m_pSpiTextStatic;
	CBasicTextBox*	m_pIntTextStatic;
	CBasicTextBox*	m_pStrTextStatic;
	CBasicTextBox*	m_pStaTextStatic;

	CUIControl*	m_pPointDisable;
	CUIControl*	m_pPointEnable;
	CBasicTextBox*	m_pPointText;	

private:	//	Status
	CBasicTextBox*	m_pLevelText;
	CBasicTextBox*	m_pDamageText;
	CBasicTextBox*	m_pDefenseText;
	CBasicTextBox*	m_pReqPaText;
	CBasicTextBox*	m_pReqSaText;	
	CBasicTextBox*	m_pReqMaText;	
	CBasicTextBox*	m_pHPText;
	CBasicTextBox*	m_pMPText;
	CBasicTextBox*	m_pSPText;
	CBasicTextBox*	m_pHitRateText;
	CBasicTextBox*	m_pAvoidRateText;
	CBasicTextBox*	m_pPremiumText;

private:	//	저항력
	CBasicTextBox*	m_pFireText;
	CBasicTextBox*	m_pColdText;
	CBasicTextBox*	m_pElectricText;
	CBasicTextBox*	m_pPoisonText;
	CBasicTextBox*	m_pSpiritText;

private:	//	사회 성향
	CBasicTextBox*	m_pAcademy;
	CBasicTextBox*	m_pActivityPoint;
	CBasicTextBox*	m_pPKStateText;

	CBasicTextBox*	m_pDepartment;
	CUIControl*		m_pBrightFrame;
	CUIControl*		m_pBrightSlider;
	CBasicTextBox*	m_pGradeText;

	CBasicTextBox* m_pMobileText;

private:
	static const int nSCHOOL_MARK = 3;
	CUIControl*	m_pSchoolMark[nSCHOOL_MARK];
};