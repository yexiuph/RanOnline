#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicProgressBar;
class	CBasicTextBox;
class	CBasicButton;

class	CBasicAudioOption : public CUIGroup
{
public:
static	const long lCONTROLVOLUME;

protected:
	enum
	{
		HWOPTION_AUDIO_SFX_PROGRESS = NO_ID + 1,
		HWOPTION_AUDIO_ENVIR_PROGRESS,
		HWOPTION_AUDIO_BGM_PROGRESS,
		HWOPTION_SFX_BUTTON,
		HWOPTION_ENVIR_BUTTON,
		HWOPTION_BGM_BUTTON
	};

public:
	CBasicAudioOption ();
	virtual	~CBasicAudioOption ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void	LoadCurrentOption ();

private:
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont );
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

private:
    CBasicProgressBar*		m_pSfxProgressBar;
	CBasicProgressBar*		m_pEnvirProgressBar;
	CBasicProgressBar*		m_pBgmProgressBar;

private:
	CBasicButton*	m_pSfxButton;
	CBasicButton*	m_pEnvirButton;
	CBasicButton*	m_pBgmButton;

private:
	D3DXVECTOR2		m_vMousePos;

public:
	BOOL m_bSndMuteSfx;
	BOOL m_bSndMuteMap;
	BOOL m_bSndMuteMusic;
};