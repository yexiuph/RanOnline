//	BasicScrollBarEx 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.12.1]
//			@ 최초제작

#pragma	once

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

class	CBasicTextBoxEx : public CBasicTextBox
{
public:
static	const int nNOT_VISIBLE;

public:
	CBasicTextBoxEx ();
	virtual	~CBasicTextBoxEx ();

public:
	void	CreateMouseOver ( D3DCOLOR dwMouseOverLineColor );
	void	CreateMouseClick ( D3DCOLOR dwMouseOverLineColor );

public:
	virtual	void	ClearText ();

protected:
	void	UpdateMousePos ( int x, int y );

public:
	void	UpdateMouseOverImage ();
	void	UpdateMouseClickImage ();

	int		GetSelectPos ();
	void	SetSelectPos ( int nIndex ) { m_nMouseOverLine = nIndex; }

public:
	void	SetSensitive ( const bool& bSensitive );	
	void	SetSensitiveView ( const bool& bSensitive );
	void	SetSelectInit( const bool& bSelectInit );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*	m_pMouseOverImage;
	D3DCOLOR	m_dwMouseOverImageColor;

	CUIControl* m_pMouseClickImage;

	bool	m_bSensitive;
	bool	m_bSensitiveView;

private:
	int		m_nMouseOverLine;

	int		m_nSELECTED_INDEX;

	bool	m_bSelectInit;
};