#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;
class	CBasicButton;
class	CD3DFontPar;
class	CBasicComboBox;
class	CBasicComboBoxRollOver;
enum	EMSCREEN_FORMAT;

#define DEFAULT_OPTION_SIZE 4

class	CBasicVideoOption : public CUIGroup
{
	static const char cRESOLUTION_SEPERATOR;

protected:
	enum
	{
		HWOPTION_VIDEO_DEFAULT_COMBO_OPEN = NO_ID + 1,
		HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER,
		HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN,
		HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER,
		HWOPTION_VIDEO_SHADOW_COMBO_OPEN,
		HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER,
		HWOPTION_VIDEO_DETAIL_COMBO_OPEN,
		HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER,
		HWOPTION_VIDEO_SIGHT_COMBO_OPEN,
		HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER,
		HWOPTION_VIDEO_BUFF_BUTTON,
		HWOPTION_VIDEO_SHADOWLAND_BUTTON,
		HWOPTION_VIDEO_REFLECT_BUTTON,
		HWOPTION_VIDEO_REFRACT_BUTTON,
		HWOPTION_VIDEO_GLOW_BUTTON,
		HWOPTION_VIDEO_POSTPROCESSING_BUTTON,
		HWOPTION_VIDEO_FRAME_LIMIT_BUTTON
	};

public:
	CBasicVideoOption ();
	virtual	~CBasicVideoOption ();

public:
	void	CreateSubControl ();

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

private:
	void	LoadComboData ();
	void	SetTextSightCombo( DWORD dwIndex );
	void	SetTextSkinDetailCombo( DWORD dwIndex );
	void	SetTextShadowCharCombo( DWORD dwIndex );
	void	SetTextResolutionCombo( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFormat);
	void	SetTextDefaultOptionCombo( DWORD dwIndex );

public:
	void	LoadCurrentOption ();

private:
	void	LoadResolution ();
	void	LoadShadow ();
	void	LoadSkinDetail ();
	void	LoadSight ();
	void	LoadBuffButton();
	void	LoadShadowlandButton ();
	void	LoadReflectButton ();
	void	LoadRefractButton ();
	void	LoadGlowButton ();
	void	LoadPostButton ();
	void	LoadFrameLimitButton ();
	void	LoadDefaultOption();
	void	SetVideoLevel(int nIndex );
	void	SetLowLevel();
	void	SetMediumLevel();
	void	SetHighLevel();

private:
	CD3DFontPar*	m_pFont;

private:
	CBasicButton*		m_pBuffButton;
	CBasicButton*		m_pShadowlandButton;
	CBasicButton*		m_pReflectButton;
	CBasicButton*		m_pRefractButton;
	CBasicButton*		m_pGlowButton;
	CBasicButton*		m_pPostButton;
	CBasicButton*		m_pFrameLimitButton;

private:
	CBasicComboBox*			m_pComboBoxResolutionOpen;
	CBasicComboBoxRollOver*	m_pComboBoxResolutionRollOver;

	CBasicComboBox*			m_pComboBoxShadowOpen;
	CBasicComboBoxRollOver*	m_pComboBoxShadowRollOver;

	CBasicComboBox*			m_pComboBoxCharDetailOpen;
	CBasicComboBoxRollOver*	m_pComboBoxCharDetailRollOver;

	CBasicComboBox*			m_pComboBoxSightOpen;
	CBasicComboBoxRollOver*	m_pComboBoxSightRollOver;
	
	CBasicComboBox*			m_pComboBoxDefaultOpen;
	CBasicComboBoxRollOver*	m_pComboBoxDefaultRollOver;

private:
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

public:
	BOOL m_bBuff;
	BOOL m_bShadowLand;
	BOOL m_bRealReflect;
	BOOL m_bRefract;
	BOOL m_bGlow;
	BOOL m_bPost;
	BOOL m_bFrameLimit;

	DWORD m_dwSight;
	DWORD m_dwSkinDetail;
	DWORD m_dwShadowChar;
	DWORD m_dwVideoLevel;

	DWORD m_dwScrHeight;
	DWORD m_dwScrWidth;
	EMSCREEN_FORMAT m_emScrFormat;
};