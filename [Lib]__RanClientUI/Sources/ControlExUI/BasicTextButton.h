//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/BasicButton.h"

class CBasicTextButtonImage;
class CBasicTextBox;

class	CBasicTextButton : public CBasicButton
{
public:
	enum
	{
		SIZE14 = 0,
		SIZE16,
		SIZE16s,
		SIZE18,
		SIZE19,
		SIZE19_RECT,
		SIZE22,

		SIZE_NSIZE = 7
	};

public:
	CBasicTextButton ();
	virtual	~CBasicTextButton ();

protected:
	virtual	void	SetFlipPosition ();
	virtual	void	ResetFlipPosition ();

public:
	void	SetOneLineText ( CString strTemp );

public:
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual	void	SetFlip ( BOOL bFlip );

public:
	void	CreateBaseButton (	const char* szButtonControl, 
								int nType, 
								WORD wFlipMode, 
								const char* szText, 
								DWORD dwFlags=0L );

private:
	void	SetRadioButton ();
	void	SetGeneralButton ();

private:
	void	CreateButtonImage ( int nType, CD3DFontPar* pFont, BOOL bReversal );

private:
	CBasicTextButtonImage*	m_pButton;
	CBasicTextButtonImage*	m_pButtonFlip;

	CBasicTextBox*		m_pButtonText;
	CUIControl*			m_pButtonTextBack;

	D3DCOLOR	m_TextColor;
	D3DCOLOR	m_TextFlipColor;
};